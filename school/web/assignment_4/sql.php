<!DOCTYPE html>
<html>
    <head>
        <title>My Webpage!</title>
        <style>
            p {
                width: 100%;
                text-align: center;
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
    <table>
        <tr>
            <th>Name</th>
            <th>Cost</th>
            <th>Seller</th>
            <th>Ram</th>
            <th>Camera</th>
            <th>RGB</th>
        </tr>
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

    $query = "SELECT * FROM items";
    $results = mysqli_query($conn, $query);

    if (!$results) {
        die("Could not query ".mysqli_error($conn));
    }

    while ($row = mysqli_fetch_assoc($results)) {
        echo "<tr><td>".$row["name"]."</td>";
        echo "<td>".$row["cost"]."</td>";
        echo "<td>".$row["seller"]."</td>";
        echo "<td>".$row["ram"]."</td>";
        echo "<td>".$row["cameras"]."</td>";
        echo "<td>".$row["rgb"]."</td></tr>";
    }

    mysqli_close($conn);
?>
    </table>

        <h1>Souce Code</h1>
        <?php
            highlight_file("./sql.php");
        ?>
    </body>
</html>

