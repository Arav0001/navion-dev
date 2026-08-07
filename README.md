# NAVION - navion-dev
NAVION is a thrust-vector-controlled rocket accompanied by a custom two-board flight computer I designed, built, and flew.

<img width="230" height="500" src="https://github.com/user-attachments/assets/80083aed-1401-477e-92dc-700b032ebbf4" />

## Results
NAVION's maiden flight reached a ~47 m apogee. The active TVC control loop tracked pitch & yaw through boost and was able to correct 4.5° of yaw error before losing control due to overwhelming pitch error and low motor power, after which the vehicle coasted and descended under parachute to a detected touchdown.
 
### Flight profile
Altitude, vertical velocity, and vertical acceleration across boost, descent, and touchdown, reconstructed from onboard sensor data, see [maiden flight data](FlightData/NAVION-1_DATA.csv):
 
<img width="1200" height="1350" alt="P_FLIGHTPROFILE" src="https://github.com/user-attachments/assets/51e62f15-54e9-412f-85c1-768aeb93697f" />
 
### TVC response
Tracked gimbal deflection (X/Y) for pitch & yaw during the boost phase, up to TVC lockout at burnout:
 
<img width="1200" height="1350" alt="P_TVCRESPONSE" src="https://github.com/user-attachments/assets/61392942-420c-4e1b-87b6-73454a660d6a" />
 
### Test footage
<video src="https://github.com/user-attachments/assets/dc627b79-1247-45d9-82b1-cf85502bb5d8" controls></video>
*Ground test of the black-powder parachute ejection system.*

## Hardware
 
### Flight computer
Two-board avionics stack: an STM32F4 control board and STM32F2 navigation board communicating over UART DMA, carrying a 9-axis IMU, barometer, GNSS receiver, SPI flash, and µSD storage.
 
<img width="2160" height="1215" alt="P_FC_DIAG8" src="https://github.com/user-attachments/assets/0b1c7a80-e261-475a-859e-54589f46b425" />
 
### TVC gimbal
3D-printed 2-axis gimbal actuating the solid rocket motor with ±5° of pitch/yaw deflection, driven by two digital servos:
 
<img width="4284" height="5712" alt="P_TVC" src="https://github.com/user-attachments/assets/7e8f95bb-d7b9-44c7-93b2-57e65b54f246" />

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
Feel free to contact me at <arav.sonawane@gatech.edu>
