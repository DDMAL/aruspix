#!/bin/sh

# the Aruspix path
aruspix_bin=/Users/laurent/projects/aruspix/bin/Debug/aruspix_headless
aruspix_res=/Users/laurent/projects/aruspix/data

# read the input and cat it into a file
data=$(</dev/stdin)
echo "$data" > /tmp/mei2svg.mei

# run the conversion
$aruspix_bin -o /tmp/mei2mei.svg -r $aruspix_res /tmp/mei2mei.mei > /tmp/aruspix_log 

echo "Content-type: text/xml\r\n"
echo "\r\n"
cat /tmp/mei2svg.svg