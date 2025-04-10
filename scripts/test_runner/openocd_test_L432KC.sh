#!/usr/bin/env bash
#
## Blink red LED
#openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-swd.cfg  -c "init; ftdi_set_signal LED_R 1; exit"
#sleep .2
#openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-swd.cfg  -c "init; ftdi_set_signal LED_R 0; exit"
#sleep .2
#
## Blink green LED
#openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-swd.cfg  -c "init; ftdi_set_signal LED_G 1; exit"
#sleep .2
#openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-swd.cfg  -c "init; ftdi_set_signal LED_G 0; exit"
#sleep .2
#
## Blink blue LED
#openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-swd.cfg  -c "init; ftdi_set_signal LED_B 1; exit"
#sleep .2
#openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-swd.cfg  -c "init; ftdi_set_signal LED_B 0; exit"
#sleep .2
#
## Blink all LEDs
#openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-swd.cfg  -c "init; ftdi_set_signal LED_R 1; ftdi_set_signal LED_G 1; ftdi_set_signal LED_B 1; exit"
#sleep .2
#openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-swd.cfg  -c "init; ftdi_set_signal LED_R 0; ftdi_set_signal LED_G 0; ftdi_set_signal LED_B 0; exit"
#sleep .2


#### IMPORTANT
#### IMPORTANT
#### IMPORTANT
#### IMPORTANT: go to NUCLEO_F439ZI/Src/Main.h and set #define DBG_UART_OVER_USB 0, recompile.


#!/usr/bin/env bash

##########################################################################################################
# Usage:
# [1] Pass path to the ELF file which will be flashed, for example:
# ./openocd_test_F439ZI.sh "/home/xxx/Desktop/libtropic-stm32/NUCLEO_F439ZI/build/stm32_example.elf"
# [2] Update amount of sleep to match the time needed for the test to run

##########################################################################################################

PATH_TO_STM32_ELF="$1"
TEST_EXECUTION_IN_SECONDS="$2"

PATH_TO_THIS_FOLDER=`pwd`

# Check if the ELF file path is provided
if [ -z "$PATH_TO_STM32_ELF" ]; then
  echo "Error: No ELF file path provided."
  echo "Usage: $0 <path_to_elf_file> <test_execution_in_seconds>"
  exit 1
fi

# Check if the test execution time is provided
if [ -z "$TEST_EXECUTION_IN_SECONDS" ]; then
  echo "Error: No test execution time provided."
  echo "Usage: $0 <path_to_elf_file> <test_execution_in_seconds>"
  exit 1
fi


echo ""
echo ""
echo "Powering up"
echo ""
echo ""
openocd -f ${PATH_TO_THIS_FOLDER}/ts11-swd.cfg -c "init;  ftdi_set_signal SPI_EN 1; ftdi_set_signal PLTF_PWR_EN 1; exit"
sleep .7

echo ""
echo ""
echo "Flashing"
echo ""
echo ""
openocd -f ${PATH_TO_THIS_FOLDER}/ts11-swd.cfg \
-f /usr/share/openocd/scripts/target/stm32l4x.cfg \
-c "init; ftdi_set_signal PLTF_PWR_EN 1; sleep 500; program ${PATH_TO_STM32_ELF} verify exit"
sleep .7

echo ""
echo ""
echo "Reset target"
echo ""
echo ""
openocd -f ${PATH_TO_THIS_FOLDER}/ts11-swd.cfg -f /usr/share/openocd/scripts/target/stm32l4x.cfg -c "init; reset; exit"
sleep ${TEST_EXECUTION_IN_SECONDS}

echo ""
echo ""
echo "Powering down"
echo ""
echo ""
openocd -f ${PATH_TO_THIS_FOLDER}/ts11-swd.cfg -c "init;  ftdi_set_signal SPI_EN 0; ftdi_set_signal PLTF_PWR_EN 0; exit"
