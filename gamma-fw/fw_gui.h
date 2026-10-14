/*!
  \file   fw_gui.h
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Отображение информации во вкладке FIRMWARE
*/

#ifndef __FW_GUI_H_
#define __FW_GUI_H_

#include <QtWidgets>

#include "main.h"
#include "response.h"

class COMM;
class MW;
class FIRMWARE;
class CTable;
class KTable;

class FIRMWARE_GUI : public QObject
{
    Q_OBJECT;

    static      QString secs_to_minute(int sec);
    static      QString str_center(const QString& str, int width);
    static      QString str_left(const QString& str, int width);
    static      QString str_right(const QString& str, int width);

public:
    FIRMWARE_GUI(APP *app);
    ~FIRMWARE_GUI();

    inline APP     *app()  const { return m_app; }
    inline MW      *mw()   const { return m_app->mw(); }
    inline COMM    *comm() const { return m_app->comm(); }
    inline QWidget *page() const { return m_gui_page; }

    void        set_backgr(FIRMWARE *fw);
    void        set_content_font_size(int sz);
    void        showstatus(const QString &msg, int sec = 5) const;
    void        clear_fw_fields();
    bool        wish_write_data() const;
    bool        action_confirmation() const;
    void        display_error_message(const QString &msg);
    void        refresh_ctable();
    void        refresh_ktable();

    void        slot_start_fw_read();
    void        firmware_read_finished();
    void        requests_completed(int nodecode);

    void        set_ACK_field_cmd(const QString &text, const QString &tip = {});
    void        set_ACK_field_code(const QString &text, const QString &tip = {});
    void        set_ACK_field_status(const QString &text, const QString &tip = {});
    void        set_ACK_field_text(const QString &text, const QString &tip = {});

public:
    void        update_fields(byte cmd, byte ext);

private:
    void        update_field_0x01();
    void        update_field_0x05();
    void        update_field_0x06();
    void        update_field_0x0C();
    void        update_field_0x11();
    void        update_field_0x13();
    void        update_field_0x18();
    void        update_field_0x20();
    void        update_field_0x40();
    void        update_field_0x41();
    void        update_field_0x42();
    void        update_field_0x43();
    void        update_field_0x44();
    void        update_field_0x46();
    void        update_field_0x50();
    void        update_field_0x51();
    void        update_field_0x1e(byte ext);
    void        update_field_0x1f();
    void        update_field_0xf1();
    void        update_field_0xf8();
    void        update_field_0xfc();
    bool        check_field_ack(byte code, byte ack, ushort status, QLineEdit *field);
    bool        check_field_ack(byte code, byte ack, ushort status, QAbstractSpinBox *field);

    QWidget     *create_fw_page();
    QVBoxLayout *create_survey_and_adc_column();
    QVBoxLayout *create_ctable_column();
    QVBoxLayout *create_ktable_column();
    QVBoxLayout *create_settings_and_hvps_column();
    QWidget     *create_ack_status_panel();
    QHBoxLayout *create_bottom_read_layout();
    void        fw_new_hv();

private:
    APP        *m_app;                        // главный объект
    FIRMWARE    *m_fw;

private:
public:
    QPushButton     *m_button_fw_read;        // кнопка однократного запроса данных с устройства

private:
    // экран настройки/прошивки гамма-модуля
    QLineEdit      *m_fld_fw_partnumber;            // код устройства
    QLineEdit      *m_fld_fw_accel_incl;            // направление акселерометра
    QLineEdit      *m_fld_fw_accel_raw;             // исходное значение акселерометра
    QLineEdit      *m_fld_fw_accel_corr;            // значение акселерометра после корректировки
    QLineEdit      *m_fld_fw_CPS;                   // измерянное значение гамма - отсчетов
    QLineEdit      *m_fld_fw_Temp;                  // температура устройства
    QLineEdit      *m_fld_fw_ADC[8];                // АЦП процессора
    QDoubleSpinBox  *m_fld_fw_grav_field;            // значение гравитации местности
    QComboBox       *m_fld_fw_grav_list;             // список местностей
    QSpinBox        *m_fld_fw_warmuptime;            // время стабилизации ВН
    //LineEdit      *m_fld_fw_cfgreg;                // регистр конфигурации
    QSpinBox        *m_fld_fw_acqtime;               // время считывания значений
    QLineEdit       *m_fld_fw_hv_opr;                // рабочее текущее напряжение
    QLineEdit       *m_fld_fw_hv_min;                // калибровочное напряжение
    QLineEdit       *m_fld_fw_hv_max;                // калибровочное напряжение
    QLineEdit       *m_fld_fw_hv_tst;                // калибровочное напряжение
    ////LineEdit20      *m_fld_fw_statusreg;             // регистр состояний
    QLineEdit       *m_fld_fw_ACK_status;
    QLineEdit       *m_fld_fw_ACK_cmd;
    QLineEdit       *m_fld_fw_ACK_code;
    QLineEdit       *m_fld_fw_ACK_text;

