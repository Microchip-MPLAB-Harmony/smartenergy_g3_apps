# General Operation

The first step is to plug the hardware device to the power line. Then, connect the USB cable to the host computer. Now we are ready to start the PLC PHY Tester software using, for example, the link created on your desktop during the install process. In order to work with PLC PHY Tester, the hardware must be flashed with the PHY Tester tool embedded application.

The PLC PHY Tester has been designed to work as a wizard commonly used by many desktop applications. The wizard is structured in tabs that are shown and enabled as the user sets configurations specific for each tab, and goes to the next tab by means of the proper button.

1.  **Welcome Tab**

    The main window that appears as soon as the application is started shows a welcome message and asks the user to select the serial port where Microchip PLC Development Board has been connected.

    ![](GUID-86184203-60AE-414E-A008-051541AA790B-low.png "Starting Window")

    The user must select the proper port and baudrate to connect and then press the “Connect” button. As soon as the button is pressed the button text changes to “connecting” triggering a process of board identification; after few seconds the button text changes to “Disconnect” which means that the identification process has finished. A new tab \(Product Information\) appears on the wizard and the “Next” button is enabled allowing the user to advance to the following step of the configuration. In case of error, please check the FW of the board and configuration of the serial connection.

2.  **Product Information Tab**

    As soon as the user presses the Next button in the Welcome tab, a new tab is shown:



    ![](GUID-3C9A6E49-581D-4A6D-A06D-46EB2698EC5C-low.png)

    The Product Information tab shows basic board information and also asks the user to configure the board as transmitter or receiver.

    The information shown is related to the physical layer implemented in the firmware of the board:

    -   **Product ID:** Shows a text string identifying the Microchip PLC product \(platform\)
    -   **Model ID:** Is a 16-bits unsigned integer that identifies the model of the board
    -   **Firmware ID:** Is a 32-bits unsigned integer that identifies the physical layer firmware version running on the board
    The next tab depends on transmission or reception test selection. For reception tests “Reception Parameters” and “Rx Test Parameters” tabs are added; for transmission tests “Transmission Parameters” and “Tx Test Parameters” tabs are added. Finally, independently of the kind of test selected, two more tabs are added: “Configuration Summary” and “Test Execution”. All of these tabs are disabled at this point; as the user navigates through the wizard with the Next and Prev. buttons, the tabs become enabled.

3.  **Reception Parameters for G3-PLC PHY Layer**

    ![](GUID-A176C35B-529E-4F87-961B-B3CC725545BD-low.png "Reception Parameters Tab for G3-PLC PHY Layer ")

    This tab allows the user to configure all parameters related with the reception of frames. Configurable parameters are as follows:

    -   Tone-Mask: it allows to suppress the energy of carriers as G3-PLC specification defines. An array of booleans is used to define the tone mask. The length of array depends on bandplan: \(CENELEC-A = 36; FCC = 72; ARIB = 54\), 1 means carrier disabled and 0 means carrier enabled. It is important to configure the same value for Tone-Mask both in transmission board as well as in reception board, otherwise communication will fail
    -   Tonal Noise Adaptation: The PHY layer is able to detect tonal noise and configure some input filters in order to cancel this noise. The user can select:
        -   Automatic Configuration: Hardware is performing noise captures every fixed interval \(configurable, see *“G3-PLC FW Manual”*\). After each capture the hardware selects the proper filtering for the detected noise \(if any\). User can configure to only perform the noise captures when hardware is not receiving \(only available for ATPL250A/SAM4CP16C platforms\). This option is activated by means of the “Delay Noise capture after correct reception” check box.
        -   Manual Configuration: In this mode the hardware only performs noise analysis when the user presses “Search and Adapt to Noise”. The user can also configure the hardware to configure notch filter to certain frequency by means of the “Adapt to Target Frequency”. Finally the user can disable noise adaptation using the “Disable Noise Filtering” option
    -   The “Perform EVM and SNR Test” option changes message and interval of reception in order to make a test that evaluates the PHY layer performance. In test execution tab some extra columns are added in order to obtain more information about performance of the PHY layer. For more information check application note *“PLC PHY Performance Validation”*
