#!/bin/bash

HEAD="<!DOCTYPE html>
<html>
<!--"

INNER_HEAD="-->
    <head>
        <title>Ryan Bruno's Blog</title>
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">
        <meta charset=\"UTF-8\">
        <style>
            * {
                box-sizing: border-box;
            }

            body {
                font-size: 18px;
                background-color: #f6f9fc;
                color: #222222;
                max-width: 100%;
                margin: 0 0;
            }

            div {
                background-color: white;
                margin: 16px auto;
                max-width: 1000px;
                width: calc(100% - 32px);
                padding: 20px 30px;
            }

            a {
                color: darkgrey;
            }

            a:hover {
                color: lightgrey;
            }

            h2 {
                margin: 8px 0;
            }

            h6 {
                margin: 0 0;
                color: grey;
            }

            pre {
                border: 1px solid #bbccdd;
                background-color: #ebf1f5;
                padding: 8px 8px;
            }

            code {
                background-color: #ebf1f5;
            }
        </style>
    </head>
    <body>
        <div>"

INNER_TAIL="        </div>
    </body>
<!--"

TAIL="-->
</html>"

pandoc -f markdown -t html $1 | cat <(echo "$INNER_HEAD") - <(echo "$INNER_TAIL") | gpg --clearsign | cat <(echo -e "$HEAD") - <(echo -e "$TAIL") > $(sed -e 's/\.md$//' <(echo $1))
