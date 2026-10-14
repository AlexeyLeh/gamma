/*!
  \file   diagn_gui.h
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Отображение информации в окне диагностики
*/

#ifndef __DIAGN_GUI_H_
#define __DIAGN_GUI_H_

#include <mutex>

#include <QtWidgets>

#include "main.h"
#include "lineedit.h"
#include "response.h"

class COMM;
class MW;
class DIAGN;
class TERMINAL;

class DIAGN_GUI : public QObject
{
    Q_OBJECT;

    static constexpr const char *start_diagn_scan_button = QT_TRANSLATE_NOOP("DIAGN", "  Start scanning  ");
    static constexpr const char *stop_diagn_scan_button  = QT_TRANSLATE_NOOP("DIAGN", "  Stop scanning  ");

public:
    DIAGN_GUI(APP *app);
    ~DIAGN_GUI();

    inline APP     *app()  const { return m_app; }
    inline MW      *mw()   const { return m_app->mw(); }
    inline COMM    *comm() const { return m_app->comm(); }
    inline QWidget *page() const { return m_gui_page; }

    TERMINAL    *terminal() const;
    void        set_backgr(DIAGN *diagn);
    void        set_content_font_size(int sz);
    void        showstatus(const QString& msg, int sec = 5);
    void        requests_completed(int nodecode);

    void        display_info_message(const QString& msg);
    void        display_error_message(const QString& msg);
    void        display_error_message_sec(const QString& msg, int sec = 10);

    void        set_poll_time(int sec);
    int         E4_CPS_avrg();
    void        set_E4_CPS_avrg(int counts);
    void        set_enabled_button_set_lo_hv(bool status);
    void        set_enabled_button_set_hi_hv(bool status);

    bool        start_diagnostic();
    void        stop_diagnostic();

    void        set_led_icon_off();
    void        set_led_icon_active();
    void        set_led_icon_idle();
    void        set_led_icon_alarm();

public:
    void        update_fields(byte cmd, byte ext);

private:
    void        update_field_0x01();
    void        update_field_0x05();
    void        update_field_0x06(byte ext);
    void        update_field_0x11(byte ext);
    void        update_field_0x13();
    void        update_field_0x18(byte ext);
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

private:
    QWidget     *create_diagnostic_page();
    void        clear_terminal_fields(int mode);
    void        write_term_protocol_record(const QString &str, int width = 0);
    bool        open_term_protocol();
    void        close_term_protocol();
    void        set_diagnostic_fields_checked(int state);

public slots:
    void        slot_set_term_log_enabled(bool status);

private slots:
    void        slot_run_diagnostic();
    void        slot_set_poll_time(int num);
    void        slot_select_terminal_log_path();
    void        slot_clear_terminal();
    void        slot_set_gamma_counts_avrg(int counts);

    //// void        slot_edit_operating_voltage();
    //// void        slot_edit_calibration_voltages();
    //// void        slot_edit_acq_time();
    void        slot_set_hv_low_level();
    void        slot_set_hv_high_level();

private:
public:
    // экран диагностики гамма-модуля
    LineEdit8   *m_fld_diag_gamma_cmd;          // поле для отображения номера команды
    QLineEdit   *m_fld_diag_gamma_cnts;         // поле для отображения значения
    QCheckBox   *m_fld_diag_gamma_log;          // отметка для протоколирования
    QLabel      *m_fld_diag_gamma_ms;           // поле для отображения времени чтения
    QSpinBox    *m_fld_diag_gamma_avrg_cnts;    // количество замеров для усреднения

    LineEdit8   *m_fld_diag_Temperature_cmd;    // поле для отображения номера команды
    QLineEdit   *m_fld_diag_Temperature;        // поле для отображения значения
    QCheckBox   *m_fld_diag_Temperature_log;    // отметка для протоколирования
    QLabel      *m_fld_diag_Temperature_ms;     // поле для отображения времени чтения

    LineEdit8   *m_fld_diag_accel_raw_cmd;      // поле для отображения номера команды
    QLineEdit   *m_fld_diag_accel_raw;          // поле для отображения значения
    QCheckBox   *m_fld_diag_accel_raw_log;      // отметка для протоколирования
    QLabel      *m_fld_diag_accel_raw_ms;       // поле для отображения времени чтения
    QSpinBox    *m_fld_diag_accel_raw_avrg;     // количество замеров для усреднения

    QLineEdit   *m_fld_diag_accel_incl;         // поле для отображения значения
    QCheckBox   *m_fld_diag_accel_incl_log;     // отметка для протоколирования
    QLabel      *m_fld_diag_accel_incl_ms;      // поле для отображения времени чтения
    LineEdit8   *m_fld_diag_accel_incl_cmd;     // поле для отображения номера команды
    QSpinBox    *m_fld_diag_accel_incl_avrg;    // количество замеров для усреднения

    QLineEdit   *m_fld_diag_accel_corr;         // поле для отображения значения
    QCheckBox   *m_fld_diag_accel_corr_log;     // отметка для протоколирования
    QLabel      *m_fld_diag_accel_corr_ms;      // поле для отображения времени чтения
    LineEdit8   *m_fld_diag_accel_corr_cmd;     // поле для отображения номера команды
    QSpinBox    *m_fld_diag_accel_corr_avrg;    // количество замеров для усреднения

