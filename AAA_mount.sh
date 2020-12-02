sudo mkdir /mnt/SKR
path=$(blkid --label SKR)
sudo mount $path /mnt/SKR
ls /mnt/SKR
