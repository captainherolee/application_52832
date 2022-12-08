TARGET_FILE="nrf52832_xxaa"
VERSION="220617"
NRF_VERSION="52"
SOFTDEVICE_ID="0x0101"
SOFTDEVICE_VERSION="7.2.0"
FINAL_VERSION="1.0"
DEVICE_NAME="hero"

DATE=$(date "+%Y%m%d")

## Copy hex file
cp ./bootloader/${TARGET_FILE}.hex ./${TARGET_FILE}.hex

## nrf version check
nrfutil version

## Make binary
nrfutil pkg generate --application ./${TARGET_FILE}.hex --application-version ${VERSION} --key-file ./key/key.pem --hw-version ${NRF_VERSION} --sd-req ${SOFTDEVICE_ID} ${DEVICE_NAME}_v${FINAL_VERSION}.zip 

## Check binary
nrfutil pkg display ${DEVICE_NAME}_v${FINAL_VERSION}.zip 

echo "Finished."
