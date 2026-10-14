/*!
  \file   api_gui.h
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Отображение информации во вкладке API
*/

#ifndef __API_GUI_H_
#define __API_GUI_H_

#include <QComboBox>
#include <QFile>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QProgressBar>
#include <QSettings>
#include <QRadioButton>

#include "api.h"
#include "lineedit.h"
#include "response.h"

class COMM;
class APP;
class MW;
class TABLE;

class API_GUI : public QObject
{
    Q_OBJECT;

public:
    API_GUI(APP *m);
    ~API_GUI();

    inline APP     *app()  const { return m_app; }
    inline MW      *mw()   const { return m_app->mw(); }
    inline COMM    *comm() const { return m_app->comm(); }
    inline NodeE4  *nodeE4() const { return m_app->nodeE4(); }
    inline QWidget *page() const { return m_gui_page; }

    void            set_backgr(API *api);
    void            setup_initial_values();
    void            setup_titles(const API::api_data_t &data);
    bool            simulation();
    void            set_content_font_size(int sz);
    void            showstatus(const QString &msg, int sec = 5) const;

    TABLE           *table1() const;
    TABLE           *table2() const;
    void            clear_fields();
    QString         version_short() const;

    void            display_info_message(const QString &title, const QString &msg, const QString &filename = {});
    void            display_error_message(const QString &title, const QString &msg, const QString &status_msg = {});

    bool            is_selected_old_table() const;
    bool            is_selected_new_table() const;
    void            update_field__calibration_factor(bool ok);
    void            update_field__standard_deviation(bool ok);

    const           QString part_number();
    void            update_part_number(const QString &pn);
    const           QString serial_number();
    void            update_serial_number(const QString &sn);
    const           QString ESKD();
    void            update_ESKD(const QString &eskd);
    void            update_operator(const QString &oper);

public:
    bool            start_api_test();
    void            stop_api_test();
    void            set_init_label();
    void            set_run_label();
    void            set_stop_label();
    void            set_err_label();
    void            set_pass_label();
    void            set_failed_label();
    void            set_passed();
    bool            message_about_selected_api_source(API::api_mode_t api_mode) const;
    void            requests_completed(int nodecode);
    void            finish_message(bool ok = true);

    void            set_led_icon_off();
    void            set_led_icon_active();
    void            set_led_icon_alarm();
    void            set_led_icon_idle();

    QProgressBar    *progress_bar_gamma();
    void            progress_bar_gamma_close();
    QProgressBar    *progress_bar_samples();
    void            progress_bar_samples_close();

////signals:
////    void            signal__message(QMessageBox::Icon icon, const QString &msg);

public:
    void            update_fields(byte cmd, byte ext = 0);
    void            update_field_acquire_time(int acq_time, int samples_for_avrg, int m_total_one_acq_time);
    void            update_field_steps__done(int sample, int index, int total);
    void            update_field_steps__reset(int total);
    void            update_field_estimated_time(int remain_sec, int total_sec);
    void            update_field_estimated_time2(const QString &cdate);
    void            update_field_CPS(const QString &text);
    void            update_field_test_mode(const QString &text);

private:
    void            update_field_0x01();
    void            update_field_0x05();
    void            update_field_0x13();
    void            update_field_0x43();
    void            update_field_0x46();
    void            update_field_0xfc();

private:
    QWidget         *create_api_page();
    QWidget         *create_plato_page();
    QWidget         *create_box_pass();
    QWidget         *create_box_real_data();
    QWidget         *create_box_result_table();
    QWidget         *create_box_CPS_table();
    QWidget         *create_box_interpolation_table();
    QWidget         *create_buttons();
    TABLE           *create_interpolation_table();
    void            update_box_interpolation_table();
    TABLE           *create_CPS_table();

public slots:
    void            slot_save_report();

private slots:
    void            slot_run_api_test();

private:
    APP            *m_app{};                    // главный объект
    QWidget         *m_gui_page{};              // окно теста API
    API             *m_api{};                   // рабочие процессы теста API

private:
    // экран API-теста
    QLabel          *m_fld_api_pass_msg{};      // сообщение о результате тестирования
    QLineEdit       *m_fld_est_time{};          // ожидаемое время завершения теста
    QLineEdit       *m_fld_acq_time{};          // время измерения отсчетов в процессоре

    QLabel          *m_fld_samples_msg{};       // текстовка для количества выполняемых замеров
    QProgressBar    *m_samples_progress_bar{};  // индикатор количества выполненных замеров
    QProgressBar    *m_gamma_progress_bar{};    // индикатор выполнения всего этапов теста

    QLineEdit       *m_fld_api_HV{};            // текущее "высокое" напряжение на датчике
    QLineEdit       *m_fld_api_CPS{};           // измерянное значение гамма - отсчетов
    QLineEdit       *m_fld_api_Temp{};          // температура устройства
    QLineEdit       *m_fld_api_hot_mode{};      // температурный режим тестирования
    QLineEdit       *m_fld_PN{};                // модель инструмента
    QLineEdit       *m_fld_SN{};                // серийный номер инструмента
    QLineEdit       *m_fld_oper{};              // оператор
    QLineEdit       *m_fld_api_mode{};          // режим API-теста

    QPushButton     *m_api_led_icon{};          // индикация активности устройства
    QPushButton     *m_button_api_run{};        // кнопка запуска опроса устройства
    QPushButton     *m_button_save_report{};    // сохранить отчет

    // results
    QLabel          *m_api_cal_factor_label[2]{}; // заголовок к полю "коэффициенты калибровки old/new"
    QLineEdit       *m_fld_api_cal_factor[4]{};   // коэффициенты калибровки old/new
    QLineEdit       *m_fld_api_std_deviat0[4]{};  // квадратичное отклонение на 0 API old/new
    QLineEdit       *m_fld_api_std_deviat200[4]{};// квадратичное отклонение на 200 API old/new
    TABLE           *m_table1{};                  // таблица с замерами CPS
    TABLE           *m_table2{};                  // таблица интерполяции данных по толщине трубы
    QRadioButton    *m_fld_table_values_old{};
    QRadioButton    *m_fld_table_values_new{};

    QIcon           m_ico_gray{};               // серая иконка на кнопке
    QIcon           m_ico_red{};                // красная иконка на кнопке
    QIcon           m_ico_green{};              // зеленая иконка на кнопке
    QIcon           m_ico_yellow{};             // желтая иконка на кнопке


    // settings from .ini
public:
    void            read_settings(QSettings *st);
    void            save_settings(QSettings *st);
};

#endif // __API_GUI_H_
