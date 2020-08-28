<!DOCTYPE html>
<html>
    <head>
        <title>My Webpage!</title>
    </head>
    <body>

    <?php
        switch (intdiv(date("H"), 12)) {
        case 0:
            echo "<p>Good morning</p>";

        case 1:
            echo "<p>Good afternoon</p>";
        }
        $reasons = array("It uses fancy loops",
                         "It uses fancy case statements",
                         "It uses fancy type casting");
        echo "<p>This page is more fancy then other and I'll tell you why</p>";
        for ($i = 0; $i < count($reasons); $i++) {
            echo "<p>".$reasons[$i]."</p>";
        }

        $y = "64";
        $x = 73;
        $d = 1.9;
        $x += $y *$d;
        $msg = "<p>This webpage is $x many times better then any other webpage!</p>";
        echo $msg;


    ?>

    </body>
</html> 
