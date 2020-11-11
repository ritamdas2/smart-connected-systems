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

//IR Comms
#include "driver/rmt.h"
#include "soc/rmt_reg.h"
#include "driver/periph_ctrl.h"
#include "soc/uart_reg.h" // Little said on piazza to add this

//Timer (if needed)
#include "driver/timer.h"

////////////////////////////////////////////////////////////////////////////////
//                              Definitions                                   //
////////////////////////////////////////////////////////////////////////////////

// ---------------------------- RMT ---------------------------------------//
#define RMT_TX_CHANNEL 1                                 // RMT channel for transmitter
#define RMT_TX_GPIO_NUM 25                               // GPIO number for transmitter signal -- A1
#define RMT_CLK_DIV 100                                  // RMT counter clock divider
#define RMT_TICK_10_US (80000000 / RMT_CLK_DIV / 100000) // RMT counter value for 10 us.(Source clock is APB clock)
#define rmt_item32_tIMEOUT_US 9500                       // RMT receiver timeout value(us)

// ---------------------------- UART ---------------------------------------//
#define UART_TX_GPIO_NUM 26 // A0
#define UART_RX_GPIO_NUM 34 // A2
#define BUF_SIZE (1024)

// ---------------------- Hardware Interrupt -------------------------------//
#define GPIO_INPUT_IO_1 4
#define ESP_INTR_FLAG_DEFAULT 0
#define GPIO_INPUT_PIN_SEL 1ULL << GPIO_INPUT_IO_1

#define BUTTON_GPIO 27

// LED Output pins definitions
#define BLUEPIN 14
#define GREENPIN 32
#define REDPIN 15
#define ONBOARD 13

// ---------------------------- ID/Color ---------------------------------------//
// Default ID/color
#define ID 3
#define COLOR 'R'

// --------------------- IR transmission definitions ---------------------------//
// Variables for my ID, minVal and status plus string fragments
char start = 0x1B;
char myID = (char)ID;
char myColor = (char)COLOR;
int len_out = 4;

// -------------------- Data locking and Sending defintions --------------------//
// Mutex (for resources), and Queues (for button)
SemaphoreHandle_t mux = NULL;
static xQueueHandle gpio_evt_queue = NULL;

// flag for sending data
int sendFlag = 0;

// System tags
static const char *TAG_SYSTEM = "system"; // For debug logs

// Button 1 interrupt handler -- add to queue
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}
// ---------------------------- FSM ---------------------------------------//
#define S0 0
#define S1 1
#define S2 2

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

////////////////////////////////////////////////////////////////////////////////
//                              FUNCTIONS                                     //
////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
// UDP Functions ///////////////////////////////////////////////////////////////////

/// ********* gonna have to change some stuff here with ports and IPs ************

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
/////////////////////////////////////////////////////////////////////////////////////
// Wifi Functions ///////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////////////
// Utility Functions ///////////////////////////////////////////////////////////////////

// Checksum
char genCheckSum(char *p, int len)
{
    char temp = 0;
    for (int i = 0; i < len; i++)
    {
        temp = temp ^ p[i];
    }
    // printf("%X\n",temp);

    return temp;
}
bool checkCheckSum(uint8_t *p, int len)
{
    char temp = (char)0;
    bool isValid;
    for (int i = 0; i < len - 1; i++)
    {
        temp = temp ^ p[i];
    }
    // printf("Check: %02X ", temp);
    if (temp == p[len - 1])
    {
        isValid = true;
    }
    else
    {
        isValid = false;
    }
    return isValid;
}

/////////////////////////////////////////////////////////////////////////////////////
// Init Functions //////////////////////////////////////////////////////////////
// RMT tx init
static void rmt_tx_init()
{
    rmt_config_t rmt_tx;
    rmt_tx.channel = RMT_TX_CHANNEL;
    rmt_tx.gpio_num = RMT_TX_GPIO_NUM;
    rmt_tx.mem_block_num = 1;
    rmt_tx.clk_div = RMT_CLK_DIV;
    rmt_tx.tx_config.loop_en = false;
    rmt_tx.tx_config.carrier_duty_percent = 50;
    // Carrier Frequency of the IR receiver
    rmt_tx.tx_config.carrier_freq_hz = 38000;
    rmt_tx.tx_config.carrier_level = 1;
    rmt_tx.tx_config.carrier_en = 1;
    // Never idle -> aka ontinuous TX of 38kHz pulses
    rmt_tx.tx_config.idle_level = 1;
    rmt_tx.tx_config.idle_output_en = true;
    rmt_tx.rmt_mode = 0;
    rmt_config(&rmt_tx);
    rmt_driver_install(rmt_tx.channel, 0, 0);
}

