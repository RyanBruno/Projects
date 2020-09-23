<!DOCTYPE html>
<html>
    <head>
        <title>My Webpage!</title>
    </head>
    <body>
        <h1>First 10 Lines of Store.php</h1>
        <pre>
        <?php
            $file = "./store.php";

            $stream = fopen($file, "r");
            if (!$stream) {
                echo "Could not find file".$file;
            }

            for ($i = 0; $i < 10; $i++) {
                $line = fgets($stream);

                if (!$line) break;
                echo "<b>".str_replace("<", "&lt;",
                     str_replace("&", "&amp;",
                     $line))."</b>";
            }
        ?>
        </pre>

        <h1>Souce Code</h1>
        <?php
            highlight_file("./code.php");
        ?>
    </body>
</html>

