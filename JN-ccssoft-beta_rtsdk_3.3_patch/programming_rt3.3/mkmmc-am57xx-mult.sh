#! /bin/bash

##define log file and turn off kernel print
LOG=$HOME/mkmmc-am57xx.log
echo 3 > /proc/sys/kernel/printk
print_failure ()
{
    echo "mkmmc-am57xx failure !!!!!!"
    exit 1
}

print_success ()
{
    echo "mkmmc-am57xx success"
    exit 0
}

if ! [[ -b $1 ]]; then
    echo "$1 is not exist"
    print_failure
fi
if ! [[ -f $2 ]]; then
    echo "$2 is not exist"
    print_failure
fi
if ! [[ -f $3 ]]; then
    echo "$3 is not exist"
    print_failure
fi
if ! [[ -f $4 ]]; then
    echo "$4 is not exist"
    print_failure
fi
if ! [[ -f $5 ]]; then
    echo "$5 is not exist"
    print_failure
fi
if ! [[ -f $6 ]]; then
    echo "$6 is not exist"
    print_failure
fi

echo "All data will be cleared [y/n]"
read ANSWER
if [[ $ANSWER == "y" ]]; then
    sleep 1
else
    print_failure
fi

## step 1
echo "Upload the partition"
DISK=$1
PARTITION1=$(df | grep $DISK | awk '{if(NR==1) print $1}')
PARTITION2=$(df | grep $DISK | awk '{if(NR==2) print $1}')
PARTITION3=$(df | grep $DISK | awk '{if(NR==3) print $1}')
if [[ -b $PARTITION1 ]]; then
    umount $PARTITION1
    sleep 1
fi
if [[ -b $PARTITION2 ]]; then
    umount $PARTITION2
    sleep 1
fi
if [[ -b $PARTITION3 ]]; then
    umount $PARTITION3
    sleep 1
fi

## step 2
echo "Create the partition"
dd if=/dev/zero of=$DISK bs=1024 count=1024 >>$LOG 2>&1
if ! [[ $? -eq 0 ]]; then
    echo "dd $DISK failure"
    print_failure
fi
parted -s $DISK mklabel msdos
if ! [[ $? -eq 0 ]]; then
    echo "parted $DISK failure"
    print_failure
fi

parted -s $DISK unit cyl mkpart primary fat32 -- 0 10
if ! [[ $? -eq 0 ]]; then
    echo "parted $DISK failure"
    print_failure
fi
parted -s $DISK set 1 boot on
if ! [[ $? -eq 0 ]]; then
    echo "parted $DISK failure"
    print_failure
fi
parted -s $DISK unit cyl mkpart primary ext4 -- 10 -2
if ! [[ $? -eq 0 ]]; then
    echo "parted $DISK failure"
    print_failure
fi
sleep 5

## step 3
echo "Format the partition"
BOOT_PARTITION=$(ls $DISK* | grep -v "boot" | awk '{if(NR==2) print $1}')
ROOTFS_PARTITION=$(ls $DISK* | grep -v "boot" | awk '{if(NR==3) print $1}')
BOOT_IS_MOUNT=$(df | grep $BOOT_PARTITION)
if [[ -n $BOOT_IS_MOUNT ]]; then
    umount $BOOT_PARTITION
    sleep 1
fi
ROOTFS_IS_MOUNT=$(df | grep $ROOTFS_PARTITION)
if [[ -n $ROOTFS_IS_MOUNT ]]; then
    umount $ROOTFS_PARTITION
    sleep 1
fi
mkfs.vfat -F 32 -n boot $BOOT_PARTITION >>$LOG 2>&1
if ! [[ $? -eq 0 ]]; then
    echo "mkfs $BOOT_PARTITION failure"
    print_failure
fi
mkfs.ext4 -L rootfs $ROOTFS_PARTITION >>$LOG 2>&1
if ! [[ $? -eq 0 ]]; then
    echo "mkfs $ROOTFS_PARTITION failure"
    print_failure
fi
sleep 5

## step 4
echo "Fill up the partition"
mkdir -p /mnt/boot
mkdir -p /mnt/rootfs
mount $BOOT_PARTITION /mnt/boot
if ! [[ $? -eq 0 ]]; then
    echo "mount $BOOT_PARTITION failure"
    print_failure
fi
rm -rf /mnt/boot/*
mount $ROOTFS_PARTITION /mnt/rootfs
if ! [[ $? -eq 0 ]]; then
    echo "mount $ROOTFS_PARTITION failure"
    print_failure
fi
rm -rf /mnt/rootfs/*

cp $2 /mnt/boot
cp $3 /mnt/boot
tar -Jxvf $6 -C /mnt/rootfs >>$LOG 2>&1
if ! [[ $? -eq 0 ]]; then
    echo "Fill up the partition $ROOTFS_PARTITION failure"
    print_failure
fi
cp -r fs_patch/* /mnt/rootfs
cp $4 /mnt/rootfs/boot
cp $5 /mnt/rootfs/boot

sync
umount $BOOT_PARTITION
if ! [[ $? -eq 0 ]]; then
    echo "umount $BOOT_PARTITION failure"
    print_failure
fi
umount $ROOTFS_PARTITION
if ! [[ $? -eq 0 ]]; then
    echo "umount $ROOTFS_PARTITION failure"
    print_failure
fi
rm -rf /mnt/boot
rm -rf /mnt/rootfs
print_success
