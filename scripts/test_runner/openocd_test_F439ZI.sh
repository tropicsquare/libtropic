#!/usr/bin/env bash

# Update amount of sleep to match the time needed for the test to run. Find it on line after reset

# Update path to the ELF file
PATH_TO_STM32_ELF="/home/ppolach/Desktop/libtropic-stm32/NUCLEO_F439ZI/build/stm32_example.elf"
# Openocd uses full paths, so we need to get the path to this folder
PATH_TO_THIS_FOLDER=`pwd`

echo ""
echo ""
echo "Powering up"
echo ""
echo ""
openocd -f ${PATH_TO_THIS_FOLDER}/ts11-jtag.cfg -c "init;  ftdi_set_signal SPI_EN 1; ftdi_set_signal PLTF_PWR_EN 1; exit"
sleep .7

echo ""
echo ""
echo "Flashing"
echo ""
echo ""
openocd -f ${PATH_TO_THIS_FOLDER}/ts11-jtag.cfg \
-f /usr/share/openocd/scripts/target/stm32f4x.cfg \
-c "init; ftdi_set_signal PLTF_PWR_EN 1; sleep 500; program ${PATH_TO_STM32_ELF} verify exit"
sleep .7

echo ""
echo ""
echo "Reset target"
echo ""
echo ""
openocd -f ${PATH_TO_THIS_FOLDER}/ts11-jtag.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg -c "init; reset; exit"
# Amount of sleep == time needed for test to run
sleep 5

echo ""
echo ""
echo "Powering down"
echo ""
echo ""
openocd -f ${PATH_TO_THIS_FOLDER}/ts11-jtag.cfg -c "init;  ftdi_set_signal SPI_EN 0; ftdi_set_signal PLTF_PWR_EN 0; exit"
