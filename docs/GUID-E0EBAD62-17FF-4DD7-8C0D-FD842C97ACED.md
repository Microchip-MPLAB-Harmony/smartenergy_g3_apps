# Reception

When a PLC message is received, it is processed in the callback function `APP_PLC_DataIndCb()`. This function receives a structure of type `DRV_PLC_PHY_RECEPTION_OBJ` as a parameter containing all the available data from the message.

The callback function checks if CRC validation was OK, extracts some data about the modulation and signal received. The function checks the length of the message and sends the content and information about the signal quality to the host console.

**Structure `DRV_PLC_PHY_RECEPTION_OBJ`**

``` {#CODEBLOCK_SNB_VCT_CTB}
// *****************************************************************************
/* G3 Reception parameters
   Summary:    This struct includes all information to describe any new received message.
   Remarks:    None
*/
typedef struct __attribute__((packed, aligned(1))) {
  uint8_t *pReceivedData;       		/* Pointer to received data buffer */
  uint32_t time;  				/* Instant when frame was received (end of message) referred to 1us PHY counter */
  uint32_t frameDuration;       		/* Frame duration referred to 1us PHY counter (Preamble + FCH + Payload) */
  uint16_t dataLength;          		/* Length of the received data in bytes */
  uint16_t rssi;  				/* Reception RSSI in dBuV */
  uint8_t zctDiff;				/* ZCT info */
  uint8_t rsCorrectedErrors;    		/* Errors corrected by Reed-Solomon */
  DRV_PLC_PHY_MOD_TYPE modType; 		/* Modulation type */
  DRV_PLC_PHY_MOD_SCHEME modScheme;		/* Modulation scheme */
  DRV_PLC_PHY_DEL_TYPE delimiterType;		/* DT field coming in header */
  uint8_t crcOk;  	/* MAC CRC. 1: OK; 0: BAD; 0xFE: Timeout Error; 0xFF: CRC capability disabled (PLC_ID_CRC_TX_RX_CAPABILITY) */
  uint16_t agcFine;      			/* Test data information */
  uint32_t agcFactor;    			/* Test data information */
  int16_t agcOffsetMeas; 			/* Test data information */
  uint8_t agcActive;     			/* Test data information */
  uint8_t agcPgaValue;   			/* Test data information */
  int16_t snrFch;        			/* Test data information */
  int16_t snrPay;        			/* Test data information */
  uint16_t payloadCorruptedCarriers;		/* Number of corrupted carriers */
  uint16_t payloadNoisedSymbols;		/* Number of noised symbols */
  uint8_t payloadSnrWorstCarrier;		/* SNR of the worst carrier */
  uint8_t payloadSnrWorstSymbol;		/* SNR of the worst symbol */
  uint8_t payloadSnrImpulsive;			/* SNR of impulsive noise */
  uint8_t payloadSnrBand;             	 	/* SNR of Narrowband noise */ 
  uint8_t payloadSnrBackground;			/* Background SNR */
  uint8_t lqi;					/* Link Quality Indicator */
  uint8_t toneMap[TONE_MAP_SIZE_MAX];		/* Reception Tone Map */
  uint8_t carrierSnr[PROTOCOL_CARRIERS_MAX];	/* SNR per carrier */
} DRV_PLC_PHY_RECEPTION_OBJ;
 
```

The structure contains all the information available about the message received. The fields of the structure are:

