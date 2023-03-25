dd if=/dev/zero of=cedos.img count=144
parted cedos.img mklabel msdos
parted cedos.img mkpart primary FAT32 8s 128s -s

dd if=/dev/zero of=fat.img count=128
mkfs.fat fat.img

mkdir -p ./mnt
sudo mount fat.img ./mnt
sudo cp ./build/release/components/kernel.bin ./mnt
sudo cp ./build/release/components/apps.o ./mnt
sudo umount ./mnt

dd if=fat.img of=cedos.img seek=8 conv=notrunc
dd bs=1 if=./build/release/components/boot.bin of=cedos.img count=446 conv=notrunc
dd if=./build/release/components/boot.bin of=cedos.img skip=1 seek=1 count=7 conv=notrunc

parted cedos.img print list all