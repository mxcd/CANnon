#!/bin/bash

echo "Sending CANnon project to Pi"

rm ~/GT/CANnon/RasPi/*.o
rm ~/GT/CANnon/RasPi/cannon

rsync -arvze ssh ~/GT/CANnon/RasPi/* max@10.26.2.237:CANnon/


echo "Running make file"
ssh max@10.26.2.237 "cd /home/max/CANnon && chmod u+rwx clean.sh && ./clean.sh && make"
#ssh max@10.26.2.237 "cd /home/max/CANnon && chmod u+rwx stopCanInterface.sh && sudo ./stopCanInterface.sh"
#ssh max@10.26.2.237 "cd /home/max/CANnon && chmod u+rwx startCanInterface.sh && sudo ./startCanInterface.sh"