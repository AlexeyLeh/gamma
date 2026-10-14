;*****************************************************************************************
; DESCRIPTION: This is the comspec file for the Powercomms Box (nodename = BA)
;
; CREATED BY:  Matt Wandstrat
;
;*****************************************************************************************
;
; The HEADER section defines the partnumber of this COMSPEC File, its date, and its time
;
;--------------------------------------------------------------------------------------
[HEADER]
PN=77578ca100
DATE=6/18/1999
TIME=14:30:00
;--------------------------------------------------------------------------------------
; The BOOTLOADER section defines the code space for this node as well as the boot name
; The SADDR defines the starting address (passed bootrom) for code (HEX Number)
; The EADDR defines the last valid address of program space
; The BOOTNAME is the two-digit hexadecimal name of the bootrom node
;--------------------------------------------------------------------------------------
[BOOTLOADER]
SADDR=4000
EADDR=10FFFF
BOOTNAME=22
OVERLAY=?
CTABLE=?
;For OVERLAY and CTABLE 0 = not an option, 1 =  option exists
;--------------------------------------------------------------------------------------
; The STATUS section defines the use of each and every status bit, along with a long
; description of why it would be set.
; Bits that are not used do not have to be defined here. The API will place default
; labels for them
;--------------------------------------------------------------------------------------
[STATUS]
;	
[BIT 15] Not Used
;
This Bit Should Never Be Set 
;
[BIT 14] Not Used
;
This Bit Should Never Be Set 
;
[BIT 13] INT_ERR
;
Uninitialized interrupt error	
;
[BIT 12] COP_ERR
;
Computer Operating Properly error
;
[BIT 11] CMR_ERR
;
Clock Monitor Error	
;
[BIT 10] IOT_ERR
;
Illegal Opcode Error
;
[BIT 9] EXTRAM_ERR
;
External RAM Error
;
[BIT 8] INTRAM_ERR
;
Internal RAM Error
;
[BIT 7] CODECKSUM_ERR
;
CODE Checksum Error
;
[BIT 6] DATACKSUM_ERR
;
DATA Checksum Error
;
[BIT 5] EEWRITE_ERR
;
EEPROM Write Error
;
[BIT 4] Not Used
;
This Bit Should Never Be Set 
;
[BIT 3] Not Used
;
This Bit Should Never Be Set 
;
[BIT 2] Not Used
;
This Bit Should Never Be Set  
;
[BIT 1] PASSIVATION_TEST
;
Battery Passivation Test in Progress
;
[BIT 0] DEPASSIVATING
;
Battery Depassivation in Progress
;
;--------------------------------------------------------------------------------------
;
; This is the CMDS section. Define all valid commands and their respective formats
; It has the following format:
; [xx] Descriptor
; [RTIME]=ms where ms is the maximum response time in millisec
; [CTYPE]=?
;	The [CTYPE] (command type is defined as following:
;
;	[CTYPE]=1 is a command that takes no other input than the command itself, and returns data.
;	[CTYPE]=2 is a command that takes a bucket input and returns data
;	[CTYPE]=3 is a read/write type command. When just the command is sent, it returns data, but when data is added to the send message, it writes the data, and returns only a minimal response.
;
;	If your command does not fall into any of the above categories you may use zero ([CTYPE]=0).
;
;Parameter DataType1<space>Short Description<,>Units<;>Long Description>
;
;Parameter Data Types:
;  C - signed char (single BYTE)
;  B - unsigned char (single BYTE)
;  I - signed short int (two BYTES)
;  W - unsigned short int (two BYTES)
;  X - unsigned short int (two BYTES) – KTABLE checksum
;  T - unsigned triple byte (three BYTES)
;  L - unsigned long int (four BYTES)
;  l - signed long int (four BYTES) (lowercase L)
;  F - 4-byte IEEE float (four BYTES)
;  A - ASCII Bytes
;
;--------------------------------------------------------------------------------------
[CMDS]
[01] Read/Write Input Voltage
[Rtime]=300
[CTYPE]=3
;
F Voltage,0.1 Volt; Voltage in 10ths of a volt
;
;Read/Write Input Voltage <0x01>
;The input voltage is voltage at the pod supplied by the module. Units are tenth of a volt and the range is 200.0 to 400.0. 
;
;Read Input Voltage
;This command instructs the node to read and report the input voltage of the pod.
;
;Tx = <msg header><0x01><chksum>
;
;Rx = <msg header><0x01><ack><status><main voltage><chksum>
;
;<main voltage> ::= <IEEE FP>
;
;Write Input Voltage
;This command instructs the node to set the input voltage of the pod.
;
;Tx = <msg header><0x01><main voltage><chksum>
;
;<main voltage> ::= <IEEE FP>
;
;Rx = <msg header><0x01><ack><status><chksum>
;
[02] Read/Write Power Input Switch
[RTime]=200
[CTYPE]=3
;
B Switch,N/A; 00=Open, 01=Closed
;
;Read/Write Input Power Switch <0x02>
;On power-up, the main voltage from the PowerComm Module is only applied to the 5 volt regulator of the PowerComm Pod. This allows the 
;microcontroller to verify that the voltage from the module is correct before applying it to additional pod circuitry. This command applies or 
;removes the input voltage (see Input Voltage command <0x01>) for the other pod circuitry and also acts to enable or disable the main voltage 
;for the tool under test. Since a tool under test may be powered internally, an additional output switch isolates the main voltage from the output. 
;(see Output Switch command <0x03>).
;
;Read Power Input Switch 
;This command instructs the node to return the power input switch status.
;
;Tx = <msg header><0x02><chksum>
;
;Rx = <msg header><0x02><ack><status><switch state><chksum>
;
;<switch state> ::= <open> | <closed>
;	<open>::= <0x00>
;	<closed> ::= <0x01>
;
;Write Power Input Switch
;This command instructs the node to open or close the power input switch. When the switch is closed main voltage is applied to the pod circuitry, 
;enabling input voltage to the unit under test when the output switch is closed.
;
;Tx = <msg header><0x02>><switch state>  <chksum>
;
;<switch state> ::= <open> | <closed>
;	<open>::= <0x00>
;	<closed> ::= <0x01>
;
;Rx = <msg header><0x02><ack><status><chksum>
;
[03] Read/Write Power Output Switch
[RTime]=200
[CTYPE]=3
;
B Switch,N/A; 00=Open, 01=Closed
;
;Read/Write Output Switch <0x03>
;On power-up, the output switch defaults to the open state. This command connects or disconnects the output power line to the pod circuitry. 
;Since a tool under test may be powered internally, an additional power enable switch isolates the main voltage from the output. (see Power Input 
;Switch command <0x02>.) Internal tool power may be sensed when the power input switch is open and the output switch is closed. With the power
; input switch closed and the output switch closed, the pods main voltage is applied to the tool under test.
;
;WARNING: Be aware that applying the pods input voltage to a unit under test that is internally powered is an abnormal condition and should only 
;be allowed with specific tools under specific conditions. It is advised that the sensing of a tool under test for internal power be performed before 
;allowing the power input switch to be closed when the output switch is closed.
;
;Read Output Switch 
;This command requests the node return the output switch status.
;
;NOTE:   The control for this switch is a write-only latch.  Therefore, this command returns what the state of the switch SHOULD BE.
;
;Tx = <msg header><0x03><chksum>
;
;Rx = <msg header><0x03><ack><status><switch state><chksum>
;
;<switch state> ::= <open> | <closed>
;	<open>::= <0x00>
;	<closed> ::= <0x01>
;
;Write Output Switch
;This command instructs the node to open/close the output switch. When the switch is closed the output power line is connected to the pod 
;circuitry.
;
;Tx = <msg header><0x03><switch state>  <chksum>
;
;<switch state> ::= <open> | <closed>
;	<open>::= <0x00>
;	<closed> ::= <0x01>
;
;Rx = <msg header><0x03><ack><status><chksum>
;
[04] Read/Write Tool Load
[RTime]=200
[CTYPE]=3
;
B Tool Load,N/A; 00=No Load, 01=Load 1, 02=Load 2, 03=Load 1 & 2
;
;Read/Write Tool Load <0x04>
;The pod is able to apply various resistive loads to the main voltage line. This command is to be used with the Power Enable Switch Command 
;<0x02> and/or the Output Switch Command <0x03>. If the unit under test is internally powered by a battery, this command can be used to 
;depassivate the battery. This is accomplished by opening the power enable switch, closing the output switch and selecting the desired resistive load 
;from below. Additionally, this command may be used to load test various values of the main voltage from the PowerComm Module. This is 
;accomplished by opening the output switch, closing the power enable switch and selecting the desired resistive load from below.
;
;WARNING: The PowerComm Pod is a small sealed enclosure. When these resistive loads are activated, they cause a heat build-up in the enclosure. 
;Therefore, a resistive load should only be activated for the minimum time required to perform the task.
;
;Read Tool Load
;This command instructs the node to return the tool load status.
;
;Tx = <msg header><0x04><chksum>
;
;Rx = <msg header><0x04><ack><status><tool load state><chksum>
;
;<tool load state> ::= <no load> | <load 1> | <load 2> | <load 1 & load 2>
;	<no load> ::= <0x00>
;	<load 1> ::= <0x01>
;	<load 2> ::= <0x02>
;	<load 1 & load 2> ::= <0x03>
;
;Write Tool Load
;This command instructs the node to set the tool load.
;
;Tx = <msg header><0x04><tool load state><chksum>
;
;<tool load state> ::= <no load> | <load 1> | <load 2> | <load 1 & load 2>
;	<no load> ::= <0x00>
;	<load 1> ::= <0x01>
;	<load 2> ::= <0x02>
;	<load 1 & load 2> ::= <0x03>
;
;Rx = <msg header><0x04><ack><status><chksum>
;
[05] Read/Write Modem Load
[RTime]=200
[CTYPE]=3
;
B Modem Load,N/A; 00=1 Modem, 01=3 Modems, 02=5,04=7,08=9 Modems
;
;Read/Write Modem Load <0x05>
;A variety of AC coupled loads are available for testing the modem integrity of the unit under test.
;
;Read Modem Load
;This command instructs the node to return the modem load status.
;
;Tx = <msg header><0x05><chksum>
;
;Rx = <msg header><0x05><ack><status><modem load ><chksum>
;
;<modem load > ::= <1 modem>|<3 modems>|<5 modems>|<7 modems>|<9 modems>
;	<1 modem > ::= <0x00>
;	<3 modems> ::= <0x01>
;	<5 modems> ::= <0x02>
;	<7 modems> ::= <0x04>
;	<9 modems> ::= <0x08>
;
;Write Modem Load
;This command instructs the node to set the modem load.
;
;Tx = <msg header><0x05><modem load state><chksum>
;
;<modem load > ::= <1 modem>|<3 modems>|<5 modems>|<7 modems>|<9 modems>
;	<1 modem > ::= <0x00>
;	<3 modems> ::= <0x01>
;	<5 modems> ::= <0x02>
;	<7 modems> ::= <0x04>
;	<9 modems> ::= <0x08>
;
;Rx = <msg header><0x05><ack><status><chksum>
;
[06] Read/Write 9-Bit Mode
[RTIME]=200
[CTYPE]=3
;
B Nine Bit Mode,N/A; 00 = A&B OFF,01=AON,02=BON,03=A&BON
;
;Read/Write 9-Bit Mode <0x06>
The PowerComm Pod is capable of interfacing through two Host ports (HostA & HostB).  These ports are electrically connected to an RS-485 
;transceiver and can be independently set to either 8 or 9 bit data.  When a host port is set for 9 data bits, it also uses the address mark method of 
;communicating where the first character in a message (address character) is marked with the 9th bit ON.  Subsequent characters are not marked 
;(9th bit OFF).  On power-up we default to 8-bit mode. These ports are provided with both 8 and 9 bit capability for backwards and forwards 
;compatibility.
;
;Read 9-Bit Mode
;This returns the current state of HostA and HostB 9-bit mode.
;
;Tx = <msg header><0x06><chksum>
;
;Rx = <msg header><0x06><ack><status><9-bit mode><chksum>
;
;<9-bit mode> ::= <A OFF, B OFF> | <A ON, B OFF> | <A OFF, B ON> | <A ON, B ON>
;	<A OFF, B OFF> ::= <0x00>
;	<A ON, B OFF> ::= <0x01>
;	<A OFF, B ON> ::= <0x02>
;	<A ON, B ON> ::= <0x03>
;
;Write 9-Bit Mode
;This command controls the state of HostA and HostB 9-bit mode.
;
;Tx = <msg header><0x06><9-bit mode><chksum>
;
;<9-bit mode> ::= <A OFF, B OFF> | <A ON, B OFF> | <A OFF, B ON> | <A ON, B ON>
;	<A OFF, B OFF> ::= <0x00>
;	<A ON, B OFF> ::= <0x01>
;	<A OFF, B ON> ::= <0x02>
;	<A ON, B ON> ::= <0x03>
;
;Rx = <msg header><0x06><ack><status><chksum>
;
[07] Read Output Voltage
[RTIME]=200
[CTYPE]=3
;
F Voltage,0.1 Volt; Returns voltage in 10ths of a volt
;
;Read Output Voltage <0x07>
;This command instructs the node to return the output voltage of the pod in tenths of a volt.
;
;EXPLANATION: There are two in-line switches separating the pod's main voltage from the output power line: the Power Enable switch and the 
;Output switch. It is between these switches where voltage measurements are taken. By closing one and/or both switches, various conditions can be 
;determined. With only the Output Switch closed we can check whether a tool is being externally powered. With only the Power Enable switch 
;closed, fine tuning of the main voltage can be achieved. Finally, with both switches closed the loading effect of the unit under test may be observed. 
;It is strongly recommended that a reading of the states of both switches be performed prior to issuing of this command.
;
;Tx = <msg header><0x07><chksum>
;
;Rx = <msg header><0x07><ack><status><output voltage><chksum>
;
;<output voltage> ::= <IEEE FP>
;
[08] Read Current
[RTIME]=200
[CTYPE]=1
;
F Current,milliamps; Return Current in milliamps
;
;Read Current <0x08>
;This command instructs the node to return current in milliamps.  The measurement taken depends on which power supply is being sensed 
;(see R/W Power Sense command).
;
;Tx = <msg header><0x08><chksum>
;
;Rx = <msg header><0x08><ack><status><milliamps><chksum>
;
;<milliamps> ::= <IEEE FP>
;
[09] Read/Write Flow Simulation
[RTIME]=200
[CTYPE]=3
;
B Flow Sim,N/A; 01=ON, 00=OFF
;
;Read/Write Flow Simulation <0x09>
;The pod can control the flow state by using this command.  Setting Simulated Flow Control to ON will cause the pod to drive the FLOW line low 
;or high depending on Flow State (see Read/Write Flow State).  Setting Simulated Flow to OFF will allow external sources to drive the FLOW line 
;low or high.
;
;Read Flow Simulation
;This command reports the status of flow simulation.
;
;Tx = <msg header><0x09><chksum>
;
;Rx = <msg header><0x09><ack><status><Flow Sim><chksum>
;
;<Flow Sim>::= <ON> | <OFF>
;	<ON> ::= 0x01
;	<OFF> ::= 0x00
;
;Write Flow Simulation
;This command turns simulated flow control on or off.
;
;Tx = <msg header><0x09><Flow Sim><chksum>
;
;<Flow Sim> ::= <ON> | <OFF>
;	<ON> ::= 0x01
;	<OFF> ::= 0x00
;
;Rx = <msg header><0x09><ack><status><chksum>
;
[0A] Read/Write Flow State
[RTIME]=200
[CTYPE]=3
;
B Flow State,N/A; 01=ON, 00=OFF
;
;Read/Write Flow State <0x0A>
;This command reads/writes the state of the FLOW line
;
;Read Flow State
;This command returns the state of the FLOW line.
;
;Tx = <msg header><0x0A><chksum>
;
;Rx = <msg header><0x0A><ack><status><FLOW state><chksum>
;
;<FLOW state> ::= <ON> | <OFF>
;	<ON> ::= 0x01
;	<OFF> ::= 0x00
;
;Write Flow State
;When Flow is being simulated (see Read/Write Flow Simulation) this command will change the state of the FLOW line.  When FLOW is not being 
;simulated, this command will return an 0x80 acknowledge (Flow is not being simulated).
;
;Tx = <msg header><0x0A><FLOW state><chksum>
;
;<FLOW state> ::= <ON> | <OFF>
;	<ON> ::= 0x01
;	<OFF> ::= 0x00
;
;Rx = <msg header><0x0A><ack><status><chksum>
;
[0B] Read Pulser State
[RTIME]=200
[CTYPE]=1
;
B Pulser State,N/A; 01=ON, 00=OFF
;
;Read Pulser State <0x0B>
;This command returns the current state of the pulser.  Navi tools have a pulse line connected to the Pod and collar based tools are decoded using 
;current sensing.  This command returns ON if either of these methods is detecting a pulser energized condition.
;
;Tx = <msg header><0x0B><chksum>
;
;Rx = <msg header><0x0B><ack><status><pulser><chksum>
;
;<pulser> ::= <OFF> | <ON>
;	<OFF> ::= <0x00>
;	<ON> ::= <0x01>
;
[0C] Read/Write Pump State
[RTIME]=200
[CTYPE]=3
;
B Pumps State,N/A; 01=ON, 00=OFF
;
;Read/Write Pumps State <0x0C>
;This command controls the state of the PUMPS LED on the front panel of the Pod.
;
;Read Pumps State
;This command instructs the node to return the pumps state.
;
;Tx = <msg header><0x0C><chksum>
;
;Rx = <msg header><0x0C><ack><status><pumps state><chksum>
;
;<pumps state> ::= <OFF> | <ON>
;	<OFF> ::= 0x00
;	<ON> ::= 0x01
;
;Write Pumps State
;This command instructs the node to control the pumps state.
;
;Tx = <msg header><0x0C><pumps state><chksum>
;
;<pumps state> ::= <OFF> | <ON>
;	<OFF> ::= 0x00
;	<ON> ::= 0x01
;
;Rx = <msg header><0x0C><ack><status><chksum>
;
[0D] Read/Write Power Sense
[RTIME]=200
[CTYPE]=3
;
B Power Sense,N/A; 01=IVTL,02=IB1,O4=IB2,08=IBS, 03=IB1+IVTL, 05=IB2+IVTL, 08=IBS+IVTL
;
;Read/Write Power Sense <0x0D>
;This command controls the routing of power to the Pod's voltage and current sensing circuitry.  The power sensing can be routed to measure 
;internal pod power, external battery 1 power, external battery 2 power and pod supplied external power.  Each power source may be connected 
;by itself or in combination with internal pod power.  The valid routings are:  IVTL, IB1, IB2, IBS, IVTL+IB1, IVTL+IB2, IVTL+IBS.
;
;Read Power Sense
;This command returns the current routing of power to the sensing circuitry.
;
;Tx = <msg header><0x0D><chksum>
;
;Rx = <msg header><0x0D><ack><status><sense><chksum>
;
;<sense> ::= <IVTL> | <IB1> | <IB2> | <IBS> | <IVTL+IB1> | <IVTL+IB2> | <IVTL+IBS>
;<IVTL> ::= 0x01
;<IB1> ::= 0x02
;<IB2> ::= 0x04
;<IBS> ::= 0x08
;
;Write Power Sense
;This command changes the routing of power to the sensing circuitry.
;
;Tx = <msg header><0x0D><ack><status><sense><chksum>
;
;<sense> ::= <IVTL> | <IB1> | <IB2> | <IBS> | <IVTL+IB1> | <IVTL+IB2> | <IVTL+IBS>
;<IVTL> ::= 0x01
;<IB1> ::= 0x02
;<IB2> ::= 0x04
;<IBS> ::= 0x08
;
;Rx = <msg header><0x0D><ack><status><chksum>
;
[0E] Read/Write Navi Power Source
[RTIME]=200
[CTYPE]=3
;
B Power Source,N/A; 00=External, 01=POD Supplied
;
;Read/Write Navi Power Source <0x0E>
;This command controls whether VBU1 or Pod supplied voltage is connected to a NAVI tool.
;
;Note: This command can be used to verify the functionality of the battery control circuitry in a dual battery system. 
;
;Read Navi Power Source
;This command returns the current source of the Navi voltage.
;
;Tx = <msg header><0x0E><chksum>
;
;Rx = <msg header><0x0E><ack><status><tool voltage><chksum>
;
;<tool voltage> ::= <externally supplied> | <pod supplied>
;<externally supplied> ::= 0x00
;<pod supplied> ::= 0x01
;
;Write Navi Power Source
;This command controls the source of the Navi voltage.
;
;Tx = <msg header><0x0E><tool voltage><chksum>
;
;<tool voltage> ::= <externally supplied> | <pod supplied>
;<externally supplied> ::= 0x00
;<pod supplied> ::= 0x01
;
;Rx = <msg header><0x0E><ack><status><chksum>
;
[0F] Read/Write Batteries
[RTIME]=200
[CTYPE]=3
;
B Battery State,N/A; 00=Not Interrupted, 01=Interrupted
;
;Read/Write Batteries <0x0F>
;This command can interrupt the battery connection to a NAVI tool.  On Pod power up, the NAVI batteries are interrupted.
;
;Read Batteries
;This command returns the state of the external battery connection.
;
;Tx = <msg header><0x0F><chksum>
;
;Rx = <msg header><0x0F><ack><status><battery state><chksum>
;
;<battery state> ::= <interrupted> | <not interrupted>
;<not interrupted> ::= 0x00
;<interrupted> ::= 0x01
;
;Write Batteries
;This sets the state of the external battery connection.
;
;Tx = <msg header><0x0F><battery state><chksum>
;
;<battery state> ::= <interrupted> | <not interrupted>
;<not interrupted> ::= 0x00
;<interrupted> ::= 0x01
;
;Rx = <msg header><0x0F><ack><status><chksum>
;
[10] Read/Write Transparent Mode
[RTIME]=200
[CTYPE]=3
;
B CH A Mode,N/A;  00=OFF 01=ON
B CH B Mode,N/A;  00=OFF 01=ON
;
;Read/Write Transparent Mode  <0x10>
;This command controls the transparency of communication between the host & tool ports.  The port pairs are HostA/ToolA, HostB/ToolB.  
;When transparency mode is ON, host port traffic will echo on the associated tool port and tool port traffic will echo on the associated host port.  
;When a command addressed to the PowerComm Pod is received on a host port, bus traffic on the associated tool port will be ignored until the 
;Pod has finished its reply.
;
;Read Transparent Mode
;This command returns the state of the transparency mode.
;
;Tx = <msg header><0x10><chksum>
;
;Rx = <msg header><0x10><ack><status><channela><channelb><chksum>
;
;<channela> ::= <OFF> | <ON>
;<channelb> ::= <OFF> | <ON>
;<OFF> ::= 0x00
;<ON> ::= 0x01
;
;Write Transparent Mode
;This command controls the state of the transparency mode.
;
;Tx = <msg header><0x10><channela><channelb><chksum>
;
;<channela> ::= <OFF> | <ON>
;<channelb> ::= <OFF> | <ON>
;<OFF> ::= 0x00
;<ON> ::= 0x01
;
;Rx = <msg header><0x0A><ack><status><chksum>
;
[11] Read Comm. Channel
[RTIME]=200
[CTYPE]=1
;
B Channel,N/A; HostA=0x1, HOSTB=0x3
;
;Read Comm. Channel <0x11>
;This command responds with the host communication channel being queried.
;
;Tx = <msg header><0x11><chksum>
;
;Rx = <msg header><0x0F><ack><status><hostchannel><chksum>
;
;<hostchannel> ::= <HOSTA> | <HOSTB>
;<HOSTA> ::= 0x01
;<HOSTB> ::= 0x03
;
[12] Read/Write ToolA Type
[RTIME]=200
[CTYPE]=3
;
B Type,N/A; 00=IO1039, 01=DV485-B, 02=M30
;
;Read/Write ToolA Type <0x12>
;This command controls the routing of the ToolA communication.  ToolA can communicate on the DV-485-B bus or on the IO1039 bus or on 
;the M30 bus.
;
;Read ToolA Type
;This command returns the current routing of the ToolA communication.
;
;Tx = <msg header><0x12><chksum>
;
;Rx = <msg header><0x12><ack><status><toola type><chksum>
;
;< toola type > ::= <IO1039> | <DV-485-B> | <M30>
;<IO1039> ::= 0x00
;<DV-485-B> ::= 0x01
;<M30> ::= 0x02
;
;Write ToolA Type
;This configures the routing of the ToolA communication.
;
;Tx = <msg header><0x12><toola type ><chksum>
;
;< toola type > ::= <IO1039> | <DV-485-B> | <M30>
;<IO1039> ::= 0x00
;<DV-485-B> ::= 0x01
;<M30> ::= 0x02
;
;Rx = <msg header><0x12><ack><status><chksum>
;
[13] Read/Write BPS Rate
[RTIME]=500
[CTYPE]=3
;
B HostA, N/A; 0xBB=9600, 0xCC=19.2, 0x22=38.4, 0x00=NO CHANGE(Write Only)
B ToolA, N/A; 0xBB=9600, 0xCC=19.2, 0x22=38.4, 0x00=NO CHANGE(Write Only)
B HostB, N/A; 0xBB=9600, 0xCC=19.2, 0x22=38.4, 0x00=NO CHANGE(Write Only)
B ToolB, N/A; 0xBB=9600, 0xCC=19.2, 0x22=38.4, 0x00=NO CHANGE(Write Only)
;
;Read/Write BPS Rate <0x13>
;This command controls the BPS rate for each communication channel.
;
;Read BPS Rate
;This command returns the current BPS rate of all four communication channels.
;
;Tx = <msg header><0x13><chksum>
;
;Rx = <msg header><0x13><ack><status><hosta><toola><hostb><toolb><chksum>
;
;<hosta> ::= <bps>
;<toola> ::= <bps>
;<hostb> ::= <bps>
;<toolb> ::= <bps>
;
;<bps> ::= <9600>|<19200>|<38400>|<no change>
;<9600> ::= 0xBB
;<19200> ::= 0xCC
;<38400> ::= 0x22
;<no change> ::= 0x00
;
;Write BPS Rate
;This command changes the bps rate of all four communication channels.
;
;Tx = <msg header><0x13><hosta><toola><hostb><toolb><chksum>
;
;<hosta> ::= <bps>
;<toola> ::= <bps>
;<hostb> ::= <bps>
;<toolb> ::= <bps>
;
;<bps> ::= <9600>|<19200>|<38400>|<no change>
;<9600> ::= 0xBB
;<19200> ::= 0xCC
;<38400> ::= 0x22
;<no change> ::== 0x00
;
;Rx = <msg header><0x13><ack><status><chksum>
;
[14] Read POD State
[RTIME]=200
[CTYPE]=1
;
W State, Status; Pod State See Comspec
F Input Volts,0.1 volt; Input Volts
F Output Volts,0.1 volt; Output Volts
F Current, milliamps; Current
;
;Read Pod State <0x14>
;This command returns the data from a combination of commands.  The table below shows the 
;information contained in the pod state word.
;
;Bit (Hex)	Internal Label	Description	
;15 (0x8000)	LOAD2	100W load	
;14 (0x4000)	LOAD1	330W load	
;13 (0x2000)	MLOAD9	load equivalent to 9 modems	
;12 (0x1000)	MLOAD7	load equivalent to 7 modems	
;11 (0x0800)	MLOAD5	load equivalent to 5 modems	
;10 (0x0400)	MLOAD3	load equivalent to 3 modems	
;9 (0x0200)	VOUTDR	Output voltage switch	
;8 (0x0100)	SIMFLOW	Simulated or monitored Flow state	
;7 (0x0080)	DECODE | PULSE	Pulser monitor	
;6 (0x0040)	VDRV	Input voltage switch	
;5 (0x0020)	B1SIM	Battery one simulated	
;4 (0x0010)	BOFF	Batteries off (interrupted)	
;3 (0x0008)	IBS	Current read from Battery South	
;2 (0x0004)	IB2	Current read from Battery 2	
;1 (0x0002)	IB1	Current read from Battery 1	
;0 (0x0001)	IVTL	Current read from internal voltage	
;
;Tx = <msg header><0x13><chksum>
;
;Rx = <msg header><0x13><ack><status><pod state><input volts><output volts><current><chksum>
;
;<pod state> ::= see above table
;<input volts> ::= <IEEE FP>
;<output volts> ::= <IEEE FP>
;<current> ::= <IEEE FP>
;
[15] Start Passivation Test
[RTIME]=200
[CTYPE]=1
; Takes 10 seconds to do, will return 0x81 for ackcode is not finished, use 0x16 to get data
;
;Start Passivation Test <0x15>
;This command starts the battery passivation test.  The test asserts a 330W voltage load for 
;600ms, then adds a 100W voltage load (330W//100W) for 400ms.  This 1 second cycle is repeated 
;ten times for a total of 10 seconds.  During the tenth cycle, V1 is measured 350ms into the 
;330W load, then V2 is measured 350ms into the 330W//100W load.  While this test is in progress, 
;the PASSIVATION_TEST bit is asserted in the Node Status word.  If this command is received while 
;a passivation test is already in progress, an 0x81 acknowledge will be returned (passivation 
;test in progress) and the test in progress will not be disturbed.  Once the test has completed, 
;V1 and V2 can be read using the RdPassivationTestVoltages command. 
;
;Tx = <msg header><0x15><chksum>
;
;Rx = <msg header><0x15><ack><status><chksum>
;
[16] Read Passivation Test
[RTIME]=200
[CTYPE]=1
F V1,N/A;
F V2,N/A;
;
;Read Passivation Test Voltages <0x16>
;This command returns the results (V1 and V2) of the most recently completed Passivation Test.
;
;Tx = <msg header><0x16><chksum>
;
;Rx = <msg header><0x16><ack><status><V1><V2><chksum>
;
;<V1> ::= <IEEE FP>
;<V2> ::= <IEEE FP>
;
[17] Start Depassivation Test
[RTIME]=200
[CTYPE]=2
;
W Duration, seconds; 0=AUTO, 1-600 is manual
;
; Takes 10 seconds to do, will return 0x81 for ackcode is not finished, use 0x18 to get data
;
;Start Depassivation <0x17>
;This command starts the battery depassivation.  The procedure asserts a 330W voltage load for 
;600ms.  During the 600ms V1 is read every 100ms and these 6 readings are averaged to form V1ave.
;A 100W voltage load is then added (330W//100W)  for 400ms.  During this 400ms V2 is read every 
;100ms and these 4 readings are averaged to form V2ave.  This 1 second cycle is repeated for the 
;Depassivation Duration specified in the command.  If the Depassivation Duration is specified as 
;0, this indicates AUTO mode which runs until V1ave >= 31 volts AND V2ave is >= 24 volts, or for 
;10 minutes, whichever comes first.  During this procedure, the DEPASSIVATING bit is asserted in 
;the Node Status word.  If this command is received while a passivate test or a depassivation is
;already in progress, an 0x81 acknowledge will be returned (passivation test in progress) and the 
;test in progress will not be disturbed.  V1ave and V2ave can be read at the completion of a 1 
;second cycle using the RdDePassivateVoltages command.
;
;Tx = <msg header><0x17><depassivation duration><chksum>
;
;<depassivation duration> ::= <auto> | <manual>
;<auto> ::= 0
;<manual> ::= 1 to 600 (seconds)
;
;Rx = <msg header><0x17><ack><status><chksum>
;
[18] Read Depassivation Test
[RTIME]=200
[CTYPE]=1
F V1ave,N/A;
F V2ave, N/A;
;
;Read Depassivation Voltages <0x18>
;This command returns the most recent V1ave and V2ave.
;
;Tx = <msg header><0x18><chksum>
;
;Rx = <msg header><0x18><ack><status><V1ave><V2ave><chksum>
;
;<V1ave> ::= <IEEE FP>
;<V2ave> ::= <IEEE FP>
;
[19] Stop Depassivation Test
[RTIME]=200
[CTYPE]=1
;
;Stop Depassivate <0x19>
;This command stops any depassivation currently in progress.
;
;Tx = <msg header><0x19><chksum>
;
;Rx = <msg header><0x19><ack><status><chksum>
;
;
;
;Generic Commands and Responses
;These commands are the generic ADAM bus commands supported by all nodes.
;
;
;
[F1] Read/Write Master Status Register
[RTIME]=200
[CTYPE]=3
L MasterStatus, N/A; 0x80000000 = Master State, 0x40000000=Flow State, 0x20000000=Rotation, 0x10000000=High Inclination
;
;Read/Write Master Status Register <0xF1>
;The Master Status Register is a 32-bit register that is maintained by the Bus Master.  
;The Bus Master will broadcast (transmit to node <0x03>) this register whenever it changes.  
;Since this is a broadcast message and slave nodes do not respond, it is sent three times with 
;a 250 millisecond delay between each message.  This is to decrease the possibility of a slave 
;node not receiving it.
;
;The Master Status Register:
;0x80000000 (Bit 31):  Master State
;	1= the Bus Master is running tasks that require data from slave nodes.
;0x40000000 (Bit 30):  Flow State
;	1= Flow On has been detected by the Bus Master.
;0x20000000 (Bit 29):  Rotation State
;	1= Rotation has been detected by the Bus Master.
;0x10000000 (Bit 28):  Inclination State
;	1= High Inclination has been detected by the Bus Master.
;Bit 27 - Bit 0: Unassigned
;
;Read Master Status Register
;Requests the node transmit its copy of the MSR.
;
;Tx = <msg header><0xF1><chksum>
;
;Rx = <msg header><0xF1><command ack><status><MSR><chksum>
;
;<MSR> ::= <MSByte><byte><byte><LSByte>
;
;Write Master Status Register
;Commands the node to update its copy of the MSR and take whatever action is required.
;
;Tx = <msg header><0xF1><MSR><chksum>
;
;<MSR> ::= <MSByte><byte><byte><LSByte>
;
;Rx = Slave nodes do not respond to Broadcast Messages.
;

[F4] Read/Write Comments
[RTIME]=200
[CTYPE]=3
;
A256 Comments,N/A; Comments in/out from tool
;
;Read/Write Comments Command <0xF4>
;The Comment area is meant for general use. Information such as date of last calibration and 
;serial number of tool are the kind of thing that may be stored here. The data is read from and 
;written to non-volatile memory.
;
;Read Comments Command
;Requests the PowerComm to transmit the data in the comments area (256 bytes).
;
;Tx = <msg header><0xF4><chksum>
;
;Rx = <msg header><0xF4><ack><status><response data><chksum>
;
;Write Comments Command
;Commands the PowerComm to store data in the comments area.  Command data is limited to 256 bytes.
;
;Tx = <msg header><0xF4><command data><chksum>
;
;Rx = <msg header><0xF4><ack><status><chksum>
;
[F7] Read Checksum
[RTIME]=200
[CTYPE]=3
;
X Checksum,N/A;Checksum
X chk2
X chk3
;
;Read Checksum <0xF7>
;This command instructs the node to calculate and return a checksum of the code memory.
;
;Tx = <msg header><0xF7><chksum>
;
;Rx = <msg header><0xF7><ack><status><codesum><chksum>
;
[F9] Read/Write Mode
[RTIME]=200
[CTYPE]=3
;
B Mode,N/A; 03=Soft Reset, 04=Hard Reset
;
;Read/Write Mode Command <0xF9>
;Reads the current mode of operation or changes the current mode of operation.  PowerComm does 
;not support bootrom mode.
;
;Read Mode Command
;Requests the PowerComm transmit the current mode.
;
;Tx = <msg header><0xF9><chksum>
;
;Rx = <msg header><0xF9><ack><status><mode><chksum>
;
;<mode> ::= <soft reset> | <hard reset>
;<soft reset> ::= <03>
;<hard reset> ::= <04>
;
;Write Mode Command
;Commands the PowerComm to change its operating mode.
;
;Tx = <msg header><0xF9><mode><chksum>
;
;Rx = <msg header><0xF9><ack><status><chksum>
;
[FA] R/W Memory
[RTIME]=2000
[CTYPE]=3   ;Read / Write command
B Type,type;0=Internal RAM, 1=External RAM, 2=Code space
T Address,hex;Address to read or write
W Bytes,bytes;number of bytes to either read or write
A Data,bytes;variable length data array read or to write
;
;Memory Read Command <0xFA>
;Requests the PowerComm to transmit the specified number of bytes beginning at the specified 
;start address in memory. 
;
;Tx = <msg header><0xFA><memory type><address><byte count>< chksum>
;
;<memory type> ::= <RAM> | <ROM>
;<RAM> ::= <01>
;<ROM> ::= <02>
;<address> ::= <ms byte><byte><LSB byte>
;<byte count> ::= <ms byte><LSB byte>
;
;Rx = <msg header><0xFA><ack><status><memory type><address><byte count><data> {<data>} <chksum>
;
;<memory type> ::= <RAM> | <ROM>
;<RAM> ::= <0x01>
;<ROM> ::= <0x02>
;<address> ::= <ms byte><byte><LSB byte>
;<byte count> ::= <ms byte><LSB byte>
;<data> ::= byte
;
[FB] Read Status
[RTIME]=200
[CTYPE]=1
;
W Status,N/A;
;
;Status Read Command <0xFB>
;This command requests the complete status of the PowerComm node.
;
;Tx = <msg header><0xFB><chksum>
;
;Rx = <msg header><0xFB><ack><status><chksum>
;
[FC] Read Part Number
[RTIME]=200
[CTYPE]=1
;
A10 PartNumber,ascii; 
;
;Software P/N Read Command <0xFC>
;This command requests the PowerComm to transmit its software identification part number to the 
;System Master.
;
;Tx = <msg header><0xFC><chksum>
;
;Rx = <msg header><0xFC><ack><status><numeric character><numeric character><numeric character>
;	<numeric character><numeric character><alpha character><alphanumeric character>
;	<numeric character><numeric character><numeric character><chksum>
;
;<numeric character><numeric character><numeric character><numeric character><numeric character> ::= drawing number
;<alpha character> ::= "R" | "T" where "R" is a commercial release "T" is a test release
;<alphanumeric character> ::= characters A-Z and 0-9
;<numeric character><numeric character><numeric character> ::= release number
;
;NOTE: Check the part number guidelines for more information on the definition of a part number.
;
;
;