    QLineEdit   *m_fld_diag_gamma_AcqTime;      // поле для отображения значения
    QCheckBox   *m_fld_diag_gamma_AcqTime_log;  // отметка для протоколирования
    QLabel      *m_fld_diag_gamma_AcqTime_ms;   // поле для отображения времени чтения
    LineEdit8   *m_fld_diag_gamma_AcqTime_cmd;  // поле для отображения номера команды

    QLineEdit   *m_fld_diag_HV_max;             // поле для отображения значения
    QCheckBox   *m_fld_diag_HV_max_log;         // отметка для протоколирования
    QLabel      *m_fld_diag_HV_max_ms;          // поле для отображения времени чтения
    LineEdit8   *m_fld_diag_HV_max_cmd;         // поле для отображения номера команды

    QLineEdit   *m_fld_diag_HV;                 // поле для отображения значения
    QCheckBox   *m_fld_diag_HV_log;             // отметка для протоколирования
    QLabel      *m_fld_diag_HV_ms;              // поле для отображения времени чтения
    LineEdit8   *m_fld_diag_HV_cmd;             // поле для отображения номера команды

    QLineEdit   *m_fld_diag_HV_corr;            // поле для отображения значения
    QCheckBox   *m_fld_diag_HV_corr_log;        // отметка для протоколирования
    QLabel      *m_fld_diag_HV_corr_ms;         // поле для отображения времени чтения
    LineEdit8   *m_fld_diag_HV_corr_cmd;        // поле для отображения номера команды

    QLineEdit   *m_fld_diag_HV_warm;            // поле для отображения значения
    QCheckBox   *m_fld_diag_HV_warm_log;        // отметка для протоколирования
    QLabel      *m_fld_diag_HV_warm_ms;         // поле для отображения времени чтения
    LineEdit8   *m_fld_diag_HV_warm_cmd;        // поле для отображения номера команды

    QLineEdit   *m_fld_diag_ADC[8];             // поле для отображения значения
    QCheckBox   *m_fld_diag_ADC_log[8];         // отметка для протоколирования
    QLabel      *m_fld_diag_ADC_ms[8];          // поле для отображения времени чтения
    LineEdit8   *m_fld_diag_ADC_cmd[8];         // поле для отображения номера команды

    QLineEdit   *m_fld_diag_config_reg;         // поле для отображения значения
    QCheckBox   *m_fld_diag_config_reg_log;     // отметка для протоколирования
    QLabel      *m_fld_diag_config_reg_ms;      // поле для отображения времени чтения
    LineEdit8   *m_fld_diag_config_reg_cmd;     // поле для отображения номера команды

    QLineEdit   *m_fld_diag_status;             // поле для отображения значения
    QCheckBox   *m_fld_diag_status_log;         // отметка для протоколирования
    QLabel      *m_fld_diag_status_ms;          // поле для отображения времени чтения
    LineEdit8   *m_fld_diag_status_cmd;         // поле для отображения номера команды

    QLineEdit   *m_fld_diag_rectask;            // поле для отображения значения
    QCheckBox   *m_fld_diag_rectask_log;        // отметка для протоколирования
    QLabel      *m_fld_diag_rectask_ms;         // поле для отображения времени чтения
    LineEdit8   *m_fld_diag_rectask_cmd;        // поле для отображения номера команды

    QLineEdit   *m_fld_diag_Partnumber;         // поле для отображения значения
    QCheckBox   *m_fld_diag_Partnumber_log;     // отметка для протоколирования
    QLabel      *m_fld_diag_Partnumber_ms;      // поле для отображения времени чтения
    LineEdit8   *m_fld_diag_Partnumber_cmd;     // поле для отображения номера команды

    QPushButton *m_diagn_led_icon;              // индикация активности устройства
    QCheckBox   *m_diagnostic_select_all;       // выбрать все поля для отображения
    QPushButton *m_button_diagn_start;          // кнопка запуска опроса устройства

    QLineEdit   *m_fld_diag_term_fname_log;     // поле выбора имени отчета терминала
    QPushButton *m_term_select_log_button;      // кнопка выбора файла отчета терминала
    QCheckBox   *m_chkbox_term_log_enabled;     // разрешение протоколирования терминала
    QPushButton *m_term_clear_button;           // очистить терминал и поля отображения

    QSpinBox    *m_fld_diag_poll_time;          // период опроса устройства
    QPushButton *m_fld_diag_HV_set_lo;          // кнопка для проверки нижней границы ЦАП
    QPushButton *m_fld_diag_HV_set_hi;          // кнопка для проверки верхней границы ЦАП

private:
    APP        *m_app;                        // главный объект
    DIAGN       *m_diagn;
    TERMINAL    *m_terminal;
    QWidget     *m_gui_page;

    QIcon        m_ico_gray;                    // серая иконка на кнопке
    QIcon        m_ico_red;                     // красная иконка на кнопке
    QIcon        m_ico_green;                   // зеленая иконка на кнопке
    QIcon        m_ico_yellow;                  // желтая иконка на кнопке

    QFile        m_term_log_fd;                 // дескриптор файла

    // settings from .ini
public:
    void                read_settings(QSettings *st);
    void                save_settings(QSettings *st);
};

#endif // __DIAGN_GUI_H_
