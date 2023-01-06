# ARM (CORTEX-M)  YOLO Demo

## Project Introduction
This project is to demonstrate the transplantation of `YOLO` neural network model to the microcontroller. MCU used is `STM32F746NGH6U` chip is `ARM CORTEX - M7` series.

## Effect Demonstration
The following is a real machine demonstration (`STM32F746G_DISCO`)

<p align="center">
  <img alt="demo" src="../../../../img/1.jpg">
</p>
<p align="center">
  <img alt="demo" src="../../../../img/2.jpg">
</p>

At the same time, the lower computer will also output the coordinates of the box, which are respectively x1, y1 (upper left corner), x2, y2 (lower left corner)
```bash
LOG: 11, 11, 44, 42
```

## Build Project
### Build Firmware
Install cross compiler and builder
```bash
sudo apt install make gcc-arm-none-eabi
```
compile
```bash
make
```
After compilation, the build directory will be generated in the current directory, and the `arm-none-eabi-objcopy` tool will be used to convert elf into `hex` and `bin` files
```bash
&:ls *.hex *.bin *.elf
stm32f746g_disco.hex
bstm32f746g_disco.bin
stm32f746g_disco.elf
```
For convenience, I provide the configuration file of openocd in the directory, of course, only for `STM32F74G_DISCO`
The configuration and use of openocd are ported to `zephyr rtos`
```bash
./openocd/bin/openocd -s openocd/boards/arm/stm32f746g_disco/support -s openocd/sysroots/x86_64-pokysdk-linux/usr/share/openocd/scripts -f openocd/boards/arm/stm32f746g_disco/support/openocd.cfg '-c init' '-c targets' -c 'reset halt' -c 'flash write_image erase ./build/stm32f746g_disco.hex' -c 'reset halt' -c 'verify_image ./build/stm32f746g_disco.hex' -c 'reset run' -c shutdown
```
if the burning is successful, you should see the following picture:
<p align="center">
  <img alt="test" src="../../../../img/test.jpg">
</p>
