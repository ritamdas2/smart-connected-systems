# Code Readme

### Brian: fob_code.c

### Ritam: server.js

The server.js file was responsible for the following: creating the webclient, sending and receiving messages from the webclient using socket.io, creating the database, and adding entries (votes) to the database once they were received from the leader ESP. The server also emitted a message to the frontend containing the desired queries.

### Ram: index.html

The frontend aspect of this Quest revolved around getting the proper queries from the server to be displayed on the client. We set up the format of the html file to have two tables (one for candidate and total votes, one for the entire database), and a reset button.
We used socket.io with our levelDB queries in order to retrieve the data properly. The format of the message being sent from the server would be given in a specific array which we would have to properly index into. We created different variables to hold different values from the respective array index. This allowed us to see when a message was being sent over- or in this case, when a vote was being cast.
The biggest problems we faced was displaying the javascript variables from inside the socket.io portions and inside functions. HTML had a couple of ways to do it such as "Document.write" and through .innerHTML. Displaying these values in our tables was a problem since it would not register the actual values. Usually, we got an error such as undefined value or a blank segment. We solved this issue by:
