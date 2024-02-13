![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

# Microchip MPLAB® Harmony 3 Release Notes

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

### Known Issues

- None

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
- Harmony dev\_packs repository, v3.18.1
- Harmony smartenergy repository, v1.2.0
- Harmony smartenergy\_g3 repository, v1.0.0
- Harmony crypto repository, v3.8.1
- Harmony net repository, v3.11.0
- Harmony gfx repository, v3.13.0
- Harmony wireless\_15\_4\_phy repository, v1.1.0
- Harmony wireless\_pic32cxbz\_wbz repository, v1.2.0
- Harmony usb repository, 3.12.0 for demos requiring USB
- wolfSSL v5.4.0 (https://github.com/Microchip-MPLAB-Harmony/wolfssl/tree/v5.4.0) for demos requiring wolfSSL encryption
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
