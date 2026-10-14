/*!
  \file   plato_gui.h
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Отображение информации во вкладке PLATO
*/

#ifndef __PLATO_GUI_H_
#define __PLATO_GUI_H_

#include <QtWidgets>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
QT_CHARTS_USE_NAMESPACE

#include "lineedit.h"
#include "plato.h"
#include "plato_data.h"
#include "response.h"
#include "table.h"

class COMM;
class APP;
class MW;
class PLATO;

class PLATO_GUI : public QObject
{
    Q_OBJECT;

public:
    static constexpr const char *start_scan_button  = QT_TRANSLATE_NOOP("PLATO", "  Start scanning  ");
    static constexpr const char *stop_scan_button   = QT_TRANSLATE_NOOP("PLATO", "  Stop scanning  ");
    static constexpr const char *simulation_ext = "gsim";

    static void  setStyleSheet(QWidget *wgt, const QString &style);
    static void  setStyleSheet(QWidget &wgt, const QString &style);

public:
    PLATO_GUI(APP *m);
    virtual ~PLATO_GUI();

    inline APP     *app()  const { return m_app; }
    inline MW      *mw()   const { return m_app->mw(); }
    inline COMM    *comm() const { return m_app->comm(); }
    inline QWidget *page() const { return m_gui_page; }

    void          set_backgr(PLATO *plato);
    void          setup_initial_values(int mode = 0);
    void          set_content_font_size(int sz);
    void          showstatus(const QString &msg = "", int sec = 5) const;

    void          display_info_message(QMessageBox::Icon icon, const QString &msg, const QString &dirname = {});
    void          display_error_message(const QString &msg, int sec = 60);

    const         QString part_number() const;
    void          update_part_number(const QString &pn);
    const         QString serial_number() const;
    void          update_serial_number(const QString &sn);
    void          update_operator(const QString &oper);

    void          show_temperaure_mode(int temperature, const QString &text);
    void          check_chart_temperature_range(PLATO::tmode_t tmode, int temper);
    TABLE         *table();
    void          table_set_record(int nrow, const QStringList &data);
    void          clear();
    void          clear_fields();
    void          clear_plato_table();
    void          clear_plato_table(PLATO::tmode_t tmode);
    void          clear_plato_chart(PLATO::tmode_t tmode);

    auto          axis_x();
    auto          axis_y();
    auto          axis_y2();
    int           axis_x_min();
    int           axis_x_max();
    int           axis_x_ticks();
    int           axis_y_min();
    int           axis_y_max();
    int           axis_y_ticks();
    int           axis_y2_min();
    int           axis_y2_max();
    int           axis_y2_ticks();
    void          set_chart_x_range(int x_min, int x_max);
    void          set_chart_y_range(int y_min, int y_max);
    void          set_chart_y2_range(int y_min, int y_max);
    void          set_chart_y_min(int y_min);

public:
    bool          start_plato_test();
    void          stop_plato_test();
    void          set_empty_label(const QString &text = {});
    void          set_init_label();
    void          set_run_label();
    void          set_stop_label(const QString &text = {}, const QString &tip = {});
    void          set_err_label();
    void          set_pass_label();
    void          set_failed_label(const QString &tip = {});
    void          set_passed();
    void          requests_completed(int nodecode);
    void          finishing_test(bool ok = true);
    bool          ask_about_incorrect_test_voltage(const QString &title, const QString &msg);

public:
    void          set_led_icon_off();
    void          set_led_icon_active();
    void          set_led_icon_alarm();
    void          set_led_icon_idle();

    QProgressBar  *progress_bar();

public:
    inline auto *button_plato_run()
    {
        return m_button_plato_run;
    }

public:
    void          update_fields(byte cmd, byte ext = {});
    void          update_field__acquire_time(int acq_time);
    void          update_field__acquire_time_details(int warmup_time, int gamma_acq_bucket, int acq_one_gamma, int samples_for_avrg);
    void          update_field__remain_time(int remain_sec, int total_sec);
    void          update_field__estimated_time(const QString &cdate);
    void          update_field__steps_done(int step, int total);

private:
    void          update_field_0x01();
    void          update_field_0x05();
    void          update_field_0x13();
    void          update_field_0x42();
    void          update_field_0x43();
    void          update_field_0x46();
    void          update_field_0xfc();

private:
    QWidget       *create_plato_page();
    QWidget       *create_box1();
    QWidget       *create_box2();
    QWidget       *create_box3_chart();
    QWidget       *create_box4_table();
    QWidget       *create_buttons();
    QGroupBox     *create_tolerance();
    void          create_table();
    void          create_chart();

// protected:
//     bool eventFilter(QObject *obj, QEvent *event) override;

public slots:
    void          slot_save_report();
    void          slot_load_report();
    void          slot_load_last_test();
    void          simulation_save();
    bool simulation_load();

private slots:
    void          slot_run_plato_test();
    void          slot_write_hpvs(bool);

private:
    APP             *m_app;                         // главный объект
    QWidget         *m_gui_page;                    // окно тестирования линейности модуля
    PLATO           *m_plato;                       // класс тестирования модуля/расчетов
    TABLE           *m_table;                       // таблица замеров

