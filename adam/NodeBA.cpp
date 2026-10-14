/*!
  \file   NodeBA.cpp
  \author Aleksey Lekhtin
  \brief  БТО
  \date   10.05.2023
*/

#include <iostream>

#include <QTextBrowser>

#include "bto_gui.h"
#include "dbg.h"
#include "NodeBA.h"
#include "unmarshal_float.h"

//---------------------------------------------------------
NodeBA::NodeBA() :
    NODE(CODExBA, "BTO"),
    m_gui(nullptr),
    m_required_voltage(0),
    m_output_voltage(0),
    m_output_amperage(0),
    m_power_output_type(NodeBA::PowerOutputType::OFF),
    m_power_input_sw(NodeBA::PowerInputSwitch::OFF),
    m_bto_rtime(1000)
{
    create_bto_requests();
    create_gamma_requests();
}

//---------------------------------------------------------
NodeBA::~NodeBA()
{
    DBG(4);
}

//---------------------------------------------------------
void NodeBA::set_gui(BTO_GUI *gui)
{
    m_gui = gui;
}

//---------------------------------------------------------
int NodeBA::rtime() const
{
    return m_bto_rtime;
}

//---------------------------------------------------------
void NodeBA::set_rtime(int rtime)
{
    m_bto_rtime = rtime;
}

//---------------------------------------------------------
QString NodeBA::frame_name(byte cmd)
{
    ////char buf[32] = {0};
    ////snprintf(buf, sizeof(buf)-1, "[0x%02X]", cmd );
    switch(cmd)
    {
        case  0x01: return "RequiredVoltage";
        case  0x02: return "PowerInputSwitch";
        case  0x03: return "PowerOutputSwitch";
        case  0x04: return "ToolLoad";
        case  0x06: return "9BitMode";
        case  0x07: return "OutputVoltage";
        case  0x08: return "Amperage";
        case  0x09: return "FlowSimulation";
        case  0x0A: return "FlowState";
        case  0x0B: return "PulserState";
        case  0x0C: return "PumpState";
        case  0x0D: return "PowerSense";
        case  0x10: return "TransparentModeState";
        case  0x11: return "CommChannel";
        case  0x12: return "ToolAType";
        case  0x13: return "BPSRate";
        case  0xf0: return "ServiceData";
        case  0x15: return "StartPassivatTest";
        case  0x16: return "PassivationTest";
        case  0x17: return "StartDepassTest";
        case  0x18: return "DepassivationTest";
        case  0x19: return "StopDepassTest";
        case  0x20: return "ServiceParameter";
        case  0x27: return "GetExternal";
        case  0x28: return "Output";
        case  0x40: return "Set HVPS Low";
        case  0x41: return "Set HVPS High";
        case  0x42: return "Read/WriteCalibrate HVPS";
        case  0x43: return "Read/Write Voltage";
        case  0x44: return "Read HVPScorr";
        case  0x46: return "Read/Write HVPS Warm-up";
        case  0x50: return "Read/Write CTABLE";
        case  0x51: return "Read/Write Local Gravity";
        case  0xfb: return "Wrong command";
        case  0xfd: return "Value is outside limit";
        case  0xfc: return "PartNumber";
        case  0xf9: return "Mode";
        case  0xf1: return "MasterStatusRegister";
        default:
        {
            return "?";
        }
    }
}

//---------------------------------------------------------
void NodeBA::create_bto_requests()
{
    m_bto_requests = std::list<Adam::Frame> {
            ReadPowerOutputSwitch(),
            ReadPowerInputSwitch(),
            ReadRequiredVoltage(),
            ReadOutputVoltage(),
            ReadAmperage()
    };

    //if (code() == NodeBA::CODEx7A) {
    //}
}

//---------------------------------------------------------
const std::list<Adam::Frame> &NodeBA::bto_requests() const
{
    return m_bto_requests;
}

