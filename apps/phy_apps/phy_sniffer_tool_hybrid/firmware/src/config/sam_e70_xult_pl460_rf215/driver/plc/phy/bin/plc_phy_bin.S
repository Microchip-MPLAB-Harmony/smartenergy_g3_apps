/*******************************************************************************
  PLC Binary file Assembler

  Summary:
    Library for Microchip PLC Stack

  Description:
    - Join PLC PHY binary file together PLC application
*******************************************************************************/

/*
Copyright (C) 2021, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/

  .section .rodata
  .global plc_phy_bin_start
  .global plc_phy_bin_end
  .global plc_phy_bin2_start
  .global plc_phy_bin2_end

  .align 8
plc_phy_bin_start:
  .incbin "./PLC_PHY_G3_FCC.bin"
  .align 8
plc_phy_bin_end:
plc_phy_bin2_start:
plc_phy_bin2_end:
