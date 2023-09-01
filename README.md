# ESP32-OpenLIN

[open-LIN/open-LIN-c](https://github.com/open-LIN/open-LIN-c) implementation in ESP32.

# Introduction
This project [implements hardware abstraction APIs](https://github.com/CW-B-W/ESP32-openLIN/blob/master/src/open_lin_hw_esp32.cpp) of `open-LIN-c` in ESP32 based on [CW-B-W/ESP32-SoftwareLIN](https://github.com/CW-B-W/ESP32-SoftwareLIN).  

Besides of the implementation, there are [simple examples](https://github.com/CW-B-W/ESP32-openLIN/blob/master/examples) to illustrate how to use `open-LIN-c`.  

## Features
* Send / Receive LIN bus frames on both LIN master and LIN slave
* Autobaud detection is supported by LIN slave

# Quick start guide
1. Use VS Code with extension PlatformIO to open the project folder
2. Build and upload the ESP32 firmware with PlatformIO
    * If this device is acting as a LIN master, copy the `examples/master.cpp` to `src/`
    * If this device is acting as a LIN slave, copy the `examples/slave.cpp` to `src/`
3. Connect the UART pins of the ESP32 LIN master and ESP32 LIN slave

# How to use

## To send periodic frames
### On Master
In `examples/master.cpp`, `master_frame_table` is used to store the periodic frames information. `open-LIN-c` keeps iterating through the table and send each frame itme.  
Refer to https://github.com/CW-B-W/ESP32-openLIN/blob/677470aaf2609fb12ef86f3ba0c760aa747735f0/examples/master.cpp#L44-L47

### On Slave
In `examples/master.cpp`, `frame_slot` is used to store the periodic frames information.  
When a framed is received, `open-LIN-c` searches the received frame PID in the `frame_slot` and return the frame information if found.  
Refer to https://github.com/CW-B-W/ESP32-openLIN/blob/677470aaf2609fb12ef86f3ba0c760aa747735f0/examples/slave.cpp#L54-L57

## Modify handlers
### Master on received frame handler
In `examples/master.cpp`, function `open_lin_master_dl_rx_callback` is used to handle the frames received. Refer to https://github.com/CW-B-W/ESP32-openLIN/blob/677470aaf2609fb12ef86f3ba0c760aa747735f0/examples/master.cpp#L84-L91

### Master on error handler
In `examples/master.cpp`, function `open_lin_error_handler` is used to handle the errors. Refer to https://github.com/CW-B-W/ESP32-openLIN/blob/677470aaf2609fb12ef86f3ba0c760aa747735f0/examples/master.cpp#L93-L153

### Slave on received frame handler
In `examples/slave.cpp`, function `open_lin_on_rx_frame` is used to handle the frames received. Refer to https://github.com/CW-B-W/ESP32-openLIN/blob/677470aaf2609fb12ef86f3ba0c760aa747735f0/examples/slave.cpp#L115-L122

### Slave on error handler
In `examples/slave.cpp`, function `open_lin_error_handler` is used to handle the errors. Refer to https://github.com/CW-B-W/ESP32-openLIN/blob/677470aaf2609fb12ef86f3ba0c760aa747735f0/examples/slave.cpp#L124-L185

## Enable/Disable autobaud for LIN slave
In `examples/slave.cpp`, if `LIN_AUTOBAUD` is defined, autobaud is enabled, otherwise autobaud is disabled. Refer to https://github.com/CW-B-W/ESP32-openLIN/blob/677470aaf2609fb12ef86f3ba0c760aa747735f0/examples/slave.cpp#L35

# Testing - Microchip LIN Serial Analyzer
This project was tested with [Microchip LIN Serial Analyzer](https://www.microchip.com/en-us/development-tool/apgdt001) and [NXP TJA1021](https://www.nxp.com/docs/en/data-sheet/TJA1021.pdf) module.  
(Sleep was not used in this test)

## Connection
![](https://github.com/CW-B-W/ESP32-openLIN/assets/76680670/a1ae1a09-027d-4a56-a39f-d6e7e818a5c4)

## ESP32 as Master, LIN Analyzer as Slave

### Setup
1. Use `examples/master.cpp` to do this test.
2. Open [LIN Serial Analyzer Debug Tool](https://www.microchip.com/en-us/software-library/lin_analyzer)
3. Hold ESP32 reset button (Don't release now)
4. Connect Debug Tool with `LIN Serial Analyzer`
5. Choose to use Enhanced Checksum Type in Debug Tool
6. Setup Slave Response in Debug Tool
7. Select the responses and click `Add Slave Response Buffer`
8. Release ESP32 reset button
9. LIN frames should be detected and shown on the screen

### Result
<img width="75%" src="https://github.com/CW-B-W/ESP32-openLIN/assets/76680670/4d62c92f-fe68-4224-aa7c-221c1020bcaa"></img>

Compared with the result in logic analyzer
![](https://github.com/CW-B-W/ESP32-openLIN/assets/76680670/65f8c5ee-6844-4af9-b3d4-06d5ed067e57)

## ESP32 as Slave, LIN Analyzer as Master

### Setup
1. Use `examples/slave.cpp` to do this test.
2. Open [LIN Serial Analyzer Debug Tool](https://www.microchip.com/en-us/software-library/lin_analyzer)
3. Connect Debug Tool with `LIN Serial Analyzer`
4. Choose to use Enhanced Checksum Type in Debug Tool
5. Setup Master frames in Debug Tool
6. Select the two frames and click `Send Continuous`
7. LIN frames should be detected and shown on the screen

### Result
<img width="75%" src="https://github.com/CW-B-W/ESP32-openLIN/assets/76680670/1d270cab-7424-4662-8f48-2380c32b4007"></img>

Compared with the result in logic analyzer
![](https://github.com/CW-B-W/ESP32-openLIN/assets/76680670/65f8c5ee-6844-4af9-b3d4-06d5ed067e57)

# Testing - Vector VN1611
This project was also tested with VN1611.  
We can take advantages of [Vector XL Driver Library](https://www.vector.com/int/en/products/products-a-z/libraries-drivers/xl-driver-library/) to test it with VN1611.
