/*!
  \file   NodeE4.h
  \author Aleksey Lekhtin
  \brief  Gamma node
  \date   10.05.2023
*/

#ifndef __NODE_E4_H__
#define __NODE_E4_H__

#include <list>

#include <QString>

#include "frame.h"
#include "node.h"
#include "response.h"
#include "NodeE4_ctable.h"
#include "NodeE4_ktable.h"

class NodeE4 : public NODE
{
public:
    static constexpr byte CODExE4 = (byte)0xE4; // SRIG

    enum Status {
        FirstTempAcquisitionIsInProgress = 0x01,      //[BIT 0] First Temperature acquisition is in Progress(busy)
        FirstGammaAcquisitionIsInProgress = 0x02,     //[BIT 1] First Gamma acquisition is in Progress
        HVCalibrateCommandIsInProgress = 0x04,        //[BIT 2] HV Calibrate Command is in progress
        HVIsOff = 0x08,                               //[BIT 3] HV is off
        InternalEEPROMError = 0x10,                   //[BIT 4] Internal EEPROM Error
        ErrorWritingEEPROM = 0x20,                    //[BIT 5] Error Writing EEPROM
        DataMemoryChecksumError = 0x40,               //[BIT 6] Data Memory Checksum Error
        ProgramMemoryChecksumError = 0x80,            //[BIT 7] Program Memory Checksum Error
        InternalRAMError = 0x100,                     //[BIT 8] Internal RAM Error
        ExternalRAMError = 0x200,                     //[BIT 9] External RAM Error
        IllegalOpcodeReset = 0x400,                   //[BIT 10] Illegal Opcode Reset
        ClockMonitorReset = 0x800,                    //[BIT 11] Clock Monitor Reset
        ComputerOperatingProperlyReset = 0x1000,      //[BIT 12] Computer Operating Properly Reset
        UninitializedInterruptReset = 0x2000,         //[BIT 13] Uninitialized Interrupt Reset
        FirstAccelerAcquisitionIsInProgress = 0x4000, //[BIT 14] First Accelerometer acquisition is in progress
        AccelerometerIsInCalibrationMode = 0x8000     //[BIT 15] Accelerometer is in Calibration mode
    };

    enum class AvgType {
        Short = (byte)1,
        Medium = 2,
        Long = 3,
    };

    enum class AdcSignal {
        begin               = (byte)0,
        Vdc15               = (byte)0,
        Vdc5                = 1,
        Dv485               = 2,
        HighVoltageControl  = 3,
        TempSensor          = 4,
        TempSensorCalibr    = 5,
        Vdc25Reference      = 6,
        PowerSupplyAmperage = 7,
        end                 = 7,
    };

public:
    NodeE4();
    ~NodeE4();


    int         hv() const;
    QString     partnumber() const;
    //// void        gamma_accumulation_turnOn();
    //// void        gamma_accumulation_turnOff();
    void        clear();
    int         response(const Adam::Response &response);

    static QString ADC_name(int ext);
    static QString ADC_name_short(int ext);
    static QString frame_name(byte cmd);

