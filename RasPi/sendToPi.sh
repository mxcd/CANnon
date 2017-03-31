#!/bin/bash

echo "Sending CANnon project to Pi"

rsync -arvze ssh ~/GT/CANnon/RasPi/* max@10.26.2.237:CANnon/

echo "Running make file"
ssh max@10.26.2.237 "cd /home/max/CANnon && make"