    QPushButton     *m_fw_but_set_grav;          // установить значание гравитации
    QPushButton     *m_fw_but_set_partnumber;    // установить код
    QPushButton     *m_fw_but_set_warmuptime;    // установить время стабилизации ВН
    //QPushButton     *m_fw_but_set_cfgreg;        // установить значение регистра
    QPushButton     *m_fw_but_set_acqtime;       // установить время считывания значений
    ////QPushButton     *m_fw_but_set_accel_pwron;   // включить питание аккселерометра
    ////QPushButton     *m_fw_but_set_accel_pwroff;  // выключить питание аккселерометра
    ////QPushButton     *m_fw_but_set_ADC_calibration;      // Начать калибровку АЦП
    ////QPushButton     *m_fw_but_set_hv_test;       // установка калибровочного напряжения
    ////QPushButton     *m_fw_but_set_hv_min;        // установка калибровочного напряжения
    ////QPushButton     *m_fw_but_set_hv_max;        // установка калибровочного напряжения

    ////QPushButton     *m_button_fw_read_statusreg;     // считать регистр состояний
    ////QPushButton     *m_button_fw_write_status_on;    // задать состояние в регистр
    ////QPushButton     *m_button_fw_write_status_off;   // задать состояние в регистр
    ////class LineEdit16 : public LineEdit16
    ////{
    ////    Q_OBJECT;
    ////public:
    ////    LineEdit16(QWidget *parent = nullptr);
    ////    LineEdit16(const QString& text, QWidget *parent = nullptr);
    ////    LineEdit16(const QString& text, Qt::Alignment align, QWidget *parent = nullptr);
    ////    QSize sizeHint() const;
    ////};

public:
    QPushButton     *m_fw_but_set_hvps;          // установка калибровочных напряжений
    QPushButton     *m_fw_but_set_hvps_oper;       // установка калибровочного напряжения

public:
    // калибровочная таблица для акселерометра Gz
    QLineEdit   *m_ctable_accel_CRC;
    QLineEdit   *m_ctable_accel_pn;
    QLineEdit   *m_ctable_accel_scale0;
    QLineEdit   *m_ctable_accel_scale1;
    QLineEdit   *m_ctable_accel_scale2;
    QLineEdit   *m_ctable_accel_scale3;
    QLineEdit   *m_ctable_accel_bias0;
    QLineEdit   *m_ctable_accel_bias1;
    QLineEdit   *m_ctable_accel_bias2;
    QLineEdit   *m_ctable_accel_bias3;
    QLineEdit   *m_ctable_accel_SN;
    QLineEdit   *m_ctable_accel_Date;
    QLineEdit   *m_ctable_accel_Time;
    QLineEdit   *m_ctable_error;                   // field for errors about CTable
    QPushButton *m_fw_but_set_ctable;              // записать новую таблицу поправок аккселероментра

public:
    // KTable
    QLineEdit   *m_fld_ktable_CRC;                     //
    QLineEdit   *m_fld_ktable_PN;                      //
    QLineEdit   *m_fld_Short_bucket_size;              //
    QLineEdit   *m_fld_Medium_bucket_size;             //
    QLineEdit   *m_fld_Long_bucket_size;               //
    QLineEdit   *m_fld_Calibration_time;               //
    QLineEdit   *m_fld_Acquire_frequency;              //
    QLineEdit   *m_fld_ktable_error;                   // field for errors about KTable
    QPushButton *m_fw_but_set_ktable;              // записать новую таблицу KTable

private slots:
    void slot_cktable_edit_finished();
    void slot_select_current_gravity(int index);
    void slot_edit_grav_field_finished();
    void slot_fw_write_gravity();
    void slot_fw_write_warmup_time();
    void slot_fw_write_acqtime();
    void slot_fw_accel_poweron();
    void slot_fw_accel_poweroff();
    void slot_fw_hv_oper();
    void slot_fw_set_hvps();

private:
    QWidget      *m_gui_page;               // окно настроек FIRMWARE

    //// private:
public:
    QIcon         m_ico_gray;               // серая иконка на кнопке
    QIcon         m_ico_red;                // красная иконка на кнопке
    QIcon         m_ico_green;              // зеленая иконка на кнопке
    QIcon         m_ico_yellow;             // желтая иконка на кнопке

    QPushButton  *m_fld_check_HV_set_lo;    // кнопка для проверки нижней границы ЦАП
    QPushButton  *m_fld_check_HV_set_hi;    // кнопка для проверки верхней границы ЦАП
};


#endif // __FW_GUI_H_