    static const QString status_description(const byte ack, const Status status);
    static const QString status_tip(const byte ack, const Status status);

private:
    bool check_ack(const Adam::Response &response);

public:
    static Adam::Frame ReadGammaCPS(byte buckets = 1);                              // 0x01
    static Adam::Frame ReadTemperature();                                           // 0x05
    static Adam::Frame ReadAccelRaw(AvgType avgType);                               // 0x06
    static Adam::Frame ReadAccelInclination(AvgType avgType);                       // 0x11
    static Adam::Frame ReadGammaRayAcqusitionTime();                                // 0x13
    static Adam::Frame WriteGammaRayAcqusitionTime(ushort value);                   // 0x13
    static Adam::Frame ReadAccelCorrectedData(AvgType avgType);                     // 0x18
    static Adam::Frame WriteAccelPowerOn();                                         // 0x20
    static Adam::Frame WriteAccelPowerOff();                                        // 0x20
    static Adam::Frame WriteStartADCCalibration();                                  // 0x20
    static Adam::Frame SetHVLow();                                                  // 0x40
    static Adam::Frame SetHVHigh();                                                 // 0x41
    static Adam::Frame ReadCalibrationVoltages();                                   // 0x42
    static Adam::Frame WriteCalibrationVoltages(ushort low, ushort high, ushort test); // 0x42
    static Adam::Frame ReadOperationVoltage();                                      // 0x43
    static Adam::Frame WriteOperationVoltage(ushort value);                         // 0x43
    static Adam::Frame ReadHVCorr();                                                // 0x44
    static Adam::Frame ReadHVWarmUp();                                              // 0x46
    static Adam::Frame WriteHVWarmUp(byte value);                                   // 0x46
    static Adam::Frame ReadCtable();                                                // 0x50
    static Adam::Frame WriteCTable(const CTable *ctable);                           // 0x50
    static Adam::Frame ReadKtable();                                                // 0x0C
    static Adam::Frame WriteKTable(const KTable *ktable);                           // 0x0C
    static Adam::Frame ReadLocalGravity();                                          // 0x51
    static Adam::Frame WriteLocalGravity(float value);                              // 0x51
    static Adam::Frame ReadAdcSignal(AdcSignal signal);                             // 0x1e
    static Adam::Frame ReadConfigRegister();                                        // 0x1f
    static Adam::Frame WriteConfigRegister(byte value);                             // 0x1f
    static Adam::Frame ReadMasterStatusRegister();                                  // 0xf1
    static Adam::Frame WriteMasterStatusRegister(const std::vector<byte> &payload); // 0xf1
    static Adam::Frame ReadRecordTask(int ext);                                     // 0xf8
    static Adam::Frame WriteRecordTask(const std::vector<byte> &payload);           // 0xf8
    static Adam::Frame ReadPartNumber();                                            // 0xfc

private:
    int         response_0x01(const Adam::Response &response);
    int         response_0x05(const Adam::Response &response);
    int         response_0x06(const Adam::Response &response);
    int         response_0x0C(const Adam::Response &response);
    int         response_0x11(const Adam::Response &response);
    int         response_0x13(const Adam::Response &response);
    int         response_0x18(const Adam::Response &response);
    int         response_0x20(const Adam::Response &response);
    int         response_0x40(const Adam::Response &response);
    int         response_0x41(const Adam::Response &response);
    int         response_0x42(const Adam::Response &response);
    int         response_0x43(const Adam::Response &response);
    int         response_0x44(const Adam::Response &response);
    int         response_0x46(const Adam::Response &response);
    int         response_0x50(const Adam::Response &response);
    int         response_0x51(const Adam::Response &response);
    int         response_0x1e(const Adam::Response &response);
    int         response_0x1f(const Adam::Response &response);
    int         response_0xf1(const Adam::Response &response);
    int         response_0xf8(const Adam::Response &response);
    int         response_0xfc(const Adam::Response &response);

public:
    int         m_gamma_elapsed_time{};       // время выполнения запроса данных
    byte        m_gamma_ack{};
    ushort      m_gamma_status{};
    int         m_gamma_ext{};
    float       m_gamma_value{0};

    //// bool        m_gamma_accumulation;       // признак начала измерения отсчетов датчика
    //// double      m_gamma_sum;                // накопление гамма-отсчетов
    //// int         m_gamma_counter;            // количество измерений
    //// double      m_gamma_avrg;               // среднее значение накопленных гамма-отсчетов

    int         m_temperature_elapsed_time;
    byte        m_temperature_ack;
    ushort      m_temperature_status;
    int         m_temperature_num;
    float       m_temperature_value;

