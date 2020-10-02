<!DOCTYPE html>
<html>
    <head>
        <title>My Webpage!</title>
        <style>
            p {
                text-align: center;
                width: 100%;
            }
            table, th, td {
                border: 1px solid black;
            }
            table {
                width: 80%;
                margin: 0 auto;
            }
            div {
                width: 300px;
                max-width: 90%;
                margin: 0 auto;
            }
        </style>
    </head>
    <body>
<?php
    require '../login.php';


    ini_set(’session.gc_maxlifetime’,10);
    ini_set(‘session.gc_probability’,1);
    ini_set(‘session.gc_divisor’,1);
    session_set_cookie_params(10);
    session_start();
    //print_r($_SESSION);
    $conn = mysqli_connect($servername, $username, $password, $database);

    if (mysqli_connect_errno())
        die("Failed to connect to MySQL: " . mysqli_connect_error());

    $results = mysqli_query($conn, "SHOW TABLES LIKE 'users'");

    if (mysqli_num_rows($results) == 0) {

        $query = "CREATE TABLE users (
            username VARCHAR(200) PRIMARY KEY,
            password VARCHAR(200)
            )";

        if (!mysqli_query($conn, $query))
            die("Could not create table!");

        $query = "INSERT INTO users (username,password)
                    VALUES ('user','".password_hash('pass',PASSWORD_BCRYPT)."');";

        $results = mysqli_multi_query($conn, $query);

        if (!$results)
            die("Could not insert data!");
    
        mysqli_close($conn);
        $conn = mysqli_connect($servername, $username, $password, $database);
    }


    if ((isset($_SESSION['REMOTE_ADDR']) && $_SESSION['hostname'] = $_SERVER['REMOTE_ADDR']) ||
        (isset($_SESSION['time']) && $_SESSION['time'] + 10 < time()) ||
        ($_SERVER['REQUEST_METHOD'] == "POST" && isset($_POST['logout']))) {
        session_unset();
        session_destroy(); 
    } else if ($_SERVER['REQUEST_METHOD'] == "POST") {

        $query = "SELECT * FROM users";
        $query .= " WHERE username='".$_POST['username']."'";

        $results = mysqli_query($conn, $query);

        if (!$results) {
            die("Could not query ".mysqli_error($conn));
        }

        if (mysqli_num_rows($results) > 0) {
            $row = mysqli_fetch_assoc($results);

            if (password_verify($_POST['password'], $row['password'])) {
                # Start Session
                $_SESSION['username'] = $_POST['username'];
                $_SESSION['hostname'] = $_SERVER['REMOTE_ADDR'];
                $_SESSION['time'] = time();
            } else {
                echo "<p>Invalid Login</p>";
            }
        } else {
            echo "<p>Invalid Login</p>";
        }
    }

    if (isset($_SESSION['username'])) {
        # Logged in
        $query = "SELECT * FROM users;";

        $results = mysqli_query($conn, $query);

        if (!$results) {
            die("Could not query ".mysqli_error($conn));
        }

        echo "<form method='POST'><input type='hidden' name='logout' value='plz'><input type='submit' value='Logout'></form>";
        echo "<center><h1>Users</h1></center>";
        echo "<table><tr><th>Username</th><th>Password</th></tr>";
        while ($row = mysqli_fetch_assoc($results)) {
            echo "<tr><td>".$row["username"]."</td>";
            echo "<td>".$row["password"]."</td></tr>";
        }
        echo "</table>";

        echo "<h1>Souce Code</h1>";
        highlight_file("./login.php");
    } else {
        # not logged in 

        echo "   <div><h1>Please Login</h1>";
        echo "   <form method='POST'>";
        echo "       <label>Username</label>";
        echo "       </br>";
        echo "       <input type='text' name='username'>";
        echo "       </br>";
        echo "       </br>";
        echo "       <label>Password</label>";
        echo "       </br>";
        echo "       <input type='password' name='password'>";
        echo "       </br>";
        echo "       </br>";
        echo "       <input type='submit' value='Login'>";
        echo "   </form></div>";
    }

    mysqli_close($conn);
?>
    </body>
</html>

