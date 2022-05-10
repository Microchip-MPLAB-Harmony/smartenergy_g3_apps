# Appendix B. G3 SQLite Log Database

The PLC Sniffer creates a relational database using SQLite to store the G3 logs. Using this type of database to store the information helps to handle logs from complex networks and to inspect the data in very flexible ways. Third party tools to handle SQLite databases can also be used. A useful tool for SQLite database inspection is the *SQLite Manager* plug-in for Firefox browser.

The current version of the database has 14 tables:

-   **LogInfo:** contains information about the database
-   **Frame:** each row of this table represents a PLC message on the power line. This table contains the information displayed on the Capture and Log windows. Includes the raw data of the received PDU and some parameters from the PHY Layer provided by the hardware. Also includes a timestamp value
-   **Beacon:** disassembled information of the Beacon PDU as defined in G3 standard
-   **Data:** data PDU as defined in G3 standard. The data payload may contain a message from the adaptation sublayer. If possible, it will be disassembled and the proper information will be inserted in MeshHeader, BroadcastHeader, FragmentationHeader, LBP\_Header, LBP\_EAP and LBP\_ConfParam tables
-   **ToneMapResponse:** disassembled information of the Tone Map Response PDU as defined in G3 standard
-   **DecryptedData:** When data payload is encrypted and the key is known, the decrypted data will be stored in this table. The key will only be known if the capture starts before the bootstrapping
-   **MeshHeader:** disassembled Mesh Header encapsulated in Data PDU as defined in G3 standard
-   **BroadcastHeader:** disassembled broadcast header encapsulated in Data PDU as defined in G3 standard
-   **FragmentationHeader:** disassembled fragmentation header encapsulated in Data PDU as defined in G3 standard
-   **LBP\_Header:** disassembled LoWPAN bootstrapping protocol header encapsulated in Data PDU as defined in G3 standard
-   **LBP\_EAP:** disassembled EAP message encapsulated in Data PDU as defined in G3 standard
-   **LBP\_ConfParam:** disassembled configuration parameter message encapsulated in Data PDU as defined in G3 standard
-   **RREQ:** disassembled Route Request \(message type of LOADng routing protocol\) message encapsulated in Data PDU as defined in G3 standard
-   **RREP:** disassembled Route Reply \(message type of LOADng routing protocol\) message encapsulated in Data PDU as defined in G3 standard
-   **RERR:** disassembled Route Error \(message type of LOADng routing protocol\) message encapsulated in Data PDU as defined in G3 standard
-   **PREQ:** disassembled Path Request \(message type of LOADng routing protocol\) message encapsulated in Data PDU as defined in G3 standard
-   **PREP:** disassembled Path Reply \(message type of LOADng routing protocol\) message encapsulated in Data PDU as defined in G3 standard
-   **RLCREQ:** disassembled Route Link Cost Request \(message type of LOADng routing protocol\) message encapsulated in Data PDU as defined in G3 standard
-   **RLCREP:** disassembled Route Link Cost Reply \(message type of LOADng routing protocol\) message encapsulated in Data PDU as defined in G3 standard
-   **LOADng\_ForwardPaths:** Forward paths associated to a PREQ message
-   **LOADng\_ReversePaths:** Reverse paths associated to a PREQ message
-   **Node:** table containing the current status of all the nodes detected on the network
-   **Event:** table containing the status events that define the node table. This table allows to reproduce the status of a node at any time of the log
-   **G3KH:** G3 Key hierarchy, table containing all the keys knows by the sniffer. If the PSK key is supplied, the sniffer will know all key hierarchy and when a bootstrap sequence is completed, the sniffer will learn the GMK key

All tables are linked through a common “idFrame” index. This unique index is assigned when a new frame is inserted on the database. This allows building complex search queries with SQL. [Figure   1](#FIG_ELQ_2BN_SCB) and [Figure   2](#FIG_JMY_GBN_SCB) show the tables and relations of a G3 log database. Table fields are encoded as specified in the PRIME standard except CmdType Identifier. This field is specified in the next section.

![](GUID-5B6A5175-CBDD-4BC3-AFB5-3DDD7124D0D7-low.png "PLC Sniffer G3 Log Database, Part-1")

![](GUID-FB07A343-281F-4F47-9087-9DA3BC6599C2-low.png "PLC Sniffer G3 Log Database, Part-2")

-   **[Command Type Identifier](GUID-C38003EC-3345-481E-B69C-EA4F23795447.md)**  

-   **[Event Table](GUID-E8EA7841-B747-4373-9373-443D66C7E220.md)**  

-   **[Sample SQLite Filters](GUID-66762CCC-9D4D-406B-94FD-EB2C7F863A64.md)**  


**Parent topic:**[PHY Sniffer](GUID-8D66ECA9-8C74-42B9-8915-33D381579FBB.md)

