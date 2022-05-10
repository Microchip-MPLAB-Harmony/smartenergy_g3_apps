# BN Disconnection

This is the diagram that appears in the PRIME specification 1.3.6 showing the disconnection process initiated by a Base Node. It shows how the base node sends a disconnection request to the Service Node via another switch.

![](GUID-1CE06E66-C016-4C6D-B1CD-E8BBBB942DD9-low.jpg "Disconnection Initiated by the Base Node")

The sniffer capture shown below shows the disconnection process initiated by a Base Node but in this case, there is not any intermediate switch, so there are only 2 devices involved: the Base Node and the terminal or Service Node. The next image shows the disconnection process with the 2 frames involved:

![](GUID-9999E116-3E2B-4218-A48F-A374C1BF81C3-low.png)

In order to obtain more details, if you go to the *packet view*, it is possible to find the type of messages and associate them with the previous diagram \(In this case is a 4-32 connection\):

-   Frame 152:

    ![](GUID-E0EF6C87-0625-4EFD-A713-5ECBF7A032B4-low.png)

-   Frame 153:

    ![](GUID-2A03F238-1878-4582-AE2E-8322DEC35A32-low.png)


**Note:** As the Negative field is equal to 1, it indicates that is a connection closing.

**Parent topic:**[Appendix C. PRIMEv1.3 Use Case Examples](GUID-373ECDE6-AFFD-44B5-AE97-7CF1A8FCC4AD.md)

