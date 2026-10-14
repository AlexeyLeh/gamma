/*!
  \file   NodeE4.cpp
  \author Aleksey Lekhtin
  \brief  Gamma node
  \date   10.05.2023
*/


#include <string>

#include <QObject>
#include <QtDebug>
#include <QString>

#include "dbg.h"
#include "NodeE4.h"
#include "unmarshal_float.h"

//---------------------------------------------------------
NodeE4::NodeE4() :
    NODE(0xE4, "Gamma"),
    m_ctable(new CTable()),
    m_ktable(new KTable())
{
    clear();
}

//---------------------------------------------------------
NodeE4::~NodeE4()
{
    DBG(4);
    delete(m_ctable);
    delete(m_ktable);
}

//---------------------------------------------------------
void NodeE4::clear()
{
    m_gamma_ext = 0;
    m_gamma_value = 0;
    //// m_gamma_accumulation = false;
    //// m_gamma_sum = 0;
    //// m_gamma_counter = 0;
    m_gamma_ack = 0;
    m_gamma_status = 0;
    m_gamma_elapsed_time = 0;

    m_temperature_num = 0;
    m_temperature_value = 0;
    m_temperature_ack = 0;
    m_temperature_status = 0;
    m_temperature_elapsed_time = 0;

    memset(m_accelerometer_raw_value, 0, sizeof(m_accelerometer_raw_value));
    memset(m_accelerometer_raw_ack, 0, sizeof(m_accelerometer_raw_ack));
    memset(m_accelerometer_raw_status, 0, sizeof(m_accelerometer_raw_status));
    memset(m_accelerometer_raw_elapsed_time, 0, sizeof(m_accelerometer_raw_elapsed_time));

    memset(m_accelerometer_incl_value, 0, sizeof(m_accelerometer_incl_value));
    memset(m_accelerometer_incl_ack, 0, sizeof(m_accelerometer_incl_ack));
    memset(m_accelerometer_incl_status, 0, sizeof(m_accelerometer_incl_status));
    memset(m_accelerometer_incl_elapsed_time, 0, sizeof(m_accelerometer_incl_elapsed_time));

    memset(m_accelerometer_corr_value, 0, sizeof(m_accelerometer_corr_value));
    memset(m_accelerometer_corr_ack, 0, sizeof(m_accelerometer_corr_ack));
    memset(m_accelerometer_corr_status, 0, sizeof(m_accelerometer_corr_status));
    memset(m_accelerometer_corr_elapsed_time, 0, sizeof(m_accelerometer_corr_elapsed_time));

    m_gamma_acq_num = 0;
    m_gamma_acq_time = 0;
    m_gamma_acq_ack = 0;
    m_gamma_acq_status = 0;
    m_gamma_acq_time_elapsed_time = 0;

    m_dev_control_num = 0;
    m_dev_control_value = 0;
    m_dev_control_ack = 0;
    m_dev_control_status = 0;
    m_dev_control_elapsed_time = 0;

    m_hv_high_elapsed_time = 0;
    m_hv_low_ack = 0;
    m_hv_low_status = 0;
    m_hv_low_elapsed_time = 0;

    m_calibr_hv_min = 0;
    m_calibr_hv_max = 0;
    m_calibr_hv_tst = 0;
    m_calibr_hv_ack = 0;
    m_calibr_hv_status = 0;
    m_calibr_hv_elapsed_time = 0;

    m_operating_hv = 0;
    m_hv_ack = 0;
    m_hv_status = 0;
    m_hv_elapsed_time = 0;

    m_hv_corr = 0;
    m_hv_corr_ack = 0;
    m_hv_corr_status = 0;
    m_hv_corr_elapsed_time = 0;

    m_warmup_num = 0;
    m_warmup_time = 0;
    m_warmup_ack = 0;
    m_warmup_status = 0;
    m_warmup_elapsed_time = 0;

    m_gravity = 0;
    m_gravity_ack = 0;
    m_gravity_status = 0;
    m_gravity_elapsed_time = 0;

    m_voltage15 = 0;
    m_voltage5 = 0;
    m_dv485 = 0;
    m_high_voltage_control = 0;
    m_temperature_sensor = 0;
    m_temperature_circuit_calibration = 0;
    m_vdc25_reference = 0;
    m_power_amperage = 0;
    memset(m_adc_ack, 0, sizeof(m_adc_ack));
    memset(m_adc_status, 0, sizeof(m_adc_status));
    memset(m_adc_elapsed_time, 0, sizeof(m_adc_elapsed_time));

    m_config_register = 0;
    m_config_register_ack = 0;
    m_config_register_status = 0;
    m_config_register_elapsed_time = 0;

    m_statusreg = 0;
    m_statusreg_ack = 0;
    m_statusreg_status = 0;
    m_statusreg_elapsed_time = 0;

    m_rectask_num = 0;
    m_rectask_initdelay = 0;
    m_rectask_period = 0;
    m_rectask_records = 0;
    m_rectask_buckets = 0;
    m_rectask_logmode = 0;
    m_rectask_ack = 0;
    m_rectask_status = 0;
    m_rectask_elapsed_time = 0;

    m_partnumber.clear();
    m_partnumber_ack = 0;
    m_partnumber_status = 0;
    m_partnumber_elapsed_time = 0;

    m_ctable->clear();
    m_ctable_ack = 0;
    m_ctable_status = 0;
    m_ctable_elapsed_time = 0;

    m_ktable->clear();
    m_ktable_ack = 0;
    m_ktable_status = 0;
    m_ktable_elapsed_time = 0;
}

QString NodeE4::frame_name(byte cmd)
{
    //// char buf[32] = {0};
    //// snprintf(buf, sizeof(buf)-1, "[0x%02X]", cmd );
    switch(cmd)
    {
        case 0x01: return "GammaCPS";
        case 0x05: return "Temperature";
        case 0x06: return "AccelerometerRawData";
        case 0x0C: return "Ktable";
        case 0x11: return "AccelerometerInclinationData";
        case 0x13: return "GammaRayAcqusitionTime";
        case 0x18: return "AccelerometerCorrectedData";
        case 0x20: return "Device Control";
        case 0x40: return "SetHVLow";
        case 0x41: return "SetHVHigh";
        case 0x42: return "CalibrationHV";
        case 0x43: return "Voltage";
        case 0x44: return "HVCorr";
        case 0x46: return "HVWarmUp";
        case 0x50: return "Ctable";
        case 0x51: return "LocalGravity";
        case 0x1e: return "AdcSignal";
        case 0x1f: return "ConfigRegister";
        case 0xf1: return "MasterStatusRegister";
        case 0xf8: return "RecordTask";
        case 0xfc: return "PartNumber";
        default:
        {
            return "?";
        }
    }
}

//---------------------------------------------------------
int NodeE4::hv() const
{
    return m_operating_hv;
}

//---------------------------------------------------------
QString NodeE4::partnumber() const
{
    return m_partnumber;
}

//// //---------------------------------------------------------
//// void NodeE4::gamma_accumulation_turnOn()
//// {
////     //// m_gamma_sum = 0;
////     //// m_gamma_counter = 0;
////     //// m_gamma_avrg = 0;
////     //// m_gamma_accumulation = true;
//// }

//// //---------------------------------------------------------
//// void NodeE4::gamma_accumulation_turnOff()
//// {
////     m_gamma_accumulation = false;
//// }

