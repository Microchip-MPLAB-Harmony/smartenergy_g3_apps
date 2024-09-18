![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

# Microchip MPLAB® Harmony 3 Release Notes

## Harmony 3 Smart Energy G3 application examples v1.1.1

### Development kit and demo application support

The following development kits are used on provided G3 Demo Applications:

- [SAM E70 Xplained Ultra Evaluation Kit](https://www.microchip.com/en-us/development-tool/DM320113)
- [PIC32CXMTG-EK Evaluation Kit](https://www.microchip.com/en-us/development-tool/EV11K09A)
- [PIC32CXMTSH-DB Evaluation Kit](https://www.microchip.com/en-us/development-tool/EV84M21A)
- [SAM D20 XPLAINED PRO EVALUATION KIT](https://www.microchip.com/en-us/development-tool/ATSAMD20-XPRO)
- [WBZ451 Curiosity Development Board](https://www.microchip.com/en-us/development-tool/EV96B94A)
- [PL460 Evaluation Kit](https://www.microchip.com/en-us/development-tool/EV13L63A)
- [ATREB215-XPRO-A EXTENSION BOARD](https://www.microchip.com/en-us/development-tool/ATREB215-XPRO-A)

### New Features

No new functionalities or apps added in this release.
Applications updated to use the code from new releases of dependent Harmony 3 modules.
The following table provides a list of available applications, supported platforms and a brief description of functionalities:

| Application | Platform | Description |
| ----------- | -------- | ----------- |
| PHY PLC and Go | [PIC32CX-MTG, SAMD20, SAME70] + PL460-EK | PLC Chat Demo application using PLC PHY API |
| PHY Tester Tool | [PIC32CX-MTG, PIC32CX-MTSH, SAMD20, SAME70, WBZ451] + PL460-EK | PLC PHY application to send/receive frames using Microchip PLC PHY Tester Tool or Python libraries through a Serial Link on development board |
| PHY Tester Hybrid Tool | [PIC32CX-MTG, SAME70] + PL460-EK + ATREB215-XPRO-A | PLC & RF PHY application to send/receive frames using Python libraries through a Serial Link on development board |
| PHY Sniffer Tool | [PIC32CX-MTG, SAMD20, SAME70] + PL460-EK | PHY application to spy traffic on PLC medium and send it for graphical presentation connected to Microchip Hybrid Sniffer Tool |
| PHY Sniffer Hybrid Tool | [PIC32CX-MTG, SAME70] + PL460-EK + ATREB215-XPRO-A | PHY application to spy traffic on PLC & RF media and send it for graphical presentation connected to Microchip Hybrid Sniffer Tool |
| PHY Tx Test Console | [PIC32CX-MTG, PIC32CX-MTSH, SAMD20, SAME70] + PL460-EK | PLC PHY demo application to manage PLC transmissions via serial console |
| MAC RT PLC and Go | [PIC32CX-MTG, SAMD20, SAME70, WBZ451] + PL460-EK | PLC MAC RT Chat Demo application using G3_MAC_RT API |
| G3 Coordinator ICMP Cycles | PIC32CX-MTG + PL460-EK + ATREB215-XPRO-A, SAMD20 + PL460-EK | G3 Full PAN-Coordinator implementation including IPv6 layer, with an application which sends periodic ICMP frames to every G3 Node registered to its PAN |
| G3 Coordinator UDP | PIC32CX-MTG + PL460-EK + ATREB215-XPRO-A, [SAMD20, WBZ451] + PL460-EK | G3 Full PAN-Coordinator implementation including IPv6 layer, with an application which sends periodic UDP frames to every G3 Node registered to its PAN, and an application implementing a UDP responder as defined for G3 Conformance testing |
| G3 Device UDP | PIC32CX-MTG + PL460-EK + ATREB215-XPRO-A, [SAMD20, WBZ451] + PL460-EK | G3 PAN-Device implementation including IPv6 layer, with an application implementing a UDP responder as defined for G3 Conformance testing |
| G3 Modem | PIC32CX-MTG + PL460-EK + ATREB215-XPRO-A, SAMD20 + PL460-EK | G3 Node serialized on top of G3 Adaptation Layer, which can be initialized as G3 PAN-Coordinator or G3 Device, and further controlled via serialization |
| Metering Demo G3 Device UDP | PIC32CX-MTSH + PL460-EK + ATREB215-XPRO-A | A complete example of the most common functionalities included in an electricity meter running on PIC32CX-MTSH device, including PLC and RF communication capabilities by implementing a complete G3 PAN-Device |

### G3 Certification Profile

The provided application examples:

- G3 Coordinator UDP
- G3 Device UDP
- Metering Demo G3 Device UDP

Can be used to perform an official G3 Alliance Certification.

To achieve the functionality required for Certification, some changes have to be manually done in the code:

- Enable *APP_G3_MANAGEMENT_CONFORMANCE_TEST* macro on file *\src\app_g3_management.h*.
- On function *_TCPIPStackCreateTimer()* form file *\src\config\[platform_name]\library\tcpip\src\tcpip_manager.c*, set *stackLinkTmo* variable to **0** so TCPIP task is called immediately after the Link becomes available. This is necessary for certain tests where an IP frame has to be responded just after node is registered.
- On file *\src\config\[platform_name]\configuration.h*, replace *TCPIP_NETWORK_CONFIG_IPV6_UNICAST_RS* macro with *TCPIP_NETWORK_CONFIG_IPV6_NO_RS* value. This avoids the sending of any RS frame by the node, as requested by certification tests.

### Known Issues

- On example applications using Cryptography, file *\src\third_party\wolfssl\wolfcrypt\src\dh.c* has to be reverted after regeneration, to avoid a warning from compiler which throws a compilation error.
- On example applications for SAMD20 platform, file *\src\config\sam_d20_xpro_pl460\peripheral\sercom\spi_master\plib_sercom0_spi_master.c* has to be reverted after regeneration, to preserve some required initialization values which are deleted when file is regenerated.
- Case mismatch between #include statements and filenames throw compilation errors on non-Windows environments. They have to be manually fixed by user.

### Development Tools

- [MPLAB® X IDE v6.20](https://www.microchip.com/mplab/mplab-x-ide)
- [MPLAB® XC32 C/C++ Compiler v4.40](https://www.microchip.com/mplab/compilers)
- MPLAB® X IDE plug-ins:
  - MPLAB® Code Configurator 5.5.1 or higher
- [Microchip PLC PHY Tester Tool v3.1.3](https://www.microchip.com/en-us/software-library/se_plc_phy_tester_tool)
- [Microchip Hybrid Sniffer v2.0.4](https://www.microchip.com/en-us/software-library/se_plc_sniffer)

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see smartenergy_g3_apps/package.yml):

- Harmony smartenergy repository, v1.2.1
  - Harmony gfx repository, v3.13.0
- Harmony smartenergy\_g3 repository, v1.0.1
  - Harmony crypto repository, v4.0.0-E1
  - Harmony net repository, v3.12.0
- Harmony core repository, v3.13.5
- Harmony csp repository, v3.19.1
- Harmony wolfssl repository, v5.6.7-E1
- Harmony wireless\_15\_4\_phy repository, v1.2.0
- Harmony wireless\_pic32cxbz\_wbz repository, v1.2.0
- Harmony usb repository, 3.12.0 for demos requiring USB
- CMSIS-FreeRTOS v10.5.1 (https://github.com/ARM-software/CMSIS-FreeRTOS/tree/v10.5.1) for demos requiring FreeRTOS support

## Harmony 3 Smart Energy G3 application examples v1.1.0

### Development kit and demo application support

The following development kits are used on provided G3 Demo Applications:

- [SAM E70 Xplained Ultra Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/PartNO/DM320113)
- [PIC32CXMTG-EK Evaluation Kit](https://www.microchip.com/en-us/development-tool/EV11K09A)
- [PIC32CXMTSH-DB Evaluation Kit](https://www.microchip.com/en-us/development-tool/EV84M21A)
- [SAM D20 XPLAINED PRO EVALUATION KIT](https://www.microchip.com/en-us/development-tool/ATSAMD20-XPRO)
- [WBZ451 Curiosity Development Board](https://www.microchip.com/en-us/development-tool/EV96B94A)
- [PL460 Evaluation Kit](https://www.microchip.com/en-us/development-tool/EV13L63A)
- [ATREB215-XPRO-A EXTENSION BOARD](https://www.microchip.com/en-us/development-tool/ATREB215-XPRO-A)

### New Features

The following table provides a list of available applications, supported platforms and a brief description of functionalities:

| Application | Platform | Description |
| ----------- | -------- | ----------- |
| PHY PLC and Go | [PIC32CX-MTG, SAMD20, SAME70] + PL460-EK | PLC Chat Demo application using PLC PHY API |
| PHY Tester Tool | [PIC32CX-MTG, PIC32CX-MTSH, SAMD20, SAME70, WBZ451] + PL460-EK | PLC PHY application to send/receive frames using Microchip PLC PHY Tester Tool or Python libraries through a Serial Link on development board |
| PHY Tester Hybrid Tool | [PIC32CX-MTG, SAME70] + PL460-EK + ATREB215-XPRO-A | PLC & RF PHY application to send/receive frames using Python libraries through a Serial Link on development board |
| PHY Sniffer Tool | [PIC32CX-MTG, SAMD20, SAME70] + PL460-EK | PHY application to spy traffic on PLC medium and send it for graphical presentation connected to Microchip Hybrid Sniffer Tool |
| PHY Sniffer Hybrid Tool | [PIC32CX-MTG, SAME70] + PL460-EK + ATREB215-XPRO-A | PHY application to spy traffic on PLC & RF media and send it for graphical presentation connected to Microchip Hybrid Sniffer Tool |
| PHY Tx Test Console | [PIC32CX-MTG, PIC32CX-MTSH, SAMD20, SAME70] + PL460-EK | PLC PHY demo application to manage PLC transmissions via serial console |
| MAC RT PLC and Go | [PIC32CX-MTG, SAMD20, SAME70, WBZ451] + PL460-EK | PLC MAC RT Chat Demo application using G3_MAC_RT API |
| G3 Coordinator ICMP Cycles | PIC32CX-MTG + PL460-EK + ATREB215-XPRO-A, SAMD20 + PL460-EK | G3 Full PAN-Coordinator implementation including IPv6 layer, with an application which sends periodic ICMP frames to every G3 Node registered to its PAN |
| G3 Coordinator UDP | PIC32CX-MTG + PL460-EK + ATREB215-XPRO-A, [SAMD20, WBZ451] + PL460-EK | G3 Full PAN-Coordinator implementation including IPv6 layer, with an application which sends periodic UDP frames to every G3 Node registered to its PAN, and an application implementing a UDP responder as defined for G3 Conformance testing |
| G3 Device UDP | PIC32CX-MTG + PL460-EK + ATREB215-XPRO-A, [SAMD20, WBZ451] + PL460-EK | G3 PAN-Device implementation including IPv6 layer, with an application implementing a UDP responder as defined for G3 Conformance testing |
| G3 Modem | PIC32CX-MTG + PL460-EK + ATREB215-XPRO-A, SAMD20 + PL460-EK | G3 Node serialized on top of G3 Adaptation Layer, which can be initialized as G3 PAN-Coordinator or G3 Device, and further controlled via serialization |
| Metering Demo G3 Device UDP | PIC32CX-MTSH + PL460-EK + ATREB215-XPRO-A | A complete example of the most common functionalities included in an electricity meter running on PIC32CX-MTSH device, including PLC and RF communication capabilities by implementing a complete G3 PAN-Device |

### Known Issues

- On example applications using Cryptography, file *\src\third_party\wolfssl\wolfcrypt\src\dh.c* has to be reverted after regeneration, to avoid a warning from compiler which throws a compilation error.
- Case mismatch between #include statements and filenames throw compilation errors on non-Windows environments. They have to be manually fixed by user.

### Development Tools

- [MPLAB® X IDE v6.20](https://www.microchip.com/mplab/mplab-x-ide)
- [MPLAB® XC32 C/C++ Compiler v4.35](https://www.microchip.com/mplab/compilers)
- MPLAB® X IDE plug-ins:
  - MPLAB® Code Configurator 5.5.0 or higher
- [Microchip PLC PHY Tester Tool v3.1.3](https://www.microchip.com/en-us/software-library/se_plc_phy_tester_tool)
- [Microchip Hybrid Sniffer v2.0.4](https://www.microchip.com/en-us/software-library/se_plc_sniffer)

In order to regenerate source code for any of the applications, you will also need to use the following versions of the dependent modules (see smartenergy_g3_apps/package.yml):

- Harmony core repository, v3.13.2
- Harmony csp repository, v3.18.2
- Harmony bsp repository, v3.17.0
- Harmony smartenergy repository, v1.2.0
- Harmony smartenergy\_g3 repository, v1.0.0
- Harmony crypto repository, v4.0.0-E1
- Harmony net repository, v3.11.0
- Harmony gfx repository, v3.13.0
- Harmony wireless\_15\_4\_phy repository, v1.1.0
- Harmony wireless\_pic32cxbz\_wbz repository, v1.2.0
- Harmony usb repository, 3.12.0 for demos requiring USB
- CMSIS-FreeRTOS v10.5.1 (https://github.com/ARM-software/CMSIS-FreeRTOS/tree/v10.5.1) for demos requiring FreeRTOS support

## Harmony 3 Smart Energy G3 application examples v1.0.0

### Development kit and demo application support

Following table provides number of G3 application examples available for different development kits.

| Development Kits  | MPLAB X applications | IAR applications | KEIL applications |
|:-----------------:|:-------------------:|:----------------:|:-----------------:|
| [SAM E70 Xplained Ultra Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/PartNO/DM320113) | 4 | 0 | 0 |

### New Features

The following table provides the list of the SE PLC applications:

| Application                 | Platform                        | Description                                                          |
| ------------ | ------------ | ------------ |
| PLC PHY and Go         | SAME70                  | PLC demo using PLC PHY API |
| PHY Tester Tool         | SAME70                  | PLC PHY  application to connect to Microchip PLC PHY Tester Tool |
| PHY Sniffer Tool         | SAME70                  | PLC PHY  application to connect to Microchip PLC PHY Sniffer Tool |
| PHY Tx Test Console         | SAME70                  | PLC PHY demo application to manage PLC transmissions via serial console |
| MAC RT PLC and Go         | SAME70                  | PLC MAC RT chat demo application using G3_MAC_RT API |

### Known Issues

- None

### Development Tools

- [MPLAB® X IDE v6.00](https://www.microchip.com/mplab/mplab-x-ide)
- MPLAB® X IDE plug-ins:
  - MPLAB® Code Configurator (MCC) v5.1.4
- [MPLAB® XC32 C/C++ Compiler v4.10](https://www.microchip.com/mplab/compilers)
- [Microchip PLC PHY Tester Tool v3.1.3](https://www.microchip.com/en-us/software-library/se_plc_phy_tester_tool)
- [Microchip PLC Sniffer v2.0.3](https://www.microchip.com/en-us/software-library/se_plc_sniffer)
