## Check dir
SHELL_PATH=$(pwd -P)
PATHS=$(echo $SHELL_PATH | tr "/" "\n")
PATH_ARRAY=()

for dir in $PATHS:
do
    PATH_ARRAY+=("$dir")
done

PATH_NUM=${#PATH_ARRAY[@]}
echo ${PATH_NUM}

CURR_PATH=${PATH_ARRAY[$((PATH_NUM-1))]}

echo $CURR_PATH

if [ $CURR_PATH == "bootloader:" ]
then
    ## Copy hex
    echo "Copying compiled hex file to this directory..."
    cp ./../_build/nrf52832_xxaa.hex ./nrf52832_xxaa.hex
    echo "Done."

    ## nrf version check
    nrfutil version

    ## Make Setting
    nrfutil settings generate --family NRF52 --application nrf52832_xxaa.hex --application-version 1 --bootloader-version 1 --bl-settings-version 2 bootloader_settings.hex
else
    ## Copy hex
    echo "Copying compiled hex file to this directory..."
    cp ./_build/nrf52832_xxaa.hex ./bootloader/nrf52832_xxaa.hex
    echo "Done."

    ## nrf version check
    nrfutil version

    ## Make Setting
    nrfutil settings generate --family NRF52 --application ./bootloader/nrf52832_xxaa.hex --application-version 1 --bootloader-version 1 --bl-settings-version 2 ./bootloader/bootloader_settings.hex
fi

exit 0



echo "Finished."