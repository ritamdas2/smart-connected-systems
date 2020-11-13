# Quest 4: E-Voting V2

Authors: Raghurama Bukkarayasamudram, Ritam Das, Brian Macomber

## Date: 2020-11-13

## Summary

## Self-Assessment

### Objective Criteria

| Objective Criterion | Rating | Max Value |
| ------------------- | :----: | :-------: |
| Objective One       |        |     1     |
| Objective Two       |        |     1     |
| Objective Three     |        |     1     |
| Objective Four      |        |     1     |
| Objective Five      |        |     1     |
| Objective Six       |        |     1     |
| Objective Seven     |        |     1     |

### Qualitative Criteria

| Qualitative Criterion                          | Rating | Max Value |
| ---------------------------------------------- | :----: | :-------: |
| Quality of solution                            |        |     5     |
| Quality of report.md including use of graphics |        |     3     |
| Quality of code reporting                      |        |     3     |
| Quality of video presentation                  |        |     3     |

## Solution Design

#### Investigative Question: List 5 different ways that you can hack the system (including influencing the vote outcome or preventing votes via denial of service). Explain how you would mitigate these issues in your system.

There are three main areas of our system that present vulnerabilities: the receiver/transmitter communication, the UDP socket communication, and the web client. 

Anyone can create their own receiver/transmitter and cast a false vote and send it to the poll leader. This is the security threat we face when using RX/TX communication. This risk could be mitigated by having the leader confirm the identity of the transmitter it receives votes from, thus only authorized users may cast a vote.  

The communication between the ESP32 and the node server uses UDP, which also presents a security vulnerability. This no acknowledgement protocol is highly susceptible to spoofing and DOS attacks. Thus, anyone can send large volumes of packets to the server, potentially preventing allowing authorized users to cast votes or view data. To mititgate this, a section of code in server.js could be implemented such that all incoming packets to the server are monitored and limited at a certain threshold (packets per second, etc.), temporarily denying further incoming packets until below the threshold again.

UDP communication is also vulnerable to packet sniffing. An unauthorized user could intercept packets that are sent over UDP from the ESP32 to the node server and decipher the data within those packets (Wireshark). A potential solution to this is to use TCP communication, which is a safer handshaking protocol. TCP first confirms that the recipient is valid for data transmission and sends data following this acknowledgement. However, TCP would increase latency when communicating to the node server as UDP is a much faster protocol with a security tradeoff. 

An unauthorized user also has the potential to log into the Pi directly and read into the database and server code. This could result in tampered data and unauthorized admin access. To prevent this, the wireless network the Pi is connected to should be setup for the WPA2 security mode, which is a much stronger security mode than WEP (currently being used & can be brute forced easily).  

An unauthorized user could even steal the microSD card on the Pi and decode the information on there through another computer. We could mitigate the risk here by encrypting the microSD to add an extra layer of security and lock the information held on the SD card in the case it is stolen,


## Sketches and Photos
![Screen Shot 2020-11-13 at 3 02 03 PM](https://user-images.githubusercontent.com/37518854/99116278-c04b2c80-25c1-11eb-87f1-6f6455189958.png)

<center><img src="./images/ece444.png" width="25%" /></center>  
<center> </center>

## Supporting Artifacts

- [Link to video demo](). Not to exceed 120s

## Modules, Tools, Source Used Including Attribution

## References

---