    int         m_accelerometer_raw_elapsed_time[3];
    byte        m_accelerometer_raw_ack[3];
    ushort      m_accelerometer_raw_status[3];
    float       m_accelerometer_raw_value[3];           // измеренные значения датчика

    int         m_accelerometer_incl_elapsed_time[3];
    byte        m_accelerometer_incl_ack[3];
    ushort      m_accelerometer_incl_status[3];
    float       m_accelerometer_incl_value[3];   // измеренные значения датчика

    int         m_accelerometer_corr_elapsed_time[3];
    byte        m_accelerometer_corr_ack[3];
    ushort      m_accelerometer_corr_status[3];
    float       m_accelerometer_corr_value[3];     // корректированные значения датчика

    int         m_gamma_acq_time_elapsed_time;
    byte        m_gamma_acq_ack;
    ushort      m_gamma_acq_status;
    ushort      m_gamma_acq_num;
    float       m_gamma_acq_time;           // Gamma Ray Acquisition Time

    int         m_dev_control_elapsed_time;
    byte        m_dev_control_ack;
    ushort      m_dev_control_status;
    int         m_dev_control_num;
    float       m_dev_control_value;

    int         m_hv_low_elapsed_time;
    byte        m_hv_low_ack;
    ushort      m_hv_low_status;

    int         m_hv_high_elapsed_time;
    byte        m_hv_high_ack;
    ushort      m_hv_high_status;

    int         m_calibr_hv_elapsed_time;
    byte        m_calibr_hv_ack;
    ushort      m_calibr_hv_status;
    int         m_calibr_hv_min{};          // калибровочное напряжение
    int         m_calibr_hv_max{};          // калибровочное напряжение
    int         m_calibr_hv_tst{};          // калибровочное напряжение
    int         m_operating_hv{};

    int         m_hv_elapsed_time;
    byte        m_hv_ack;
    ushort      m_hv_status;

    int         m_hv_corr_elapsed_time;
    byte        m_hv_corr_ack;
    ushort      m_hv_corr_status;
    int         m_hv_corr;

    int         m_warmup_elapsed_time;
    byte        m_warmup_ack;
    ushort      m_warmup_status;
    int         m_warmup_num;
    float       m_warmup_time;

    int         m_gravity_elapsed_time;
    byte        m_gravity_ack;
    ushort      m_gravity_status;
    float       m_gravity;

    int         m_adc_elapsed_time[8];
    byte        m_adc_ack[8];
    ushort      m_adc_status[8];
    float       m_voltage15;
    float       m_voltage5;
    float       m_dv485;
    float       m_high_voltage_control;
    float       m_temperature_sensor;
    float       m_temperature_circuit_calibration;
    float       m_vdc25_reference;
    float       m_power_amperage;

    int         m_config_register_elapsed_time;
    byte        m_config_register_ack;
    ushort      m_config_register_status;
    uint        m_config_register;

    int         m_statusreg_elapsed_time;
    byte        m_statusreg_ack;
    ushort      m_statusreg_status;
    uint32_t    m_statusreg;

    int         m_rectask_elapsed_time;
    byte        m_rectask_ack;
    ushort      m_rectask_status;
    int         m_rectask_num;
    ushort      m_rectask_initdelay;
    ushort      m_rectask_period;
    byte        m_rectask_records;
    byte        m_rectask_buckets;
    byte        m_rectask_logmode;

    QString     m_partnumber;
    int         m_partnumber_elapsed_time;
    byte        m_partnumber_ack;
    ushort      m_partnumber_status;

    int         m_ctable_elapsed_time;
    byte        m_ctable_ack;
    ushort      m_ctable_status;
    CTable      *m_ctable;                  // калибровочная таблица для акселерометра

    int         m_ktable_elapsed_time;
    byte        m_ktable_ack;
    ushort      m_ktable_status;
    KTable      *m_ktable;                  // калибровочная таблица для акселерометра
};


#endif // __NODE_E4_H__
