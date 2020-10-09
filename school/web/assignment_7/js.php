<!DOCTYPE html>
<html>
    <head>
        <title>My Webpage!</title>
        <style>
            div.flex {
                display: flex;
            }
            div.flex p.flex-half {
                width: 50%;
                text-align: center;
            }
        </style>
    </head>
    <body>

    <div class="flex">
        <p class="flex-half" id="rowNum">rows: 5</p>
        <p class="flex-half" id="colNum">columns: 3</p>
    </div>

    <?php
        if ($_SERVER['REQUEST_METHOD'] == "POST") {
            echo "<p>Phrase = ".$_POST["phrase"]."</p>";
            # Validator 4
            if (!preg_match("\d\d-\d\d-\d\d\d\d", $_POST["phrase"])) {
               echo "<p>Validator 4 Failed. Expected a date.</p>";
               die();
            }

            # Validator 5
            if (preg_match("[^0-9A-F]", $_POST["phrase"])) {
                echo "<p>Validator 5 Failed. Expected only 0-9 and A-F.</p>";
                die();
            }
            
        }
    ?>
    <table style="margin: 0 auto;">

    <script>
        /* Labels used to randomly set a button value */
        let labels = [
            "Buffalo", "Miami", "New England", "New York",
            "Baltimore", "Cincinnati", "Cleveland", "Pittsburgh",
            "Houston", "Indianapolis", "Tennessee", "Denver",
            "Kansas City", "Las Vegas", "Los Angeles", "Dallas",
            "New York", "Philadelphia", "Washington", "Chicago",
            "Detroit", "Green Bay", "Minnesota", "Atlanta",
            "Carolina", "New Orleans", "Tampa Bay", "Arizona Cardinals",
            "Los Angeles", "San Francisco", "Seattle",
        ];

        /* Shuffle labels */
        labels.sort(function (a, b) { return 0.5 - Math.random() })

        /* A template button. The code below replaces {{color}}
         * and {{value}}.
         */
        let button = `
           <form method="POST">
           <input type="submit" name="phrase" value="{{value}}"
               style="width: 100%; color: {{color}};"
               onclick="return part4(this);">
           </form>
        `;
        /* Returns the first parseable int from a
         * given node in the DOM by id.
         */
        let firstNumInText = (i) => parseInt(
                        document.getElementById(i)
                             .innerHTML
                             .replace(/[^0-9]*/, " "),
                    10);

        let rowNum = firstNumInText("rowNum");
        let colNum = firstNumInText("colNum");

        /* Row loop */
        while (rowNum-- > 0) {
            document.write("<tr>");

            /* Column Loop */
            for (let i = 0; i < colNum; i++) {
                let color = "blue";

                /* Is row + col odd? */
                if ((i + rowNum) & 1)
                    color = "red";

                document.write("<td>");
                document.write(button
                                /* Replace all {{color}} with color */
                                .split("{{color}}").join(color)
                                /* Same but for {{value}} to labels*/
                                .split("{{value}}").join(labels[rowNum + i]));
                document.write("</td>");
            }

            document.write("</tr>");
        }

        /* Called when a button is clicked. */
        function part4(node) {
            let text = node.value;
            let phrase = "";

            /* Maps Phrases to values based on ASCII values. */
            switch (Math.floor(text.charCodeAt(0) % 5)) {
                case 0:
                    phrase = "30";
                    break;
                case 1:
                    phrase = "2.3";
                    break;
                case 2:
                    phrase = "Hello";
                    break;
                case 3:
                    phrase = "01/02/2020";
                    break;
                case 4:
                    phrase = "A03F";
                    break;

            }
            /* Sets value to it can be posted */
            node.value = phrase;
            /* Run some validators */
            validatePhrase(phrase);
            /* Sends the post request */
            return true;
        }

        function validatePhrase(phrase) {
            console.log(phrase);
            /* First Validator */
            if (isNaN(parseInt(phrase, 10)))
                alert("Validator 1 Failed. Expected an integer.");

            /* Second Validator */
            if (!phrase.includes('.'))
                alert("Validator 2 Failed. Expected a real number.");

            /* Third Validator */
            if (phrase.match(/[^A-Za-z]/) != null)
                alert("Validator 3 Failed. Expected only A-Z and a-z.");
        }

    </script>

    </table>

    <?php
        highlight_file("./js.php");
    ?>

    </body>
</html>

