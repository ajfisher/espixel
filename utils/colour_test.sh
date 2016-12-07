#!/bin/bash

declare -a colours=("880000" "008800" "000088" "888800" "008888" "880088" "888888" "000000")

mqtt_app=./node_modules/.bin/mqtt_pub

if [ -z "$1" ]
then
    stripid=0
else
    stripid=$1
fi

if [ -z "$2" ]
then
    sleeptime=1
else
    sleeptime=$2
fi

if [ -z "$3" ]
then
    nodetarget="ESP_f2df79"
else
    nodetarget="$3"
fi

if [ -z "$4" ]
then
    host="127.0.0.1"
else
    host="$4"
fi

mqtt="$mqtt_app -h $host"

echo $mqtt
# while true construct
while :
do
    echo "Strip animate 10 seconds"
    $mqtt -t "${nodetarget}/${stripid}/i/c/strip" -m "000000"
    $mqtt -t "${nodetarget}/${stripid}/i/c/px/1" -m "888800"
    $mqtt -t "${nodetarget}/${stripid}/i/c/anim/shift" -m "1:100:10000:1:1"
    sleep 9
    echo "should stop imminently"
    sleep 3

    echo "go backwards 5 seconds"
    $mqtt -t "${nodetarget}/${stripid}/i/c/anim/shift" -m "1:100:5000:0:1"
    sleep 4
    echo "should stop imminently"
    sleep 3

    echo "Colour cycle"
    for colour in "${colours[@]}"
    do
        $mqtt -t "${nodetarget}/${stripid}/i/c/strip" -m "$colour"
        sleep ${sleeptime}
    done
done