    // TODO - сделать переменные private:
public:
    // graph
    QChart          *m_chart;                       // график
    QChartView      *m_chartview;                   // график
    QValueAxis      *m_axisX;                       // ось X
    QValueAxis      *m_axisY;                       // ось Y
    QValueAxis      *m_axisY2;                      // ось Y
    QLineSeries     *m_series_cold_CPS;             // 'холодные' измерения гамма-отсчетов
    QLineSeries     *m_series_hot_CPS;              // 'гоярчие' измерения гамма-отсчетов
    QLineSeries     *m_series_cold_temper;          // линия 'холодной' температуры
    QLineSeries     *m_series_hot_temper;           // линия 'горячей' температуры

    QLineSeries     *m_series_HV_lo_limit;          // граница минимального напряжение, отрисованная на графике
    QLineSeries     *m_series_HV_hi_limit;          // граница максимального напряжение, отрисованная на графике
    QLineSeries     *m_series_CPS_lo_limit;         // граница минимального числа отсчетов, отрисованных на графике
    QLineSeries     *m_series_CPS_hi_limit;         // границамаксимального числа отсчетов, отрисованных на графике

    QLineSeries     *m_series_cold_HV_lo_border;    // минимальное напряжение линейности в допуске, на графике
    QLineSeries     *m_series_cold_HV_hi_border;    // максимальное напряжение линейности в допуске, на графике
    QLineSeries     *m_series_hot_HV_lo_border;     // минимальное напряжение линейности в допуске, на графике
    QLineSeries     *m_series_hot_HV_hi_border;     // максимальное напряжение линейности в допуске, на графике
    QLineSeries     *m_series_HV_accepted;          // итоговое калибровочное напряжение, на графике

    // TODO - сделать переменные private:
public:
    QLabel          *m_fld_plato_pass_msg{};        // сообщение о результате тестирования
    QPushButton     *m_button_write_HV{};           // записать расчетное напряжение в гамма-модуль
    QPushButton     *m_button_plato_run{};          // запуск теста
    QPushButton     *m_button_load_simulation{};    // запуск симуляции
    QPushButton     *m_button_save_report{};        // сохранить отчет
    QPushButton     *m_button_load_report{};        // загрузить отчет
    QPushButton     *m_plato_led_icon{};            // индикация активности устройства

    QLabel          *m_fld_samples_msg{};           // текстовка для количества выполняемых замеров
    QProgressBar    *m_samples_progress_bar{};      // индикатор количества выполненных замеров
    QProgressBar    *m_progress_bar{};              // индикатор выполнения этапов теста

    // QLabel       *m_fld_plato_est_time{};        // ожидаемое время завершения теста
    // QLabel       *m_fld_hv_step{};               // шаг увеличения высоковольтного напряжения
    // QLabel       *m_fld_acq_time{};              // время измерения отсчетов в процессоре
    // QLabel       *m_fld_hv_accepted{};           // напряжение калибровки по результатам тестирования
    QLineEdit       *m_fld_est_time{};              // ожидаемое время завершения теста
    QLineEdit       *m_fld_hv_step{};               // шаг увеличения высоковольтного напряжения
    QLineEdit       *m_fld_acq_time{};              // время измерения отсчетов в процессоре
    QLineEdit       *m_fld_hv_accepted{};           // напряжение калибровки по результатам тестирования

