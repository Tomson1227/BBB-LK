#! /bin/bash

RED_START="\033[1;31m"
GREEN_START="\033[1;32m"
YELOW_START="\033[1;33m"
COLOR_END="\033[0m"

function configure_mount_dir
{
    BOOT_DIR=/mnt/BBB_boot
    ROOT_DIR=/mnt/BBB_root
    # printf "BOOT_DIR=$BOOT_DIR\nROOT_DIR=$ROOT_DIR\n"
}

function configure_kernel
{
    # Default for RPi 3 Model B
    export PATH=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/bin/:$PATH
    export PATH=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf/bin:$PATH
    UBOOT_CROSS_COMPILE="ccache arm-none-eabi-"
    BUSYBOX_CROSS_COMPILE="ccache arm-none-linux-gnueabihf-"
    ARCH=arm
    # printf "ARCH=$ARCH\nUBOOT_CROSS_COMPILE=$UBOOT_CROSS_COMPILE\nBUSYBOX_CROSS_COMPILE=$BUSYBOX_CROSS_COMPILE\n"
}

function configure_build_dirs
{
    LINUX_DIR=~/repos/linux-stable
    UBOOT_DIR=~/repos/u-boot
    BUSYBOX_DIR=~/repos/busybox
    BUSYBOX_INSTALL_DIR="${BUSYBOX_DIR}/_install"
    # printf "LINUX_DIR=$LINUX_DIR\nBUSYBOX_DIR=q$BUSYBOX_DIR\n"
}

function mount_part {
    if grep -qs $2 /proc/mounts; then
            sudo umount $2
    fi

    if grep -qs $(basename -- $1) /proc/partitions; then
        if grep -qs $1 /proc/mounts; then
            sudo umount $1
        fi
    
        sudo mount $1 $2 ; grep -qs $1 /proc/mounts
        # printf "${GREEN_START}${1} mount to${2}${COLOR_END}\n"
        return $?
    fi

    return 1
}

function mount_SD {
    SD1=`find /dev/ -name "sd?1"`
    SD2=`find /dev/ -name "sd?2"`

    if [[ "$SD1" == "" || "$SD2" == "" ]]; then
        return 1
    fi
    sudo mkdir -p $BOOT_DIR $ROOT_DIR 
    mount_part $SD1 $BOOT_DIR && mount_part $SD2 $ROOT_DIR   

    return $?
}

function umount_SD 
{
    if grep -qs $SD1 /proc/mounts; then
        sudo umount $SD1
        # printf "${GREEN_START}${SD1} umount${COLOR_END}\n"
    fi

    if grep -qs $SD2 /proc/mounts; then
        sudo umount $SD2
        # printf "${GREEN_START}${SD2} umount${COLOR_END}\n"
    fi

}

function build_bootloader
{
    # export PATH=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/bin/:$PATH
    # UBOOT_CROSS_COMPILE='ccache arm-none-eabi-'
    if [[ ! -f $UBOOT_DIR/MLO || ! -f $UBOOT_DIR/u-boot.img ]]; then
        cd $UBOOT_DIR
        curl https://patchwork.ozlabs.org/series/130450/mbox/ | git am
        make am335x_boneblack_defconfig
        make CROSS_COMPILE="$UBOOT_CROSS_COMPILE" ARCH="$ARCH" -j$((`nproc` -1)) 
        cd -
    else
        printf "Bootloader already exist\n"
        read -p "Rebuild it? [N/y]: " OPTION
        case $OPTION in 
        [Yy]* )
            cd $UBOOT_DIR
            curl https://patchwork.ozlabs.org/series/130450/mbox/ | git am
            make am335x_boneblack_defconfig
            make CROSS_COMPILE="$UBOOT_CROSS_COMPILE" ARCH="$ARCH" -j$((`nproc` -1)) 
            cd - ;;
        *) ;;
        esac
    fi
}

