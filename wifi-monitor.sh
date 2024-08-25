#!/bin/bash

# Default HOST value
HOST_DEFAULT=192.168.4.165

# Use the first command-line argument as HOST if provided, otherwise use the default
HOST="${1:-$HOST_DEFAULT}"

echo "Connecting to $HOST"
while true
do
  nc $HOST 8000
  echo "Disconnected from $HOST, retrying..."
done
