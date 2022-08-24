#!/bin/sh

# Build OS using AUTO.ISO minimal auto-install as bootstrap to merge codebase, recompile system, attempt build limine UEFI hybrid ISO

# make sure we are in the correct directory
SCRIPT_DIR=$(realpath "$(dirname "$0")")
SCRIPT_NAME=$(basename "$0")
EXPECTED_DIR=$(realpath "$PWD")

if test "${EXPECTED_DIR}" != "${SCRIPT_DIR}"
then
	( cd "$SCRIPT_DIR" || exit ; "./$SCRIPT_NAME" "$@" );
	exit
fi

# Uncomment if you use doas instead of sudo
#alias sudo=doas 

TMPDIR="/tmp/zealtmp"
TMPISODIR="$TMPDIR/iso"
TMPDISK="$TMPDIR/ZealOS.raw"
TMPMOUNT="$TMPDIR/mnt"

mount_tempdisk() {
	sudo modprobe nbd
	sudo qemu-nbd -c /dev/nbd0 -f raw $TMPDISK
	sudo partprobe /dev/nbd0
	sudo mount /dev/nbd0p1 $TMPMOUNT
}

umount_tempdisk() {
	sync
	sudo umount $TMPMOUNT
	sudo qemu-nbd -d /dev/nbd0
}

[ ! -d $TMPMOUNT ] && mkdir -p $TMPMOUNT
[ ! -d $TMPISODIR ] && mkdir -p $TMPISODIR
set -e

echo "Building ZealBooter..."
( cd ../zealbooter && make clean all )

set +e

echo "Making temp vdisk, running auto-install..."
qemu-img create -f raw $TMPDISK 192M
qemu-system-x86_64 -machine q35,accel=kvm -drive format=raw,file=$TMPDISK -m 1G -rtc base=localtime -smp 4 -cdrom AUTO-VM.ISO -device isa-debug-exit

echo "Mounting vdisk, copying src/Kernel/KStart16.ZC and src/Kernel/KernelA.HH ..."
rm ../src/Home/Registry.ZC 2> /dev/null
rm ../src/Home/MakeHome.ZC 2> /dev/null
mount_tempdisk
sudo cp -rf ../src/Kernel/KStart16.ZC $TMPMOUNT/Kernel/
sudo cp -rf ../src/Kernel/KernelA.HH $TMPMOUNT/Kernel/
umount_tempdisk

echo "Rebuilding kernel headers..."
qemu-system-x86_64 -machine q35,accel=kvm -drive format=raw,file=$TMPDISK -m 1G -rtc base=localtime -smp 4 -device isa-debug-exit

echo "Mounting vdisk, copying all src/ kernel code..."
rm ../src/Home/Registry.ZC 2> /dev/null
rm ../src/Home/MakeHome.ZC 2> /dev/null
mount_tempdisk
sudo cp -rf ../src/Kernel/* $TMPMOUNT/Kernel/
umount_tempdisk

echo "Rebuilding kernel..."
qemu-system-x86_64 -machine q35,accel=kvm -drive format=raw,file=$TMPDISK -m 1G -rtc base=localtime -smp 4 -device isa-debug-exit

echo "Mounting vdisk and copying all src/ code..."
rm ../src/Home/Registry.ZC 2> /dev/null
rm ../src/Home/MakeHome.ZC 2> /dev/null
rm ../src/Boot/Kernel.ZXE 2> /dev/null
mount_tempdisk
sudo cp -r ../src/* $TMPMOUNT

if [ ! -d "limine" ]; then
    git clone https://github.com/limine-bootloader/limine.git --branch=v3.0-branch-binary --depth=1
    make -C limine
fi

sudo mkdir -p $TMPMOUNT/EFI/BOOT
sudo cp limine/BOOTX64.EFI $TMPMOUNT/EFI/BOOT/BOOTX64.EFI
sudo cp limine/limine.sys $TMPMOUNT/
sudo cp ../zealbooter/zealbooter.elf $TMPMOUNT/Boot/ZealBooter.ELF
umount_tempdisk

if [ ! -d "ovmf" ]; then
    echo "Downloading OVMF..."
    mkdir ovmf
    cd ovmf
    curl -o OVMF-X64.zip https://efi.akeo.ie/OVMF/OVMF-X64.zip
    7z x OVMF-X64.zip
    cd ..
fi

./limine/limine-deploy $TMPDISK

echo "Rebuilding kernel and OS..."
qemu-system-x86_64 -machine q35,accel=kvm -drive format=raw,file=$TMPDISK -m 1G -rtc base=localtime -bios ovmf/OVMF.fd -smp 4 -device isa-debug-exit

mount_tempdisk
sudo cp -rf $TMPMOUNT/* $TMPISODIR
sudo cp limine/limine-cd-efi.bin $TMPISODIR/Boot/
sudo cp limine/limine-cd.bin $TMPISODIR/Boot/
sudo cp limine/limine.sys $TMPISODIR/
sudo cp $TMPMOUNT/limine.cfg $TMPISODIR/limine.cfg
sudo rm -rf $TMPISODIR/EFI
sudo cp -rf ../zealbooter/zealbooter.elf $TMPISODIR/Boot/ZealBooter.ELF
umount_tempdisk

sudo ls $TMPISODIR -al

xorriso -as mkisofs -b Boot/limine-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot Boot/limine-cd-efi.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        $TMPISODIR -o ZealOS-UEFI-limine-dev.iso

./limine/limine-deploy ZealOS-UEFI-limine-dev.iso

echo "Testing UEFI ISO boot ..."
qemu-system-x86_64 -machine q35,accel=kvm -m 1G -rtc base=localtime -bios ovmf/OVMF.fd -smp 4 -cdrom ZealOS-UEFI-limine-dev.iso
echo "Testing BIOS ISO boot ..."
qemu-system-x86_64 -machine q35,accel=kvm -m 1G -rtc base=localtime -smp 4 -cdrom ZealOS-UEFI-limine-dev.iso

echo "Deleting temp folder..."
sudo rm -rf $TMPDIR
sudo rm -rf $TMPISODIR
echo "Finished."