//---------------------------------------------------------
void NodeBA::create_gamma_requests()
{
    m_gamma_requests = std::list<Adam::Frame>{
            NodeE4::ReadPartNumber(),                               // 0xfc
            NodeE4::ReadOperationVoltage(),                         // 0x43
            NodeE4::ReadTemperature(),                              // 0x05
            NodeE4::ReadGammaCPS(1),                                // 0x01
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::Vdc15),        // 0x1e
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::Vdc5),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::Dv485),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::Vdc25Reference),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::HighVoltageControl),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::TempSensor),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::TempSensorCalibr),
    };
}

//---------------------------------------------------------
const std::list<Adam::Frame> &NodeBA::gamma_requests() const
{
    return m_gamma_requests;
}

//---------------------------------------------------------
std::string NodeBA::power_output_name(PowerOutputType sw)
{
    if (sw == PowerOutputType::INT)
        return std::string("PowerOutput::INT");
    if (sw == PowerOutputType::EXT)
        return std::string("PowerOutput::EXT");
    return std::string("PowerOutput::OFF");
}

//---------------------------------------------------------
std::string NodeBA::input_switch_name(PowerInputSwitch sw)
{
    if (sw == PowerInputSwitch::ON)
        return std::string("PowerInputSwitch::ON");

    return std::string("PowerInputSwitch::OFF");
}

//---------------------------------------------------------
float NodeBA::output_voltage() const
{
    return m_output_voltage;
}

//---------------------------------------------------------
NodeBA::PowerOutputType NodeBA::power_output_type() const
{
    return m_power_output_type;
}

//---------------------------------------------------------
NodeBA::PowerInputSwitch NodeBA::power_input_switch() const
{
    return m_power_input_sw;
}

//////---------------------------------------------------------
////NodeBA::PowerInputSwitch NodeBA::power_input_switch_gui() const
////{
////    return m_power_input_gui_state;
////}

