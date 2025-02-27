#!/usr/bin/env bash
#
## Blink red LED
#openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-jtag.cfg  -c "init; ftdi_set_signal LED_R 1; exit"
#sleep .2
#openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-jtag.cfg  -c "init; ftdi_set_signal LED_R 0; exit"
#sleep .2
#
## Blink green LED
#openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-jtag.cfg  -c "init; ftdi_set_signal LED_G 1; exit"
#sleep .2
#openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-jtag.cfg  -c "init; ftdi_set_signal LED_G 0; exit"
#sleep .2
#
## Blink blue LED
#openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-jtag.cfg  -c "init; ftdi_set_signal LED_B 1; exit"
#sleep .2
#openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-jtag.cfg  -c "init; ftdi_set_signal LED_B 0; exit"
#sleep .2
#
## Blink all LEDs
#openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-jtag.cfg  -c "init; ftdi_set_signal LED_R 1; ftdi_set_signal LED_G 1; ftdi_set_signal LED_B 1; exit"
#sleep .2
#openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-jtag.cfg  -c "init; ftdi_set_signal LED_R 0; ftdi_set_signal LED_G 0; ftdi_set_signal LED_B 0; exit"
#sleep .2


#### IMPORTANT
#### IMPORTANT
#### IMPORTANT
#### IMPORTANT: go to NUCLEO_F439ZI/Src/Main.h and set #define DBG_UART_OVER_USB 0, recompile.

echo ""
echo ""
echo "Powering up"
echo ""
echo ""
openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-jtag.cfg -c "init;  ftdi_set_signal SPI_EN 1; ftdi_set_signal PLTF_PWR_EN 1; exit"
sleep .7

echo ""
echo ""
echo "Flashing"
echo ""
echo ""
openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-jtag.cfg \
-f /usr/share/openocd/scripts/target/stm32f4x.cfg \
-c "init; ftdi_set_signal PLTF_PWR_EN 1; sleep 500; program ../../tests/platforms/libtropic-stm32/NUCLEO_F439ZI/build/stm32_example.elf verify exit"
sleep .7

echo ""
echo ""
echo "Reset target"
echo ""
echo ""
openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-jtag.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg -c "init; reset; exit"
sleep 60

echo ""
echo ""
echo "Powering down"
echo ""
echo ""
openocd -f /home/ppolach/Desktop/libtropic/scripts/test_runner/ts11-jtag.cfg -c "init;  ftdi_set_signal SPI_EN 0; ftdi_set_signal PLTF_PWR_EN 0; exit"
