<?php

// The header() function is the standard way to send HTTP headers in PHP.
// It works reliably with the php-cgi interpreter.

// This tells the php-cgi interpreter to set the status line correctly.
header("Status: 302");

// Set the Location header for the redirect.
header("Location: https://www.google.com/search?q=webserv+redirect+test");

header("Content-Type: text/html");

// It's good practice to call exit() after a redirect to ensure
// no other output is sent.
exit();

?>
