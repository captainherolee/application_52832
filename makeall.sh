#! /bin/bash

# Bootloader setting & Rom build script
#          for user     : ./build.sh
#          for su       : ./build.sh super
# ================================================

function make_bootloader_settings() {
    echo "Creating bootloader settings..."
    ./bootloader/makesetting.sh
    echo "Bootloader settings successfully created"
}

function make_rom() {
    echo "Creating rom..."
    ./makerom.sh
    echo "Rom successfully created"
}

make_bootloader_settings
make_rom

echo "Done."

exit 0