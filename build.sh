#!/bin/bash
# based on the instructions from edk2-platform
set -e
. build_common.sh
# not actually GCC5; it's GCC7 on Ubuntu 18.04.
./build_bootshim.sh
GCC5_AARCH64_PREFIX=aarch64-linux-gnu- build -j$(nproc) -s -n 0 -a AARCH64 -t GCC5 -p EXYNOS7885Pkg/Devices/a30.dsc
cat BootShim/BootShim.bin workspace/Build/EXYNOS7885Pkg/DEBUG_GCC5/FV/EXYNOS7885PKG_UEFI.fd > workspace/UEFI
mkbootimg --kernel workspace/Build/EXYNOS7885Pkg/DEBUG_GCC5/FV/EXYNOS7885PKG_UEFI.fd --base 0x10000000 --tags_offset 0x00000100 --pagesize 2048 --header_version 1 --dtb workspace/dtb -o workspace/boot.img
cd workspace/
tar -c boot.img -f boot.tar
cd ..
