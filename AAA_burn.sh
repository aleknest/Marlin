platformio run -e LPC1768

sudo mkdir /mnt/SKR
path=$(blkid --label SKR)
sudo mount $path /mnt/SKR
sudo cp ./.pio/build/LPC1768/firmware.bin /mnt/SKR
sudo sync
ls /mnt/SKR

#M997