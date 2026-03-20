![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

# MPLAB® Harmony 3 Smart Energy G3 application examples

MPLAB® Harmony 3 is an extension of the MPLAB® ecosystem for creating embedded firmware solutions for Microchip 32-bit SAM and PIC® microcontroller and microprocessor devices. Refer to the following links for more information.

- [Microchip 32-bit MCUs](https://www.microchip.com/design-centers/32-bit)
- [Microchip 32-bit MPUs](https://www.microchip.com/design-centers/32-bit-mpus)
- [Microchip MPLAB X IDE](https://www.microchip.com/mplab/mplab-x-ide)
- [Microchip MPLAB® Harmony](https://www.microchip.com/mplab/mplab-harmony)
- [Microchip MPLAB® Harmony Pages](https://microchip-mplab-harmony.github.io/)

This repository contains the MPLAB® Harmony 3 Smart Energy G3-PLC and G3-Hybrid application examples.

Refer to the following links for release notes, training materials, and interface reference information.
- [Release Notes](./release_notes.md)
- [License](License.md)

# Documentation

Click [here](https://onlinedocs.microchip.com/v2/keyword-lookup?keyword=SE_G3_apps_Introduction&redirect=true) to view the online documentation of MPLAB® Harmony 3 Smart Energy library repository.

To view the documentation offline, follow these steps:
 - Download the publication as a zip file from [here](https://onlinedocs.microchip.com/download/GUID-B127057F-C502-44FE-A992-E011A3AF104D?type=webhelp).
 - Extract the zip file into a folder.
 - Navigate to the folder and open **index.html** in a web browser of your choice.

# Contents Summary

| Folder     | Description                             |
| ---        | ---                                     |
| apps       | Contains G3 PHY, MAC-RT and full-stack example applications. |

# Code Examples

The following applications are provided to demonstrate the typical use cases of G3 at both PHY and full stack levels.

| Name               | Description |
| ----               | ----------- |
| [PHY PLC and Go](https://onlinedocs.microchip.com/v2/keyword-lookup?keyword=SE_PHY_PLC_Go&redirect=true) | This example is intended to show a simple application running on top of the G3-PLC PHY layer. |
| [PHY Tester Tool](https://onlinedocs.microchip.com/v2/keyword-lookup?keyword=SE_PHY_Tester_Tool&redirect=true) | The PHY Tester tool is an application example that allows checking the complete performance of the Microchip G3-PLC PHY Layer on PLC boards. |
| [Hybrid PHY Tester Tool](https://onlinedocs.microchip.com/v2/keyword-lookup?keyword=SE_PHY_Tester_Tool&redirect=true) | The Hybrid PHY Tester tool is an application example that allows checking the complete performance of the Microchip G3-PLC and IEEE 802.15.4 PHY Layers on Hybrid PLC-RF boards. |
| [PHY Sniffer Tool](https://onlinedocs.microchip.com/v2/keyword-lookup?keyword=SE_PHY_Sniffer_Tool&redirect=true) | The PHY Sniffer is an application example that uses the PHY layer to monitor PLC frames in the G3-PLC network and send them via USI serialization. |
| [Hybrid PHY Sniffer Tool](https://onlinedocs.microchip.com/v2/keyword-lookup?keyword=SE_PHY_Sniffer_Tool&redirect=true) | The Hybrid PHY Sniffer is an application example that uses the PHY layers to monitor PLC and RF frames in the G3-Hybrid network and send them via USI serialization. |
| [PLC PHY Tx Test Console](https://onlinedocs.microchip.com/v2/keyword-lookup?keyword=SE_PLC_PHY_TX_Test_console&redirect=true) | The PLC PHY Tx Test Console is an application example that demonstrates the complete transmission performance of the Microchip G3-PLC PHY Layer, avoiding timing limitations in the PC host. |
| [MAC-RT PLC and Go](https://onlinedocs.microchip.com/v2/keyword-lookup?keyword=SE_MAC_RT_PLC_Go&redirect=true) | This example is intended to show a simple application running on top of the G3-PLC Real-Time MAC (MAC-RT) layer. |
| [My First PLC App](https://onlinedocs.microchip.com/v2/keyword-lookup?keyword=SE_your_first_plc_application_mtg&redirect=true) | This is the example used in the *Getting Started with MPLAB®; Harmony PLC Stack* tutorial given in the Smart Energy documentation (*smartenergy* repository). |
| [G3 Modem](https://onlinedocs.microchip.com/v2/keyword-lookup?keyword=SE_G3_Modem&redirect=true) | This example provides access to G3 MAC and ADP layers through USI serialization. |
| [G3 Device UDP/ICMPv6 Responder](https://onlinedocs.microchip.com/v2/keyword-lookup?keyword=SE_G3_Device_UDP&redirect=true) | This example implements a G3-Hybrid Device, capable of joining a G3 network and replying to UDP and ICMPv6 requests. It can be used to pass the G3 Conformance certification tests. |
| [G3 Coordinator UDP Cycles/Responder](https://onlinedocs.microchip.com/v2/keyword-lookup?keyword=SE_G3_Coordinator_UDP&redirect=true) | This example implements a G3-Hybrid Coordinator, capable of creating a G3 network and cycling the joined devices with UDP requests. It also replies to UDP and ICMPv6 requests and can be used to pass the G3 Conformance certification tests. |
| [G3 Coordinator ICMPv6 Cycles](https://onlinedocs.microchip.com/v2/keyword-lookup?keyword=SE_G3_Coordinator_ICMP&redirect=true) | This example implements a G3-Hybrid Coordinator, capable of creating a G3 network and cycling the joined devices with ICMPv6 Echo Requests. |
| [Metering Demo G3 Device UDP/ICMPv6 Responder](https://onlinedocs.microchip.com/v2/keyword-lookup?keyword=SE_Metering_Demo_G3_Device_UDP&redirect=true) | This example implements a metering application and a G3-Hybrid Device, capable of joining a G3 network and replying to UDP and ICMPv6 requests. It demonstrates how to send metrology data through UDP. It can be used to pass the G3 Conformance certification tests. |

____

[![License](https://img.shields.io/badge/license-Harmony%20license-orange.svg)](https://github.com/Microchip-MPLAB-Harmony/smartenergy_g3_apps/blob/master/License.md)
[![Commit activity](https://img.shields.io/github/commit-activity/y/Microchip-MPLAB-Harmony/smartenergy_g3_apps.svg)](https://github.com/Microchip-MPLAB-Harmony/smartenergy_g3_apps/graphs/commit-activity)
[![Contributors](https://img.shields.io/github/contributors-anon/Microchip-MPLAB-Harmony/smartenergy_g3_apps.svg)]()

____

[![Developer Help](https://img.shields.io/badge/Youtube-Developer%20Help-red.svg)](https://www.youtube.com/MicrochipDeveloperHelp)
[![Developer Help](https://img.shields.io/badge/XWiki-Developer%20Help-torquiose.svg)](https://developerhelp.microchip.com/xwiki/bin/view/software-tools/harmony/)
[![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/user/MicrochipTechnology)
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/microchip-technology)
[![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/microchiptechnology/)
[![Follow us on Twitter](https://img.shields.io/twitter/follow/MicrochipTech.svg?style=social)](https://twitter.com/MicrochipTech)

[![](https://img.shields.io/github/stars/Microchip-MPLAB-Harmony/smartenergy_g3_apps.svg?style=social)]()
[![](https://img.shields.io/github/watchers/Microchip-MPLAB-Harmony/smartenergy_g3_apps.svg?style=social)]()