// Configure UART
static void uart_init()
{
    // Basic configs
    uart_config_t uart_config = {
        .baud_rate = 1200, // Slow BAUD rate
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    uart_param_config(UART_NUM_1, &uart_config);

    // Set UART pins using UART0 default pins
    uart_set_pin(UART_NUM_1, UART_TX_GPIO_NUM, UART_RX_GPIO_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Reverse receive logic line
    uart_set_line_inverse(UART_NUM_1, UART_SIGNAL_RXD_INV); // ****** changed 2nd arg from UART_INVERSE_RXD

    // Install UART driver
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);
}

// GPIO init for LEDs
static void led_init()
{
    gpio_pad_select_gpio(BLUEPIN);
    gpio_pad_select_gpio(GREENPIN);
    gpio_pad_select_gpio(REDPIN);
    gpio_pad_select_gpio(ONBOARD);
    gpio_set_direction(BLUEPIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(GREENPIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(REDPIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ONBOARD, GPIO_MODE_OUTPUT);
}

// Button 1 interrupt init
static void button_init()
{
    gpio_config_t io_conf;
    //interrupt of rising edge
    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
    gpio_intr_enable(GPIO_INPUT_IO_1);
    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void *)GPIO_INPUT_IO_1);
    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task

    // other button init
    gpio_pad_select_gpio(BUTTON_GPIO);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
}

////////////////////////////////////////////////////////////////////////////////

// Tasks ///////////////////////////////////////////////////////////////////////
// Button task -- rotate through myIDs
void button_task()
{
    uint32_t io_num;
    while (1)
    {

        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            xSemaphoreTake(mux, portMAX_DELAY);
            if (myID == 3)
            {
                myID = 1;
            }
            else
            {
                myID++;
            }

            if (myColor == 'R')
            {
                myColor = 'G';
            }
            else if (myColor == 'G')
            {
                myColor = 'Y';
            }
            else if (myColor == 'Y')
            {
                myColor = 'R';
            }
            xSemaphoreGive(mux);
            printf("Button pressed. LED color: %c\n", myColor);
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// Button 2 task -- send data through IR
void button_2_task()
{
    int button_flag;
    while (1)
    {
        button_flag = gpio_get_level(BUTTON_GPIO);
        if (!button_flag)
        {
            printf("button 2 pressed\n");
            sendFlag = 1;
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        sendFlag = 0;
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// IR Send and Receive task
void recv_task()
{
    // Buffer for input data
    uint8_t *data_in = (uint8_t *)malloc(BUF_SIZE);
    while (1)
    {
        if (sendFlag) // send data when button 2 is pushed
        {
            char *data_out = (char *)malloc(len_out);
            xSemaphoreTake(mux, portMAX_DELAY);
            data_out[0] = start;
            data_out[1] = (char)myColor;
            data_out[2] = (char)myID;
            data_out[3] = genCheckSum(data_out, len_out - 1);

            // ESP_LOG_BUFFER_HEXDUMP(TAG_SYSTEM, data_out, len_out, ESP_LOG_INFO);

            for (int i = 0; i < 5; i++)
            {
                uart_write_bytes(UART_NUM_1, data_out, len_out);
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            sendFlag = 0;
            printf("data sucessfully sent\n");
            xSemaphoreGive(mux);
        }
        else // recieve data if not sending data
        {
            int len_in = uart_read_bytes(UART_NUM_1, data_in, BUF_SIZE, 20 / portTICK_RATE_MS);
            if (len_in > 0)
            {
                if (data_in[0] == start)
                {
                    if (checkCheckSum(data_in, len_out))
                    {

                        ESP_LOG_BUFFER_HEXDUMP(TAG_SYSTEM, data_in, len_out, ESP_LOG_INFO);
                        // change led data for current esp
                        myColor = (char)data_in[1];
                        myID = (char)data_in[2];
                        printf("data successfully recieved!\n");
                    }
                }
            }
            else
            {
                // printf("Nothing received.\n");
            }
        }
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
    free(data_in);
}

// LED task to light LED
void led_task()
{
    while (1)
    {
        switch ((int)myColor)
        {
        case 'R': // Red
            gpio_set_level(GREENPIN, 0);
            gpio_set_level(REDPIN, 1);
            gpio_set_level(BLUEPIN, 0);
            // printf("Current state: %c\n",status);
            break;
        case 'Y': // Yellow            (actually just blue)
            gpio_set_level(GREENPIN, 0);
            gpio_set_level(REDPIN, 0);
            gpio_set_level(BLUEPIN, 1);
            // printf("Current state: %c\n",status);
            break;
        case 'G': // Green
            gpio_set_level(GREENPIN, 1);
            gpio_set_level(REDPIN, 0);
            gpio_set_level(BLUEPIN, 0);
            // printf("Current state: %c\n",status);
            break;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// LED task to blink onboard LED based on ID
void id_task()
{
    while (1)
    {
        for (int i = 0; i < (int)myID; i++)
        {
            gpio_set_level(ONBOARD, 1);
            vTaskDelay(200 / portTICK_PERIOD_MS);
            gpio_set_level(ONBOARD, 0);
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

////////////////////////////////////////////////////////////////////////////////
//                             MAIN                                           //
////////////////////////////////////////////////////////////////////////////////

void app_main()
{
    // Mutex for current values when sending
    mux = xSemaphoreCreateMutex();

    // start in initial state
    int state = S0;

    // initalize the wifi
    initalizeWiFi();

    // Initialize all the things
    rmt_tx_init();
    uart_init();
    led_init();
    button_init();

    //udp client start
    xTaskCreate(udp_client_task, "udp_client", 4096, NULL, 5, NULL);

    // Create tasks for receive, send, set gpio, and button
    xTaskCreate(recv_task, "uart_rx_task", 1024 * 4, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(led_task, "set_traffic_task", 1024 * 2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(id_task, "set_id_task", 1024 * 2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(button_task, "button_task", 1024 * 2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(button_2_task, "button_2_task", 1024 * 2, NULL, configMAX_PRIORITIES, NULL);

    while (1)
    {
        // do FSM stuff here
    }
}