//---------------------------------------------------------
Adam::Frame NodeE4::ReadGammaCPS(byte buckets)
{
    if (buckets <  1) buckets = 1;
    if (buckets > 32) buckets = 32;
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x01, buckets, 500);
    frame.set_frame_name(frame_name(0x01));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadTemperature()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x05, (byte)1, 300);
    frame.set_frame_name(frame_name(0x05));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadAccelRaw(AvgType avgType)
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x06, (byte)avgType);
    frame.set_frame_name(frame_name(0x06));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadAccelInclination(AvgType avgType)
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x11, (byte)avgType);
    frame.set_frame_name(frame_name(0x11));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadAccelCorrectedData(AvgType avgType)
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x18, (byte)avgType);
    frame.set_frame_name(frame_name(0x18));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadGammaRayAcqusitionTime()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x13);
    frame.set_frame_name(frame_name(0x13));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::WriteGammaRayAcqusitionTime(ushort value)
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x13, Adam::encode_to_big_endian(value));
    frame.set_frame_name(frame_name(0x13) + " " + QString("Write"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadAdcSignal(AdcSignal signal)
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x1E, (byte)signal);
    frame.set_frame_name(frame_name(0x1E));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadRecordTask(int ext)
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0xF8, (byte)ext);
    frame.set_frame_name(frame_name(0xF8));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::WriteRecordTask(const std::vector<byte> &payload)
{
    // ;Write Recorder Task
    // ;Tx = <msg header> <0xF8> <cmd ext> <init delay> <period> <records> <buckets> <log mode> <chksum>
    // ;<cmd ext>  ::= <0x01> | <0x05> | <0x06>
    // ;<init delay>   ::= <MSByte> <LSByte>
    // ;<period>   ::= <MSByte> <LSByte>
    // ;<records>  ::= <byte>
    // ;<buckets>  ::= <byte>
    // ;<log mode> ::= <byte>
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0xF8, payload, 500);
    frame.set_frame_name(frame_name(0xF8) + " " + QString("Write"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadPartNumber()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0xfc);
    frame.set_frame_name(frame_name(0xfc) + " " + QString("Read"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadCtable()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x50);
    frame.set_frame_name(frame_name(0x50) + " " + QString("Read"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::WriteCTable(const CTable *ctable)
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x50, ctable->serialize());
    frame.set_frame_name(frame_name(0x50) + " " + QString("Write"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadKtable()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x0C);
    frame.set_frame_name(frame_name(0x0C) + " " + QString("Read"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::WriteKTable(const KTable *ktable)
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x0C, ktable->serialize());
    frame.set_frame_name(frame_name(0x0C) + " " + QString("Write"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadLocalGravity()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x51);
    frame.set_frame_name(frame_name(0x51) + " " + QString("Read"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::WriteLocalGravity(float value)
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x51, Adam::encode_ieee754(value));
    frame.set_frame_name(frame_name(0x51) + " " + QString("Write"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadConfigRegister()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x1f);
    frame.set_frame_name(frame_name(0x1f) + " " + QString("Read"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::WriteConfigRegister(byte value)
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x1f, std::vector<byte>{value});
    frame.set_frame_name(frame_name(0x1f) + " " + QString("Write"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::SetHVLow()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x40);
    frame.set_frame_name(frame_name(0x40) + " " + QString("Set"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::SetHVHigh()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x41);
    frame.set_frame_name(frame_name(0x41) + " " + QString("Set"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadCalibrationVoltages()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x42);
    frame.set_frame_name(frame_name(0x42) + " " + QString("Read"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::WriteCalibrationVoltages(ushort low, ushort high, ushort test)
{
    auto payload1 = Adam::encode_to_big_endian(low);
    auto payload2 = Adam::encode_to_big_endian(high);
    auto payload3 = Adam::encode_to_big_endian(test);
    std::vector<byte> payload{payload1};
    payload.insert(payload.end(), payload2.begin(), payload2.end());
    payload.insert(payload.end(), payload3.begin(), payload3.end());
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x42, payload);
    frame.set_frame_name(frame_name(0x42) + " " + QString("Write"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadOperationVoltage()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x43, 300);
    frame.set_frame_name(frame_name(0x43) + " " + QString("Read"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::WriteOperationVoltage(ushort value)
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x43, Adam::encode_to_big_endian(value), 300);
    frame.set_frame_name(frame_name(0x43) + " " + QString("Write"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadHVCorr()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x44);
    frame.set_frame_name(frame_name(0x44) + " " + QString("Read"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadHVWarmUp()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x46);
    frame.set_frame_name(frame_name(0x46) + " " + QString("Read"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::WriteHVWarmUp(byte value)
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x46, std::vector<byte>{value});
    frame.set_frame_name(frame_name(0x46) + " " + QString("Write"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::ReadMasterStatusRegister()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0xf1);
    frame.set_frame_name(frame_name(0xf1) + " " + QString("Read"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::WriteMasterStatusRegister(const std::vector<byte> &payload)
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0xf1, payload);
    frame.set_frame_name(frame_name(0xf1) + " " + QString("Write"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::WriteAccelPowerOn()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x20, 0x01);
    frame.set_frame_name(frame_name(0x20) + " " + QString("Write"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::WriteAccelPowerOff()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x20, 0x00);
    frame.set_frame_name(frame_name(0x20) + " " + QString("Write"));
    return frame;
}

//---------------------------------------------------------
Adam::Frame NodeE4::WriteStartADCCalibration()
{
    auto frame = Adam::Frame(NodeE4::CODExE4, 0xff, 0x20, 0x10);
    frame.set_frame_name(frame_name(0x20) + " " + QString("Write"));
    return frame;
}

//---------------------------------------------------------
QString NodeE4::ADC_name(int ext)
{
    AdcSignal num = static_cast<AdcSignal>(ext);
    switch (num)
    {
    case NodeE4::AdcSignal::Vdc15:
        return "Vdc15";
    case NodeE4::AdcSignal::Vdc5:
        return "Vdc5";
    case NodeE4::AdcSignal::Dv485:
        return "Dv485";
    case NodeE4::AdcSignal::HighVoltageControl:
        return "HighVoltageControl";
    case NodeE4::AdcSignal::TempSensor:
        return "Temper";
    case NodeE4::AdcSignal::TempSensorCalibr:
        return "TempCircuitCalibration";
    case NodeE4::AdcSignal::Vdc25Reference:
        return "Vdc25Reference";
    case NodeE4::AdcSignal::PowerSupplyAmperage:
        return "PowerSupplyAmperage";
    }
    return "-";
}

//---------------------------------------------------------
QString NodeE4::ADC_name_short(int ext)
{
    AdcSignal num = static_cast<AdcSignal>(ext);
    switch (num)
    {
    case NodeE4::AdcSignal::Vdc15:
        return "Vdc15";
    case NodeE4::AdcSignal::Vdc5:
        return "Vdc5";
    case NodeE4::AdcSignal::Dv485:
        return "Dv485";
    case NodeE4::AdcSignal::HighVoltageControl:
        return "HVCtl";
    case NodeE4::AdcSignal::TempSensor:
        return "Temper";
    case NodeE4::AdcSignal::TempSensorCalibr:
        return "TempClb";
    case NodeE4::AdcSignal::Vdc25Reference:
        return "Vdc25Ref";
    case NodeE4::AdcSignal::PowerSupplyAmperage:
        return "PwrAmper";
    }
    return "-";
}

//---------------------------------------------------------------------------
int NodeE4::response_0x01(const Adam::Response &response) // Read Gamma Ray Data
{
    // [01] Read Gamma Ray Data
    // [RTIME]=200
    // [CTYPE]=2
    // ;
    // B CMDEXT,N/A;Command Extention
    // F Gamma,cps;Gamma Ray counts per second
    //
    // ;This command tells the node to transmit gamma ray data.  This data is the average of "n" buckets
    // ;(acquisitions).  The gamma ray returned is IEEE floating point counts per second.  Bucket
    // ;limits are 1 (0x01) to 32 (0x20).  If the bucket count requested is outside the limits, the
    // ;node returns an <0xFD> cmd ack with no data.  If more buckets are requested than what is
    // ;available, the bucket count in the response indicates the number of buckets that were averaged.
    //
    // ;Tx = <msg header> <0x01> <cmd ext> <chksum>
    // ;<cmd ext> ::= <0x01>...<0x20>
    //
    // ;Rx = <msg header> <0x01> <cmd ack> <slave status> <cmd ext> <gamma ray> <chksum>
    // ;<cmd ack> 	::= <0x00> | <0xFD>
    // ;<cmd ext> 	::= <0x01>...<0x20>
    // ;<gamma ray>	::= <IEEE FP>

    m_gamma_ack = response.ack();
    m_gamma_status = response.status();
    m_gamma_elapsed_time = response.elapsed_time();
    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3")
            .arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0'))
            .arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }

    const auto &data = response.payload();
    qint8 ext = data.at(0);
    if (ext >= 1 && ext <= 32)
    {
        m_gamma_ext = ext;
        m_gamma_value = Adam::decode_ieee754(data.data()+1, data.size()-1);
        DBGF(2, "NodeE4 [%02X] ext:%d, cps:%5.2f (%dms)",
             response.cmd(), m_gamma_ext, m_gamma_value, m_gamma_elapsed_time);
    }
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x05(const Adam::Response &response) // Read Temperature Data
{
    // [05] Read Temperature Data
    // [RTIME]=200
    // [CTYPE]=2
    // ;
    // B CMDEXT,N/A;Command Extention
    // F Temp,deg C;temperature of the SRIG
    // ;
    // ;This command tells the node to transmit the internal temperature of the SRIG based on "n"
    // ;buckets of acquisitions (averaged).  One acquisition is the average of four analogs to digital
    // ;conversions.  The temperature returned is IEEE floating point degrees Celsius.  Bucket limit
    // ;is 1.  If the bucket count is outside the limit, the node returns an <0xFD> cmd ack with no data.
    // ;
    // ;Tx = <msg header> <0x05> <cmd ext> <chksum>
    // ;<cmd ext> ::= <0x01>
    // ;
    // ;Rx = <msg header> <0x05> <cmd ack> <slave status> <cmd ext> <temperature> <chksum>
    // ;<cmd ack> ::= <0x00> | <0xFB> | <0xFD>
    // ;<cmd ext> ::= <0x01>
    // ;<temperature> ::= <IEEE FP>
    // "fce4 000f 05 00 0000 01 41f2fc90 04b4"

    m_temperature_ack = response.ack();
    m_temperature_status = response.status();
    m_temperature_elapsed_time = response.elapsed_time();
    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3").arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }

    const auto &data = response.payload();
    m_temperature_num = data.at(0);
    m_temperature_value = Adam::decode_ieee754(data.data()+1, data.size()-1);
    DBG(2, QString("[%1] ext:%2 temperature:%3 (%4ms)")
        .arg(response.cmd(), 2, 16, QChar('0'))
        .arg(m_temperature_num)
        .arg(m_temperature_value, 0, 'f', 2)
        .arg(m_temperature_elapsed_time));

    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x06(const Adam::Response &response) // Read  Accelerometer Raw Data
{
    // B Avg Type,N/A; 01=Short, 02=Medium, 03=Long
    // F VGz,volts;Raw voltage from the 16 bit ADC that is attached to the Z axis accelerometer
    // ;
    // ;This command request for the raw voltage data from the 16 bit ADC
    // ;that is attached to the Z-axis accelerometer.
    // ;Tx = <msg header> <0x06> <cmd ext> <chksum>
    // ;<cmd ext> ::= <0x01> | <0x02> | <0x03>
    // ;
    // ;Rx = <msg header> <0x06> <cmd ack> <slave status> <cmd ext> <VGz> <chksum>
    // ;<cmd ack>  ::= <0x00> | <0xFB> | <0xFD>
    // ;<cmd ext>  ::= <0x01> | <0x02> | <0x03>
    // ;<VGz> ::= <IEEE FP>
    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3").arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }

    const auto &data = response.payload();
    auto ext = data.at(0);
    if (ext >= 1 && ext <= 3)
    {
        int i = ext-1;
        m_accelerometer_raw_ack[i] = response.ack();
        m_accelerometer_raw_status[i] = response.status();
        m_accelerometer_raw_elapsed_time[i] = response.elapsed_time();
        float value = Adam::decode_ieee754(data.data()+1, data.size()-1);
        m_accelerometer_raw_value[i] = value;
        DBGF(2, "NodeE4 [%02X] ext:%d accelerometer raw: %0.2f (%dms)",
             response.cmd(), ext, value, m_accelerometer_raw_elapsed_time[i]);
    }
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x11(const Adam::Response &response) // Read Accelerometer Inclination Data
{
    // B CmdExt,N/A;Command Extention
    // F Inc,deg;Inclination
    // ;
    // ;This command request to read the inclination from the MCU.
    // ;The command extension is the size of the bucket.
    // ;
    // ;Tx = <msg header> <0x11> <cmd ext> <chksum>
    // ;<cmd ext> ::= <0x01> | <0x02> | <0x03>
    // ;
    // ;Rx = <msg header> <0x11> <cmd ack> <slave status> <cmd ext> <Inclination> <chksum>
    // ;<cmd ack>	 ::= <0x00> | <0xFB> | <0xFD>
    // ;<cmd ext>	 ::= <0x01> | <0x02> | <0x03>
    // ;<Inclination>	::= <IEEE FP>

    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3").arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }

    const auto &data = response.payload();
    auto ext = data.at(0);
    if (ext >= 1 && ext <= 3) {
        int i = ext-1;
        m_accelerometer_incl_ack[i] = response.ack();
        m_accelerometer_incl_status[i] = response.status();
        m_accelerometer_incl_elapsed_time[i] = response.elapsed_time();
        float value = Adam::decode_ieee754(data.data() + 1, data.size() - 1);
        m_accelerometer_incl_value[i] = value;
        DBG(2, QString("[0x%1] ext:%2 inclination: %3 (%4ms)")
            .arg(response.cmd())
            .arg(ext)
            .arg(value, 0, 'f', 2, QChar('0'))
            .arg(m_accelerometer_incl_elapsed_time[i]));
    }
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x13(const Adam::Response &response) // Read/Write Gamma Ray Acquisition Time
{
    // [RTIME]=200
    // [CTYPE]=3
    // ;
    // W Acq time,10th sec;Acquisition Time
    // ;
    // ;The gamma acquisition time is the period used to acquire one gamma sample.  A 'bucket' refers
    // ;to one of these acquisitions.  The units used are tenths of a second and the limits are
    // ;10 to 600 (1s to 60s).
    // ;
    // ;Read Gamma Ray Acquisition Time
    // ;This command instructs the node to return the gamma acquisition time.  Gamma Acquisition time
    // ;reads from and writes to non-volatile memory.
    // ;
    // ;Tx = <msg header> <0x13> <chksum>
    // ;
    // ;Rx = <msg header> <0x13> <cmd ack> <slave status> <acq time> <chksum>
    // ;<cmd ack> ::= <0x00>
    // ;<acq time> ::= <MSByte> <LSByte>
    // ;
    // ; Write Gamma Ray Acquisition Time
    // ;This command instructs the node to change the gamma acquisition time.  The acquisition time
    // ;changes immediately but this does not affect any sample in progress.  If acq time is outside
    // ;the limit, the node returns an <0xFD> cmd ack and sets it to the limit.
    // ;
    // ;Tx = <msg header> <0x13> <acq time> <chksum>
    // ;<acq time> ::= <MSByte> <LSByte>
    // ;
    // ;Rx = <msg header> <0x13> <cmd ack> <slave status> <chksum>
    // ;<cmd ack> ::= <0x00> | <0xFD>

    m_gamma_acq_ack = response.ack();
    m_gamma_acq_status = response.status();
    m_gamma_acq_time_elapsed_time = response.elapsed_time();
    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3").arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }

    const auto &data = response.payload();
    m_gamma_acq_num = ((ushort)data.at(0) << 8) | (ushort)data.at(1);
    m_gamma_acq_time = m_gamma_acq_num*0.1;
    DBGF(2, "NodeE4 [%02X] acquisition time: '%d' (%0.2f sec) (%dms)",
         response.cmd(), m_gamma_acq_num, m_gamma_acq_time, m_gamma_acq_time_elapsed_time);
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x18(const Adam::Response &response) // Read Accelerometer corrected data
{
    // B CmdExt,N/A;Command Extention
    // F Gz,g's;accelerometer Z axis data which is corrected for bias, scale factor, and
    // ;	temperature
    // ;
    // ;This command request for the accelerometer Z-axis data which is being corrected for bias,
    // ;scale factor, and temperature, using 3rd-order models.
    // ;
    // ;Tx = <msg header> <0x18> <cmd ext> <chksum>
    // ;<cmd ext> ::= <0x01> | <0x02> | <0x03>
    // ;
    // ;Rx = <msg header> <0x08> <cmd ack> <slave status> <cmd ext> <Gz> <chksum>
    // ;<cmd ack	 ::= <0x00> | <0xFB> | <0xFD>
    // ;<cmd ext>	 ::= <0x01> | <0x02> | <0x03>
    // ;<Gz> 		::= <IEEE FP>

    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3").arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }

    const auto &data = response.payload();
    auto ext = data.at(0);
    if (ext >= 1 && ext <= 3)
    {
        int i = ext-1;
        m_accelerometer_corr_ack[i] = response.ack();
        m_accelerometer_corr_status[i] = response.status();
        m_accelerometer_corr_elapsed_time[i] = response.elapsed_time();
        float value = Adam::decode_ieee754(data.data()+1, data.size()-1);
        m_accelerometer_corr_value[i] = value;
        DBGF(2, "NodeE4 [%02X] ext:%d accelerometer corr: %0.2f (%dms)",
             response.cmd(), ext, value, m_accelerometer_corr_elapsed_time[i]);
    }
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x20(const Adam::Response &response) // Device Control
{
    // [20] Device Control
    // [RTIME]=200
    // [CTYPE]=2
    // ;
    // B Hex,Hex;Hex Byte
    // F ADC,N/A;ADC Reading
    // ;
    // ;This command allows the control of the SRIG built-in devices. This command is written to aid
    // ;the testing of the hardware.
    // ;
    // ;Tx = <msg header> <0x20> <cmd ext> <chksum>
    // ;<cmd ext> ::= <hex byte>
    // ;The one byte command extension can be divided into two parts: the device number and the
    // ;function number. The device number is a 5-bit number which is from bit3 to bit7 of the command
    // ;extension, and the function number is a 3-bit number which is from bit0 to bit2. The SRIG board
    // ;therefore can have up to 32 devices, and each device can have up to 8 control functions.
    // ;
    // ;The current implemented SRIG device control is stated as follows.
    // ;
    // ;Device name                Device# Func#  Description                 cmd ext
    // ;_____________________________________________________________________________________
    // ;Accelerometer power supply 00000   000    Turn off                    <0x00>
    // ;                                   001    Turn on                     <0x01>
    // ;Serial Communication       00001   000    Enable DV485/Disable LT1039 <0x08>
    // ;                                   001    Enable LT1039/Disable DV485 <0x09>
    // ;ADC7701                    00010   000    Start calibration           <0x10>
    // ;
    // ;When turn on the accelerometer power supply, the firmware will also wake up the ADC7701 and
    // ;start the accelerometer acquisition timer. When turn off the accelerometer power, the firmware
    // ;will put the ADC7701 into sleep mode and stop the acquisition timer.  While both of the
    // ;accelerometer power supply and ADC7701 control is effective immediately,the serial communication
    // ;control will only be effective after reset, thus the return cmd ack will be 0x03.
    // ;
    // ;Rx = <msg header> <0x20> <cmd ack> <slave status> <cmd ext> <ADC reading> <chksum>
    // ;<cmd ack> ::= <0x00> | <0x03> | <0xFD>
    // ;<cmd ext> ::= <hex byte>
    // ;<ADC reading>::= <IEEE FP>

    m_dev_control_ack = response.ack();
    m_dev_control_status = response.status();
    m_dev_control_elapsed_time = response.elapsed_time();
    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3").arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }

    const auto &data = response.payload();
    m_dev_control_num = data.at(0);
    float value = Adam::decode_ieee754(data.data()+1, data.size()-1);
    m_dev_control_value = value;
    DBGF(2, "NodeE4 [%02X] Device Control: %d - %f (%dms)",
         response.cmd(), m_dev_control_num, m_dev_control_value, m_dev_control_elapsed_time);
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x40(const Adam::Response &response) // Set HV Low
{
    // [40] Set HV Low
    // [RTIME]=200
    // [CTYPE]=0
    // ;
    // ;Use this command to learn the lowest possible voltage of the HV.  You will need this
    // ;information to calibrate the HV.  This command sets the DAC to 0x00, the node returns a
    // ;<0x01> cmd ack and the HV turns on for ten seconds.  Record this voltage for future use.
    // ;
    // ;Tx = <msg header> <0x40> <chksum>
    // ;
    // ;Rx = <msg header> <0x40> <cmd ack> <slave status> <chksum>
    // ;<cmd ack> ::= <0x01> | <0x04>
    m_hv_low_ack = response.ack();
    m_hv_low_status = response.status();
    m_hv_low_elapsed_time = response.elapsed_time();
    DBGF(2, "NodeE4 [%02X] HV Low ack: '0x%02X' (%dms)",
         response.cmd(), response.ack(), m_hv_low_elapsed_time);
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x41(const Adam::Response &response) // Set HV High
{
    // [41] Set HV High
    // [RTIME]=200
    // [CTYPE]=0
    // ;
    // ;Use this command to learn the highest  possible voltage of the HV.  You will need this
    // ;information to calibrate the HV.  This command sets the DAC to 0xFF, the node returns a
    // ;<0x01> cmd ack and the HV turns on for ten seconds.  Record this voltage for future use.
    // ;
    // ;Tx = <msg header> <0x41> <chksum>
    // ;
    // ;Rx = <msg header> <0x41> <cmd ack> <slave status> <chksum>
    // ;<cmd ack> ::= <0x01> | <0x04>
    m_hv_high_ack = response.ack();
    m_hv_high_status = response.status();
    m_hv_high_elapsed_time = response.elapsed_time();
    DBGF(2, "NodeE4 [%02X] Set HV High ack: '0x%02X' (%dms)",
         response.cmd(), response.ack(), m_hv_high_elapsed_time);
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x42(const Adam::Response &response) // Read/Write Calibrate HV
{
    // [42] Read/Write Calibrate HV
    // [RTIME]=200
    // [CTYPE]=3
    // ;
    // W Low,volts;Low Volt
    // W High,volts;High Volt
    // W Test,volts;Test Volt
    // ;
    // ;Use the recorded high and low voltages to calibrate the HV.  The tool uses these parameters
    // ;to calculate a scale factor and an offset.  LowVolt, HighVolt and TestVolt read from and write
    // ;to non-volatile memory.
    // ;
    // ;Read Calibrate HV
    // ;This command will read the HighVolt/LowVolt/TestVolt parameters.
    // ;
    // ;Tx = <msg header> <0x42> <chksum>
    // ;
    // ;Rx = <msg header> <0x42> <cmd ack> <slave status> <LowVolt> <HighVolt> <TestVolt> <chksum>
    // ;<cmd ack>	::= <0x00>
    // ;<LowVolt>	::= <MSByte> <LSByte>
    // ;<HighVolt>	::= <MSByte> <LSByte>
    // ;<TestVolt>	::= <MSByte> <LSByte>
    // ;
    // ;Write Calibrate HV
    // ;When you invoke this command with the hexadecimal voltage parameters, the tool will store the
    // ;voltages in non-volatile memory.  It will then calibrate the HV and set it to <TestVolt> for
    // ;ten seconds.  This is so the user can evaluate the accuracy of the newly calculated HV bias
    // ;and scale factor.
    // ;
    // ;Tx = <msg header> <0x42> <LowVolt> <HighVolt> <TestVolt> <chksum>
    // ;<LowVolt>	::= <MSByte> <LSByte>
    // ;<HighVolt>	::= <MSByte> <LSByte>
    // ;<TestVolt>	::= <MSByte> <LSByte>
    // ;
    // ;Rx = <msg header> <0x42> <cmd ack> <slave status> <chksum>
    // ;<cmd ack> ::= <0x01> | <0x04>
    // ;
    // ;Note: If the slave node's gamma state is in the power management mode when command number 0x40,
    // ;0x41, or 0x42 is received, the firmware will shut off the power management and set the high
    // ;voltage accordingly for 10 seconds. If one needs to resume the power management, then he needs
    // ;to issue the related command (0xF1, 0xF8) to initiate the power management.

    m_calibr_hv_ack = response.ack();
    m_calibr_hv_status = response.status();
    m_calibr_hv_elapsed_time = response.elapsed_time();

    if (response.ack() == 1 && response.payload_size() == 0) // ответ на операцию записи
    {
        DBG(0, QString("[0x%1] '%2' ACK:0x%3, status:0x%4 - ok")
            .arg(response.cmd(), 2, 16, QChar('0')).arg(response.name())
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.ack() != 0)
    {
        QString errstr;
        if (response.ack() == 0xFD)
            errstr = "(Voltage is outside the limits)";

        DBGF(0, "NodeE4 [%02X] '%s' ACK:0x%02X, status:0x%02X %s",
             response.cmd(), qPrintable(response.name()), response.ack(), response.status(),
             qPrintable(errstr));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }


    const auto &data = response.payload();
    m_calibr_hv_min  = (data.at(0) << 8) | data.at(1);
    m_calibr_hv_max  = (data.at(2) << 8) | data.at(3);
    m_calibr_hv_tst = (data.at(4) << 8) | data.at(5);
    DBG(2, QString("0x%1 HV Low:%2 High:%3, Test:%4 (%5ms)")
        .arg(response.cmd(), 2, 16, QChar('0'))
        .arg(m_calibr_hv_min)
        .arg(m_calibr_hv_max)
        .arg(m_calibr_hv_tst)
        .arg(m_calibr_hv_elapsed_time));
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x43(const Adam::Response &response) // Read/Write Voltage
{
    // [43] Read/Write Voltage
    // [RTIME]=200
    // [CTYPE]=3
    //
    // W voltage,volts;Operating voltage level for the HV
    //
    // ;This command sets the operating voltage level for the HV.  HV Voltage reads from and writes
    // ;to non-volatile memory.
    //
    // ;Read Voltage
    // ;This command reads the HV operating voltage.
    //
    // ;Tx = <msg header> <0x43> <chksum>
    // ;
    // ;Rx = <msg header> <0x43> <cmd ack> <slave status> <voltage> <chksum>
    // ;<cmd ack> ::= <0x00>
    // ;<voltage> ::= <MSByte> <LSByte>
    //
    // ;Write Voltage
    // ;This command sets a new voltage level for the HV.  The HV voltage level changes immediately.
    // ;If data acquisition is in progress, the current bucket should be suspect.  The units are Volts
    // ;and the limits are <LowVolt> and <HighVolt>.  (These voltage parameters were stored using the
    // ;Calibrate HV command.)  If Voltage is outside the limits, an <0xFD> cmd ack is returned and
    // ;it is set to the limit.
    //
    // ;Tx = <msg header> <0x43> <voltage> <chksum>
    // ;<voltage> ::= <MSByte> <LSByte>
    // ;
    // ;Rx = <msg header> <0x43> <cmd ack> <slave status> <chksum>
    // ;<cmd ack> ::= <0x00> | <0xFD>

    m_hv_ack = response.ack();
    m_hv_status = response.status();
    m_hv_elapsed_time = response.elapsed_time();

    if (response.ack() == 0 && response.payload_size() == 0) // ответ на операцию записи
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3")
            .arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0'))
            .arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.ack() != 0)
    {
        QString errstr;
        if (response.ack() == 0xFD) {
            errstr = "(Voltage is outside the limits)";
        }
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3 %4")
            .arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0'))
            .arg(response.status(), 4, 16, QChar('0'))
            .arg(errstr));

        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }


    const auto &data = response.payload();
    m_operating_hv = (data.at(0) << 8) | data.at(1);
    DBG(2, QString("[%1] Operating voltage: %2 (%3ms)")
        .arg(response.cmd(), 2, 16, QChar('0'))
        .arg(m_operating_hv)
        .arg(m_hv_elapsed_time));
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x44(const Adam::Response &response) // Read HVcorr
{
    // [44] Read HVcorr
    // [RTIME]=200
    // [CTYPE]=1
    // ;
    // B hvcorr,N/A;HVcorr is the hex value being written to the HV DAC to produce a specified
    // ;	voltage.
    // ;
    // ;Tx = <msg header> <0x44> <chksum>
    // ;
    // ;Rx = <msg header> <0x44> <cmd ack> <slave status> <hvcorr> <chksum>
    // ;<cmd ack>	::= <0x00>
    // ;<hvcorr>	::= <byte>
    // ;

    m_hv_corr_ack = response.ack();
    m_hv_corr_status = response.status();
    m_hv_corr_elapsed_time = response.elapsed_time();
    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3").arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }


    m_hv_corr = response.payload_byte();
    DBGF(2, "NodeE4 [%02X] HV corr: 0x%02X (%dms)",
         response.cmd(), m_hv_corr, m_hv_corr_elapsed_time);
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x46(const Adam::Response &response) // Read/Write HV Warm-up
{
    // [46] Read/Write HV Warm-up
    // [RTIME]=200
    // [CTYPE]=3
    // ;
    // B wm up,10ths sec;HV Warm-up is the delay between turning on the HV and starting an
    // ;	acquisition.  Units are tenths of a second and limits are 0.0s to 10.0s.  It is read from
    // ;	and written to non-volatile memory.
    // !!! замечание от программиста - значение считывается в секундах, а не в 10ths !!!
    // ;
    // ;Read HV Warm-up
    // ;
    // ;Tx = <msg header> <0x46> <chksum>
    // ;
    // ;Rx = <msg header> <0x46> <cmd ack> <slave status> <warm-up> <chksum>
    // ;<cmd ack>	::= <0x00>
    // ;<warm-up>	::= <hex byte>
    // ;
    // ;Write HV Warm-up
    // ;This command instructs the node to change the HV warm-up time.  If warm-up is outside the
    // ;limits, an <0xFD> cmd ack will be returned and it will be set to the appropriate limit.
    // ;
    // ;Tx = <msg header> <0x46> <warm-up> <chksum>
    // ;<warm-up> ::= <hex byte>
    // ;
    // ;Rx = <msg header> <0x46> <cmd ack> <slave status> <chksum>
    // ;<cmd ack> ::= <0x00> | <0xFD>

    m_warmup_ack = response.ack();
    m_warmup_status = response.status();
    m_warmup_elapsed_time = response.elapsed_time();

    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3").arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }

    m_warmup_num = response.payload_byte();
    m_warmup_time = m_warmup_num * 0.1;
    DBGF(2, "NodeE4 [%02X] HV Warm-up: %d (%0.2f sec) (%dms)",
         response.cmd(), m_warmup_num, m_warmup_time, m_warmup_elapsed_time);
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x50(const Adam::Response &response) // Read/Write CTABLE
{
    // [50] Read/Write CTABLE
    // [RTIME]=200
    // [CTYPE]=3
    // X CS,N/A;Check Sum
    // A11 PN,N/A;Part Number
    // F Ksfz0,N/A;Scale Factor
    // F Ksfz1,N/A;Scale Factor
    // F Ksfz2,N/A;Scale Factor
    // F Ksfz3,N/A;Scale Factor
    // F Kbz0,N/A;Bias Constant
    // F Kbz1,N/A;Bias Constant
    // F Kbz2,N/A;Bias Constant
    // F Kbz3,N/A;Bias Constant
    // A7 Acc SN,N/A;Accelerometer Serial Number
    // A8 Date,N/A;Date
    // A5 Time,N/A;Time
    // A15 Res,N/A;Reserved
    // ;
    // ;The CTABLE is used to calibrate the Accelerometer and is a 80 bytes long table, therefore,
    // ;the CTABLE should be written into the EEPROM memory of the MCU prior to the operation in order
    // ;to get the correct inclination data. The format of the CTABLE is listed as follows
    // ;
    // ;Field name				Data type	Length	Default value
    // ;_____________________________________________________________________
    // ;Check sum				ushrt		2		*
    // ;Part number			char		11		77557TE100
    // ;Scale factor - Ksfz0	float		4		0.0
    // ;Scale factor - Ksfz1	float		4		1.0
    // ;Scale factor - Ksfz2	float		4		0.0
    // ;Scale factor - Ksfz3	float		4		0.0
    // ;Bias constant - Kbz0	float		4		0.0
    // ;Bias constant - Kbz1	float		4		1.0
    // ;Bias constant - Kbz2	float		4		0.0
    // ;Bias constant - Kbz3	float		4		0.0
    // ;Accelerometer S/N  	    char        7       *
    // ;Date					char		8		*
    // ;Time					char		5		*
    // ;Reserved				char		15		*
    // ;
    // ;Read CTABLE
    // ;This command instructs the node to return the CTABLE which is stored in the MCU's EEPROM memory.
    // ;Tx = <msg header> <0x50> <chksum>
    // ;Rx = <msg header> <0x50> <cmd ack> <slave status> <CTABLE> <chksum>
    // ;<cmd ack>	::= <0x00>
    // ;<CTABLE>	::= <Table of 80 bytes>
    // ;
    // ; Write CTABLE
    // ;This command instructs the node to change the contents of the CTABLE.
    // ;Tx = <msg header> <0x50> <CTABLE> <chksum>
    // ;<CTABLE> ::= <Table of 80 bytes>
    // ;Rx = <msg header> <0x50> <cmd ack> <slave status> <chksum>
    // ;<cmd ack> ::= <0x00> | <0xFD>

    m_ctable_ack = response.ack();
    m_ctable_status = response.status();
    m_ctable_elapsed_time = response.elapsed_time();
    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3").arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }

    if (response.payload_size() < 80)
    {
        DBG(0, QString("[%1] INCORRECT RESPONSE - expected data size at least 80 but got %2")
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.payload_size()));
        return 0;
    }

    *m_ctable = response.payload();
    DBG(2, QString("[%1] %2 (%3ms)")
        .arg(response.cmd(), 2, 16, QChar('0'))
        .arg(QString("size:%1 CRC:%2 (0x%3) - "
                     "PN:'%4', "
                     "%5 %6 %7 %8 "
                     "%9 %10 %11 %12 "
                     "SN:'%13' Date:'%14' Time:'%15'")
             .arg(response.payload_size())
             .arg(m_ctable->m_check_sum)
             .arg(m_ctable->m_check_sum, 4, 16, QChar('0'))
             .arg(std::string(m_ctable->m_part_number, sizeof(m_ctable->m_part_number)).c_str())
             .arg(m_ctable->m_scale_Ksfz0, 0, 'f', 8)
             .arg(m_ctable->m_scale_Ksfz1, 0, 'f', 8)
             .arg(m_ctable->m_scale_Ksfz2, 0, 'f', 8)
             .arg(m_ctable->m_scale_Ksfz3, 0, 'f', 8)
             .arg(m_ctable->m_bias_Kbz0,   0, 'f', 8)
             .arg(m_ctable->m_bias_Kbz1,   0, 'f', 8)
             .arg(m_ctable->m_bias_Kbz2,   0, 'f', 8)
             .arg(m_ctable->m_bias_Kbz3,   0, 'f', 8)
             .arg(std::string(m_ctable->m_accel_SN, sizeof(m_ctable->m_accel_SN)).c_str())
             .arg(std::string(m_ctable->m_date, sizeof(m_ctable->m_date)).c_str())
             .arg(std::string(m_ctable->m_time, sizeof(m_ctable->m_time)).c_str()))
        .arg(m_ctable_elapsed_time));
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x0C(const Adam::Response &response) // Read/Write KTABLE
{
    // [0C] Read/Write KTABLE
    // [RTIME]=200
    // [CTYPE]=3
    // ;
    // X CS,N/A;Check Sum
    // A11 PN,N/A;Part Number
    // B MBS,bytes;N/A;Medium Bucket Size
    // B LBS,bytes;Long Bucket Size
    // W CT,N/A;Calibration Time
    // B AF,N/A;Acquire Frequency
    // W SBS,N/A;Short Bucket Size
    // A44 R,N/A;Reserved
    // ;
    // ;This command read/write the 64 bytes KTABLE stored in the MCU's EEPROM memory. The layout about
    // ;the KTABLE is listed as follows.
    // ;
    // ;Field Name			Data Type	Length	Default Value	Min. Value	Max. Value
    // ;Check sum			ushrt		2		*			*		*
    // ;Part number		char		11		77557TD100	*		*
    // ;Medium bucket size	uchar		1		4			1		255
    // ;Long bucket size	uchar		1		8			1		255
    // ;Calibration time	ushrt		2		30			1		65535
    // ;Acquire frequency	uchar		1		20			1		255
    // ;Short bucket size	ushrt		2		300			1		65535
    // ;Reserved			char		44		0			*		*
    // ;
    // ;Note: A total of 64 bytes is reserved for the KTABLE. All unused and reserved bytes are
    // ;initialize to zero.  The calibration time is no longer in use, please ignore it.
    // ;
    // ;Read KTABLE
    // ;This command instructs the node to return the KTABLE which is stored in the MCU's EEPROM memory.
    // ;
    // ;Tx = <msg header> <0x0C> <chksum>
    // ;
    // ;Rx = <msg header> <0x0C> <cmd ack> <slave status> <KTABLE> <chksum>
    // ;<cmd ack>	::= <0x00>
    // ;<KTABLE>	::= <Table of 32 bytes>
    // ;
    // ;Write KTABLE
    // ;This command instructs the node to change the contents of the KTABLE which is stored in the
    // ;MCU's EEPROM memory.
    // ;
    // ;Tx = <msg header> <0x0C> <KTABLE> <chksum>
    // ;<KTABLE> ::= <Table of 32 bytes>
    // ;
    // ;Rx = <msg header> <0x0C> <cmd ack> <slave status> <chksum>
    // ;<cmd ack> ::= <0x00> | <0xFD>

    m_ctable_ack = response.ack();
    m_ctable_status = response.status();
    m_ctable_elapsed_time = response.elapsed_time();
    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3").arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }

    if (response.payload_size() < 64)
    {
        DBG(0, QString("[%1] INCORRECT RESPONSE - expected data size at least 64 but got %2")
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.payload_size()));
        return 0;
    }

    *m_ktable = response.payload();

    DBG(2, QString("[%1] %2 (%3ms)")
        .arg(response.cmd(), 2, 16, QChar('0'))
        .arg(QString( "size:%1 CRC:%2(0x%3) - "
                      "PN:'%4', sizes: short:%5 medium:%6 long:%7, "
                      "calibr time:%8, acq freq:%9 (%10ms)" )
             .arg(response.payload_size())
             .arg(m_ktable->get_checksum())
             .arg(m_ktable->get_checksum(), 4, 16, QChar('0'))
             .arg(m_ktable->get_partnumber())
             .arg((uint32_t)m_ktable->get_Short_bucket_size())
             .arg((uint32_t)m_ktable->get_Medium_bucket_size())
             .arg((uint32_t)m_ktable->get_Long_bucket_size())
             .arg((uint32_t)m_ktable->get_Calibration_time())
             .arg((uint32_t)m_ktable->get_Acquire_frequency())
             .arg(m_ctable_elapsed_time)));
            return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x51(const Adam::Response &response) // Read/Write Local Gravity
{
    // [51] Read/Write Local Gravity
    // [RTIME]=200
    // [CTYPE]=3
    // ;
    // F local,N/A;local gravity, The local gravity is stored in the MCU's EEPROM memory which is used
    // ;to calculate the inclination. In most places, this number should be 1.00 where some other places
    // ;might have silghtly different from 1.00.  The local gravity should also be written into the
    // ;EEPROM when calibrate the SRIG in order to get the correct inclination data during operation.
    // ;
    // ;Read  Local Gravity
    // ;Tx = <msg header> <0x51> <chksum>
    // ;Rx = <msg header> <0x51> <cmd ack> <slave status> <local gravity> <chksum>
    // ;<cmd ack>  ::= <0x00>
    // ;<local gravity>    ::= <IEEE FP>
    // ;
    // ;Write Local Gravity
    // ;This command instructs the node to change the local gravity.
    // ;Tx = <msg header> <0x51> <local gravity> <chksum>
    // ;<local gravity> ::= <IEEE FP>
    // ;Rx = <msg header> <0x51> <cmd ack> <slave status> <chksum>
    // ;<cmd ack> ::= <0x00> | <0xFD>

    m_gravity_ack = response.ack();
    m_gravity_status = response.status();
    m_gravity_elapsed_time = response.elapsed_time();
    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3").arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }


    const auto &data = response.payload();
    float value = Adam::decode_ieee754(data);
    m_gravity = value;
    DBGF(2, "NodeE4 [%02X] gravity: %f (%dms)",
         response.cmd(), value, m_gravity_elapsed_time);
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x1e(const Adam::Response &response) // Read the MCU's A/D converter
{
    // ;Tx = <msg header> <0x1E> <cmd ext> <chksum>
    // ;<cmd ext> ::= <0x00>..<0x07>
    // ;
    // ;Each command extension related to one analog signal. The current SRIG board design for the
    // ;analog input and the relation to the command extension is shown as follows.
    // ;
    // ;<0x00>	: +15 VDC signal.
    // ;<0x01>	: +5 VDC signal.
    // ;<0x02>	: DV485 signal.
    // ;<0x03>	: High voltage control signal.
    // ;<0x04>	: Temperature sensor signal.
    // ;<0x05>	: Temperature circuit calibration signal.
    // ;<0x06>	: 2.5V reference signal.
    // ;<0x07>	: Power supply current.
    //
    // ;Rx = <msg header> <0x1E> <cmd ack> <slave status> <cmd ext> <ADC reading> <chksum>
    // ;<cmd ack> 	::= <0x00> | <0xFD>
    // ;<cmd ext> 	::= <0x00>..<0x07>
    // ;<ADC reading>	::= <IEEE FP>

    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3").arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }


    const auto &data = response.payload();
    auto ext = static_cast<AdcSignal>(data.at(0));
    if (ext >= AdcSignal::begin && ext <= AdcSignal::end)
    {
        m_adc_ack[int(ext)] = response.ack();
        m_adc_status[int(ext)] = response.status();
        m_adc_elapsed_time[int(ext)] = response.elapsed_time();
    }

    float value = Adam::decode_ieee754(data.data()+1, data.size()-1);
    DBGF(2, "NodeE4 [%02X] ext:%d ADC: %f (%dms)",
         response.cmd(), int(ext), value, response.elapsed_time());
    switch (ext)
    {
    case NodeE4::AdcSignal::Vdc15:
        m_voltage15 = value;
        DBG(2, "AdcSignal::Vdc15");
        break;
    case NodeE4::AdcSignal::Vdc5:
        m_voltage5 = value;
        DBG(2, "AdcSignal::Vdc5");
        break;
    case NodeE4::AdcSignal::Dv485:
        m_dv485 = value;
        DBG(2, "AdcSignal::Dv485");
        break;
    case NodeE4::AdcSignal::HighVoltageControl:
        m_high_voltage_control = value;
        DBG(2, "AdcSignal::HighVoltageControl");
        break;
    case NodeE4::AdcSignal::TempSensor:
        m_temperature_sensor = value;
        DBG(2, "AdcSignal::TempSensor");
        break;
    case NodeE4::AdcSignal::TempSensorCalibr:
        m_temperature_circuit_calibration = value;
        DBG(2, "AdcSignal::TempSensorCalibr");
        break;
    case NodeE4::AdcSignal::Vdc25Reference:
        m_vdc25_reference = value;
        DBG(2, "AdcSignal::Vdc25Reference");
        break;
    case NodeE4::AdcSignal::PowerSupplyAmperage:
        m_power_amperage = value;
        DBG(2, "AdcSignal::PowerSupplyAmperage");
        break;
    }
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0x1f(const Adam::Response &response) // Read/Write CONFIG Register
{
    //;Rx = <msg header> <0x1F> <cmd ack> <slave status> <config> <chksum>
    //;<cmd ack>  ::= <00>
    //;<config> ::= <byte>

    m_config_register_ack = response.ack();
    m_config_register_status = response.status();
    m_config_register_elapsed_time = response.elapsed_time();
    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3").arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }


    uint value = response.payload_byte();
    m_config_register = value;
    DBGF(2, "NodeE4 [%02X] config register: '0x%02X' (%dms)",
         response.cmd(), value, m_config_register_elapsed_time);
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0xf1(const Adam::Response &response) // Read/Write Master Status Register
{
    // [F1] Read/Write Master Status Register
    // [RTIME]=200
    // [CTYPE]=3
    // ;
    // L MSR,N/A;Master status Registar
    // ;
    // ;The Master Status Register is a 32-bit register that is maintained by the Bus Master.  The
    // ;Bus Master will broadcast (transmit to node <0x03>) this register whenever it changes.  Since
    // ;this is a broadcast message and slave nodes do not respond, it is sent three times with a 250
    // ;millisecond delay between each message.  This is to decrease the possibility of a slave node
    // ;not receiving it.
    // ;
    // ;The Master Status Register:
    // ;
    // ;Bit 31:  Master State
    // ;   1= The Bus Master is running tasks that require data from slave nodes. When this
    // ;   bit is set, and the rotation state match the log requirement in the recorder task
    // ;   record is match, the firmware will put the node into power management mode.
    // ;   0= The Bus Master is not running any tasks which require slave node data.
    // ;   Whenever the slave node receive the MSR with this bit clear, it will shut down the
    // ;   power management function.
    // ;Bit 30:  Flow State
    // ;   1= Flow On has been detected by the Bus Master.
    // ;   0= Flow Off has been detected by the Bus Master.
    // ;Bit 29:  Rotation State
    // ;   1= Rotation has been detected by the Bus Master.
    // ;   0= Rotation has not been detected by the Bus Master.
    // ;Bit 28:  Inclination State
    // ;   1= High Inclination has been detected by the Bus Master.
    // ;   0= Low Inclination has been detected by the Bus Master.
    // ;Bit 27 - Bit 0
    // ;   Unassigned
    // ;Read Master Status Register
    // ;Requests the node transmit its copy of the MSR.
    // ;
    // ;Tx = <msg header> <0xF1> <chksum>
    // ;
    // ;Rx = <msg header> <0xF1> <cmd ack> <slave status> <MSR> <chksum>
    // ;<cmd ack>::= <0x00>
    // ;<MSR>::= <MSByte> <byte> <byte> <LSByte>
    // ;
    // ;
    // ; Write Master Status Register
    // ;Commands the node to update its copy of the MSR and take whatever action is required. This
    // ;command normally is broadcast by the Master node.
    // ;
    // ;Tx = Command
    // ;<msg header> <0xF1> <MSR> <chksum>
    // ;<MSR> ::= <MSByte> <byte> <byte> <LSByte>
    // ;
    // ;Slave nodes do not respond to Broadcast Messages.

    m_statusreg_ack = response.ack();
    m_statusreg_status = response.status();
    m_statusreg_elapsed_time = response.elapsed_time();
    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3").arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }


    const auto &data = response.payload();
    uint32_t value = 0;
    value |= data.at(0);
    value <<= 8;
    value |= data.at(1);
    value <<= 8;
    value |= data.at(2);
    value <<= 8;
    value |= data.at(3);
    m_statusreg = value;
    DBGF(2, "NodeE4 [%02X] Master Status Register: 0x%08X (%dms)",
         response.cmd(), m_statusreg, m_statusreg_elapsed_time);
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0xf8(const Adam::Response &response) // Read Record Task
{
    // B cmd ext,hex;Command Extention
    // W init delay,10ths sec;Initial Delay
    // W period,10ths sec;time between first request for data, and subsequent requests for data
    // B records,records;maximum number of data requests that will be received
    // B buckets,samples;number of samples that will be asked for with each data request
    // B log mode,hex;a flag that indicates whether data is required during rotating mode, non-rotating
    // ;	mode, or both
    // ;Tx = <msg header> <0xF8> <cmd ext> <chksum>
    // ;<cmd ext> ::= <0x01> | <0x05> | <0x06>
    // ;
    // ;Rx = <msg header> <0xF8> <cmd ext> <init delay> <period> <records> <buckets> <log mode> <chksum>
    // ;<cmd ext>  ::= <0x01> | <0x05> | <0x06>
    // ;<init delay>   ::= <MSByte> <LSByte>
    // ;<period>   ::= <MSByte> <LSByte>
    // ;<records>  ::= <byte>
    // ;<buckets>  ::= <byte>
    // ;<log mode> ::= <byte>
    // ;<cmd ack>  ::= <0x00> | <0xFD>
    // ;
    // ;Write Recorder Task
    // ;Tx = <msg header> <0xF8> <cmd ext> <init delay> <period> <records> <buckets> <log mode> <chksum>
    // ;<cmd ext>  ::= <0x01> | <0x05> | <0x06>
    // ;<init delay>   ::= <MSByte> <LSByte>
    // ;<period>   ::= <MSByte> <LSByte>
    // ;<records>  ::= <byte>
    // ;<buckets>  ::= <byte>
    // ;<log mode> ::= <byte>
    // ;
    // ;Rx = <msg header> <0xF8> <cmd ack> <slave status> <chksum>
    // ;<cmd ack> ::= <0x00> | <0xFD>

    m_rectask_ack = response.ack();
    m_rectask_status = response.status();
    m_rectask_elapsed_time = response.elapsed_time();
    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3").arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }


    const auto &data = response.payload();
    auto ext = data.at(0);
    if (ext == 1 || ext == 5 || ext == 6)
    {
        //<init delay> <period> <records> <buckets> <log mode>
        m_rectask_num       = ext;
        m_rectask_initdelay = (data.at(1) << 8) | data.at(2);
        m_rectask_period    = (data.at(3) << 8) | data.at(4);
        m_rectask_records   = data.at(5);
        m_rectask_buckets   = data.at(6);
        m_rectask_logmode   = data.at(7);

        auto txt = QStringLiteral("[%1] %2s %3s %4 %5 %6")
                .arg(m_rectask_num)
                .arg(m_rectask_initdelay)
                .arg(m_rectask_period)
                .arg(m_rectask_records)
                .arg(m_rectask_buckets)
                .arg(m_rectask_logmode);
        DBGF(2, "NodeE4 [%02X] Record Task: %s (%dms)",
             response.cmd(), qPrintable(txt), m_rectask_elapsed_time);
    }
    return 1;
}

//---------------------------------------------------------------------------
int NodeE4::response_0xfc(const Adam::Response &response) // Part Number
{
    //;Rx = <msg header> <0xFC> <cmd ack> <slave status> <response data> <chksum>
    //;<cmd ack>::= <0x00>
    //;<response data>::= 77557TA100

    if (response.ack() != 0)
    {
        DBG(2, QString("[%1], ACK:0x%2, status:0x%3").arg(response.cmd(), 2, 16, QChar('0'))
            .arg(response.ack(), 2, 16, QChar('0')).arg(response.status(), 4, 16, QChar('0')));
        return 0;
    }

    m_partnumber_ack = response.ack();
    m_partnumber_status = response.status();
    m_partnumber_elapsed_time = response.elapsed_time();
    if (response.payload_size() == 0)
    {
        DBGF(3, "NodeE4 [%02X] info: no data in response (%dms)", response.cmd(), m_rectask_elapsed_time);
        return 0;
    }

    m_partnumber = QString::fromStdString(response.payload_string());
    DBGF(2, "NodeE4 [%02X] PartNumber: '%s' (%dms)",
         response.cmd(), qPrintable(m_partnumber), m_partnumber_elapsed_time);
    return 1;
}

//---------------------------------------------------------------------------
bool NodeE4::check_ack(const Adam::Response &response)
{
    if (response.ack() == 0) return true;
    return false;

    // далее большое TODO - нужны-ли какие-либо действия при ответе с проблемами
    QString cmdHex  = QString("[%1]").arg(response.cmd(), 2, 16, QChar('0'));
    QString cAck    = QString("0x%1").arg(response.ack(), 2, 16, QChar('0')).toUpper();
    QString cStatus = QString("0x%1").arg(response.status(), 4, 16, QChar('0')).toUpper();

    if (response.ack() == 0xFB)
    {
        QString bf = QString("%1 %2").arg(cmdHex).arg(QObject::tr("The wrong request"));
        bf = QString("%1 %2 %3 - %4").arg(cmdHex, QObject::tr("ERROR"), cAck, QObject::tr("The wrong request"));
        return false;
    }
    if (response.ack() == 0xFD)
    {
        QString bf = QString("%1 %2").arg(cmdHex).arg(QObject::tr("The value outside the limit"));
        bf = QString("%1 %2 %3 - %4").arg(cmdHex, QObject::tr("ERROR"), cAck, QObject::tr("The value outside the limit"));
        return false;
    }

    QString buf2;
    QString buf = QString("%1 %2").arg(cmdHex).arg(QObject::tr("ERROR"));
    if (response.cmd() == 0x40 ||
            response.cmd() == 0x41 ||
            response.cmd() == 0x42 ||
            response.cmd() == 0x20)
    {
        buf = QString("%1 %2 0x%3").arg(cmdHex, QObject::tr("Ack")).arg(response.ack(), 2, 16, QChar('0'));
    }

    if (response.cmd() == 0x20)
    {
        if (response.ack() == 0x03)
        {
            buf  = QObject::tr("The serial communication requires reset");
            buf2 = QObject::tr("The serial communication control will only be effective after reset");
        }
    }
    else if (response.cmd() == 0x40 ||
             response.cmd() == 0x41 ||
             response.cmd() == 0x42)
    {
        if (response.ack() == 0x01)
            buf = QObject::tr("The HV turns on for ten seconds");
    }

    if (buf2.isEmpty()) buf2 = buf;
    return false;
}

//---------------------------------------------------------------------------
int NodeE4::response(const Adam::Response &response)
{
    DBG(3, QStringLiteral("response: %1").arg(QString::fromStdString(response.dump())));
    DBG(4, QStringLiteral("from [%1] ack:0x%2 status:0x%3 (%4ms)")
        .arg(response.cmd(), 2, 16, QChar('0'))
        .arg(response.ack(), 2, 16, QChar('0'))
        .arg(response.status(), 4, 16, QChar('0'))
        .arg(response.elapsed_time()));

    int ret = 0; // 0 - не надо выдавать сигнал на обновление значений в GUI
    byte cmd = response.cmd();
    switch (cmd)
    {
    case 0x01:                 // Read Gamma Ray Data
        ret = response_0x01(response);
        break;
    case 0x05:                 // Read Temperature Data
        ret = response_0x05(response);
        break;
    case 0x06:                 // Read Accelerometer Raw Data
        ret = response_0x06(response);
        break;
    case 0x0C:                 // Read/Write KTABLE
        ret = response_0x0C(response);
        break;
    case 0x11:                 // Read Accelerometer Inclination Data
        ret = response_0x11(response);
        break;
    case 0x13:                 // Read/Write Gamma Ray Acquisition Time
        ret = response_0x13(response);
        break;
    case 0x18:                 // Read Accelerometer corrected data
        ret = response_0x18(response);
        break;
    case 0x20:                 // Device Control
        ret = response_0x20(response);
        break;
    case 0x40:                 // Set HV Low
        ret = response_0x40(response);
        break;
    case 0x41:                 // Set HV High
        ret = response_0x41(response);
        break;
    case 0x42:                 // Read/Write Calibrate HV
        ret = response_0x42(response);
        break;
    case 0x43:                 // Read/Write Voltage
        ret = response_0x43(response);
        break;
    case 0x44:                 // Read HVcorr
        ret = response_0x44(response);
        break;
    case 0x46:                 // Read/Write HV Warm-up
        ret = response_0x46(response);
        break;
    case 0x50:                 // Read/Write CTABLE
        ret = response_0x50(response);
        break;
    case 0x51:                 // Read/Write Local Gravity
        ret = response_0x51(response);
        break;
    case 0x1e:                 // Read the MCU's A/D converter
        ret = response_0x1e(response);
        break;
    case 0x1f:                 // Read/Write CONFIG Register
        ret = response_0x1f(response);
        break;
    case 0xf1:                 // Read/Write Master Status Register
        ret = response_0xf1(response);
        break;
    case 0xf8:                 // Read Record Task
        ret = response_0xf8(response);
        break;
    case 0xfc:                 // PartNumber
        ret = response_0xfc(response);
        break;
    default:
        DBGF(2, "NodeE4 - unregistered cmd: 0x%02X\n", cmd);
        ret = 0;
        break;
    }
    return ret;
}

//---------------------------------------------------------------------------
const QString NodeE4::status_description(const byte ack, const NodeE4::Status status)
{
    auto bitstr = QObject::tr("bit");
    QString text;
    if (status & Status::FirstTempAcquisitionIsInProgress) {
        text += QString("FirstTempAcquisitionIsInProgress") + QStringLiteral(" ");

    }
    if (status & Status::FirstGammaAcquisitionIsInProgress) {
        text += QString("FirstGammaAcquisitionIsInProgress") + QStringLiteral(" ");

    }
    if (status & Status::HVCalibrateCommandIsInProgress) {
        text += QString("HVCalibrateCommandIsInProgress") + QStringLiteral(" ");

    }
    if (status & Status::HVIsOff) {
        text += QString("HVIsOff") + QStringLiteral(" ");

    }
    if (status & Status::InternalEEPROMError) {
        text += QString("InternalEEPROMError") + QStringLiteral(" ");

    }
    if (status & Status::ErrorWritingEEPROM) {
        text += QString("ErrorWritingEEPROM") + QStringLiteral(" ");

    }
    if (status & Status::DataMemoryChecksumError) {
        text += QString("DataMemoryChecksumError") + QStringLiteral(" ");

    }
    if (status & Status::ProgramMemoryChecksumError) {
        text += QString("ProgramMemoryChecksumError") + QStringLiteral(" ");

    }
    if (status & Status::InternalRAMError) {
        text += QString("InternalRAMError") + QStringLiteral(" ");

    }
    if (status & Status::ExternalRAMError) {
        text += QString("ExternalRAMError") + QStringLiteral(" ");

    }
    if (status & Status::IllegalOpcodeReset) {
        text += QString("%1 10:  %2").arg(bitstr).arg("Illegal Opcode Reset") + QStringLiteral(" ");

    }
    if (status & Status::ClockMonitorReset) {
        text += QString("%1 11:  %2").arg(bitstr).arg("Clock Monitor Reset") + QStringLiteral(" ");

    }
    if (status & Status::ComputerOperatingProperlyReset) {
        text += QString("ComputerOperatingProperlyReset") + QStringLiteral(" ");

    }
    if (status & Status::UninitializedInterruptReset) {
        text += QString("UninitializedInterruptReset") + QStringLiteral(" ");

    }
    if (status & Status::FirstAccelerAcquisitionIsInProgress) {
        text += QString("FirstAccelerAcquisitionIsInProgress") + QStringLiteral(" ");

    }
    if (status & Status::AccelerometerIsInCalibrationMode) {
        text += QString("AccelerometerIsInCalibrationMode") + QStringLiteral(" ");
    }

    if (!text.isEmpty()) {
        return text;
    }

    if (ack == 0xFB) {
        return  QObject::tr("The wrong request");
    }
    if (ack == 0xFD) {
        return QObject::tr("The value is outside the limit");
    }

    return {};
}

//---------------------------------------------------------------------------
const QString NodeE4::status_tip(const byte ack, const NodeE4::Status status)
{
    (void)ack;
    auto bitstr = QObject::tr("bit");
    QString text;
    if (status & Status::FirstTempAcquisitionIsInProgress) {
        text += QString("<tr><td>%1 0:  %2</td></tr>").arg(bitstr).arg("First Temperature acquisition is in Progress(busy)");

    }
    if (status & Status::FirstGammaAcquisitionIsInProgress) {
        text += QString("<tr><td>%1 1:  %2</td></tr>").arg(bitstr).arg("First Gamma acquisition is in Progress");

    }
    if (status & Status::HVCalibrateCommandIsInProgress) {
        text += QString("<tr><td>%1 2:  %2</td></tr>").arg(bitstr).arg("HV Calibrate Command is in progress");

    }
    if (status & Status::HVIsOff) {
        text += QString("<tr><td>%1 3:  %2</td></tr>").arg(bitstr).arg("HV is off");

    }
    if (status & Status::InternalEEPROMError) {
        text += QString("<tr><td>%1 4:  %2</td></tr>").arg(bitstr).arg("Internal EEPROM Error");

    }
    if (status & Status::ErrorWritingEEPROM) {
        text += QString("<tr><td>%1 5:  %2</td></tr>").arg(bitstr).arg("Error Writing EEPROM");

    }
    if (status & Status::DataMemoryChecksumError) {
        text += QString("<tr><td>%1 6:  %2</td></tr>").arg(bitstr).arg("Data Memory Checksum Error");
    }
    if (status & Status::ProgramMemoryChecksumError) {
        text += QString("<tr><td>%1 7:  %2</td></tr>").arg(bitstr).arg("Program Memory Checksum Error");

    }
    if (status & Status::InternalRAMError) {
        text += QString("<tr><td>%1 8:  %2</td></tr>").arg(bitstr).arg("Internal RAM Error");

    }
    if (status & Status::ExternalRAMError) {
        text += QString("<tr><td>%1 9:  %2</td></tr>").arg(bitstr).arg("External RAM Error");

    }
    if (status & Status::IllegalOpcodeReset) {
        text += QString("<tr><td>%1 10:  %2</td></tr>").arg(bitstr).arg("Illegal Opcode Reset");

    }
    if (status & Status::ClockMonitorReset) {
        text += QString("<tr><td>%1 11:  %2</td></tr>").arg(bitstr).arg("Clock Monitor Reset");

    }
    if (status & Status::ComputerOperatingProperlyReset) {
        text += QString("<tr><td>%1 12:  %2</td></tr>").arg(bitstr).arg("Computer Operating Properly Reset");

    }
    if (status & Status::UninitializedInterruptReset) {
        text += QString("<tr><td>%1 13:  %2</td></tr>").arg(bitstr).arg("Uninitialized Interrupt Reset");

    }
    if (status & Status::FirstAccelerAcquisitionIsInProgress) {
        text += QString("<tr><td>%1 14:  %2</td></tr>").arg(bitstr).arg("First Accelerometer acquisition is in progress");

    }
    if (status & Status::AccelerometerIsInCalibrationMode) {
        text += QString("<tr><td>%1 15:  %2</td></tr>").arg(bitstr).arg("Accelerometer is in Calibration mode");
    }

    if (text.isEmpty()) {
        return text;
    }

    text.replace(" ", "&nbsp;");
    return text;
}

