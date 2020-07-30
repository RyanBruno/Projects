#!/bin/bash

rm -r /tmp/weather ; mkdir /tmp/weather && cd /tmp/weather && wget https://cdn.star.nesdis.noaa.gov/GOES16/ABI/SECTOR/ne/Sandwich/GOES16-NE-Sandwich-600x600.gif && wget https://radar.weather.gov/Conus/RadarImg/northeast.gif && wget https://www.wpc.ncep.noaa.gov/basicwx/92fndfd.gif && sxiv -at .
