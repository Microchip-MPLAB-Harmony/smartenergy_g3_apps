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

- [Release Notes](./release_notes.md)
- [MPLAB® Harmony License](Microchip_SLA001.md)
- [MPLAB® Harmony 3 Smart Energy Examples Help](https://onlinedocs.microchip.com/g/GUID-B127057F-C502-44FE-A992-E011A3AF104D-en-US-1/index.html?output=oxygen)


To clone or download these applications from Github, go to the [main page of this repository](https://github.com/Microchip-MPLAB-Harmony/smartenergy_g3_apps) and then click **Clone** button to clone this repository or download as zip file.
This content can also be downloaded using content manager by following these [instructions](https://github.com/Microchip-MPLAB-Harmony/contentmanager/wiki).

# Contents Summary

| Folder     | Description                             |
| ---        | ---                                     |
| apps       | Contains G3 PHY, MAC-RT and full-stack example applications. |
| docs       | Contains documentation in html format for offline viewing (to be used only after cloning this repository onto a local machine). Use [onlinedocs](https://onlinedocs.microchip.com/g/GUID-B127057F-C502-44FE-A992-E011A3AF104D-en-US-1/index.html?output=oxygen) of this repository for viewing it online. |

# Code Examples

The following applications are provided to demonstrate the typical use cases of G3 at both PHY and full stack levels.

| Name               | Description |
| ----               | ----------- |
| [PHY PLC and Go](https://onlinedocs.microchip.com/oxy/GUID-B127057F-C502-44FE-A992-E011A3AF104D-en-US-1/GUID-E87515D0-BD4F-4456-8D21-13BD460238A8.html) | This example is intended to show a simple application running on top of the G3-PLC PHY layer. |
| [PHY Tester Tool](https://onlinedocs.microchip.com/oxy/GUID-B127057F-C502-44FE-A992-E011A3AF104D-en-US-1/GUID-A2A3C4CF-8C73-4F62-9809-2B710A7783A0.html) | The PHY Tester tool is an application example that allows checking the complete performance of the Microchip G3-PLC PHY Layer on PLC boards. |
| [Hybrid PHY Tester Tool](https://onlinedocs.microchip.com/oxy/GUID-B127057F-C502-44FE-A992-E011A3AF104D-en-US-1/GUID-A2A3C4CF-8C73-4F62-9809-2B710A7783A0.html) | The Hybrid PHY Tester tool is an application example that allows checking the complete performance of the Microchip G3-PLC and IEEE 802.15.4 PHY Layers on Hybrid PLC-RF boards. |
| [PHY Sniffer Tool](https://onlinedocs.microchip.com/oxy/GUID-B127057F-C502-44FE-A992-E011A3AF104D-en-US-1/GUID-DE964FB2-4D50-4330-8A62-08DDE4711A8E.html) | The PHY Sniffer is an application example that uses the PHY layer to monitor PLC frames in the G3-PLC network and send them via USI serialization. |
| [Hybrid PHY Sniffer Tool](https://onlinedocs.microchip.com/oxy/GUID-B127057F-C502-44FE-A992-E011A3AF104D-en-US-1/GUID-DE964FB2-4D50-4330-8A62-08DDE4711A8E.html) | The Hybrid PHY Sniffer is an application example that uses the PHY layers to monitor PLC and RF frames in the G3-Hybrid network and send them via USI serialization. |
| [PLC PHY Tx Test Console](https://onlinedocs.microchip.com/oxy/GUID-B127057F-C502-44FE-A992-E011A3AF104D-en-US-1/GUID-77B23EFD-7164-495F-9C75-4BE5D27443B1.html) | The PLC PHY Tx Test Console is an application example that demonstrates the complete transmission performance of the Microchip G3-PLC PHY Layer, avoiding timing limitations in the PC host. |
| [MAC-RT PLC and Go](https://onlinedocs.microchip.com/oxy/GUID-B127057F-C502-44FE-A992-E011A3AF104D-en-US-1/GUID-CE3EF484-E886-4378-90B5-20C2BC7A43D1.html) | This example is intended to show a simple application running on top of the G3-PLC Real-Time MAC (MAC-RT) layer. |
| My First PLC App | This is the example used in the *Getting Started with MPLAB®; Harmony PLC Stack* tutorial given in the Smart Energy documentation (*smartenergy* repository). |
| [G3 Modem](https://onlinedocs.microchip.com/oxy/GUID-B127057F-C502-44FE-A992-E011A3AF104D-en-US-1/GUID-3F71506A-1159-481C-A1D4-ECC27EAB7A6B.html) | This example provides access to G3 MAC and ADP layers through USI serialization. |
| [G3 Device UDP/ICMPv6 Responder](https://onlinedocs.microchip.com/oxy/GUID-B127057F-C502-44FE-A992-E011A3AF104D-en-US-1/GUID-584FC808-FADD-4FBF-9DED-E84D5271738A.html) | This example implements a G3-Hybrid Device, capable of joining a G3 network and replying to UDP and ICMPv6 requests. It can be used to pass the G3 Conformance certification tests. |
| [G3 Coordinator UDP Cycles/Responder](https://onlinedocs.microchip.com/oxy/GUID-B127057F-C502-44FE-A992-E011A3AF104D-en-US-1/GUID-BFCA859E-7CCD-47B6-9145-9FACBA1A06C3.html) | This example implements a G3-Hybrid Coordinator, capable of creating a G3 network and cycling the joined devices with UDP requests. It also replies to UDP and ICMPv6 requests and can be used to pass the G3 Conformance certification tests. |
| [G3 Coordinator ICMPv6 Cycles](https://onlinedocs.microchip.com/oxy/GUID-B127057F-C502-44FE-A992-E011A3AF104D-en-US-1/GUID-F2A5447D-AEF1-4720-B1FA-506CC0819442.html) | This example implements a G3-Hybrid Coordinator, capable of creating a G3 network and cycling the joined devices with ICMPv6 Echo Requests. |
| [Metering Demo G3 Device UDP/ICMPv6 Responder](https://onlinedocs.microchip.com/oxy/GUID-B127057F-C502-44FE-A992-E011A3AF104D-en-US-1/GUID-2EF71D08-349B-41D7-8AD9-8395EE243D44.html) | This example implements a metering application and a G3-Hybrid Device, capable of joining a G3 network and replying to UDP and ICMPv6 requests. It demonstrates how to send metrology data through UDP. It can be used to pass the G3 Conformance certification tests. |

____

[![License](https://img.shields.io/badge/license-Harmony%20license-orange.svg)](https://github.com/Microchip-MPLAB-Harmony/smartenergy_g3_apps/blob/master/Microchip_SLA001.md)
[![Latest release](https://img.shields.io/github/release/Microchip-MPLAB-Harmony/smartenergy_g3_apps.svg)](https://github.com/Microchip-MPLAB-Harmony/smartenergy_g3_apps/releases/latest)
[![Latest release date](https://img.shields.io/github/release-date/Microchip-MPLAB-Harmony/smartenergy_g3_apps.svg)](https://github.com/Microchip-MPLAB-Harmony/smartenergy_g3_apps/releases/latest)
[![Commit activity](https://img.shields.io/github/commit-activity/y/Microchip-MPLAB-Harmony/smartenergy_g3_apps.svg)](https://github.com/Microchip-MPLAB-Harmony/smartenergy_g3_apps/graphs/commit-activity)
[![Contributors](https://img.shields.io/github/contributors-anon/Microchip-MPLAB-Harmony/smartenergy_g3_apps.svg)]()

____

[![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/user/MicrochipTechnology)
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/microchip-technology)
[![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/microchiptechnology/)
[![Follow us on Twitter](https://img.shields.io/twitter/follow/MicrochipTech.svg?style=social)](https://twitter.com/MicrochipTech)

[![](https://img.shields.io/github/stars/Microchip-MPLAB-Harmony/smartenergy_g3_apps.svg?style=social)]()
[![](https://img.shields.io/github/watchers/Microchip-MPLAB-Harmony/smartenergy_g3_apps.svg?style=social)]()