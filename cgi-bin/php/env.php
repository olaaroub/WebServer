#!/usr/bin/env php
<?php

header("Content-Type: text/html");

echo "<!DOCTYPE html>
<html lang='en'>
<head>
  <meta charset='UTF-8'>
  <title>🚀 CGI Test</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background: linear-gradient(135deg, #1f4037, #99f2c8);
      color: #222;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      margin: 0;
    }
    .container {
      background: rgba(255, 255, 255, 0.9);
      padding: 2rem 3rem;
      border-radius: 15px;
      box-shadow: 0 8px 20px rgba(0,0,0,0.3);
      text-align: center;
    }
    h1 {
      margin-bottom: 1rem;
      font-size: 2rem;
    }
    pre {
      text-align: left;
      background: #f4f4f4;
      padding: 1rem;
      border-radius: 10px;
      overflow-x: auto;
      font-size: 0.9rem;
    }
  </style>
</head>
<body>
  <div class='container'>
    <h1>✅ CGI is Working!</h1>
    <p>Current Server Time: <strong>" . date("Y-m-d H:i:s") . "</strong></p>
    <h2>Environment Variables</h2>
    <pre>" . htmlspecialchars(print_r($_SERVER, true)) . "</pre>
  </div>
</body>
</html>";
?>
