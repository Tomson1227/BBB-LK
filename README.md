# BeagleBone Bleck Linux Kernel (BBB-LK)

## Baremetal toolchain
Download baremetal toolchain for Cortex-A processors (32-bit) from here:\
[gcc-arm-10.3-2021.07-x86_64-arm-none-eabi.tar.xz](https://developer.arm.com/-/media/Files/downloads/gnu-a/10.3-2021.07/binrel/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi.tar.xz)

Extract this toolchain to /opt : \
`sudo tar xJvf gcc-arm-10.3-2021.07-x86_64-arm-none-eabi.tar.xz -C /opt/`

## Linux toolchain
Download Linux toolchain for Cortex-A processors (32-bit) from here:\
[gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf.tar.xz](https://developer.arm.com/-/media/Files/downloads/gnu-a/10.3-2021.07/binrel/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf.tar.xz)

Extract this toolchain to /opt : \
`sudo tar xJvf gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf.tar.xz -C /opt/`

## u-boot
`git clone https://gitlab.denx.de/u-boot/u-boot.git` \
`cd u-boot` \
`git checkout v2020.10`

## linux-stable 
`git clone git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git` \
`cd linux-stable` \
`git checkout linux-4.9.y` \
`curl https://patchwork.ozlabs.org/series/130450/mbox/ | git am`

## busybox
`git clone git://git.busybox.net/busybox` \
`cd busybox` \
`git checkout 1_34_stable`
