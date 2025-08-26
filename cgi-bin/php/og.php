<?php
// The one true password, combining 1337 and 42.
$correct_password = "42";

// --- 0. Check for a logout request first ---
if (isset($_GET['action']) && $_GET['action'] === 'logout') {
    header("X-Session-Action: DELETE");
    header('Content-Type: text/html');
    echo "<!DOCTYPE html><html><head><title>Logged Out</title></head><body><p>You have been logged out. <a href='login.php'>Login again</a>.</p></body></html>";
    exit();
}

// --- 1. Handle a new login attempt ---
$login_error_message = '';
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = isset($_POST['username']) ? htmlspecialchars($_POST['username']) : 'guest';
    $password = isset($_POST['password']) ? $_POST['password'] : '';

    if ($password === $correct_password) {
        // SUCCESS: Tell the C++ server to create the session.
        header("X-Session-Action: CREATE;user=" . $username);
        header('Content-Type: text/html');

        // Display the success page and link to the protected area.
        echo "<!DOCTYPE html><html lang='en'><head><title>Login Successful</title></head><body>";
        echo "<h1>Welcome, " . $username . "!</h1>";
        echo "<p>You are now logged in.</p>";
        echo "<p><a href='/secret/'>Proceed to the secret area</a> or <a href='login.php?action=logout'>Log Out</a></p>";
        echo "</body></html>";
        exit();
    } else {
        // FAILURE: The password was incorrect.
        $login_error_message = "<p style='color:red;'>Incorrect password. Please try again.</p>";
    }
}

// --- 2. Display the login form ---
// This is the default view.
header('Content-Type: text/html');
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>1337 Login Portal</title>
</head>
<body>
    <h2>Enter the Matrix</h2>
    <?php echo $login_error_message; ?>
    <form action="login.php" method="POST">
        <p><label for="username">Username:</label><br><input type="text" id="username" name="username" required></p>
        <p><label for="password">Password:</label><br><input type="password" id="password" name="password" required></p>
        <p><input type="submit" value="Login"></p>
    </form>
</body>
</html>