4.  **Rx Test Parameters Tab**

    This tab allows configuration of the reception test parameters. These parameters are the amount of expected messages to be received and the time interval from message to message \(receiver instance will wait for this time prior to consider message is missed\). These parameters are used just for test statistic results. Unlike the previous one, this tab is common for all the Microchip PLC protocols. The following figure shows the tab layout:

    ![](GUID-FEC1F162-0F11-48EE-B74F-C383D6436767-low.png "Rx Test Parameters Tab")

5.  **Transmission Parameters for G3-PLC PHY Layer**

    ![](GUID-E9C273EF-FEF1-469F-8CF4-88C48D71661B-low.png "Transmission Parameters Tab for G3-PLC PHY Layer")

    This tab allows the user to configure all parameters related with the transmission of G3-PLC PHY frames. Configurable parameters are the following:

    -   Modulation Scheme: Allows configuration of differential or coherent modulation scheme
    -   Modulation Type: Allows selection between BPSK, QPSK, 8PSK and robust BPSK
    -   Tone Map: Allows disabling sub-bands \(groups of tones\), and the format depends on the selected bandplan. Each band is activated or deactivated setting to ‘1’ or ‘0’ the corresponding bit in the hex array. The different sub-bands are ordered in the hex array from least significant bit \(lower frequency sub-band\) to most significant bit \(higher frequency sub-band\). For example, in CENELEC-A bandplan \(6 sub-bands\) 0x01 represent a tone map where only the lower sub-band is active, as well as 0x20 is the tone map corresponding to a tone map where only higher sub-band is active
    -   Reed Solomon 2nd Block: Only available for FCC bandplan, it allows to introduce a second RS block as defined in the G3-PLC specification
    -   Tx Power: Allows to decrease the transmission power in 3dB steps
    -   Pre-emphasis: Allows to decrease the transmission power in steps of 3 dB to each sub-band
    -   Transmission Mode: Configures the output stage depending on the line impedance seen by the board
    -   Tone-Mask: it allows to suppress the energy of carriers as defined in the G3-PLC specification. An array of booleans is used to define the tone mask. The array length depends on the bandplan: \(CENELEC-A = 36; FCC = 72; ARIB = 54\), 1 means carrier disabled and 0 means carrier enabled.
    -   Perform EVM and SNR Test: Selecting this option changes the message and interval of transmission in order to make a test that evaluates the PHY layer performance. For more information check application note *“PLC PHY Performance Validation”*
6.  **Tx Test Parameters Tab**

    This tab is very similar to the Rx Test Parameters tab. The “Message” parameter allows configuration of the content of the message to be transmitted in ASCII characters. the “Time Interval” parameter defines the interval of time, measured in milliseconds, between the transmissions of two messages \(This time interval indicates the time between the beginnings of two frames\). Finally, the “Number of Frames” parameter allows configuration of the number of messages to be transmitted. As in the Rx Test Parameter Tab, this tab is independent of PHY layer.

    IMPORTANT: 10ms drifts in the transmission and reception times may appear due to the application running on a non-real time operating system.

    IMPORTANT: A "Time Interval" shorter than 70 milliseconds may not work properly due to PC OS and USB connection. Please use the PHY TX console firmware example to transmit messages with shorter time interval.

    ![](GUID-F7200646-E44F-47A3-A966-7EF9004CD485-low.png "Tx Test Parameters Tab")

7.  **Configuration Summary Tab**

    This is the last tab in the wizard before starting the execution of the test. This tab shows a table where all the configuration parameters are listed that have been configured along the wizard. The “Next” button has been substituted with a “Start Test” button that allows starting the message transmission or reception.

    ![](GUID-A7F68690-AD97-42FA-8CE6-14796EEDB0D3-low.png "Configuration Summary Tab of a Transmission Test")

8.  **Test Execution Tab**

    This is the tab shown while the test is being executed; it shows the transmission or reception of each message in form of a table. The following figures show examples of this tab for transmission and a reception tests:

    Both views \(Tx and Rx\) show tables representing different information depending on test type \(transmission/reception\). When the test is finished or is cancelled, a final report is shown at the bottom of the tab.

    This tab will provide additional information about signal quality in case of selecting "Performing EVM and SNR Test" check-box in the "Reception Parameters" tab.

    The user can copy the information included in the corresponding table to the clipboard by clicking on the "Copy Table" button. This tab is only able to hold the information of the last 10000 frames; the user can choose a bigger number of frames to be transmitted/received but only the last 10000 frames will be available.

