;*********************************************************************
; DESCRIPTION: This is the comspec file for the Short Radius Inclination with Gamma (SRIG) 
; (nodename = E4)
;
; CREATED BY:  Matt Wandstrat
;
;*********************************************************************
;
; The HEADER section defines the partnumber of this COMSPEC File, its date, and its time
;
;--------------------------------------------------------------------------------------
[HEADER]
PN=77557ca100
DATE=6/18/1999
TIME=14:30:00
;--------------------------------------------------------------------------------------
; The BOOTLOADER section defines the code space for this node as well as the boot name
; The SADDR defines the starting address (passed bootrom) for code (HEX Number)
; The EADDR defines the last valid address of program space
; The BOOTNAME is the two-digit hexadecimal name of the bootrom node
;--------------------------------------------------------------------------------------
[BOOTLOADER]
SADDR=0
EADDR=FFFFFF
BOOTNAME=??
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
[BIT 15] Accelerometer is in Calibration mode (busy)
;
[BIT 14] First Accelerometer acquisition is in progress (busy)
;
[BIT 13] Uninitialized Interrupt Reset
;
[BIT 12] Computer Operating Properly Reset
;
[BIT 11] Clock Monitor Reset
;
[BIT 10] Illegal Opcode Reset
;
[BIT 9] External RAM Error
;
[BIT 8] Internal RAM Error
;
[BIT 7] Program Memory Checksum Error
;
[BIT 6] Data Memory Checksum Error
;
[BIT 5] Error Writing EEPROM
;
[BIT 4] Internal EEPROM Error
;
[BIT 3] HVPS is off
;
[BIT 2] HVPS Calibrate Command is in progress (busy)
;
[BIT 1] First Gamma acquisition is in Progress (busy)
;
[BIT 0] First Temperature acquisition is in Progress (busy)
;
;--------------------------------------------------------------------------------------
;
; This is the CMDS section. Define all valid commands and their respective formats
; It has the following format:
; [xx] Descriptor
; [RTIME]=ms where ms is the maximum response time in millisec
; [CTYPE]=?
;   The [CTYPE] (command type is defined as following:
;
;   [CTYPE]=1 is a command that takes no other input than the command itself, and returns data.
;   [CTYPE]=2 is a command that takes a bucket input and returns data
;   [CTYPE]=3 is a read/write type command. When just the command is sent, it returns data, but when data is added to the send message, it writes the data, and returns only a minimal response.
;
;   If your command does not fall into any of the above categories you may use zero ([CTYPE]=0).
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
;
[01] Read Gamma Ray Data
[RTIME]=200  
[CTYPE]=2
;
B CMDEXT,N/A;Command Extention
F Gamma,cps;Gamma Ray counts per second
;
;This command tells the node to transmit gamma ray data.  This data is the average of "n" buckets
;(acquisitions).  The gamma ray returned is IEEE floating point counts per second.  Bucket 
;limits are 1 (0x01) to 32 (0x20).  If the bucket count requested is outside the limits, the 
;node returns an <0xFD> cmd ack with no data.  If more buckets are requested than what is 
;available, the bucket count in the response indicates the number of buckets that were averaged.
;
;Tx = <msg header> <0x01> <cmd ext> <chksum>
;<cmd ext> ::= <0x01>...<0x20>
;
;Rx = <msg header> <0x01> <cmd ack> <slave status> <cmd ext> <gamma ray> <chksum>
;<cmd ack>  ::= <0x00> | <0xFD>
;<cmd ext>  ::= <0x01>...<0x20>
;<gamma ray>    ::= <IEEE FP>
;
[05] Read Temperature Data
[RTIME]=200  
[CTYPE]=2
;
B CMDEXT,N/A;Command Extention
F Temp,deg C;temperature of the SRIG
;
;This command tells the node to transmit the internal temperature of the SRIG based on "n" 
;buckets of acquisitions (averaged).  One acquisition is the average of four analogs to digital 
;conversions.  The temperature returned is IEEE floating point degrees Celsius.  Bucket limit 
;is 1.  If the bucket count is outside the limit, the node returns an <0xFD> cmd ack with no data.
;
;Tx = <msg header> <0x05> <cmd ext> <chksum>
;<cmd ext> ::= <0x01>
;
;Rx = <msg header> <0x05> <cmd ack> <slave status> <cmd ext> <temperature> <chksum>
;<cmd ack> ::= <0x00> | <0xFB> | <0xFD>
;<cmd ext> ::= <0x01>
;<temperature> ::= <IEEE FP>
;
[06] Read  Accelerometer Raw Data
[RTIME]=200  
[CTYPE]=2
;
B Avg Type,N/A; 01=Short, 02=Medium, 03=Long
F VGz,volts;Raw voltage from the 16 bit ADC that is attached to the Z axis accelerometer
;
;This command request for the raw voltage data from the 16 bit ADC that is attached to the Z-axis
;accelerometer.
;Tx = <msg header> <0x06> <cmd ext> <chksum>
;<cmd ext> ::= <0x01> | <0x02> | <0x03>
;
;Rx = <msg header> <0x06> <cmd ack> <slave status> <cmd ext> <VGz> <chksum>
;<cmd ack>  ::= <0x00> | <0xFB> | <0xFD>
;<cmd ext>  ::= <0x01> | <0x02> | <0x03>
;<VGz> ::= <IEEE FP>
;
[0C] Read/Write KTABLE
[RTIME]=200  
[CTYPE]=3
;
X CS,N/A;Check Sum
A11 PN,N/A;Part Number
B MBS,bytes;N/A;Medium Bucket Size
B LBS,bytes;Long Bucket Size
W CT,N/A;Calibration Time
B AF,N/A;Acquire Frequency
W SBS,N/A;Short Bucket Size
A44 R,N/A;Reserved
;
;This command read/write the 64 bytes KTABLE stored in the MCU's EEPROM memory. The layout about 
;the KTABLE is listed as follows.
;
;Field Name         Data Type   Length  Default Value   Min. Value  Max. Value  
;Check sum          ushrt       2       *           *       *   
;Part number        char        11      77557TD100  *       *   
;Medium bucket size uchar       1       4           1       255 
;Long bucket size   uchar       1       8           1       255 
;Calibration time   ushrt       2       30          1       65535   
;Acquire frequency  uchar       1       20          1       255 
;Short bucket size  ushrt       2       300         1       65535   
;Reserved           char        44      0           *       *   
;
;Note: A total of 64 bytes is reserved for the KTABLE. All unused and reserved bytes are 
;initialize to zero.  The calibration time is no longer in use, please ignore it.
;
;Read KTABLE
;This command instructs the node to return the KTABLE which is stored in the MCU's EEPROM memory.
;
;Tx = <msg header> <0x0C> <chksum>
;
;Rx = <msg header> <0x0C> <cmd ack> <slave status> <KTABLE> <chksum>
;<cmd ack>  ::= <0x00>
;<KTABLE>   ::= <Table of 32 bytes>
;
;Write KTABLE
;This command instructs the node to change the contents of the KTABLE which is stored in the 
;MCU's EEPROM memory.
;
;Tx = <msg header> <0x0C> <KTABLE> <chksum>
;<KTABLE> ::= <Table of 32 bytes>
;
;Rx = <msg header> <0x0C> <cmd ack> <slave status> <chksum>
;<cmd ack> ::= <0x00> | <0xFD>
;
[11] Read Accelerometer Inclination Data
[RTIME]=200  
[CTYPE]=2
;
B CmdExt,N/A;Command Extention
F Inc,deg;Inclination
;
;This command request to read the inclination from the MCU. The command extension is the size 
;of the bucket.
;
;Tx = <msg header> <0x11> <cmd ext> <chksum>
;<cmd ext> ::= <0x01> | <0x02> | <0x03>
;
;Rx = <msg header> <0x11> <cmd ack> <slave status> <cmd ext> <Inclination> <chksum>
;<cmd ack>   ::= <0x00> | <0xFB> | <0xFD>
;<cmd ext>   ::= <0x01> | <0x02> | <0x03>
;<Inclination>  ::= <IEEE FP>
;
[13] Read/Write Gamma Ray Acquisition Time
[RTIME]=200  
[CTYPE]=3
;
W Acq time,10th sec;Acquisition Time
;
;The gamma acquisition time is the period used to acquire one gamma sample.  A 'bucket' refers 
;to one of these acquisitions.  The units used are tenths of a second and the limits are 
;10 to 600 (1s to 60s).
;
;Read Gamma Ray Acquisition Time
;This command instructs the node to return the gamma acquisition time.  Gamma Acquisition time 
;reads from and writes to non-volatile memory.
;
;Tx = <msg header> <0x13> <chksum>
;
;Rx = <msg header> <0x13> <cmd ack> <slave status> <acq time> <chksum>
;<cmd ack> ::= <0x00>
;<acq time> ::= <MSByte> <LSByte>
;
; Write Gamma Ray Acquisition Time
;This command instructs the node to change the gamma acquisition time.  The acquisition time 
;changes immediately but this does not affect any sample in progress.  If acq time is outside 
;the limit, the node returns an <0xFD> cmd ack and sets it to the limit.
;
;Tx = <msg header> <0x13> <acq time> <chksum>
;<acq time> ::= <MSByte> <LSByte>
;
;Rx = <msg header> <0x13> <cmd ack> <slave status> <chksum>
;<cmd ack> ::= <0x00> | <0xFD>
;
[18] Read Accelerometer corrected data
[RTIME]=200  
[CTYPE]=2
;
B CmdExt,N/A;Command Extention
F Gz,g's;accelerometer Z axis data which is corrected for bias, scale factor, and 
;   temperature
;
;This command request for the accelerometer Z-axis data which is being corrected for bias, 
;scale factor, and temperature, using 3rd-order models.
;
;Tx = <msg header> <0x18> <cmd ext> <chksum>
;<cmd ext> ::= <0x01> | <0x02> | <0x03>
;
;Rx = <msg header> <0x08> <cmd ack> <slave status> <cmd ext> <Gz> <chksum>
;<cmd ack    ::= <0x00> | <0xFB> | <0xFD>
;<cmd ext>   ::= <0x01> | <0x02> | <0x03>
;<Gz>       ::= <IEEE FP>
;
[1E] Read the MCU's A/D converter
[RTIME]=200  
[CTYPE]=2
;
B CmdExt,N/A;Command Extention
F ADC,N/A;ADC Reading
;
;This command allows reading each of the converting result of the 68HC11's A/D converter. The 
;A/D system is an 8-channel, 8-bit, multiplexed-input converter. The SRIG board currently attach 
;eight signal to the analog port. This command is used to debug both of the software and the 
;hardware.
;
;Tx = <msg header> <0x1E> <cmd ext> <chksum>
;<cmd ext> ::= <0x00>..<0x07>
;
;Each command extension related to one analog signal. The current SRIG board design for the 
;analog input and the relation to the command extension is shown as follows.
;
;<0x00> : +15 VDC signal.
;<0x01> : +5 VDC signal.
;<0x02> : DV485 signal.
;<0x03> : High voltage control signal.
;<0x04> : Temperature sensor signal.
;<0x05> : Temperature circuit calibration signal.
;<0x06> : 2.5V reference signal.
;<0x07> : Power supply current.
;
;Rx = <msg header> <0x1E> <cmd ack> <slave status> <cmd ext> <ADC reading> <chksum>
;<cmd ack>  ::= <0x00> | <0xFD>
;<cmd ext>  ::= <0x00>..<0x07>
;<ADC reading>  ::= <IEEE FP>
;
[1F] Read/Write CONFIG Register
[RTIME]=200  
[CTYPE]=3
;
B config,N/A;Config Register
;
;This command allows reading and writing of the MC68HC711's CONFIG register. This register 
;controls the MCU's COP system, enable/disable the internal EPROM and EEPROM memory. Since the 
;CONFIG register is implemented with EEPROM cells, use EEPROM procedures to erase and program 
;this register. To change the value in the CONFIG register, complete the following procedure. 
;Do not initiate a reset until the procedure is complete.
;
;              1. Erase the CONFIG register.
;              2. Program the new value to the CONFIG address.
;              3. Initiate reset.
;
;Read CONFIG Register
;This command reports the value of the MC68HC711's config register.  The operating value should 
;be 0x0B (No security and  COP, ROM & EEPROM enabled).
;
;Tx = <msg header> <0x1F> <chksum>
;
;Rx = <msg header> <0x1F> <cmd ack> <slave status> <config> <chksum>
;<cmd ack>  ::= <00>
;<config> ::= <byte>
;
; Write CONFIG Register
;This command will change the value of the MC68HC711's config register.  The change occurs in 
;the EE register only and does not take effect on the operation until a power reset occurs.
;
;Tx = <msg header> <0x1F> <config> <chksum>
;<config> ::= <byte>
;
;Rx = <msg header> <0x1F> <cmd ack> <slave status> <chksum>
;<cmd ack> ::= <0x03>
;
[20] Device Control
[RTIME]=200  
[CTYPE]=2
;
B Hex,Hex;Hex Byte
F ADC,N/A;ADC Reading
;
;This command allows the control of the SRIG built-in devices. This command is written to aid 
;the testing of the hardware.
;
;Tx = <msg header> <0x20> <cmd ext> <chksum>
;<cmd ext> ::= <hex byte>
;The one byte command extension can be divided into two parts: the device number and the 
;function number. The device number is a 5-bit number which is from bit3 to bit7 of the command 
;extension, and the function number is a 3-bit number which is from bit0 to bit2. The SRIG board 
;therefore can have up to 32 devices, and each device can have up to 8 control functions.
;
;The current implemented SRIG device control is stated as follows.
;
;Device name                Device# Func#  Description                 cmd ext
;_____________________________________________________________________________________
;Accelerometer power supply 00000   000    Turn off                    <0x00>
;                                   001    Turn on                     <0x01>
;Serial Communication       00001   000    Enable DV485/Disable LT1039 <0x08>
;                                   001    Enable LT1039/Disable DV485 <0x09>
;ADC7701                    00010   000    Start calibration           <0x10>
;
;When turn on the accelerometer power supply, the firmware will also wake up the ADC7701 and 
;start the accelerometer acquisition timer. When turn off the accelerometer power, the firmware 
;will put the ADC7701 into sleep mode and stop the acquisition timer.  While both of the 
;accelerometer power supply and ADC7701 control is effective immediately,the serial communication
;control will only be effective after reset, thus the return cmd ack will be 0x03.
;
;Rx = <msg header> <0x20> <cmd ack> <slave status> <cmd ext> <ADC reading> <chksum>
;<cmd ack> ::= <0x00> | <0x03> | <0xFD>
;<cmd ext> ::= <hex byte>
;<ADC reading>::= <IEEE FP>
;
[40] Set HVPS Low
[RTIME]=200  
[CTYPE]=0
;
;Use this command to learn the lowest possible voltage of the HVPS.  You will need this 
;information to calibrate the HVPS.  This command sets the DAC to 0x00, the node returns a 
;<0x01> cmd ack and the HVPS turns on for ten seconds.  Record this voltage for future use.
;
;Tx = <msg header> <0x40> <chksum>
;
;Rx = <msg header> <0x40> <cmd ack> <slave status> <chksum>
;<cmd ack> ::= <0x01> | <0x04>
;
[41] Set HVPS High
[RTIME]=200  
[CTYPE]=0
;
;Use this command to learn the highest  possible voltage of the HVPS.  You will need this 
;information to calibrate the HVPS.  This command sets the DAC to 0xFF, the node returns a 
;<0x01> cmd ack and the HVPS turns on for ten seconds.  Record this voltage for future use.
;
;Tx = <msg header> <0x41> <chksum>
;
;Rx = <msg header> <0x41> <cmd ack> <slave status> <chksum>
;<cmd ack> ::= <0x01> | <0x04>
;
[42] Read/Write Calibrate HVPS
[RTIME]=200  
[CTYPE]=3
;
W Low,volts;Low Volt
W High,volts;High Volt
W Test,volts;Test Volt
;
;Use the recorded high and low voltages to calibrate the HVPS.  The tool uses these parameters 
;to calculate a scale factor and an offset.  LowVolt, HighVolt and TestVolt read from and write 
;to non-volatile memory.
;
;Read Calibrate HVPS
;This command will read the HighVolt/LowVolt/TestVolt parameters.
;
;Tx = <msg header> <0x42> <chksum>
;
;Rx = <msg header> <0x42> <cmd ack> <slave status> <LowVolt> <HighVolt> <TestVolt> <chksum>
;<cmd ack>  ::= <0x00>
;<LowVolt>  ::= <MSByte> <LSByte>
;<HighVolt> ::= <MSByte> <LSByte>
;<TestVolt> ::= <MSByte> <LSByte>
;
;Write Calibrate HVPS
;When you invoke this command with the hexadecimal voltage parameters, the tool will store the 
;voltages in non-volatile memory.  It will then calibrate the HVPS and set it to <TestVolt> for 
;ten seconds.  This is so the user can evaluate the accuracy of the newly calculated HVPS bias 
;and scale factor.
;
;Tx = <msg header> <0x42> <LowVolt> <HighVolt> <TestVolt> <chksum>
;<LowVolt>  ::= <MSByte> <LSByte>
;<HighVolt> ::= <MSByte> <LSByte>
;<TestVolt> ::= <MSByte> <LSByte>
;
;Rx = <msg header> <0x42> <cmd ack> <slave status> <chksum>
;<cmd ack> ::= <0x01> | <0x04>
;
;Note: If the slave node's gamma state is in the power management mode when command number 0x40, 
;0x41, or 0x42 is received, the firmware will shut off the power management and set the high 
;voltage accordingly for 10 seconds. If one needs to resume the power management, then he needs 
;to issue the related command (0xF1, 0xF8) to initiate the power management.
;
[43] Read/Write Voltage
[RTIME]=200  
[CTYPE]=3
;
W voltage,volts;Operating voltage level for the HVPS
;
;This command sets the operating voltage level for the HVPS.  HVPS Voltage reads from and writes 
;to non-volatile memory.
;
;Read Voltage
;This command reads the HVPS operating voltage.
;
;Tx = <msg header> <0x43> <chksum>
;
;Rx = <msg header> <0x43> <cmd ack> <slave status> <voltage> <chksum>
;<cmd ack> ::= <0x00>
;<voltage> ::= <MSByte> <LSByte>
;
;Write Voltage
;This command sets a new voltage level for the HVPS.  The HVPS voltage level changes immediately.
;If data acquisition is in progress, the current bucket should be suspect.  The units are Volts 
;and the limits are <LowVolt> and <HighVolt>.  (These voltage parameters were stored using the 
;Calibrate HVPS command.)  If Voltage is outside the limits, an <0xFD> cmd ack is returned and 
;it is set to the limit.
;
;Tx = <msg header> <0x43> <voltage> <chksum>
;<voltage> ::= <MSByte> <LSByte>
;
;Rx = <msg header> <0x43> <cmd ack> <slave status> <chksum>
;<cmd ack> ::= <0x00> | <0xFD>
;
[44] Read HVPScorr
[RTIME]=200  
[CTYPE]=1
;
B hvpscorr,N/A;HVPScorr is the hex value being written to the HVPS DAC to produce a specified 
;   voltage.
;
;Tx = <msg header> <0x44> <chksum>
;
;Rx = <msg header> <0x44> <cmd ack> <slave status> <hvpscorr> <chksum>
;<cmd ack>  ::= <0x00>
;<hvpscorr> ::= <byte>
;
[46] Read/Write HVPS Warm-up
[RTIME]=200  
[CTYPE]=3
;
B wm up,10ths sec;HVPS Warm-up is the delay between turning on the HVPS and starting an 
;   acquisition.  Units are tenths of a second and limits are 0.0s to 10.0s.  It is read from 
;   and written to non-volatile memory.
;
;Read HVPS Warm-up
;
;Tx = <msg header> <0x46> <chksum>
;
;Rx = <msg header> <0x46> <cmd ack> <slave status> <warm-up> <chksum>
;<cmd ack>  ::= <0x00>
;<warm-up>  ::= <hex byte>
;
;Write HVPS Warm-up
;This command instructs the node to change the HVPS warm-up time.  If warm-up is outside the 
;limits, an <0xFD> cmd ack will be returned and it will be set to the appropriate limit.
;
;Tx = <msg header> <0x46> <warm-up> <chksum>
;<warm-up> ::= <hex byte>
;
;Rx = <msg header> <0x46> <cmd ack> <slave status> <chksum>
;<cmd ack> ::= <0x00> | <0xFD>
;
[50] Read/Write CTABLE
[RTIME]=200  
[CTYPE]=3
;
X CS,N/A;Check Sum
A11 PN,N/A;Part Number
F Ksfz0,N/A;Scale Factor
F Ksfz1,N/A;Scale Factor
F Ksfz2,N/A;Scale Factor
F Ksfz3,N/A;Scale Factor
F Kbz0,N/A;Bias Constant
F Kbz1,N/A;Bias Constant
F Kbz2,N/A;Bias Constant
F Kbz3,N/A;Bias Constant
A7 Acc SN,N/A;Accelerometer Serial Number
A8 Date,N/A;Date
A5 Time,N/A;Time
A15 Res,N/A;Reserved
;
;The CTABLE is used to calibrate the Accelerometer and is a 80 bytes long table, therefore, 
;the CTABLE should be written into the EEPROM memory of the MCU prior to the operation in order 
;to get the correct inclination data. The format of the CTABLE is listed as follows
;
;Field name             Data type   Length  Default value
;_____________________________________________________________________  
;Check sum              ushrt       2       *   
;Part number            char        11      77557TE100  
;Scale factor - Ksfz0   float       4       0.0 
;Scale factor - Ksfz1   float       4       1.0 
;Scale factor - Ksfz2   float       4       0.0 
;Scale factor - Ksfz3   float       4       0.0 
;Bias constant - Kbz0   float       4       0.0 
;Bias constant - Kbz1   float       4       1.0 
;Bias constant - Kbz2   float       4       0.0 
;Bias constant - Kbz3   float       4       0.0 
;Accelerometer S/N      char        7       *   
;Date                   char        8       *   
;Time                   char        5       *   
;Reserved               char        15      *   
;  
;Read CTABLE
;This command instructs the node to return the CTABLE which is stored in the MCU's EEPROM memory.
;
;Tx = <msg header> <0x50> <chksum>
;
;Rx = <msg header> <0x50> <cmd ack> <slave status> <CTABLE> <chksum>
;<cmd ack>  ::= <0x00>
;<CTABLE>   ::= <Table of 80 bytes>
;
; Write CTABLE
;This command instructs the node to change the contents of the CTABLE.
;
;Tx = <msg header> <0x50> <CTABLE> <chksum>
;<CTABLE> ::= <Table of 80 bytes>
;
;Rx = <msg header> <0x50> <cmd ack> <slave status> <chksum>
;<cmd ack> ::= <0x00> | <0xFD>
;
[51] Read/Write Local Gravity
[RTIME]=200  
[CTYPE]=3
;
F local,N/A;local gravity, The local gravity is stored in the MCU's EEPROM memory which is used 
;to calculate the inclination. In most places, this number should be 1.00 where some other places
;might have silghtly different from 1.00.  The local gravity should also be written into the 
;EEPROM when calibrate the SRIG in order to get the correct inclination data during operation.
;
;Read  Local Gravity
;
;Tx = <msg header> <0x51> <chksum>
;
;Rx = <msg header> <0x51> <cmd ack> <slave status> <local gravity> <chksum>
;<cmd ack>  ::= <0x00>
;<local gravity>    ::= <IEEE FP>
;
;Write Local Gravity
;This command instructs the node to change the local gravity.
;
;Tx = <msg header> <0x51> <local gravity> <chksum>
;<local gravity> ::= <IEEE FP>
;
;Rx = <msg header> <0x51> <cmd ack> <slave status> <chksum>
;<cmd ack> ::= <0x00> | <0xFD>
;
[60] Change Node Name
[RTIME]=200  
[CTYPE]=0
;
B node,N/A;node name, This command will change the value of the SRIG's node name. The new node 
;name will only be effective after reset.
;
;Tx = <msg header> <0x60> <node name> <chksum>
;<node name> ::= <byte>
;
;Rx = <msg header> <0x60> <cmd ack> <slave status> <chksum>
;<cmd ack> ::= <00>
;
;
;
;Generic ADAM Bus Commands and Responses
;These commands are the generic ADAM bus commands supported by all nodes.
;
[F1] Read/Write Master Status Register
[RTIME]=200  
[CTYPE]=3
;
L MSR,N/A;Master status Registar
;
;The Master Status Register is a 32-bit register that is maintained by the Bus Master.  The 
;Bus Master will broadcast (transmit to node <0x03>) this register whenever it changes.  Since 
;this is a broadcast message and slave nodes do not respond, it is sent three times with a 250 
;millisecond delay between each message.  This is to decrease the possibility of a slave node 
;not receiving it.
;
;The Master Status Register:
;
;Bit 31:  Master State
;   1= The Bus Master is running tasks that require data from slave nodes. When this
;   bit is set, and the rotation state match the log requirement in the recorder task
;   record is match, the firmware will put the node into power management mode.
;   0= The Bus Master is not running any tasks which require slave node data.
;   Whenever the slave node receive the MSR with this bit clear, it will shut down the
;   power management function.
;Bit 30:  Flow State
;   1= Flow On has been detected by the Bus Master.
;   0= Flow Off has been detected by the Bus Master.
;Bit 29:  Rotation State
;   1= Rotation has been detected by the Bus Master.
;   0= Rotation has not been detected by the Bus Master.
;Bit 28:  Inclination State
;   1= High Inclination has been detected by the Bus Master.
;   0= Low Inclination has been detected by the Bus Master.
;Bit 27 - Bit 0
;   Unassigned
;Read Master Status Register
;Requests the node transmit its copy of the MSR.
;
;Tx = <msg header> <0xF1> <chksum>
;
;Rx = <msg header> <0xF1> <cmd ack> <slave status> <MSR> <chksum>
;<cmd ack>::= <0x00>
;<MSR>::= <MSByte> <byte> <byte> <LSByte>
;
;
; Write Master Status Register
;Commands the node to update its copy of the MSR and take whatever action is required. This 
;command normally is broadcast by the Master node.
;
;Tx = Command
;<msg header> <0xF1> <MSR> <chksum>
;<MSR> ::= <MSByte> <byte> <byte> <LSByte>
;
;Slave nodes do not respond to Broadcast Messages.
;
[F4] Read/Write Comments
[RTIME]=1500
[CTYPE]=3
;
A256 Comm,N/A;Comments 256 bytes
;
;The Comment area is meant for general use.  Information such as date of last calibration and 
;serial number of tool are the kind of thing that may be stored here.  The data is read from 
;and written to non-volatile memory.
;
; Read Comments
;Requests the node transmit the data in the comments' area.  The size of this area is 256 bytes.
;
;Tx = <msg header> <0xF4> <chksum>
;
;Rx = <msg header> <0xF4> <cmd ack> <slave status> <response data> <chksum>
;<cmd ack> ::= <0x00>
;
; Write Comments
;Commands the node to store data in the comments area.  The length of the data string is variable,
;but is limited to 256 bytes.  Writing of data will begin at the start of the comments' area.
;
;Tx = <msg header> <0xF4> <command data> <chksum>
;
;Rx = <msg header> <0xF4> <cmd ack> <slave status> <chksum>
;<cmd ack> ::= <0x00>
;
[F6] Read/Write Baud Rate
[RTIME]=200  
[CTYPE]=3
;
W Baud,bps;This command can read or change the baud rate of the MCU's serial communication 
;interface.
;
; Read Baud Rate
;Read the baud rate from the MCU.
;
;Tx = <msg header> <0xF6> <chksum>
;
;Rx = <msg header> <0xF6> <cmd ack> <slave status> <baud rate> <chksum>
;<cmd ack>  ::= <0x00> | <0xFD>
;<baud rate>    ::= <MSByte> <LSByte>
;
;Write Baud Rate
;Commands the node that we are going to change the MCU's operating baud rate.
;Since the command will not effective immediately, the normal response will be 0x01.
;
;Tx = <msg header> <0xF6> <baud rate> <chksum>
;<baud rate>    ::= <4800> | <9600> | <19200> | <38400>
;
;Rx = <msg header> <0xF6> <cmd ack> <slave status> <chksum>
;<cmd ack> ::= <0x01> | <0xFB> | <0xFD>
;
[F7] Read Checksum
[RTIME]=200  
[CTYPE]=1
;
X Code Sum,hex;This command instructs the node to calculate and return a checksum of the code 
;   memory.
;
;Tx = <msg header> <0xF7> <chksum>
;
;Rx = <msg header> <0xF7> <cmd ack> <slave status> <codesum> <chksum>
;<codesum> ::= <MSByte> <LSByte>
;
[F8] Read/Write Recorder Task 
[RTIME]=200  
[CTYPE]=2
;
B cmd ext,hex;Command Extention
W init delay,10ths sec;Initial Delay
W period,10ths sec;time between first request for data, and subsequent requests for data
B records,records;maximum number of data requests that will be received
B buckets,samples;number of samples that will be asked for with each data request
B log mode,hex;a flag that indicates whether data is required during rotating mode, non-rotating 
;   mode, or both
;
;Recorder Task setup information is received from the master.  There is separate information for 
;each type of data to be collected (e.g. for this node, there could be one for Gamma Ray, one 
;for Temperature[not a concern for power conservation], and one for the Accelerometer).
;
;sub-slave: hex value of the command used to request data. The cmd extension for Gamma is <0x01>,
;    for temperature is <0x05>, for accelerometer is <0x06>.
;init delay: time (from now) until first request for information will come.  Units are tenths of 
;   a second and the limits are from the sum of safety time, dormant time, warm-up time, and 
;   acquisition time to 32767.
;period: time between first request for data, and subsequent requests for data.  Units are tenths
;   of a second and the limits are from the sum of dormant time, warm-up time, and acquisition 
;   time to 32767.
;records: maximum number of data requests that will be received.  If a slave node receives 
;   this  many requests, it assumes no more requests and goes into low power mode.  0x00 or 
;   0xFF (255) means there is no maximum (run forever).
;buckets: (note: this is called "cmnd ext" in SUIT) number of samples that will be asked for with 
;   each data request.  For gamma, the number is between <0x01> to <0x40>. While for 
;   Accelerometer, this number is between <0x01> to <0x03> to indicate the type of the bucket
;   (short = 300 samples, intermediate=1200 samples, long=2400 samples) that we requested.
;log mode:  a flag that indicates whether data is required during rotating mode, non-rotating 
;   mode, or both.  
;   0x02 (Bit1):    1= Data is not required when not rotating.
;               0= Data is required when not rotating.
;   0x01 (Bit0):    1= Data is not required when rotating.
;               0= Data is required when rotating.
;
;The power management state can be described as dormant state, warm-up state, and acquire state. 
;The init delay and period can be calculated by simply adding up these three state's time units. 
;In order to guaranteed that the Master node will always obtain the "freshest" information from 
;the slave node, we have put a 0.1 second delay into the init delay.
;
;The default value for the recorder task is 100 (10 seconds) for init delay, 100 (10 seconds)for 
;period, 0 for records, 1 for buckets, and 0 for log mode. The firmware will run without getting 
;into the dormant state, but we strongly recommend you put some value into the dormant state 
;since this is the whole purpose of designing this power management scheme.
;
;For the gamma HVPS the warm up time is 1 second.  The minimum acquisition time is 2.5 seconds.  
;To calculate the acquisition time from the parameters passed to this command simply multiply the
;number of buckets times 2.5.  To calculate the total on time of the HVPS add the acquisition 
;time to the warm up time of 1 second.  For example if the number of buckets is 2 then:
;           acquire time =  2 * 2.5 = 5 seconds
;           total on time = 5s + 1s = 6 seconds.
;
;If the period passed to this function is 100 (10 seconds) then the duty cycle for the gamma 
;HVPS will be 6/10 or 60%.
;
;The acquire time for the accelerometer can be calculated as follows:
;           acquire time = (Acquisition decimator)*(Bucket size) / 1200
;
;For example, the default KTABLE value for the accelerometer acquisition decimator is 20, and the
;short bucket size is 300, the accelerometer warm-up time is 2.6 seconds, then:
;                       acquire time = (20*300)/1200 = 5 seconds
;                       total on time = 5s + 2.6s = 7.6s
;
;If the period passed for this function is 100 time units (10 seconds) then the duty cycle for 
;power to the accelerometer is 7.6/10 or 76%.
;
;Please note that the acquire time 5 seconds is for the short bucket, if you want to acquire the 
;medium bucket or the long bucket, use the medium or long bucket size (1200 or 2400) in order to 
;get the correct acquire time. For example, if one wants to acquire the medium bucket with the 
;default value, then the acquire time will be:
;           (20 * 1200) /1200  =  20s
;
;Note: There are two ways to initiate the power management: issue command 0xF1 or command 0xF8. 
;The difference is that command 0xF1 use default values(100, 100, 0, 1, 0) while command 0xF8 
;can use the user's delay value to run the power management.  The accelerometer has one more way 
;to initiate power management: use device control command(0x20). 
;
;Read Recorder Task
;
;Tx = <msg header> <0xF8> <cmd ext> <chksum>
;<cmd ext> ::= <0x01> | <0x05> | <0x06>
;
;Rx = <msg header> <0xF8> <cmd ext> <init delay> <period> <records> <buckets> <log mode> <chksum>
;<cmd ext>  ::= <0x01> | <0x05> | <0x06>
;<init delay>   ::= <MSByte> <LSByte>
;<period>   ::= <MSByte> <LSByte>
;<records>  ::= <byte>
;<buckets>  ::= <byte>
;<log mode> ::= <byte>
;<cmd ack>  ::= <0x00> | <0xFD>
;
;Write Recorder Task
;Tx = <msg header> <0xF8> <cmd ext> <init delay> <period> <records> <buckets> <log mode> <chksum>
;<cmd ext>  ::= <0x01> | <0x05> | <0x06>
;<init delay>   ::= <MSByte> <LSByte>
;<period>   ::= <MSByte> <LSByte>
;<records>  ::= <byte>
;<buckets>  ::= <byte>
;<log mode> ::= <byte>
;
;Rx = <msg header> <0xF8> <cmd ack> <slave status> <chksum>
;<cmd ack> ::= <0x00> | <0xFD>
;
[F9] Read/Write Operating Mode
[RTIME]=200  
[CTYPE]=3
;
B mode,mode;00 = bootrom, 01 = application mode, 03 = soft reset mode, 04 = hard reset mode
;   Read Operating Mode
;
;Requests the node to transmit its current mode of operation.
;
;Tx = <msg header> <0xF9> <chksum>
;
;Rx = <msg header> <0xF9> <cmd ack> <slave status> <mode> <chksum>
;<cmd ack>::= <0x00>
;<mode>::= <bootrom> | <application mode> | <soft reset mode> | <hard reset mode>
;<bootrom>      ::= <0x00>
;<application mode> ::= <0x01>
;<soft reset mode>  ::= <0x03>
;<hard reset mode>  ::= <0x04>
;
;Write Operating Mode
;Commands the node to change its current mode of operation.
;
;Tx = <msg header> <0xF9> <mode> <chksum>
;<mode> ::= <bootrom> | <bootrom mode> | <application mode> | <soft reset mode> | <hard reset>
;<bootrom mode>     ::= <0x00>
;<application mode> ::= <0x01>
;<soft reset>       ::= <0x03>
;<hard reset>       ::= <0x04>
;
;Rx = <msg header> <0xF9> <cmd ack> <slave status> <mode> <chksum>
;<cmd ack> ::= <0x00> | <0xFD>
;
[FA] Read/Write Memory
[RTIME]=200  
[CTYPE]=3
;
;Read Memory
;Requests the node transmit the specified number of bytes beginning at the specified start 
;address in memory.
;
B Type,type;0=Internal RAM, 1=External RAM, 2=Code space
T Address,hex;Address to read or write
W Bytes,bytes;number of bytes to either read or write
A Data,bytes;variable length data array read or to write
;
;Tx = <msg header> <0xFA> <memory type> <address> <byte count> <chksum>
;<memory type>::= <internal RAM> | <external RAM> | <code memory>
;<internal RAM>::= <0x00>
;<external RAM>::= <0x01>
;<code memory>::= <0x02>
;<address>::= <MSByte> <byte> <LSByte>
;<byte count>::= <MSByte> <LSByte>
;
;Rx = <msg header> <0xFA> <cmd ack> <slave status> <memory type> <address> <byte count>
        <data> {<data>} <chksum>
;<cmd ack> ::= <0x00> | <0xFD>
;
;Write Memory
;Commands the node to write data in memory beginning at the specified starting address.
;
;Tx = <msg header> <0xFA> <memory type> <address> <byte count> <data> {<data>} <chksum>
;
;Rx = <msg header> <0xFA> <cmd ack> <slave status> <memory type> <address> <byte count> <chksum>
;<cmd ack> ::= <0x00> | <0xFD>
;
[FB] Read Node Status
[RTIME]=200  
[CTYPE]=0
;
;This command requests the complete status of the node.
;
;Tx = <msg header> <0xFB> <chksum>
;
;Rx = <msg header> <0xFB> <cmd ack> <slave status> <chksum>
;<cmd ack> ::= <0x00>
;
[FC] Read Software P/N
[RTIME]=200  
[CTYPE]=1
;
A10 Part Num,N/A; Part Number
;
;This command requests the node to transmit its software identification part number.
;
;Tx = <msg header> <0xFC> <chksum>
;
;Rx = <msg header> <0xFC> <cmd ack> <slave status> <response data> <chksum>
;<cmd ack>::= <0x00>
;<response data>::= 77557TA100
;
;NOTE:  Check the part number guidelines for more information on the definition of a part number.
;
