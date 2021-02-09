<!DOCTYPE html>
<html>
    <head>
        <title>My Webpage!</title>
        <style>
            p {
                width: 100%;
            }
            table, th, td {
                border: 1px solid black;
            }
            table {
                width: 80%;
                margin: 0 auto;
            }
        </style>
    </head>
    <body>

    <h1>Items</h1>

    <center>Add an item</center>
        <form method="POST">
            <label>Name</label>
            <input type="text" name="name">
            </br>
            <label>Cost</label>
            <input type="text" name="cost">
            </br>
            <label>Seller</label>
            <input type="text" name="seller">
            </br>
            <label>Ram</label>
            <input type="text" name="ram">
            </br>
            <label>RGB</label>
            <input type="text" name="rgb">
            </br>
            <label>Cameras</label>
            <input type="text" name="cameras">
            </br>
            <input type="submit" value="Add">
        </form>
<?php
    require '../login.php';

    $conn = mysqli_connect($servername, $username, $password, $database);

    if (mysqli_connect_errno())
        die("Failed to connect to MySQL: " . mysqli_connect_error());

    $results = mysqli_query($conn, "SHOW TABLES LIKE 'items'");

    if (mysqli_num_rows($results) == 0) {

        $query = "CREATE TABLE items(
            name VARCHAR(200) PRIMARY KEY,
            cost INT,
            seller VARCHAR(200),
            ram DOUBLE,
            rgb BOOLEAN,
            cameras INT
            )";

        if (!mysqli_query($conn, $query))
            die("Could not create table!");

        $query = "INSERT INTO items (name, cost, seller, ram, cameras)
                    VALUES ('IPhone', 10000, 'Apple', 2, 5.8);
                  INSERT INTO items (name, cost, seller, rgb, ram)
                    VALUES ('HP 1234', 93400, 'HP', True, 15.0);
                  INSERT INTO items (name, cost, seller, rgb, ram)
                    VALUES ('Dell 2400', 86000, 'Dell', True, 10.0);
                  INSERT INTO items (name, cost, seller, rgb, ram)
                    VALUES ('HP 4321', 93400, 'HP', False, 15.0);
                  INSERT INTO items (name, cost, seller, ram, cameras)
                    VALUES ('Samsung S7', -100, 'Samsung', 3, 9.8);
                  INSERT INTO items (name, cost, seller, ram, cameras)
                    VALUES ('IPhone 7', 70300, 'Apple', 1, 5.9);
                  INSERT INTO items (name, cost, seller, rgb, ram)
                    VALUES ('HP 3333', 33221, 'HP', False, 18.0);
                  INSERT INTO items (name, cost, seller, rgb, ram)
                    VALUES ('Dell 2401x', 19030, 'Dell', True, 11.0);
                  INSERT INTO items (name, cost, seller, rgb, ram)
                    VALUES ('HP 4322', 93400, 'HP', False, 12.0);
                  INSERT INTO items (name, cost, seller, ram, cameras)
                    VALUES ('Samsung S2', 100, 'Samsung', 1, 1.4)";

        $results = mysqli_multi_query($conn, $query);

        if (!$results)
            die("Could not insert data!");
    
        mysqli_close($conn);
        $conn = mysqli_connect($servername, $username, $password, $database);
    }

    if (isset($_GET['delete'])) {
        $query = "DELETE FROM items";
        $query .= " WHERE name='".$_GET['delete']."'";

        $results = mysqli_query($conn, $query);

        if (!$results) {
            die("Could not query ".mysqli_error($conn));
        }
    }
    if (isset($_GET['edit'])) {


       echo "<center>Update ".$_GET['edit']."</center>";
       echo "<form method='POST'>";
       echo "<label>Name</label>";
       echo "<input type='text' name='name' value='".$_GET['edit']."'>";
       echo "</br>";
       echo "<label>Cost</label>";
       echo "<input type='text' name='cost'>";
       echo "</br>";
       echo "<label>Seller</label>";
       echo "<input type='text' name='seller'>";
       echo "</br>";
       echo "<label>Ram</label>";
       echo "<input type='text' name='ram'>";
       echo "</br>";
       echo "<label>RGB</label>";
       echo "<input type='text' name='rgb'>";
       echo "</br>";
       echo "<label>Cameras</label>";
       echo "<input type='text' name='cameras'>";
       echo "</br><input type='hidden' name='update' value='true'>";
       echo "<input type='submit' value='Update'>";
       echo "</form>";

    }

    if ($_SERVER['REQUEST_METHOD'] == "POST" && isset($_POST['update'])) {

        $query = "UPDATE items";
        $query .= " SET cost='".$_POST['cost']."',seller='".$_POST['seller']."',ram='".$_POST['ram']."',rgb='".$_POST['rgb']."',cameras='".$_POST['cameras']."'";
        $query .= " WHERE name='".$_POST['name']."'";

        $results = mysqli_query($conn, $query);

        if (!$results) {
            die("Could not query ".mysqli_error($conn));
        }
    }

    if ($_SERVER['REQUEST_METHOD'] == "POST" && !isset($_POST['update'])) {
        $query = "INSERT INTO items (name, cost, seller, ram, rgb, cameras) VALUES ";
        $query .= "('".$_POST['name']."','".$_POST['cost']."','".$_POST['seller']."','".$_POST['ram']."','".$_POST['rgb']."','".$_POST['cameras']."')";

        $results = mysqli_query($conn, $query);

        if (!$results) {
            die("Could not query ".mysqli_error($conn));
        }
    }


    $query = "SELECT * FROM items";
    $results = mysqli_query($conn, $query);

    if (!$results) {
        die("Could not query ".mysqli_error($conn));
    }

    while ($row = mysqli_fetch_assoc($results)) {
        echo "<h3>Name: ".$row["name"]."</h3>";
        echo "<p>Cost: ".$row["cost"]."</p>";
        echo "<p>Seller: ".$row["seller"]."</p>";
        echo "<p>Ram: ".$row["ram"]."</p>";
        echo "<p>Cameras: ".$row["cameras"]."</p>";
        echo "<p>RGB: ".$row["rgb"]."</p>";
        echo "<form><input type='hidden' name='edit' value='".$row["name"]."'>";
        echo "<input type='submit' value='Update'></form>";

        echo "<form><input type='hidden' name='delete' value='".$row["name"]."'>";
        echo "<input type='submit' value='Delete'></form>";
    }

    mysqli_close($conn);
?>

        <h1>Souce Code</h1>
        <?php
            highlight_file("./dashboard.php");
        ?>
    </body>
</html>

