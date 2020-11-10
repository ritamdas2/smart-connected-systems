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