/* 
Contributors: Raghurama Bukkarayasamudram, Ritam Das, Brian Macomber
Date: 11/13/2020
Quest 4 - E-Voting v2
*/

/* 
Here is the code to go onto the ESP

Functions:
- Connect to Wifi
- Change LED color to reflect the candidate (button 1 to change through LED colors)
- IR transmission and recieving (push button 2 to send the data)
- UDP communication (CLIENT) to communicate with a leader ESP (when current ESP is NOT a leader)
- UDP communication (SERVER) to communicate with non-leader ESP (when current ESP is a leader)
- UDP communication (CLIENT) to communicate with nodejs server (when current ESP is a leader)

- Each ESP should also have some structure of information that holds info for all the ESPs in the system 

Also need to implement the finite state machine:
    3 states:
    - Leader
        - ESP with highest ID# will be the leader 
        - Recieve votes over UDP from non-leader ESPs (UDP server - port 3333)
        - Send vote over UDP to nodejs server (UDP client - port 1131)
        - can recieve and send votes over IR
    - Candidate
        - can recieve and send votes over IR
        - can send recieved votes over UDP to Leader
    - Neither
        - Not connected to WIFI or power or whatever (dead state)
*/

////////////////////////////////////////////////////////////////////////////////
//                              Libraries                                     //
////////////////////////////////////////////////////////////////////////////////
// Standard C libs
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
//Wifi + other ESP libraries
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

//UART
#include "driver/uart.h"
#include "esp_vfs_dev.h"

////////////////////////////////////////////////////////////////////////////////
//                              Definitions                                   //
////////////////////////////////////////////////////////////////////////////////

// ----------------------------- WiFi -------------------------------------//
#define EXAMPLE_ESP_WIFI_SSID ""
#define EXAMPLE_ESP_WIFI_PASS ""

#define EXAMPLE_ESP_MAXIMUM_RETRY 5
static EventGroupHandle_t s_wifi_event_group;
/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
static const char *WIFI_TAG = "wifi station";

static int s_retry_num = 0;
// -----------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
//                              FUNCTIONS                                     //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------- Socket / UDP Comms ------------------------//

/// ************************ gonna have to change some stuff here with ports and IPs ******

#define HOST_IP_ADDR "192.168.7.196"
#define PORT 1131

static const char *SOCKET_TAG = "example";
char payload[20] = "";
char ledBrightness[2] = "0\0";

static void
udp_client_task(void *pvParameters)
{
    char rx_buffer[128];
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;

    while (1)
    {

        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0)
        {
            ESP_LOGE(SOCKET_TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(SOCKET_TAG, "Socket created, sending to %s:%d", HOST_IP_ADDR, PORT);

        while (1)
        {
            //here is where data is sent (roll, pitch, temp)

            int err = sendto(sock, payload, strlen(payload), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            if (err < 0)
            {
                ESP_LOGE(SOCKET_TAG, "Error occurred during sending: errno %d", errno);
                break;
            }
            ESP_LOGI(SOCKET_TAG, "Message sent");

            struct sockaddr_in source_addr; // Large enough for both IPv4 or IPv6
            socklen_t socklen = sizeof(source_addr);
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            // Error occurred during receiving
            if (len < 0)
            {
                ESP_LOGE(SOCKET_TAG, "recvfrom failed: errno %d", errno);
                break;
            }
            // Data received
            else
            {
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                ESP_LOGI(SOCKET_TAG, "Received %d bytes from %s:", len, host_ip);
                ESP_LOGI(SOCKET_TAG, "%s", rx_buffer);
                //change led brightness according to respone from node server
                strcpy(ledBrightness, rx_buffer);

                if (strncmp(rx_buffer, "OK: ", 4) == 0)
                {
                    ESP_LOGI(SOCKET_TAG, "Received expected message, reconnecting");
                    break;
                }
            }

            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }

        if (sock != -1)
        {
            ESP_LOGE(SOCKET_TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

// -----------------------------------------------------------------------//
// ----------------------------- WiFi -------------------------------------//
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(WIFI_TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(WIFI_TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(WIFI_TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,

            .pmf_cfg = {
                .capable = true,
                .required = false},
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(WIFI_TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(WIFI_TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(WIFI_TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    }
    else
    {
        ESP_LOGE(WIFI_TAG, "UNEXPECTED EVENT");
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}

void initalizeWiFi()
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(WIFI_TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
}

// -------------------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
//                             MAIN                                           //
////////////////////////////////////////////////////////////////////////////////

void app_main()
{
    // initalize the wifi
    initalizeWiFi();

    //udp client start
    xTaskCreate(udp_client_task, "udp_client", 4096, NULL, 5, NULL);

    while (1)
    {
        // do stuff here
    }
}