|`pReceivedData`|Pointer to data buffer containing received frame. The received frame includes padding \(if needed\). CRC is included if the CRC capability in the PL360 is disabled|
|`time`|Instant when frame was received \(end of frame\) referred to 1µs PHY counter|
|`frameDuration`|Frame duration in µs \(Preamble + FCH + Payload\)|
|`dataLength`|Length of received frame in bytes|
|`rssi`|Received Signal Strength Indicator in dBµV|
|`zctDiff`|Phase difference with transmitting node in multiples of 60 degrees|
|`rsCorrectedErrors`|Number of errors corrected by Reed-Solomon|
|`modType`|Modulation type of the last received frame. Related constants defined in section []()|
|`modScheme`|Modulation scheme of the last received frame. Related Constants defined in section []()|
|`delimiterType`|DT field coming in header. Related Constants defined in section []()|
|`crcOk`|CRC verification result \(1: OK; 0: BAD; 0xFE: unexpected error; 0xFF: CRC capability disabled. See PIB ATPL360\_REG\_CRC\_TX\_RX\_CAPABILITY \(0x401C\)\)|
|`agcFine`|Factor that multiplies the digital input signal \(13 bits\)|
|`agcFactor`|Global amplifying factor of the main branch \(21 bits\)|
|`agcOffsetMeas`|DC offset after the ADC that will be removed in case the DC Blocker is enabled \(10 bits\)|
|`agcActive`|Flag to indicate if AGC is active|
|`agcPgaValue`|Gain value applied to the PGA \(3 bits\)|
|`snrFch`|SNR of the header in quarters of dBs|
|`snrPay`|SNR of the payload in quarters of dBs|
|`payloadCorruptedCarriers`|Number of corrupted carriers in payload due to narrow/broad-band noise|
|`payloadNoisedSymbols`|Number of corrupted symbols in payload due to impulsive noise|
|`payloadSnrWorstCarrier`|SNR for the worst case carrier of the payload in quarters of dBs|
|`payloadSnrWorstSymbol`|SNR for the worst case symbol of the payload in quarters of dBs|
|`payloadSnrImpulsive`|SNR of corrupted symbols in payload due to impulsive noise in quarters of dBs|
|`payloadSnrBand`|SNR of corrupted carriers in payload due to narrow/broad-band noise in quarters of dBs|
|`payloadSnrBackground`|SNR without taking into account corrupted carriers and symbols in quarters of dBs|
|`lqi`|Link Quality Indicator. SNR in quarters of dBs with offset of 10 dB \(value 0 means -10 dB\)|
|`toneMap`|Tone Map in received frame. Related constants explained below|
|`carrierSnr`|SNR for each carrier in dBs \(with offset of 10dB, i.e. value 0 means -10dB\). Related constants explained below|

Related symbolic constants affecting `toneMap` parameter:

``` {#CODEBLOCK_TNB_VCT_CTB}

#define NUM_SUBBANDS_CENELEC_A                     6       /* Subbands for Cenelec-A bandplan */
#define NUM_SUBBANDS_CENELEC_B                     4       /* Subbands for Cenelec-B bandplan */
#define NUM_SUBBANDS_FCC                           24      /* Subbands for FCC bandplan */
#define NUM_SUBBANDS_ARIB                          16      /* Subbands for ARIB bandplan */
[…]
#define TONE_MAP_SIZE_CENELEC                      1       /* Tone Map size for Cenelec(A,B) bandplan */
#define TONE_MAP_SIZE_FCC                          3       /* Tone Map size for FCC bandplan */
#define TONE_MAP_SIZE_ARIB                         3       /* Tone Map size for ARIB bandplan */
[…]
#define TONE_MAP_SIZE_MAX                          TONE_MAP_SIZE_FCC     
#define NUM_SUBBANDS_MAX                           NUM_SUBBANDS_FCC	/* Maximum number of subbands */

```

Related constants affecting `carrierSnr` parameter:

``` {#CODEBLOCK_UNB_VCT_CTB}
#define NUM_CARRIERS_CENELEC_A                     36       /* Number of carriers for Cenelec-A bandplan */
#define NUM_CARRIERS_CENELEC_B                     16       /* Number of carriers for Cenelec-B bandplan */
#define NUM_CARRIERS_FCC                           72       /* Number of carriers for FCC bandplan */
#define NUM_CARRIERS_ARIB                          54       /* Number of carriers for ARIB bandplan */
[…]
#define PROTOCOL_CARRIERS_MAX        NUM_CARRIERS_FCC	/* Maximum number of protocol carriers */

```

**Parent topic:**[PHY PLC & Go](GUID-E87515D0-BD4F-4456-8D21-13BD460238A8.md)

