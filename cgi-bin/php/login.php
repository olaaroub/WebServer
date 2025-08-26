<?php
$correct_password = "42";

// --- 0. Check for a logout request first ---
if (isset($_GET['action']) && $_GET['action'] === 'logout') {
    header("X-Session-Action: DELETE");
    header('Content-Type: text/html');
    echo "<!DOCTYPE html><html><head><title>Logged Out</title>
    <style>
        body { display:flex; justify-content:center; align-items:center; height:100vh; font-family:Arial, sans-serif; background:#111; color:#eee; }
        .box { text-align:center; padding:2rem; border-radius:12px; background:#1e1e1e; box-shadow:0 4px 12px rgba(0,0,0,0.6); }
        a { color:#4cafef; text-decoration:none; } a:hover { text-decoration:underline; }
    </style>
    </head><body><div class='box'><p>You have been logged out.<br><a href='login.php'>Login again</a>.</p></div></body></html>";
    exit();
}

// --- 1. Check the verification status from the C++ server ---
if (isset($_SERVER['HTTP_X_SESSION_STATUS']) && $_SERVER['HTTP_X_SESSION_STATUS'] === 'VALID') {
    header('Content-Type: text/html');
    echo "<!DOCTYPE html><html><head><title>Already Logged In</title>
    <style>
        body { display:flex; justify-content:center; align-items:center; height:100vh; font-family:Arial, sans-serif; background:#111; color:#eee; }
        .box { text-align:center; padding:2rem; border-radius:12px; background:#1e1e1e; box-shadow:0 4px 12px rgba(0,0,0,0.6); }
        a { color:#4cafef; text-decoration:none; } a:hover { text-decoration:underline; }
    </style>
    </head><body><div class='box'>
    <h1>Welcome back!</h1>
    <p>Your session is valid.</p>
    <p><a href='/secret/'>Go to the secret area</a> or <a href='login.php?action=logout'>Log Out</a></p>
    </div></body></html>";
    exit();
}

// --- 2. Handle a new login attempt ---
$login_error_message = '';
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = isset($_POST['username']) ? htmlspecialchars($_POST['username']) : 'guest';
    $password = isset($_POST['password']) ? $_POST['password'] : '';

    if ($password === $correct_password) {
        // SUCCESS: Tell the C++ server to create the session.
        header("X-Session-Action: CREATE;user=" . $username);
        header('Content-Type: text/html');

        echo "<!DOCTYPE html><html lang='en'><head><title>Login Successful</title>
        <style>
            body { display:flex; justify-content:center; align-items:center; height:100vh; font-family:Arial, sans-serif; background:#111; color:#eee; }
            .box { text-align:center; padding:2rem; border-radius:12px; background:#1e1e1e; box-shadow:0 4px 12px rgba(0,0,0,0.6); }
            a { color:#4cafef; text-decoration:none; } a:hover { text-decoration:underline; }
        </style>
        </head><body><div class='box'>";
        echo "<h1>Welcome, " . $username . "!</h1>";
        echo "<p>You are now logged in.</p>";
        echo "<p><a href='/secret/'>Proceed to the secret area</a> or <a href='login.php?action=logout'>Log Out</a></p>";
        echo "</div></body></html>";
        exit();
    } else {
        $login_error_message = "<p style='color:red;'>Incorrect password. Please try again.</p>";
    }
}

// --- 3. Show login form if no session and no successful login ---
header('Content-Type: text/html');
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>1337 Login Portal</title>
    <style>
        body { display:flex; justify-content:center; align-items:center; height:100vh; background:#111; font-family:Arial, sans-serif; color:#eee; }
        .login-box { background:#1e1e1e; padding:2rem; border-radius:12px; box-shadow:0 4px 12px rgba(0,0,0,0.6); text-align:center; width:300px; }
        input[type=text], input[type=password] { width:90%; padding:0.5rem; margin:0.5rem 0; border:1px solid #333; border-radius:6px; background:#2a2a2a; color:#eee; }
        input[type=submit] { background:#4cafef; color:#fff; border:none; padding:0.6rem 1.2rem; border-radius:8px; cursor:pointer; margin-top:1rem; }
        input[type=submit]:hover { background:#3a9bdc; }
        h2 { margin-bottom:1rem; }
    </style>
</head>
<body>
    <div class="login-box">
        <h2>Enter the Matrix</h2>
        <?php echo $login_error_message; ?>
        <form action="login.php" method="POST">
            <p><label for="username">Username:</label><br><input type="text" id="username" name="username" required></p>
            <p><label for="password">Password:</label><br><input type="password" id="password" name="password" required></p>
            <p><input type="submit" value="Login"></p>
        </form>
    </div>
</body>
</html>
