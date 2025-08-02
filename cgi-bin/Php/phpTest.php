<?php
// The first line of output MUST be the Content-Type header.
// Using the header() function is the standard way to do this in PHP.
header("Content-Type: text/html");
?>
<!DOCTYPE html>
<html>
<head>
    <title>Simple PHP CGI Test</title>
</head>
<body>
    <h1>Hello from PHP CGI!</h1>
    <p>If you can see this message, your webserver's PHP CGI execution is working correctly.</p>
    <p>Current PHP Version: <?php echo phpversion(); ?></p>
</body>
</html>

