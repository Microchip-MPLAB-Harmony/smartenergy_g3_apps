/*******************************************************************************
  G3 MAC Real Time Binary file Assembler

  Summary:
    Library for Microchip PLC Stack

  Description:
    Join G3 MAC Real Time binary file(s) together with application.
*******************************************************************************/

/*
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
  .global g3_mac_rt_bin_start
  .global g3_mac_rt_bin_end
  .global g3_mac_rt_bin2_start
  .global g3_mac_rt_bin2_end

  .align 8
g3_mac_rt_bin_start:
  .incbin "./G3_MAC_RT_FCC.bin"
  .align 8
g3_mac_rt_bin_end:
g3_mac_rt_bin2_start:
g3_mac_rt_bin2_end:
