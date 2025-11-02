# NAVION - navion-dev

This is my repository that contains all the files for my NAVION project, a thrust-vector-controlled rocket accompanied by a custom flight computer I designed.

## What's in this repo

- [`Code/`](https://github.com/Arav0001/navion-dev/tree/main/Code) - All the firmware for the flight computer.
    - [`CNTRL/`](https://github.com/Arav0001/navion-dev/tree/main/Code/CNTRL) - Firmware for the control (CNTRL) board, targeting the STM32F405RG (STM32CubeIDE Project).
	- [`NAVIG/`](https://github.com/Arav0001/navion-dev/tree/main/Code/NAVIG) - Firmware for the navigation (NAVIG) board, targeting the STM32F205RG (STM32CubeIDE Project).
	- [`CNTRL ESP32/`](https://github.com/Arav0001/navion-dev/tree/main/Code/CNTRL%20ESP32) - Firmware for the [Adafruit HUZZAH32 ESP32](https://www.adafruit.com/product/3405) that runs a webserver for launch control (PlatformIO Project). Contains launch control webserver assets in `data/`.
	- [`NAVION Ground Control/`](https://github.com/Arav0001/navion-dev/tree/main/Code/NAVION%20Ground%20Control) - Firmware for WIP (Work in progress, not currently used or working) ground control radio running off a [Adafruit Feather RFM95W](https://www.adafruit.com/product/3178) (PlatformIO Project).
- [`Designs/`](https://github.com/Arav0001/navion-dev/tree/main/Designs) - OpenRocket designs and simulations for the rocket.
- [`Schematics/`](https://github.com/Arav0001/navion-dev/tree/main/Schematics) - KiCad schematics and PCB files for the flight computer boards.

## Credits

- [FatFS](https://elm-chan.org/fsw/ff/) -> [driver I used](https://github.com/eziya/STM32_SPI_SDCARD)
- [lbthomsen’s W25QXX Driver](https://github.com/lbthomsen/stm32-w25qxx)
- [Sebastian Magdwick’s AHRS Filter](https://x-io.co.uk/open-source-imu-and-ahrs-algorithms/)
- [Adafruit BMP390 Calibration Routine](https://github.com/adafruit/Adafruit_BMP3XX)
- [Chart.js](https://github.com/chartjs/Chart.js)

## Inspiration

This project drew inspiration from [Joe Barnard's Scout rocket](https://youtu.be/SH3lR2GLgT0) over at [BPS.space](https://BPS.space). Go check him out!

## Contact

Feel free to contact me at <arav.sonawane@gmail.com>
