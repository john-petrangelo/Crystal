DEVICE=/Volumes/CIRCUITPY/

echo "Copying network module to device lib/"
cp src/network.py $DEVICE/lib/

echo "Copying main program to device root"
cp src/code.py $DEVICE/code.py

echo "Syncing filesystem"
sync