9.  **Reception Test Results**

    ![](GUID-2A5D6881-98F5-438A-B803-AACDB1153CAC-low.png "Execution Tab for G3-PLC PHY Layer Rx Test")

    Information included in the table is as follows:

    -   Frame \#: Indicates the received frame number
    -   Modulation Type: Indicates the type of modulation: BPSK, QPSK, 8PSK or BPSK\_ROBO
    -   Modulation Scheme: Indicates if modulation scheme is differential or coherent
    -   Tone Map: Indicates active sub-bands in the frame
    -   RSSI \(dBuV\): Indicates the strength of the received signal in dBuV
    -   LQI \(dB\): Link Quality Indicator \(See G3-PLC specification for more information\)
    -   Data: Is the received info in ASCII format
    -   Rx Interval: Is the interval of time between the reception of the current frame and the previous one
    -   Payload Integrity: Shows if the content of the frame is correct or not
    After all frames have been received, or the test has been cancelled, a text box with information about the test will be shown at the bottom of the tab. First of all, test timestamps are shown; this information is measured by the PC application. After that, there is a section called Frame Error information that shows information about received frames and possible errors. Fields shown in this section are:

    -   Total Frames Received: Shows the number of frames detected, not taking into account if the frame has errors
    -   Total Frames Bad FCH CRC: Shows the number of frames in which the header CRC is not valid. These frames are discarded by the firmware and are not reported in the application
    -   Total Frames Bad RS LEN: Frames that RS block is not able to correct are discarded by the firmware
    -   Total Exception Errors: Other kinds of errors in the reception chain
    -   Total Frames Bad Payload: Shows the number of frames where the content of the payload is not what it is supposed to be
    -   Total Frames Received OK: Shows the number of frames received with the correct content
10. **Transmission Test Results**

    ![](GUID-88CF08B1-8D14-403A-8C15-ADC076831FA2-low.png "Execution Tab for G3-PLC PHY Layer in Tx Test")

    There are eight columns that show the following information:

    -   Frame \#: Indicates the number of frames transmitted. It is useful to track the test progress
    -   Tx Result: Indicates the result of the transmission. If an error occurs, a descriptive text will appear
    -   RMS\_Calc: This number is related with the impedance detected in the power line; it is used by the PHY layer to determine the transmission mode.
    -   Modulation Scheme: Indicates if modulation scheme is differential or coherent
    -   Modulation Type: Indicates the type of modulation: BPSK, QPSK, 8PSK or BPSK\_ROBO
    -   Tone Map: Indicates active sub-bands in the frame
    -   Data: Shows the message received in ASCII format
    -   Tx Interval: Represents the time interval between the current frame and the previous one
    After all frames have been transmitted, a text box with information about the test will appear at the bottom of the tab.

    Test timestamps are provided; this information is measured by the PC application. After that, there is a section of information called Frame Error information that shows information about transmitted frames and possible errors. Finally, another section shows a summary of the transmission test; apart from known transmission parameters other parameters are shown:

    -   Frame Duration: Is the duration in millisecond of the frame
    -   Tx Mean Interval: Is the average interval between transmissions calculated from top table data
    -   Effective Baudrate \(Peak\): Is the effective baudrate if frames were transmitted consecutively, calculated as follows:

        ||

    -   Effective Baudrate \(Real\): Is the effective baudrate, calculated as follows:

        ||

    -   Raw Baudrate \(Peak\): Is the baudrate taking into account all headers and redundancies:

        ||

    -   Raw Baudrate \(Real\): Is the baudrate taking into account all headers and redundancies, calculated as follows:

        ||

    -   Channel Usage : Is the percentage of channel that has been used, calculated as follows:

        ||


**Parent topic:**[PHY Tester Tool](GUID-A2A3C4CF-8C73-4F62-9809-2B710A7783A0.md)

