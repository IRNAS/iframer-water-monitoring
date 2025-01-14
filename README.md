# Ifremer water monitoring


## Introduction

This repository contains all documentation and source code required to setup either a Inductive Conductivity CTZN sensor (marked on sensor as SN-PCTZB) or Optical Dissolved Oxygen OPTOD sensor (marked on sensor as SN-PODOC).
Sensors product page can be found on manufacturers [website][product_page].
The source code was written to work on [B-L072Z-LRWAN1][lora_board], while using Arduino core.

All manufacturers documentation can be found in `aqualabo_docs` folder.
Desktop application for measuring can be found in `aqualabo_software` folder.

## Prerequisites
* B-L072Z-LRWAN1
* Pira Smart RS485 Add-On board, documentation about it can be found in `rs485_addon_hardware` folder
* CTZN sensor or OPTOD sensor
* Arduino library for [Modbus protocol][arduino_library]
* Aqualabo 4200 Digital Module box, needed only for finding ModBus address and calibration.

<p align="center">
  <img src="doc_images/prerequisites.jpg" alt="setup" width="500"><br><br>
  <i>Required setup</i><br>
</p>

[product_page]: https://en.aqualabo.fr/ctzn-digital-sensor-bare-wires-3-m-cable-stainless-steel-pipe-b4002.html
[lora_board]: https://www.st.com/en/evaluation-tools/b-l072z-lrwan1.html
[arduino_library]: https://github.com/4-20ma/ModbusMaster 


## Aqualabo sensors

This library currently supports two sensors:

* CTZN inductive conductivity sensor, address 50
* OPTOD optical oxygen sensor, address 10


## Finding ModBus address of the Aqualabo sensor

The best way to find address of Aqualabo sensor is to use Calsens software, installer for it can be found in `aqualabo_software` folder.
When using Calsens software the Aqualabo 4200 Digital Module box is required.

## Calibrating the Aqualabo sensor

To calibrate the Aqualabo sensor we have to use Calsens software, installer for it can be found in `aqualabo_software` folder.
Calibration procedure is described in user manuals of each specific sensors, as well as in `Modbus_Specifications_v021-EN.pdf` document.
When using Calsens software the Aqualabo 4200 Digital Module box is required.


## Wiring

There are two different wiring setups that need to be taken care of.

### Connections between Aqualabo sensor and Add-on board

As seen on image below Aqualabo sensor has 6 wires, wires numbers 2 (blue) and 6 (green/yellow) are not needed.
Aqualabo sensor has to be connected to one of two ports P1 or P2. Markings for Addon board can be seen on schematic below, there are also silkscreen markings on the bottom side of the board.

<p align="center">
<img src="doc_images/wiring_a.png" width="250"/><img src="doc_images/wiring_b.png" width="500"/><br><br>
  <i>Wiring scheme of Aqualabo sensor and Add-on board</i><br>
</p>


### Connections between Add-on board and microcontroller

Connections are marked on image below, we need to provide following lines:
* 3.3V
* 5V
* GND
* TXD
* RXD
* GPIO18 (UART_SEL_B)
* GPIO23 (UART_SEL_A)
* GPIO24 (UART_INH)
* GPIO25 (DRIVER_EN)

**Important**: We actually do not need to switch UART lines when connection Add-on board to the microcontroller.
There is a mistake in naming in schematic and lines are already switched.
We therefore connect TX line from microcontroller to the TXD of Raspberry Pi header, same goes for RX and RXD.

<p align="center">
  <img src="doc_images/wiring_2.png" alt="setup"><br><br>
  <i> Connections on Add-on board</i><br>
</p>


## Hardware changes on the Add-on board

There were some changes needed on the board to make communication possible.
This changes are not visible in schematic and are described here.

Changes:
* R7 0 ohm resistor should be placed
* R10 0 ohm resistor should not be placed
* R11 10 Kohm resistor should be replaced with 0 ohm resistor

Additional changes are needed to achieve low current consumption on Add-on V1 board:
* Remove chip U4 uart mux chip
* Connect together with a wire solder pads 1 and 3 left behind U4 chip.
* Connect together with a wire solder pads 12 and 13 left behind U4 chip.
* Solder on 3.3V low drop regulator and connect to its input 5V line, then connect to its output 3V line of the Add-on board.

For required connections refer to the image below.

<p align="center">
<img src="doc_images/addon_modification.png" width="250"/><img src="doc_images/addon_modification.png" width="500"/><br><br>
  <i> Low current consumption modifications on Add-on board</i><br>
</p>
