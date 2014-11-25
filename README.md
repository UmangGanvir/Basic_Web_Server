Only **Server.cpp** is required to run the server. Place its executable in a directory that you wish to make the document root.<br/>
The port chosen in the code is **1111**.

Usage
-----
1. Compile and form the executable by this command on the terminal : <br/>
`g++ Server.cpp -o Server`
2. Run the executable formed by this command : <br/>
`./Server`
3. To surf your web server, open up your web browser and type the url : <br/>
`127.0.0.1:1111`


Features
--------
1. Handles GET and POST requests
2. Login validation
3. Handles requests in child process
4. Supports php