    // QLabel       *m_fld_ESKD{};                  // ЕСКД номер изделия
    // QLabel       *m_fld_SN{};                    // серийный номер изделия
    // QLabel       *m_fld_PN{};                    // прошивка изделия
    QLineEdit       *m_fld_SN{};                    // серийный номер изделия
    QLineEdit       *m_fld_PN{};                    // прошивка изделия
    QLineEdit       *m_fld_oper{};                  // оператор темтирования
    ////QCheckBox   *m_fld_hv_restore_org{}{};      // признак восстановления уставки напряжения

    QLineEdit       *m_fld_HV{};                    // текущее "высокое" напряжение на датчике
    QLineEdit       *m_fld_CPS{};                   // текущее значение отсчетов датчика
    QLineEdit       *m_fld_Temper{};                // текущая температура теста
    QLineEdit       *m_fld_temper_mode{};           // температурный режим тестирования

    LineEdit12      *m_fld_tol_HV_LoLimit{};        // нижний допуск по напряжению
    LineEdit12      *m_fld_tol_HV_HiLimit{};        // верхний допуск по напряжению
    LineEdit12      *m_fld_tol_HV_Spread{};         // допуск по линейности
    LineEdit12      *m_fld_tol_CPS_LoLimit{};       // нижний допуск по отсчетам
    LineEdit12      *m_fld_tol_CPS_HiLimit{};       // верхний допуск по отсчетам
    LineEdit12      *m_fld_tol_CPS_Spread{};        // допуск линейности по отсчетам

    LineEdit12      *m_fld_cold_HV_LoBorder{};      // нижний допуск по напряжению   (при нормальной температуре)
    LineEdit12      *m_fld_cold_HV_HiBorder{};      // верхний допуск по напряжению  (при нормальной температуре)
    LineEdit12      *m_fld_cold_HV_Spread{};        // допуск по линейности          (при нормальной температуре)
    LineEdit12      *m_fld_cold_CPS_LoBorder{};     // нижний допуск по отсчетам     (при нормальной температуре)
    LineEdit12      *m_fld_cold_CPS_HiBorder{};     // верхний допуск по отсчетам    (при нормальной температуре)
    LineEdit12      *m_fld_cold_CPS_Spread{};       // допуск линейности по отсчетам (при нормальной температуре)

    LineEdit12      *m_fld_hot_HV_LoBorder{};       // нижний допуск по напряжению   (при высокой температуре)
    LineEdit12      *m_fld_hot_HV_HiBorder{};       // верхний допуск по напряжению  (при высокой температуре)
    LineEdit12      *m_fld_hot_HV_Spread{};         // допуск по линейности          (при высокой температуре)
    LineEdit12      *m_fld_hot_CPS_LoBorder{};      // нижний допуск по отсчетам     (при высокой температуре)
    LineEdit12      *m_fld_hot_CPS_HiBorder{};      // верхний допуск по отсчетам    (при высокой температуре)
    LineEdit12      *m_fld_hot_CPS_Spread{};        // допуск линейности по отсчетам (при высокой температуре)

    LineEdit12      *m_fld_HV_Accepted_Lo{};        // расчетная граница по напряжению
    LineEdit12      *m_fld_HV_Accepted_Hi{};        // расчетная граница по напряжению
    LineEdit12      *m_fld_HV_Accepted_Spread{};    //
    LineEdit12      *m_fld_CPS_Accepted_Spread{};   //

    QIcon           m_ico_gray;                     // серая иконка на кнопке
    QIcon           m_ico_red;                      // красная иконка на кнопке
    QIcon           m_ico_green;                    // зеленая иконка на кнопке
    QIcon           m_ico_yellow;                   // желтая иконка на кнопке

private:
    QLabel          *m_simulation_file_label{};     // имя файлы выбранной симуляции
    QFileInfo       m_simulation_fileinfo;          // имя файлы выбранной симуляции

    // settings from .ini
public:
    void            read_settings(QSettings *st);
    void            save_settings(QSettings *st);
    bool            m_settings_create_html;
    QString         m_settings_last_report_dir;
    QString         m_settings_last_report_loaded_dir;
    QString         m_settings_last_simulation_loaded_dir;

    int             m_settings_graph_x_min;
    int             m_settings_graph_x_max;
    //// int             m_settings_graph_x_tick;
    int             m_settings_graph_y_min;
    int             m_settings_graph_y_max;
    //// int             m_settings_graph_y_tick;
    int             m_settings_graph_y2_min;
    int             m_settings_graph_y2_max;
    //// int             m_settings_graph_y2_tick;
};


#endif // __PLATO_GUI_H_
