#!/bin/bash

echo "Sending CANnon project to BBB"

rm ~/GT/CANnon/RasPi/*.o
rm ~/GT/CANnon/RasPi/cannon

rsync -arvze ssh ~/GT/CANnon/RasPi/* max@192.168.7.2:/usr/cannon/bin/


echo "Running make file"
ssh max@192.168.7.2 "cd /usr/cannon/bin/ and chmod u+rwx clean.sh and ./clean.sh and make"
echo "done"
#ssh max@10.26.2.237 "cd /home/max/CANnon && chmod u+rwx stopCanInterface.sh && sudo ./stopCanInterface.sh"
#ssh max@10.26.2.237 "cd /home/max/CANnon && chmod u+rwx startCanInterface.sh && sudo ./startCanInterface.sh"