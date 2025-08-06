<?php

// This script tests a server's ability to handle a CGI script
// that sends partial content and then hangs.

// 1. Print valid headers and some initial content.
echo "Content-Type: text/html\r\n";
echo "\r\n";
echo "<html><body><p>This content was sent by the script before it started hanging...</p>";

// 2. Flush the output buffer.
// This is a crucial step. It forces PHP to send the data above
// immediately to the server through the pipe, rather than waiting for the
// script to finish (which it never will).
flush();
ob_flush();
// exit(3);

// 3. Enter an infinite loop to simulate the script hanging.
while (true) {
    sleep(30);
    exit(10); // sleep(1) is used to prevent this script from using 100% CPU.
}

// The following lines will never be reached.
echo "<p>You should never see this message.</p></body></html>";

?>