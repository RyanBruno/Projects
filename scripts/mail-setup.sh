#!/bin/bash

ssh -L 127.0.0.1:2525:127.0.0.1:25 -L 127.0.0.1:9595:127.0.0.1:110 ryan@flappy.rbruno.com

