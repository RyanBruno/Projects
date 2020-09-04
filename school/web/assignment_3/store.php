
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

    <p>All Items</p>
    <table>
        <tr>
            <th>Name</th>
            <th>Cost</th>
            <th>Seller</th>
        </tr>
    <?php
        /* This begins the OOP Clases */
        class Item { // The base class
            public $name;
            public $cost;
            public $seller;

            function __construct($name, $cost, $seller) {
                $this->set_name($name);
                $this->set_cost($cost);
                $this->set_seller($seller);
            }

            /* Setters */
            function set_name($name) {
                if (gettype($name) != "string") // Type Checking
                    echo "name should be a string";

                $this->name = $name;
            }

            function set_cost($cost) {
                if (gettype($cost) != "integer") // Type Checking
                    echo "cost should be a integer";

                $this->cost = $cost;
            }

            function set_seller($seller) {
                if (gettype($seller) != "string") // Type Checking
                    echo "name should be a string";

                $this->seller = $seller;
            }

            /* Getters */
            public function get_name() {
                return $this->name;
            }

            public function get_cost() {
                return $this->cost;
            }

            public function get_seller() {
                return $this->seller;
            }
        }

        class Computer extends Item { // First subclass
            public $ram;
            public $rgb;

            function __construct($name, $cost, $seller, $rgb, $ram) {
                $this->set_name($name);
                $this->set_cost($cost);
                $this->set_seller($seller);
                $this->set_ram($ram);
                $this->set_rgb($rgb);
            }

            /* Setters */
            function set_ram($ram) {
                if (gettype($ram) != "double") // Type checking
                    echo "ram should be a double";

                $this->ram = $ram;
            }

            function set_rgb($rgb) {
                if (gettype($rgb) != "boolean") // Type checking
                    echo "rgb should be a boolean";

                $this->rgb = $rgb;
            }

            /* Getters */
            public function get_ram() {
                return $this->ram;
            }

            public function get_rgb() {
                return $this->rgb;
            }
        }

        class Phone extends Item { // Second subclass
            public $cameras;
            public $ram;

            function __construct($name, $cost, $seller, $cameras, $ram) {
                $this->set_name($name);
                $this->set_cost($cost);
                $this->set_seller($seller);
                $this->set_cameras($cameras);
                $this->set_ram($ram);
            }

            /* Getters */
            function set_cameras($cameras) {
                if (gettype($cameras) != "integer") // Type checking
                    echo "cameras should be an integer";

                $this->cameras = $cameras;
            }

            function set_ram($ram) {
                if (gettype($ram) != "double") // Type checking
                    echo "ram should be a double";

                $this->ram = $ram;
            }

            /* Getters */
            public function get_ram() {
                return $this->name;
            }

            public function get_cameras() {
                return $this->cameras;
            }
        }

        $items = array( // Construction of the 2d array
            array(new Phone("IPhone", 10000, "Apple", 2, 5.8),
                  new Computer("HP 1234", 93400, "HP", True, 15.0)),
            array(new Computer("Dell 2400", 86000, "Dell", True, 10.0),
                  new Computer("HP 1234", 93400, "HP", False, 15.0),
                  new Phone("Samsung S7", -100, "Samsung", 3, 9.8))
        );

        foreach ($items as $inner) { // Diplay the first full table
            foreach ($inner as $i) {
                echo "<tr>";
                echo "<td>".$i->get_name()."</td>";
                echo "<td>".$i->get_cost()."</td>";
                echo "<td>".$i->get_seller()."</td>";
                echo "</tr>";
            }
        }

    ?>

    </table>

    <p>Computers</p>
    <table>
        <tr>
            <th>Name</th>
            <th>Cost</th>
            <th>Seller</th>
            <th>Ram</th>
            <th>RGB</th>
        </tr>
    <?php
        foreach ($items as $inner) { // Display the second computers table
            foreach ($inner as $i) {
                if ($i instanceof Computer) {
                    echo "<tr>";
                    echo "<td>".$i->get_name()."</td>";
                    echo "<td>".$i->get_cost()."</td>";
                    echo "<td>".$i->get_seller()."</td>";
                    echo "<td>".$i->get_ram()."</td>";
                    echo "<td>".$i->get_rgb()."</td>";
                    echo "</tr>";
                }
            }
        }
    ?>

    </table>

    <p>Phones</p>
    <table>
        <tr>
            <th>Name</th>
            <th>Cost</th>
            <th>Seller</th>
            <th>Camera</th>
            <th>Ram</th>
        </tr>

    <?php

        foreach ($items as $inner) { // Display the third phone table
            foreach ($inner as $i) {
                if ($i instanceof Phone) {
                    echo "<tr>";
                    echo "<td>".$i->get_name()."</td>";
                    echo "<td>".$i->get_cost()."</td>";
                    echo "<td>".$i->get_seller()."</td>";
                    echo "<td>".$i->get_cameras()."</td>";
                    echo "<td>".$i->get_ram()."</td>";
                    echo "</tr>";
                }
            }
        }
    ?>
    </table>
    <p>Here is all my HTML and PHP code below (see comments for sections)</p>
    <pre>
    <code>
        <?php
            /* A little trick to display everything */
            echo str_replace("<", "&lt;", str_replace("&", "&amp;", file_get_contents("store.php")));
        ?>
    </code>
    </pre>

    </body>
</html>
