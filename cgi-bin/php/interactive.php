<?php
header("Content-Type: text/html");


$name = isset($_GET['name']) ? htmlspecialchars($_GET['name']) : '';
?>
<!DOCTYPE html>
<html>
<head>
    <title>Simple Interactive PHP Script</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f0f2f5;
        }
        .container {
            text-align: center;
            background-color: white;
            padding: 40px;
            border-radius: 10px;
            box-shadow: 0 4px 12px rgba(0,0,0,0.1);
        }
        input[type="text"] {
            padding: 10px;
            font-size: 16px;
            border: 1px solid #ccc;
            border-radius: 5px;
        }
        input[type="submit"] {
            padding: 10px 20px;
            font-size: 16px;
            border: none;
            border-radius: 5px;
            background-color: #007bff;
            color: white;
            cursor: pointer;
        }
    </style>
</head>
<body>
    <div class="container">
        <?php if (!empty($name)): ?>
            <!-- If a name was provided, display a greeting -->
            <h1>Hello, <?php echo $name; ?>!</h1>
            <p><a href="<?php echo htmlspecialchars($_SERVER['SCRIPT_NAME']); ?>">Try again with a different name.</a></p>
        <?php else: ?>
            <!-- If no name was provided, display the form -->
            <h1>What is your name?</h1>
            <form action="<?php echo htmlspecialchars($_SERVER['SCRIPT_NAME']); ?>" method="GET">
                <input type="text" name="name" placeholder="Enter your name">
                <input type="submit" value="Submit">
            </form>
        <?php endif; ?>
    </div>
</body>
</html>