//---------------------------------------------------------
const QString NodeBA::status_description(NodeBA::Status status)
{
    auto bitstr = QObject::tr("bit");

    QString text;
    if (status & Status::BatteryDepassivationInProgress) {
        text += QString("<tr><td>%1 0:  %2<td></tr>").arg(bitstr).arg("Battery Depassivation In Progress");
    } else if (status & Status::BatteryPassivationTestInProgress) {
        text += QString("<tr><td>%1 1:  %2<td></tr>").arg(bitstr).arg("Battery Passivation Test In Progress");
    } else if (status & Status::EepromWriteError) {
        text += QString("<tr><td>%1 5:  %2<td></tr>").arg(bitstr).arg("Eeprom Write Error");
    } else if (status & Status::DataChecksumError) {
        text += QString("<tr><td>%1 6:  %2<td></tr>").arg(bitstr).arg("Data Checksum Error");
    } else if (status & Status::CodeChecksumError) {
        text += QString("<tr><td>%1 7:  %2<td></tr>").arg(bitstr).arg("Code Checksum Error");
    } else if (status & Status::InternalRamError) {
        text += QString("<tr><td>%1 8:  %2<td></tr>").arg(bitstr).arg("Internal Ram Error");
    } else if (status & Status::ExternalRamError) {
        text += QString("<tr><td>%1 9:  %2<td></tr>").arg(bitstr).arg("External Ram Error");
    } else if (status & Status::IllegalOpcodeError) {
        text += QString("<tr><td>%1 10: %2<td></tr>").arg(bitstr).arg("Illegal Opcode Error");
    } else if (status & Status::ClockMonitorError) {
        text += QString("<tr><td>%1 11: %2<td></tr>").arg(bitstr).arg("Clock Monitor Error");
    } else if (status & Status::ComputerOperatingProperlyError) {
        text += QString("<tr><td>%1 12: %2<td></tr>").arg(bitstr).arg("Computer OperatingProperly Error");
    } else if (status & Status::UninitializedInterruptError) {
        text += QString("<tr><td>%1 13: %2<td></tr>").arg(bitstr).arg("Uninitialized Interrupt Error");
    }

    if (text.isEmpty()) {
        return QString("%1 0000").arg(QObject::tr("Status:"));
    }

    text.replace(" ", "&nbsp;");
    return QString("%1\n%2").arg(QObject::tr("Status:"), text);
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadRequiredVoltage()
{
    auto frame = Adam::Frame(code(), 0xff, 0x01, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x01));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::WriteRequiredVoltage(float voltage)
{
    if (m_code == NodeBA::CODExBA) {
        voltage *= 10.0;
    }
    DBGF(3, "%f", voltage);
    auto frame = Adam::Frame(code(), 0xff, 0x01, Adam::encode_ieee754(voltage), m_bto_rtime);
    frame.set_frame_name(QString("Write") + frame_name(0x01));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadPowerInputSwitch()
{
    auto frame = Adam::Frame(code(), 0xff, 0x02, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x02));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::WritePowerInputSwitch(PowerInputSwitch powerSwitch)
{
    auto frame = Adam::Frame(code(), 0xff, 0x02, (byte) powerSwitch, m_bto_rtime);
    frame.set_frame_name(QString("Write") + frame_name(0x02));
    frame.set_no_response_required();
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadPowerOutputSwitch()
{
    auto frame = Adam::Frame(code(), 0xff, 0x03, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x03));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::WritePowerOutputSwitch(PowerOutputType powerSwitch)
{
    auto frame = Adam::Frame(code(), 0xff, 0x03, (byte) powerSwitch, m_bto_rtime);
    frame.set_frame_name(QString("Write") + frame_name(0x03));
    frame.set_no_response_required();
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadToolLoad()
{
    auto frame = Adam::Frame(code(), 0xff, 0x03, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x03));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::WriteToolLoad(ToolLoadState state)
{
    auto frame = Adam::Frame(code(), 0xff, 0x04, (byte) state, m_bto_rtime);
    frame.set_frame_name(QString("Write") + frame_name(0x04));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::Read9BitMode()
{
    auto frame = Adam::Frame(code(), 0xff, 0x06, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x06));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::Write9BitMode(NineBitMode mode)
{
    auto frame = Adam::Frame(code(), 0xff, 0x06, (byte) mode, m_bto_rtime);
    frame.set_frame_name(QString("Write") + frame_name(0x06));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadOutputVoltage()
{
    auto frame = Adam::Frame(code(), 0xff, 0x07, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x07));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadAmperage()
{
    auto frame = Adam::Frame(code(), 0xff, 0x08, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x08));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::WriteRequiredAmperage(float amperage)
{
    if (m_code == NodeBA::CODExBA) {
        amperage *= 1000.0;
    }
    DBGF(3, "%f", amperage);
    auto frame = Adam::Frame(code(), 0xff, 0x08, Adam::encode_ieee754(amperage), m_bto_rtime);
    frame.set_frame_name(QString("Write") + frame_name(0x08));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadFlowSimulation()
{
    auto frame = Adam::Frame(code(), 0xff, 0x09, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x09));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::WriteFlowSimulation(State state)
{
    auto frame = Adam::Frame(code(), 0xff, 0x09, (byte) state, m_bto_rtime);
    frame.set_frame_name(QString("Write") + frame_name(0x09));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadFlowState()
{
    auto frame = Adam::Frame(code(), 0xff, 0x0A, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x0A));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::WriteFlowState(State state)
{
    auto frame = Adam::Frame(code(), 0xff, 0x0A, (byte) state, m_bto_rtime);
    frame.set_frame_name(QString("Write") + frame_name(0x0A));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadPulserState()
{
    auto frame = Adam::Frame(code(), 0xff, 0x0B, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x0B));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadPumpState()
{
    auto frame = Adam::Frame(code(), 0xff, 0x0C, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x0C));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::WritePumpState(State state)
{
    auto frame = Adam::Frame(code(), 0xff, 0x0C, (byte) state, m_bto_rtime);
    frame.set_frame_name(QString("Write") + frame_name(0x0C));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadPowerSense()
{
    auto frame = Adam::Frame(code(), 0xff, 0x0D, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x0D));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::WritePowerSense(PowerSense powerSense)
{
    auto frame = Adam::Frame(code(), 0xff, 0x0D, (byte) powerSense, m_bto_rtime);
    frame.set_frame_name(QString("Write") + frame_name(0x0D));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadTransparentModeState()
{
    auto frame = Adam::Frame(code(), 0xff, 0x10, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x10));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::WriteTransparentModeState(State chA, State chB)
{
    std::vector<byte> state{(byte) chA, (byte) chB};
    auto frame = Adam::Frame(code(), 0xff, 0x10, state, m_bto_rtime);
    frame.set_frame_name(QString("Write") + frame_name(0x10));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadCommChannel()
{
    auto frame = Adam::Frame(code(), 0xff, 0x11, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x11));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadToolAType()
{
    auto frame = Adam::Frame(code(), 0xff, 0x12, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x12));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::WriteToolAType(ToolAType type)
{
    auto frame = Adam::Frame(code(), 0xff, 0x12, (byte) type, m_bto_rtime);
    frame.set_frame_name(QString("Write") + frame_name(0x12));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadBPSRate()
{
    auto frame = Adam::Frame(code(), 0xff, 0x13, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x13));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadServiceData()
{
    auto frame = Adam::Frame(code(), 0xff, 0xf0, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0xf0));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::WriteServiceData(float value)
{
    auto frame = Adam::Frame(code(), 0xff, 0xf0, Adam::encode_ieee754(value), m_bto_rtime);
    frame.set_frame_name(QString("Write") + frame_name(0xf0));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::StartPassivationTest(PassivationParameters data)
{
    auto frame = Adam::Frame(code(), 0xff, 0x15, data.serialize(), m_bto_rtime);
    frame.set_frame_name(QString("Start") + frame_name(0x15));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadPassivationTest()
{
    auto frame = Adam::Frame(code(), 0xff, 0x16, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x16));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::StartDepassivationTest(DepassivationParameters data)
{
    auto frame = Adam::Frame(code(), 0xff, 0x17, data.serialize(), m_bto_rtime);
    frame.set_frame_name(QString("Start") + frame_name(0x17));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadDepassivationTest()
{
    auto frame = Adam::Frame(code(), 0xff, 0x18, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x18));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::StopDepassivationTest()
{
    auto frame = Adam::Frame(code(), 0xff, 0x19, m_bto_rtime);
    frame.set_frame_name(QString("Stop") + frame_name(0x19));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::WriteServiceParameter(byte parameter, float data)
{
    std::vector<byte> payload{parameter};
    auto bytes = Adam::encode_ieee754(data);
    payload.insert(payload.end(), bytes.begin(), bytes.end());
    auto frame = Adam::Frame(code(), 0xff, 0x20, payload, m_bto_rtime);
    frame.set_frame_name(QString("Write") + frame_name(0x20));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadServiceParameter(byte parameter)
{
    auto frame = Adam::Frame(code(), 0xff, 0x20, parameter, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x20));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadStatus()
{
    auto frame = Adam::Frame(code(), 0xff, 0xfb, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0xfb));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::WriteConnectStatus(bool status)
{
    std::vector<byte> st{(byte) (status ? 1 : 0), (byte) 0};
    auto frame = Adam::Frame(code(), 0xff, 0xfb, st, m_bto_rtime);
    frame.set_frame_name(QString("Write") + frame_name(0xfb));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadPartNumber()
{
    auto frame = Adam::Frame(code(), 0xff, 0xfc, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0xfc));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::WriteMode(Mode mode)
{
    auto frame = Adam::Frame(code(), 0xff, 0xf9, (byte)mode, m_bto_rtime);
    frame.set_frame_name(QString("Write") + frame_name(0xf9));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::GetExternal()
{
    auto frame = Adam::Frame(code(), 0xff, 0x27, m_bto_rtime);
    frame.set_frame_name(QString("Get") + frame_name(0x27));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeBA::ReadOutput()
{
    auto frame = Adam::Frame(code(), 0xff, 0x28, m_bto_rtime);
    frame.set_frame_name(QString("Read") + frame_name(0x28));
    return frame;
}

//---------------------------------------------------------
ushort NodeBA::DepassivationParameters::get_TestTime()
{
    return m_testTime;
}

//---------------------------------------------------------
void NodeBA::DepassivationParameters::set_TestTime(ushort value)
{
    m_testTime = value;
}

//---------------------------------------------------------
byte NodeBA::DepassivationParameters::get_MeasurementPeriod()
{
    return m_measurePeriod;
}

//---------------------------------------------------------
void NodeBA::DepassivationParameters::set_MeasurementPeriod(byte value)
{
    m_measurePeriod = value;
}

//---------------------------------------------------------
byte NodeBA::DepassivationParameters::get_Load1Time()
{
    return m_load1Time;
}

//---------------------------------------------------------
void NodeBA::DepassivationParameters::set_Load1Time(byte value)
{
    m_load1Time = value;
}

//---------------------------------------------------------
byte NodeBA::DepassivationParameters::get_Load2Time()
{
    return m_load2Time;
}

//---------------------------------------------------------
void NodeBA::DepassivationParameters::set_Load2Time(byte value)
{
    m_load2Time = value;
}

//---------------------------------------------------------
float NodeBA::DepassivationParameters::get_TargetV1()
{
    return m_targetV1;
}

//---------------------------------------------------------
void NodeBA::DepassivationParameters::set_TargetV1(float value)
{
    m_targetV1 = value;
}

//---------------------------------------------------------
float NodeBA::DepassivationParameters::get_TargetV2()
{
    return m_targetV2;
}

//---------------------------------------------------------
void NodeBA::DepassivationParameters::set_TargetV2(float value)
{
    m_targetV2 = value;
}

//---------------------------------------------------------
std::vector<byte> NodeBA::DepassivationParameters::serialize() const
{
    std::vector<byte> bytes;
    bytes.insert(bytes.end(), (byte *) &m_testTime, (byte *) &m_testTime + sizeof(m_testTime));
    bytes.insert(bytes.end(),
                 (byte *) &m_measurePeriod,
                 (byte *) &m_measurePeriod + sizeof(m_measurePeriod));
    bytes.insert(bytes.end(), (byte *) &m_load1Time, (byte *) &m_load1Time + sizeof(m_load1Time));
    bytes.insert(bytes.end(), (byte *) &m_load2Time, (byte *) &m_load2Time + sizeof(m_load2Time));
    bytes.insert(bytes.end(), (byte *) &m_targetV1, (byte *) &m_targetV1 + sizeof(m_targetV1));
    bytes.insert(bytes.end(), (byte *) &m_targetV2, (byte *) &m_targetV2 + sizeof(m_targetV2));
    return bytes;
}

//---------------------------------------------------------
void NodeBA::DepassivationParameters::deserialize(std::vector<byte> &bytes)
{
    const byte *p = bytes.data();

    m_testTime = *(ushort *) p;
    p += sizeof(ushort);

    m_measurePeriod = *(byte *) p;
    p += sizeof(byte);

    m_load1Time = *(byte *) p;
    p += sizeof(byte);

    m_load2Time = *(byte *) p;
    p += sizeof(byte);

    m_targetV1 = *(byte *) p;
    p += sizeof(byte);

    m_targetV2 = *(byte *) p;
    p += sizeof(byte);
}

//---------------------------------------------------------
ushort NodeBA::PassivationParameters::get_TestTime()
{
    return m_testTime;
}

//---------------------------------------------------------
void NodeBA::PassivationParameters::set_TestTime(ushort value)
{
    m_testTime = value;
}

//---------------------------------------------------------
byte NodeBA::PassivationParameters::get_MeasurementPeriod()
{
    return m_measurePeriod;
}

//---------------------------------------------------------
void NodeBA::PassivationParameters::set_MeasurementPeriod(byte value)
{
    m_measurePeriod = value;
}

//---------------------------------------------------------
byte NodeBA::PassivationParameters::get_Load1Time()
{
    return m_load1Time;
}

//---------------------------------------------------------
void NodeBA::PassivationParameters::set_Load1Time(byte value)
{
    m_load1Time = value;
}

//---------------------------------------------------------
byte NodeBA::PassivationParameters::get_Load2Time()
{
    return m_load2Time;
}

//---------------------------------------------------------
void NodeBA::PassivationParameters::set_Load2Time(byte value)
{
    m_load2Time = value;
}

//---------------------------------------------------------
std::vector<byte> NodeBA::PassivationParameters::serialize() const
{
    std::vector<byte> bytes;
    bytes.insert(bytes.end(), (byte *) &m_testTime, (byte *) &m_testTime + sizeof(m_testTime));
    bytes.insert(bytes.end(),
                 (byte *) &m_measurePeriod,
                 (byte *) &m_measurePeriod + sizeof(m_measurePeriod));
    bytes.insert(bytes.end(), (byte *) &m_load1Time, (byte *) &m_load1Time + sizeof(m_load1Time));
    bytes.insert(bytes.end(), (byte *) &m_load2Time, (byte *) &m_load2Time + sizeof(m_load2Time));
    return bytes;
}

//---------------------------------------------------------
void NodeBA::PassivationParameters::deserialize(std::vector<byte> &bytes)
{
    const byte *p = bytes.data();

    m_testTime = *(ushort *) p;
    p += sizeof(ushort);

    m_measurePeriod = *(byte *) p;
    p += sizeof(byte);

    m_load1Time = *(byte *) p;
    p += sizeof(byte);

    m_load2Time = *(byte *) p;
    p += sizeof(byte);
}

//---------------------------------------------------------
int NodeBA::parse_response_0x01(const Adam::Response &response) // Required Voltage
{
    m_required_voltage_ack = response.ack();
    m_required_voltage_status = response.status();
    m_required_voltage_elapsed_time = response.elapsed_time();
    if (response.ack() != 0)
    {
        DBGFNoNL(2, "[%02X] cmd:%02X, ACK:0x%02X, status:0x%02x\n",
             (int)code(), response.cmd(), response.ack(), response.status());
        return 0;
    }
    if (response.payload_size() == 0)
    {
        DBGF(2, "from  [%02X] cmd:%02X %s - ACK:0x%02X, status:0x%02x (%dms)",
            (int)code(), response.cmd(), qPrintable(response.name()),
             response.ack(), response.status(), m_power_output_sw_elapsed_time);
        return 0;
    }
    if (response.payload_size() < 4)
    {
        DBGFNoNL(1, "[%02X] cmd:%02X INCORRECT RESPONSE - expected data size at least 4 but got %d\n",
             (int)code(), response.cmd(), (int)response.payload_size());
        return 0;
    }

    auto value = Adam::decode_ieee754(response.payload());
    m_required_voltage = value;
    if (m_code == NodeBA::CODExBA) {
        m_required_voltage /= 10.0;
    }
    DBGF(2, "from  [%02X] cmd:%02X %-22s [%.2f] (%dms)",
         (int)code(), response.cmd(), qPrintable(frame_name(response.cmd())),
         value, m_required_voltage_elapsed_time);
    return 1;
}

//---------------------------------------------------------
int NodeBA::parse_response_0x02(const Adam::Response &response) // Input Power Switch
{
    m_power_input_sw_ack = response.ack();
    m_power_input_sw_status = response.status();
    m_power_input_sw_elapsed_time = response.elapsed_time();
    if (response.ack() != 0)
    {
        DBGFNoNL(2, "[%02X] cmd:%02X, ACK:0x%02X, status:0x%02x\n",
            (int)code(), response.cmd(), response.ack(), response.status());
        return 0;
    }
    if (response.payload_size() == 0)
    {
        DBGF(2, "from  [%02X] cmd:%02X %s - ACK:0x%02X, status:0x%02x (%dms)",
            (int)code(), response.cmd(), qPrintable(response.name()),
             response.ack(), response.status(), m_power_output_sw_elapsed_time);
        return 0;
    }

    auto state = (NodeBA::PowerInputSwitch) response.payload_byte();
    m_power_input_sw = state;
    DBGF(2, "from  [%s] cmd:%02X %-22s [%s] (%dms)",
         code_name().c_str(), response.cmd(),
         qPrintable(frame_name(response.cmd())),
         (m_power_input_sw == PowerInputSwitch::ON ? "ON": "OFF"),
         m_power_input_sw_elapsed_time);
    return 1;
}

//---------------------------------------------------------
int NodeBA::parse_response_0x03(const Adam::Response &response) // Output Power Ttype
{
    m_power_output_sw_ack = response.ack();
    m_power_output_sw_status = response.status();
    m_power_output_sw_elapsed_time = response.elapsed_time();
    if (response.ack() != 0)
    {
        DBGF(2, "[%02X] cmd:%02X, ACK:0x%02X, status:0x%02x",
            (int)code(), response.cmd(), response.ack(), response.status());
        return 0;
    }
    if (response.payload_size() == 0)
    {
        DBGF(2, "from  [%02X] cmd:%02X %s - ACK:0x%02X, status:0x%02x (%dms)",
            (int)code(), response.cmd(), qPrintable(response.name()),
             response.ack(), response.status(), m_power_output_sw_elapsed_time);
        return 0;
    }

    auto state = (NodeBA::PowerOutputType) response.payload_byte();
    m_power_output_type = state;
    DBGF(2, "from  [%02X] cmd:%02X %-22s [%s] (%dms)",
        (int)code(), response.cmd(),
         qPrintable(frame_name(response.cmd())),
         (state == NodeBA::PowerOutputType::EXT ? "EXT" :
              (state == NodeBA::PowerOutputType::INT ? "INT": "OFF")),
         m_power_output_sw_elapsed_time);
    return 1;
}

//---------------------------------------------------------
int NodeBA::parse_response_0x07(const Adam::Response &response) // Output voltage
{
    m_output_voltage_ack = response.ack();
    m_output_voltage_status = response.status();
    m_output_voltage_elapsed_time = response.elapsed_time();
    if (response.ack() != 0) {
        DBGFNoNL(2, "[%02X] cmd:%02X, ACK:0x%02X, status:0x%02x\n",
            (int)code(), response.cmd(), response.ack(), response.status());
        return 0;
    }
    if (response.payload_size() < 4)
    {
        DBGF(1, "[%02X] cmd:%02X INCORRECT RESPONSE - expected data size at least 4 but got %d\n",
             (int)code(), response.cmd(), (int)response.payload_size());
        return 0;
    }

    auto value = Adam::decode_ieee754(response.payload());
    m_output_voltage = value;
    if (m_code == NodeBA::CODExBA) {
        m_output_voltage /= 10.0;
    }
    DBGF(2, "from  [%02X] cmd:%02X %-22s [%.2f] (%dms)",
        (int)code(), response.cmd(), qPrintable(frame_name(response.cmd())),
         value, m_output_voltage_elapsed_time);
    return 1;
}

//---------------------------------------------------------
int NodeBA::parse_response_0x08(const Adam::Response &response) // Output Amperage
{
    m_output_amperage_ack = response.ack();
    m_output_amperage_status = response.status();
    m_output_amperage_elapsed_time = response.elapsed_time();
    if (response.ack() != 0) {
        DBGF(2, "[%02X] cmd:%02X, ACK:0x%02X, status:0x%02x",
            (int)code(), response.cmd(), response.ack(), response.status());
        return 0;
    }
    if (response.payload_size() == 0)
    {
        DBGF(2, "from  [%02X] cmd:%02X %s - ACK:0x%02X, status:0x%02x (%dms)",
            (int)code(), response.cmd(), qPrintable(response.name()),
             response.ack(), response.status(), m_power_output_sw_elapsed_time);
        return 0;
    }
    if (response.payload_size() < 4)
    {
        DBGF(1, "[%02X] cmd:%02X INCORRECT RESPONSE - expected data size at least 4 but got %d\n",
             (int)code(), response.cmd(), (int)response.payload_size());
        return 0;
    }

    auto value = Adam::decode_ieee754(response.payload());
    ////m_output_amperage = value / 1000.0;
    m_output_amperage = value;
    DBGF(2, "from  [%02X] cmd:%02X %-22s [%f] (%dms)",
             (int)code(), response.cmd(), qPrintable(frame_name(response.cmd())),
             value, m_output_amperage_elapsed_time);
    return 1;
}

//---------------------------------------------------------
int NodeBA::parse_response_0xfb(const Adam::Response &response) // Status
{
    m_status_ack = response.ack();
    m_status_status = response.status();
    m_status_elapsed_time = response.elapsed_time();
    if (response.ack() != 0)
    {
        DBGF(2, "[%02X] cmd:%02X, ACK:0x%02X, status:0x%02x",
            (int)code(), response.cmd(), response.ack(), response.status());
        return 0;
    }
    if (response.payload_size() > 0)
    {
        auto value = response.payload_byte();
        m_status = value;
        DBGF(2, "from  [%02X] cmd:%02X Status: 0x%02X (len:%d) (%dms)",
            (int)code(), response.cmd(), value, value, m_status_elapsed_time);
    }
    else
    {
        DBGF(2, "from  [%02X] cmd:%02X Status: [no data] len:%d (%dms)",
            (int)code(), response.cmd(), (int)response.payload_size(), m_status_elapsed_time);
    }
    return 1;
}

//---------------------------------------------------------
int NodeBA::parse_response_0xfc(const Adam::Response &response) // Part Number
{
    m_partnumber_ack = response.ack();
    m_partnumber_status = response.status();
    m_partnumber_elapsed_time = response.elapsed_time();
    if (response.ack() != 0)
    {
        DBGF(2, "[%02X] cmd:%02X, ACK:0x%02X, status:0x%02x",
            (int)code(), response.cmd(), response.ack(), response.status());
        return 0;
    }

    m_partnumber = response.payload_string();
    DBGF(2, "from  [%02X] cmd:%02X %-22s [%s] (%dms)",
        (int)code(), response.cmd(),
         qPrintable(frame_name(response.cmd())),
         m_partnumber.c_str(), m_partnumber_elapsed_time);
    return 1;
}

//---------------------------------------------------------
int NodeBA::response(const Adam::Response &response)
{
    DBGF(4, "[%02X] cmd:%02X ack:0x%02X status:0x%04x (%dms)",
         code(), (int)response.cmd(), (int)response.ack(), (int)response.status(),
         response.elapsed_time());

    int ret = 0; // 0 - не надо выдавать сигнал на обновление значений в GUI
    byte cmd = response.cmd();
    switch (cmd)
    {
    case 0x01: // Required Voltage
        ret = parse_response_0x01(response);
        break;
    case 0x02: // Input Power Switch
        ret = parse_response_0x02(response);
        break;
    case 0x03: // Output Power Switch
        ret = parse_response_0x03(response);
        break;
    case 0x07: // Output Voltage
        ret = parse_response_0x07(response);
        break;
    case 0x08: // Output Amperage
        ret = parse_response_0x08(response);
        break;
    case 0xfb: // Status
        ret = parse_response_0xfb(response);
        break;
    case 0xfc: // Part Number
        ret = parse_response_0xfc(response);
        break;

    default:
        DBGF(2, "unregistered cmd: 0x%02X", cmd);
        ret = 0;
        break;
    }
    return ret;
}
