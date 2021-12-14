#! /bin/bash

OVERLAY_DIR=~/repos/linux/scripts/dtc/include-prefixes/arm/overlays
DRIVES_DIR=~/repos/linux/drivers/misc

OVELAYS=$(find ./overlays -name "*.dts" -exec basename -- "{}" \;)
FILES=$(find . -maxdepth 1 -type f \( -name "*.c" -o -name "*.h" \) -exec basename -- "{}" \;)
C_FILES=$(find . -maxdepth 1 -type f -name "*.c" -exec basename -- "{}" \;)

function install_drivers() {

    if [ -d $OVERLAY_DIR ]; then
        printf "$OVERLAY_DIR:\n"
        for i in ${OVELAYS[@]}; do
            if [ -f $OVERLAY_DIR/$i ]; then
                printf "\033[1;33mEXIST\033[0m: $i\n"
            else
                cp ./overlays/$i $OVERLAY_DIR
                printf "\033[1;32mCOPYED\033[0m: $i\n"
            fi
        done
    fi

    if [ -f $OVERLAY_DIR/Makefile ]; then
        printf "$OVERLAY_DIR/Makefile:\n"
        for i in ${C_FILES[@]}; do
            i="${i%.dts}.dtbo"

            if grep -q "$i" $OVERLAY_DIR/Makefile; then
                printf "\033[1;33mEXIST\033[0m: $(grep "$i" $OVERLAY_DIR/Makefile)\n"
                true
            else
                echo "dtbo-y += $i" >> $OVERLAY_DIR/Makefile
                printf "\033[1;32mADDED\033[0m: $(grep "$i" $OVERLAY_DIR/Makefile)\n"
            fi
        done
    fi

    if [ -d $DRIVES_DIR ]; then
        printf "$DRIVES_DIR:\n"
        for i in ${FILES[@]}; do
            if [ -f $DRIVES_DIR/$i ]; then
                printf "\033[1;33mEXIST\033[0m: $i\n"
            else
                cp $i $DRIVES_DIR
                printf "\033[1;32mCOPYED\033[0m: $i\n"
            fi
        done
    fi

    if [ -f $DRIVES_DIR/Makefile ]; then
        printf "$DRIVES_DIR/Makefile:\n"
        for i in ${C_FILES[@]}; do
            i="${i%.c}.o"

            if grep -q "$i" $DRIVES_DIR/Makefile; then
                printf "\033[1;33mEXIST\033[0m: $(grep "$i" $DRIVES_DIR/Makefile)\n"
                true
            else
                echo "obj-y += $i" >> $DRIVES_DIR/Makefile
                printf "\033[1;32mADDED\033[0m: $(grep "$i" $DRIVES_DIR/Makefile)\n"
            fi
        done
    fi
}

function update_drivers() {
    
    if [ -d $OVERLAY_DIR ]; then
        printf "$OVERLAY_DIR:\n"
        for i in ${OVELAYS[@]}; do
            cp ./overlays/$i $OVERLAY_DIR
            printf "\033[1;32mUPDATE\033[0m: $i\n"
        done
    fi

    if [ -f $OVERLAY_DIR/Makefile ]; then
        printf "$OVERLAY_DIR/Makefile:\n"
        for i in ${C_FILES[@]}; do
            i="${i%.dts}.dtbo"

            if grep -q "$i" $OVERLAY_DIR/Makefile; then
                printf "\033[1;33mEXIST\033[0m: $(grep "$i" $OVERLAY_DIR/Makefile)\n"
                true
            else
                echo "dtbo-y += $i" >> $OVERLAY_DIR/Makefile
                printf "\033[1;32mADDED\033[0m: $(grep "$i" $OVERLAY_DIR/Makefile)\n"
            fi
        done
    fi

    if [ -d $DRIVES_DIR ]; then
        printf "$DRIVES_DIR:\n"
        for i in ${FILES[@]}; do
            cp $i $DRIVES_DIR
            printf "\033[1;32mUPDATE\033[0m: $i\n"
        done
    fi

    if [ -f $DRIVES_DIR/Makefile ]; then
        printf "$DRIVES_DIR/Makefile:\n"
        for i in ${C_FILES[@]}; do
            i="${i%.c}.o"

            if grep -q "$i" $DRIVES_DIR/Makefile; then
                printf "\033[1;33mEXIST\033[0m: $(grep "$i" $DRIVES_DIR/Makefile)\n"
                true
            else
                echo "obj-y += $i" >> $DRIVES_DIR/Makefile
                printf "\033[1;32mADDED\033[0m: $(grep "$i" $DRIVES_DIR/Makefile)\n"
            fi
        done
    fi
}

function delete_drivers() {

    if [ -d $OVERLAY_DIR ]; then
        printf "$OVERLAY_DIR:\n"
        for i in ${OVELAYS[@]}; do
            if [ -f $OVERLAY_DIR/$i ]; then
                rm -f $OVERLAY_DIR/$i
                printf "\033[1;31mDELATED\033[0m: $i\n"
            else
                true
            fi
        done
    fi

    if [ -f $OVERLAY_DIR/Makefile ]; then
        printf "$OVERLAY_DIR/Makefile:\n"
        for i in ${C_FILES[@]}; do
            i="${i%.dts}.dtbo"
            if grep -q "dtbo-y += $i" $OVERLAY_DIR/Makefile; then
                sed -i "/dtbo-y += $i/d" $OVERLAY_DIR/Makefile
                printf "\033[1;31mERASED\033[0m: dtbo-y += $i\n"
            else
                true
            fi
        done
    fi

    if [ -d $DRIVES_DIR ]; then
        printf "$DRIVES_DIR:\n"
        for i in ${FILES[@]}; do
            if [ -f $DRIVES_DIR/$i ]; then
                rm -f $DRIVES_DIR/$i
                printf "\033[1;31mDELATED\033[0m: $i\n"
            fi
        done
    fi

    if [ -f $DRIVES_DIR/Makefile ]; then
        printf "$DRIVES_DIR/Makefile:\n"
        for i in ${C_FILES[@]}; do
            i="${i%.c}.o"
            if grep -q "obj-y += $i" $DRIVES_DIR/Makefile; then
                sed -i "/obj-y += $i/d" $DRIVES_DIR/Makefile
                printf "\033[1;31mERASED\033[0m: obj-y += $i\n"
            else
                true
            fi
        done
    fi
}

select CHOSE in INSTALL UPDATE DELETE EXIT; do
    case $CHOSE in
        INSTALL)
            install_drivers 
            ;;
        UPDATE)  
            update_drivers  
            ;;
        DELETE)  
            delete_drivers  
            ;;
        EXIT)   
            break 
            ;;
        *) 
            ;;
    esac
done
