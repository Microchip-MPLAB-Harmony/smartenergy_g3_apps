# Security Configuration

G3 Security is based on a key hierarchy. A global secret key \(PSK, Pre-Shared Key\) must be known by all the devices in the network. This PSK key allows a device to complete the bootstrap protocol securely with a network coordinator. During the bootstrap protocol, the network coordinator will provide the GMK \(Group Master Key\) to the devices. The GMK key will be used to cipher all data packets in the network.

It is possible to configure these keys into the sniffer application in order to be able to decipher all the communications. If PSK is configured, the LoWPAN Bootstrap protocol will be decoded and verified. Also, the sniffer will learn the GMK key when a full bootstrap sequence is decoded. From that moment, it will be able to decode all frames.

If only the GMK is configured, the sniffer will not be able to decode nor verify the EAP messages in the bootstrap protocol but it will be able to decode the data messages in the network.

PSK and GMK can be configured using a database initialization script. An initialization script *\{Program\_Install\}/Init-DB-Scripts/PSK\_IOT.txt* containing both keys used in the G3 Alliance Interoperability tests is provided.

It is important to note that if database is open in “append mode”, the security initialization may not take effect if a prior GMK/PSK key has been initialized.

**Parent topic:**[G3](GUID-AEF828B2-7BEE-47DA-84FC-8959348255B2.md)

