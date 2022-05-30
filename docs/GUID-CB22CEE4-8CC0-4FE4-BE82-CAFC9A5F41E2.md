# Application Example

Microchip PLC & Go application is a point to multi-point chat application built on top of the PHY layer of state-of-the-art G3-PLC protocol.

Note that `phy_plc_and_go` application example can be used with Microchip G3-PLC stack version 1.4.1 or above in any Microchip platforms implementing PL360/PL460 device as PLC modem.

This application example assumes that a host device \(a PC, for example\) is connected through a serial interface \(USB, UART\) to a Microchip evaluation kit acting as a PLC modem.

The figure below provides an example of the minimum hardware and firmware resources required to run PLC & Go.

![](GUID-7C8B71F0-B089-48D4-968D-A3C35B19EB74-low.png "PLC & Go Block Diagram")

The PLC & Go application interchanges data with the PC through a serial port using a terminal emulator \(see figure PLC & Go console where terminal emulator TeraTerm is used\). The settings of the serial port are: 921600 bps, 8 data bits, no parity, 1 stop bit and flow control.

When the serial port is opened, the PLC & Go application shows in the console some information about the hardware platform and the firmware running, and it waits for a message to be sent. The message is sent when a carriage return character is received or the maximum length of the PLC data message is reached. If the user sends the ASCII character 0x13 \(‘CTRL+S’\), a configuration menu is shown.

The PLC & Go application allows to:

-   Configure modulation type and modulation scheme
-   Configure the band of transmission \(in case of using a Microchip evaluation board that supports several bands of transmission\).

In addition, the application provides some information about the transmitted messages \(length of the message\) and the received messages \(Modulation type and scheme, signal quality\), which can be displayed by means of a terminal emulator in the PC.

![](GUID-E5E793E2-188B-42FD-9F42-E2C926C5EC9A-low.png "PLC & Go Console")

In the example project, the `app_plc.c` file contains:

-   PLC initialization and configuration functions
-   Handlers for received messages, transmission and events

The `app_console.c` file contains the chat application which communicates with the PC using the serial port.

**Parent topic:**[PHY PLC & Go](GUID-E87515D0-BD4F-4456-8D21-13BD460238A8.md)

