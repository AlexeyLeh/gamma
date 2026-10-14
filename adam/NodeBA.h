/*!
  \file   Node.h
  \author Aleksey Lekhtin
  \brief  БТО
  \date   10.05.2023
*/

#ifndef __NODE_BA_H__
#define __NODE_BA_H__

#include <list>

#include "frame.h"
#include "node.h"
#include "response.h"

class BTO_GUI;

class NodeBA : public NODE
{
public:
    NodeBA();
    ~NodeBA();
    void set_gui(BTO_GUI *gui);

    static constexpr float THRESHOLD_4_5V = 4.5;    // V
    static constexpr NODECODE CODExBA = 0xBA;       // БТО V1
    static constexpr NODECODE CODEx7A = 0x7A;       // БТО V2

    static QString frame_name(byte cmd);

public:
    enum Status {
        BatteryDepassivationInProgress = 1,         //[BIT 0] DEPASSIVATING
        BatteryPassivationTestInProgress = 1 << 1,  //[BIT 1] PASSIVATION_TEST
        EepromWriteError = 1 << 5,                  //[BIT 5] EEWRITE_ERR
        DataChecksumError = 1 << 6,                 //[BIT 6] DATACKSUM_ERR
        CodeChecksumError = 1 << 7,                 //[BIT 7] CODECKSUM_ERR
        InternalRamError = 1 << 8,                  //[BIT 8] INTRAM_ERR
        ExternalRamError = 1 << 9,                  //[BIT 9] EXTRAM_ERR
        IllegalOpcodeError = 1 << 10,               //[BIT 10] IOT_ERR
        ClockMonitorError = 1 << 11,                //[BIT 11] CMR_ERR
        ComputerOperatingProperlyError = 1 << 12,   //[BIT 12] COP_ERR
        UninitializedInterruptError = 1 << 13,      //[BIT 13] INT_ERR
    };

    enum class BtoStatus {
        Passivation = 1 << 6,
        Depassivation = 1 << 7,
        InputSwitchClosed = 1 << 8,
        OutputSwitchClosed = 1 << 11,
    };

    enum class PowerInputSwitch {
        OFF = (byte)0,
        ON = 1,
    };

    enum class PowerOutputType { OFF = (byte)0, INT = 1, EXT = 2 };

    enum class State { On = 1, Off = 0 };

    enum class CommChannel { HostA = 0x01, HostB = 0x03 };

    enum class PowerSense {
        Ivtl = 1,
        Ib1 = 2,
        Ib2 = 4,
        Ibs = 8,
        Ib1Ivtl = 3,
        Ib2Ivtl = 5,
        IbsIvtl = 8,
    };

    enum class ToolAType { IO1039 = 0, DV485B = 1, M30 = 2 };

    enum class NineBitMode { ABOff = 0, AOn = 1, BOn = 2, ABOn = 3 };

    enum class Mode { SoftReset = 3, HardReset = 4 };

    enum class ToolLoadState { NoLoad = 0, Load1 = 1, Load2 = 2, Load1Load2 = 3 };

    struct PassivationParameters {
    public:
        ushort get_TestTime();
        void set_TestTime(ushort value);

        byte get_MeasurementPeriod();
        void set_MeasurementPeriod(byte value);

        byte get_Load1Time();
        void set_Load1Time(byte value);

        byte get_Load2Time();
        void set_Load2Time(byte value);

    public:
        std::vector<byte> serialize() const;
        void deserialize(std::vector<byte> &bytes);

    private:
        ushort m_testTime;  //// [Endian(Endianness.BigEndian)] private ushort
        /// testTime;
        byte m_measurePeriod;
        byte m_load1Time;
        byte m_load2Time;
    };

    struct PassivationResults {
        float V1;  //// [Endian(Endianness.BigEndian)]
        float V2;  //// [Endian(Endianness.BigEndian)]
    };

    struct DepassivationParameters {
    public:
        ushort  get_TestTime();
        void    set_TestTime(ushort value);

        byte    get_MeasurementPeriod();
        void    set_MeasurementPeriod(byte value);

        byte    get_Load1Time();
        void    set_Load1Time(byte value);

        byte    get_Load2Time();
        void    set_Load2Time(byte value);

        float   get_TargetV1();
        void    set_TargetV1(float value);

        float   get_TargetV2();
        void    set_TargetV2(float value);

    public:
        std::vector<byte> serialize() const;
        void deserialize(std::vector<byte> &bytes);

    private:
        ushort m_testTime;
        byte m_measurePeriod;
        byte m_load1Time;
        byte m_load2Time;
        float m_targetV1;
        float m_targetV2;
    };

public:
    int  rtime() const;
    void set_rtime(int rtime);
    void create_bto_requests();
    const std::list<Adam::Frame> &bto_requests() const;

    void create_gamma_requests();
    const std::list<Adam::Frame> &gamma_requests() const;

    Adam::Frame ReadRequiredVoltage();                                   /// 0x01
    Adam::Frame WriteRequiredVoltage(float voltage);                     /// 0x01

    Adam::Frame ReadPowerInputSwitch();                                  /// 0x02
    Adam::Frame WritePowerInputSwitch(PowerInputSwitch powerSwitch);     /// 0x02

    Adam::Frame ReadPowerOutputSwitch();                                 /// 0x03
    Adam::Frame WritePowerOutputSwitch(PowerOutputType powerSwitch);         /// 0x03

