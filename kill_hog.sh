#!/bin/bash

PORT=5000
PID=$(lsof -t -i:$PORT)

if [ -n "$PID" ]; then
    echo "Killing process $PID using port $PORT..."
    kill -9 $PID
    echo "Process terminated."
else
    echo "No process found using port $PORT."
fi
