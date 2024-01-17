DEVICE=/Volumes/CIRCUITPY/

echo "Copying web files to device assets/"
cp -R assets/ $DEVICE/assets/

echo "Copying python modules to device lib/"
cp src/handlers.py src/network.py src/utils.py $DEVICE/lib/

echo "Copying main program to device root"
cp src/code.py $DEVICE/code.py

echo "Syncing filesystem"
sync
