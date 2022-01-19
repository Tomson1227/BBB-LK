SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
KERNEL_DIR=~/repos/linux-stable
GCC_FLAGS="-E -P -x assembler-with-cpp -I$KERNEL_DIR/include"
# DTC_FLAGS="-@ -I dts -O dtb"
DTC_FLAGS="-W no-unit_address_vs_reg -I dts -O dtb -b 0 -@"
ALL_DTS=($(find -type f -name "*.dts" -exec basename -- "{}" \;)) 
ALL_DTSO=($(find -type f -name "*.dtso" -exec basename -- "{}" \;))

# $1 - file to compile
function compile_overlay
{
    local SRC="$SCRIPT_DIR/$1"
    local TEMP="$SCRIPT_DIR/${1}.tmp"
    local OVERLAY="$SCRIPT_DIR/${1/.dts*/.dtbo}"
    
    # cd $KERNEL_DIR
    #     dtc -I dts -O dtb -b 0 -@ -o $OVERLAY $SRC
    # cd -

    cd $KERNEL_DIR
    printf "\033[30;1mPWD: $PWD\033[0m\n"

        printf "\033[31;1mgcc $GCC_FLAGS -o $TEMP $SRC\033[0m\n"
        gcc $GCC_FLAGS -o $TEMP $SRC

        printf "\033[32;1mdtc $DTC_FLAGS -o $OVERLAY $TEMP\033[0m\n"
        dtc $DTC_FLAGS -o $OVERLAY $TEMP

        printf "\033[33;1mrm -f $TEMP\033[0m\n"
        rm -f $TEMP

    cd -
    printf "\033[34;1mPWD: $PWD\033[0m\n"
}

# printf "\033[34;1mFind DTS files:\033[0m\n"
# for i in ${!ALL_DTS[@]}; do
#     printf "$i ${ALL_DTS[i]}\n"
# done

# printf "\n\033[35;1mFind DTSO files:\033[0m\n"
# for i in ${!ALL_DTSO[@]}; do
#     printf "$i ${ALL_DTSO[i]}\n"
# done

printf "\033[36;1mChose to compile:\033[0m\n"
select CHOSE in ${ALL_DTS[@]} ${ALL_DTSO[@]} QUIT; do
    if [ $CHOSE = "QUIT" ]; then
        break
    fi

    compile_overlay $CHOSE
done

exit 0

# BASE="/home/opovshenko/repos/linux-stable/arch/arm/boot/dts/am335x-boneblack.dtb"
BASE="am335x-boneblack.dtb"
OUT=${SCRIPT_DIR}/out.dtb

function show_overlay 
{
    fdtdump $OVERLAY | less
}

function apply_overlay 
{
    fdtoverlay -i $BASE -o $OUT $1
}
