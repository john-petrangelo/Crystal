DEVICE=/Volumes/CIRCUITPY/

echo "Copying network modules to device lib/"
cp src/handlers.py $DEVICE/lib/
cp src/network.py $DEVICE/lib/

echo "Copying web files to device assets/"
cp -R assets/ $DEVICE/assets/

echo "Copying main program to device root"
cp src/code.py $DEVICE/code.py

echo "Syncing filesystem"
sync