    Adam::Frame ReadToolLoad();                                          /// 0x04
    Adam::Frame WriteToolLoad(ToolLoadState state);                      /// 0x04

    Adam::Frame Read9BitMode();                                          /// 0x06
    Adam::Frame Write9BitMode(NineBitMode mode);                         /// 0x06

    Adam::Frame ReadOutputVoltage();                                     /// 0x07

    Adam::Frame ReadAmperage();                                          /// 0x08
    Adam::Frame WriteRequiredAmperage(float amperage);                   /// 0x08

    Adam::Frame ReadFlowSimulation();                                    /// 0x09
    Adam::Frame WriteFlowSimulation(State state);                        /// 0x09

    Adam::Frame ReadFlowState();                                         /// 0x0A
    Adam::Frame WriteFlowState(State state);                             /// 0x0A

    Adam::Frame ReadPulserState();                                       /// 0x0B

    Adam::Frame ReadPumpState();                                         /// 0x0C
    Adam::Frame WritePumpState(State state);                             /// 0x0C

    Adam::Frame ReadPowerSense();                                        /// 0x0D
    Adam::Frame WritePowerSense(PowerSense powerSense);                  /// 0x0D

    Adam::Frame ReadTransparentModeState();                              /// 0x10
    Adam::Frame WriteTransparentModeState(State chA, State chB);         /// 0x10

    Adam::Frame ReadCommChannel();                                       /// 0x11

    Adam::Frame ReadToolAType();                                         /// 0x12
    Adam::Frame WriteToolAType(ToolAType type);                          /// 0x12

    Adam::Frame ReadBPSRate();                                           /// 0x13

    Adam::Frame ReadServiceData();                                       /// 0xf0
    Adam::Frame WriteServiceData(float data);                            /// 0xf0

    Adam::Frame StartPassivationTest(PassivationParameters data);        /// 0x15
    Adam::Frame ReadPassivationTest();                                   /// 0x16

    Adam::Frame StartDepassivationTest(DepassivationParameters data);    /// 0x17
    Adam::Frame ReadDepassivationTest();                                 /// 0x18
    Adam::Frame StopDepassivationTest();                                 /// 0x19

    Adam::Frame WriteServiceParameter(byte parameter, float data);       /// 0x20
    Adam::Frame ReadServiceParameter(byte parameter);                    /// 0x20

    Adam::Frame ReadStatus();                                            /// 0xfb
    Adam::Frame WriteConnectStatus(bool status);                         /// 0xfb

    Adam::Frame ReadPartNumber();                                        /// 0xfc
    Adam::Frame WriteMode(Mode mode);                                    /// 0xf9
    Adam::Frame GetExternal();                                           /// 0x27
    Adam::Frame ReadOutput();                                            /// 0x28

public:
    static std::string power_output_name(PowerOutputType sw);
    static std::string input_switch_name(PowerInputSwitch sw);

    float output_voltage() const;
    NodeBA::PowerOutputType power_output_type() const;
    NodeBA::PowerInputSwitch  power_input_switch() const;
    ////NodeBA::PowerInputSwitch  power_input_switch_gui() const;

    int response(const Adam::Response &resp);

    static const QString status_description(NodeBA::Status status);

private:
    int parse_response_0x01(const Adam::Response& response);
    int parse_response_0x02(const Adam::Response& response);
    int parse_response_0x03(const Adam::Response& response);
    int parse_response_0x07(const Adam::Response& response);
    int parse_response_0x08(const Adam::Response& response);
    int parse_response_0xfb(const Adam::Response& response);
    int parse_response_0xfc(const Adam::Response& response);

private:
    BTO_GUI *m_gui;

public:
    // уставка значения для БТО
    float  m_required_voltage;
    byte   m_required_voltage_ack;
    ushort m_required_voltage_status;
    int    m_required_voltage_elapsed_time;

    // текущее значение в БТО
    float  m_output_voltage;
    byte   m_output_voltage_ack;
    ushort m_output_voltage_status;
    int    m_output_voltage_elapsed_time;

    // текущее значение в БТО или уставка для БТО
    float  m_output_amperage;
    byte   m_output_amperage_ack;
    ushort m_output_amperage_status;
    int    m_output_amperage_elapsed_time;

    // состояние "железного" ключа в БТО для выдачи выходного напряжения
    NodeBA::PowerOutputType m_power_output_type;
    byte   m_power_output_sw_ack;
    ushort m_power_output_sw_status;
    int    m_power_output_sw_elapsed_time;

    // состояние "железного" ключа в БТО для подачи питания
    NodeBA::PowerInputSwitch m_power_input_sw;
    byte   m_power_input_sw_ack;
    ushort m_power_input_sw_status;
    int    m_power_input_sw_elapsed_time;
    ////NodeBA::PowerInputSwitch m_power_input_gui_state; // состояние gui-кнопки

    byte    m_status;
    byte    m_status_ack;
    ushort  m_status_status;
    int     m_status_elapsed_time;

    std::string m_partnumber;
    byte    m_partnumber_ack;
    ushort  m_partnumber_status;
    int     m_partnumber_elapsed_time;


private:
    std::list<Adam::Frame> m_bto_requests;
    std::list<Adam::Frame> m_gamma_requests;
    ////int m_bto_requests_rtime;
    ////int m_gamma_requests_rtime;
    int m_bto_rtime;
};

#endif //  __NODE_BA_H__