function compile_kernel
{
    if [ ! -f $LINUX_DIR/fragments/bbb.cfg ]; then
        mkdir -p fragments
        printf "# Use multi_v7_defconfig as a base for merge_config.sh
# --- USB ---
# Enable USB on BBB (AM335x)
CONFIG_USB_ANNOUNCE_NEW_DEVICES=y
CONFIG_USB_EHCI_ROOT_HUB_TT=y
CONFIG_AM335X_PHY_USB=y
CONFIG_USB_MUSB_TUSB6010=y
CONFIG_USB_MUSB_OMAP2PLUS=y
CONFIG_USB_MUSB_HDRC=y
CONFIG_USB_MUSB_DSPS=y
CONFIG_USB_MUSB_AM35X=y
CONFIG_USB_CONFIGFS=y
CONFIG_NOP_USB_XCEIV=y
# For USB keyboard and mouse
CONFIG_USB_HID=y
CONFIG_USB_HIDDEV=y
# For PL2303, FTDI, etc
CONFIG_USB_SERIAL=y
CONFIG_USB_SERIAL_PL2303=y
CONFIG_USB_SERIAL_GENERIC=y
CONFIG_USB_SERIAL_SIMPLE=y
CONFIG_USB_SERIAL_FTDI_SIO=y
# For USB mass storage devices (like flash USB stick)
CONFIG_USB_ULPI=y
CONFIG_USB_ULPI_BUS=y
# --- Networking ---
CONFIG_BRIDGE=y
# --- Device Tree Overl" | tee $LINUX_DIR/fragments/bbb.cfg > /dev/null
    fi

    if [[ ! -f $LINUX_DIR/arch/arm/boot/zImage || ! -f $LINUX_DIR/arch/arm/boot/dts/am335x-boneblack.dtb || ! -f $LINUX_DIR/System.map || ! -f $LINUX_DIR/.config ]]; then
        cd $LINUX_DIR
        ./scripts/kconfig/merge_config.sh arch/arm/configs/multi_v7_defconfig fragments/bbb.cfg
        make -j$((`nproc` -1)) zImage modules am335x-boneblack.dtb
        cd -
    else
        printf "Kernel already exist\n"
        read -p "Rebuild it? [N/y]: " OPTION
        case $OPTION in 
        [Yy]* )
            cd $LINUX_DIR
            ./scripts/kconfig/merge_config.sh arch/arm/configs/multi_v7_defconfig fragments/bbb.cfg
            make -j$((`nproc` -1)) zImage modules am335x-boneblack.dtb
            cd - ;;
        *) ;;
        esac
    fi
}

function compile_busybox 
{
    if [[ ! -d $BUSYBOX_INSTALL_DIR ]]; then
        cd $BUSYBOX_DIR
        make defconfig
        make CROSS_COMPILE="$BUSYBOX_CROSS_COMPILE" ARCH="$ARCH" -j$((`nproc` -1))
        make install
        mkdir -p $BUSYBOX_INSTALL_DIR/{boot,dev,etc\/init.d,lib,proc,root,sys\/kernel\/debug,tmp}
        cd -
    else
        printf "BusyBox already exist\n"
        read -p "Rebuild it? [N/y]: " OPTION
        case $OPTION in 
        [Yy]* )
            cd $BUSYBOX_DIR
            make defconfig
            make CROSS_COMPILE="$BUSYBOX_CROSS_COMPILE" ARCH="$ARCH" -j$((`nproc` -1))
            make install
            mkdir -p $BUSYBOX_INSTALL_DIR/{boot,dev,etc\/init.d,lib,proc,root,sys\/kernel\/debug,tmp}
            cd - ;;
        *) ;;
        esac
    fi
}

#Populate /boot
function populate_boot 
{
    if [[ ! -f $LINUX_DIR/arch/arm/boot/zImage || ! -f $LINUX_DIR/arch/arm/boot/dts/am335x-boneblack.dtb || ! -f $LINUX_DIR/System.map || ! -f $LINUX_DIR/.config ]]; then
        compile_kernel
    fi

    cp $LINUX_DIR/arch/arm/boot/zImage $BUSYBOX_INSTALL_DIR/boot
    cp $LINUX_DIR/arch/arm/boot/dts/am335x-boneblack.dtb $BUSYBOX_INSTALL_DIR/boot
    cp $LINUX_DIR/System.map $BUSYBOX_INSTALL_DIR/boot
    cp $LINUX_DIR/.config $BUSYBOX_INSTALL_DIR/boot/config
}

