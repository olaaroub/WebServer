

 <?php
// --- 0. Check for a logout request first ---
if (isset($_GET['action']) && $_GET['action'] === 'logout') {
    // This custom header is the instruction for your C++ server to delete the session
    header("X-Session-Action: DELETE");
    header('Content-Type: text/html');
    echo "<!DOCTYPE html><html><head><title>Logged Out</title></head>";
    echo "<body><h1>You have been successfully logged out.</h1>";
    echo "<p><a href='login.php'>Log in again</a></p></body></html>";
    exit(); // Stop the script here
}


// --- 1. Check for an existing session ---
// This part handles users who are already logged in.
if (isset($_COOKIE['sessionid'])) {
    header('Content-Type: text/html');
    echo "<h1>You are already logged in.</h1>";
    echo "<p>Your session ID is: " . htmlspecialchars($_COOKIE['sessionid']) . "</p>";
    echo '<p><a href="/secret/veryPrivateFile.html">Proceed to the secret area.</a></p>';
    // Add the logout link
    echo '<p><a href="login.php?action=logout">Log Out</a></p>';
    exit(); // Stop the script from running further.
}

// --- 2. The Login Logic ---
// This part handles new visitors or failed login attempts.

// Default state is to show the login form
$show_login_form = true;
$login_message = '';

// Check if the form was submitted via POST
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = isset($_POST['username']) ? $_POST['username'] : '';
    $password = isset($_POST['password']) ? $_POST['password'] : '';

    // Simple hardcoded check for demonstration
    if ($username === 'admin' && $password === '123') {
        // This custom header is the instruction for your C++ server to create a session
        header("X-Session-Action: CREATE;user=admin");

        $show_login_form = false;
        $login_message = "<h1>Login Successful!</h1><p>Welcome, admin. You now have a session.</p>";
    } else {
        $login_message = "<p style='color:red;'>Invalid credentials. Please try again.</p>";
    }
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <title>Login Page</title>
</head>
<body>

    <?php if ($show_login_form): ?>
        <h2>Please Log In</h2>
        <?php echo $login_message; ?>
        <form action="login.php" method="POST">
            <label for="username">Username:</label>
            <input type="text" id="username" name="username" value="admin">
            <br><br>
            <label for="password">Password:</label>
            <input type="password" id="password" name="password" value="123">
            <br><br>
            <input type="submit" value="Log In">
        </form>
    <?php else: ?>
        <?php echo $login_message; ?>
        <p><a href="/secret/veryPrivateFile.html">Go to a protected page</a></p>
    <?php endif; ?>

</body>
</html>