#Populate /lib
function populate_lib 
{
    cd $LINUX_DIR
    make ARCH=arm INSTALL_MOD_PATH=$BUSYBOX_INSTALL_DIR modules_install

    libc_dir=$(${CROSS_COMPILE}gcc -print-sysroot)/lib
    cp -a $libc_dir/*.so* $BUSYBOX_INSTALL_DIR/lib
    cd -
}

#Populate /etc
function populate_etc 
{
    echo '$MODALIAS=.* root:root 660 @modprobe "$MODALIAS"' > $BUSYBOX_INSTALL_DIR/etc/mdev.conf
    echo 'root:x:0:' > $BUSYBOX_INSTALL_DIR/etc/group
    echo 'root:x:0:0:root:/root:/bin/sh' > $BUSYBOX_INSTALL_DIR/etc/passwd
    echo 'root::10933:0:99999:7:::' > $BUSYBOX_INSTALL_DIR/etc/shadow
    echo "nameserver 8.8.8.8" > $BUSYBOX_INSTALL_DIR/etc/resolv.conf
}

function build_rootfs 
{
    compile_busybox

    touch $BUSYBOX_INSTALL_DIR/etc/init.d/rcS
    chmod +x $BUSYBOX_INSTALL_DIR/etc/init.d/rcS

    printf "#! /bin/sh
mount -t sysfs none /sys
mount -t proc none /proc
mount -t debugfs none /sys/kernel/debug
echo /sbin/mdev > /proc/sys/kernel/hotplug
mdev -s
" | tee $BUSYBOX_INSTALL_DIR/etc/init.d/rcS > /dev/null

    if [ ! -L $BUSYBOX_INSTALL_DIR/init ]; then
        ln -s $BUSYBOX_INSTALL_DIR/bin/busybox $BUSYBOX_INSTALL_DIR/init
    fi

    populate_boot
    populate_lib
    populate_etc
}

function copy_to_SD 
{
    if [[ ! -f $UBOOT_DIR/MLO || ! -f $UBOOT_DIR/u-boot.img ]]; then
        printf "Bootloader does not created!\n"
        read -p "Build one? [N/y]: " OPTION
        case $OPTION in 
        [Yy]* )
            build_bootloader
            printf "${GREEN_START}Copying${COLOR_END}: $BOOT_DIR\n"
            sudo cp $UBOOT_DIR/MLO $UBOOT_DIR/u-boot.img $BOOT_DIR
            break ;;
        *)
            break ;;
        esac
    else
        printf "${GREEN_START}Copying${COLOR_END}: $BOOT_DIR\n"
        sudo cp $UBOOT_DIR/MLO $UBOOT_DIR/u-boot.img $BOOT_DIR
    fi

    if [[ ! -d $BUSYBOX_INSTALL_DIR ]]; then
        printf "Rootfs does not created!\n"
        read -p "Build one? [N/y]: " OPTION
        case $OPTION in 
        [Yy]* )
            build_rootfs
            printf "${GREEN_START}Copying${COLOR_END}: $ROOT_DIR\n"
            sudo cp -R $BUSYBOX_INSTALL_DIR/* $ROOT_DIR
            break ;;
        *)
            break ;;
        esac
    else
        printf "${GREEN_START}Copying${COLOR_END}: $ROOT_DIR\n"
        sudo cp -R $BUSYBOX_INSTALL_DIR/* $ROOT_DIR
    fi

    printf "${GREEN_START}Done\n${COLOR_END}"
}

function clear_SD 
{
    printf "${RED_START}Clear${COLOR_END}: /mnt/$(basename -- $BOOT_DIR)/\n"
    sudo rm -rf /mnt/$(basename -- $BOOT_DIR)/*
    printf "${RED_START}Clear${COLOR_END}: /mnt/$(basename -- $BOOT_DIR)/\n"
    sudo rm -rf /mnt/$(basename -- $ROOT_DIR)/*
    printf "${RED_START}Done\n${COLOR_END}"
}

configure_mount_dir
configure_kernel
configure_build_dirs

select OPTION in Build_Bootloader Build_Kernel Build_RootFS Boot_SD Clean_SD Exit; do
    case $OPTION in
    Build_Bootloader)
        build_bootloader
        ;;
    Build_Kernel)
        compile_kernel
        ;;
    Build_RootFS)
        build_rootfs
        ;;
    Boot_SD)
        if mount_SD; then
            copy_to_SD
            umount_SD
        else
            printf "${RED_START}SD do not pluged${COLOR_END}\n"
        fi    
        ;;
    Clean_SD)
        if mount_SD; then
            clear_SD
            umount_SD
        else
            printf "${RED_START}SD do not pluged${COLOR_END}\n"
        fi
        ;;
    Exit)  
        break 
        ;;
    *)
        ;;
    esac
done
