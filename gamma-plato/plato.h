/*!
  \file   plato.h
  \author Aleksey Lekhtin
  \date   20.10.2016
  \brief  Non-gui object
*/

#ifndef __PLATO_H_
#define __PLATO_H_

#include <QObject>
#include <QFile>
#include <QFrame>
#include <QLabel>
#include <QMovie>
#include <QSettings>
#include <QTimer>

#include <mutex>

#include "main.h"
#include "node.h"
#include "plato_data.h"
#include "response.h"

class COMM;
class CTable;
class MW;
class NodeE4;
class PLATO_GUI;

class PLATO : public QObject
{
    Q_OBJECT;

public:
    enum tmode_t
    {
        COLD = (int)0,
        HOT
    };
    Q_ENUM(tmode_t);

    enum stage_t
    {
        INIT = (int)0,
        SET_VOLTAGE,
        WARMUP,
        READ_GAMMA,
        FINISH,
        STOP,
    };
    Q_ENUM(stage_t);

private:
    typedef struct
    {
        stage_t stage;      // нумерация теста
        int step;           // этапы теста
    } test_stage_t;

public:
    typedef struct
    {
        int         hv_lo_limit;
        int         hv_hi_limit;
        int         hv_sp_limit;

        int         cps_lo_limit;
        int         cps_hi_limit;
        int         cps_sp_limit;

        int         chart_hv_lo;
        int         chart_hv_hi;
        int         chart_hv_tick;
        int         chart_cps_lo;
        int         chart_cps_hi;
        int         chart_cps_tick;

        int         hv_cold_lo_border;
        int         hv_cold_hi_border;
        int         hv_cold_spread;

        int         hv_hot_lo_border;
        int         hv_hot_hi_border;
        int         hv_hot_spread;

        double      cps_cold_lo_border;
        double      cps_cold_hi_border;
        double      cps_cold_spread;

        double      cps_hot_lo_border;
        double      cps_hot_hi_border;
        double      cps_hot_spread;
        double      cps_accepted_sp;

        int         hv_accepted_lo;
        int         hv_accepted_hi;
        int         hv_accepted_sp;
        int         hv_accepted;
        int         hv_orig;

        double      temper_cold_avrg;
        double      temper_hot_avrg;

        QVector<PLATO_DATA> cold;
        QVector<PLATO_DATA> hot;

        // результаты калибровки
        QString     failed_message;
        QDateTime   date;
        QString     toolname;
        QString     eskdnum;
        QString     toolsn;
        QString     toolpn;
        QString     location;
        QString     oper;
        int         acqtime;

    public:
        inline size_t size() const {
            return std::min(cold.size(), hot.size());
        }

        inline bool empty() const {
            return cold.empty() || hot.empty();
        }

    } calibration_data_t;

public:
    PLATO(APP *m, PLATO_GUI *gui);
    ~PLATO();

    inline APP       *app()    const { return m_app; }
    inline MW        *mw()     const { return m_app->mw(); }
    inline COMM      *comm()   const { return m_app->comm(); }
    inline NodeE4    *nodeE4() const { return m_app->nodeE4(); }
    inline PLATO_GUI *gui()    const { return m_gui; }


    void            set_data(const calibration_data_t &data);
    bool            set_simulation_data(const calibration_data_t &data);
    int             total_steps();
    double          hot_mode_temperature() const;
    bool            passed() const;
    const QString   failed_message() const;

    void            start_plato_timer();
    void            stop_plato_timer();
    int             polling_time();
    void            set_polling_time(int sec);

    const QString   report_path() const;
    void            protocol_filename_create();
    const QFileInfo &protocol_filename() const;
    void            protocol_filename_set(const QString &fn);
    void            protocol_filename_clear();
    QString         protocol_filename_last_log();
    void            protocol_create_last_log(const calibration_data_t &data);
    QString         save_report();
    QString         load_report(const QString &filename, calibration_data_t &data);
    QString         simulation_save(const calibration_data_t &data, QFileInfo &out_finfo);
    QString         simulation_load(calibration_data_t &data, const QFileInfo &finfo);

    QString         report_check_creation();

    int             set_working_voltage(int operation_value, int min_value, int tst_value, int max_value);
    int             set_operating_voltage(int value);
    void            restore_original_voltage();

    void            init_simulation_data(calibration_data_t &data);
    PLATO::calibration_data_t       &data();
    const PLATO::calibration_data_t &data() const;
    QVector<PLATO_DATA>             &get_data_recs(tmode_t tmode);

private:
    int             init_test();
    int             warmup();
    int             set_voltage();
    int             read_gamma();
    bool            temperature_control(tmode_t tmode);
    void            update_chart(tmode_t tmode, int index);
    void            update_table(tmode_t tmode, int index, bool select_row = true);
    double          calculate_slope(tmode_t tmode, int index);
    bool            calculate_borders(tmode_t tmode, int index);
    void            check_borders();
    void            update_borders();
    void            update_spreads();
    void            update_failed_message(const QString &msg, const QString &tip);
    void            store_data(tmode_t tmode, int index);
    void            create_summary(tmode_t tmode, calibration_data_t &data);
    void            create_summary();

    QString         protocol_make_html_template_header() const;
    void            protocol_make_header(const calibration_data_t &data, QString &text_header, QString &html_header);
    void            protocol_make_content(const calibration_data_t &data, QString &txt_page, QString &html_page);
    QString         protocol_open(const QString &fname);
    void            protocol_close();
    void            protocol_write_record(const QString& str);
    void            protocol_make_screenshot();

    void            clear_plato_values(tmode_t tmode);
    void            clear_data_recs(tmode_t tmode, calibration_data_t &data);

private slots:
    void            slot_plato_test();
    void            slot_comm__comport_disconnected();
    void            slot_comm__not_responded(NODECODE node, QString err_string);

private:
    APP             *m_app{};
    PLATO_GUI       *m_gui{};

public:
    int             m_acq_one_gamma{};          // время одного измерения гамма-отсчета (из прошивки)
    int             m_acq_total_time{};         // итоговое время по накоплению и расчету всех замеров гаммы
    int             m_waiting_for_acqure;       // счетчик ожидания готовности гаммы
    int             m_acq_counter;              // счетчик чтения CPS
    float           m_gamma_avrg{};             // накопление для расчета среднего
    int             m_warmup_period{};          // время ожидания установки напряжения в тестировании
    int             m_warmup_counter{};         // отсчет времени прогрева (сек)
    int             m_acq_sample_time{};        // время по накоплению и расчету одного замера гаммы
    tmode_t         m_temper_mode{PLATO::COLD}; // температурный режим теста
    bool            m_table_sort_status{false}; // статус сортировки таблицы (рабочая переменная)

    int             m_hv_lo_limit;              //
    int             m_hv_hi_limit;              //
    int             m_hv_spread_limit;          // Разсброс напряжения
    double          m_cps_lo_limit;             //
    double          m_cps_hi_limit;             //
    double          m_cps_spread_limit;         // spread. полка. между hi и lo

    int             m_hv_test_initial{};        // начальное напряжение тестирование линейности
    int             m_hv_test_final{};          // конечное напряжение тестирование линейности

    int             m_calibr_hv_min{};          // калибровочные границы напряжений, прошитые в модуль
    int             m_calibr_hv_max{};          // калибровочные границы напряжений, прошитые в модуль
    int             m_calibr_hv_tst{};          // калибровочные границы напряжений, прошитые в модуль

    int             m_HV_orig{};                // текущее рабочее напряжение
    int             m_HV_orig_lo{};             // запомненные калибровочные напряжения
    int             m_HV_orig_tst{};            // запомненные калибровочные напряжения
    int             m_HV_orig_hi{};             // запомненные калибровочные напряжения

public:
    // результаты тестирования
    int             m_cold_hv_lo_border{};      // границы допустимых значений по результатам тестирования
    int             m_cold_hv_hi_border{};      //
    int             m_cold_hv_spread{};         //
    int             m_hot_hv_lo_border{};       // границы допустимых значений по результатам тестирования
    int             m_hot_hv_hi_border{};       //
    int             m_hot_hv_spread{};          //

    double          m_cold_cps_lo_border{};     //
    double          m_cold_cps_hi_border{};     //
    double          m_cold_cps_spread{};        //
    double          m_hot_cps_lo_border{};      //
    double          m_hot_cps_hi_border{};      //
    double          m_hot_cps_spread{};         //

    int             m_HV_AcceptedLo{};          //
    int             m_HV_AcceptedHi{};          //
    int             m_HV_Accepted{};            // калибровочное напряжение по результатам тестирования
    int             m_HV_Spread_Accepted{};     //
    double          m_CPS_Spread_Accepted{};    //

private:
    // результаты тестирования
    QString         m_failed_message;           // описание причины неудачи тестирования (так-же и признак этого)
    calibration_data_t m_cal_data{};            // данные калибровки
    calibration_data_t m_sim_data{};            // данные симуляции калибровки

private:
    // рабочий процесс тестирования
    QTimer          m_plato_timer;              // таймер для поведениея теста по шагам
    test_stage_t    m_test_stage{};             //
    qint64          m_prev_msec{0};             //
    qint64          m_timer_delay{1000};        //
    int             m_total_steps{};            // количество шагов тестирования напряжения
    int             m_current_step{};           // количество измеренных значений
    int             m_progress_bar_total{};     // диапазон индикатора процесса
    int             m_progress_bar_index{};     // число для индикатора
    QFile           m_report_fd;                // дескриптор файла отчета
    QFileInfo       m_report_filename;          // имя файла отчета

    // settings from .ini
public:
    void            read_settings(QSettings *st);
    void            save_settings(QSettings *st);

    int             m_settings_HV_LoLimit;          // лимит рабочего напяржения
    int             m_settings_HV_HiLimit;          // лимит рабочего напяржения
    int             m_settings_HV_SpreadLimit;      // допуск по напряжению (полка)

    int             m_settings_CPS_LoLimit;         // минимальное число отсчетов
    int             m_settings_CPS_HiLimit;         // максимальное число отсчетов
    int             m_settings_CPS_SpreadLimit;     // допуск по отсчетам

    int             m_settings_HV_Test_Lo;          // начальное напряжение тестирование линейности
    int             m_settings_HV_Test_Hi;          // конечное напряжение тестирование линейности
    int             m_settings_HV_step;             // шаг увеличения тестируемого напряжения
    double          m_settings_HV_Proportion;       // Коэффициент пропорциональности для расчета оптимального напряжения

    int             m_settings_num_pnt_for_slope;   // количество точек для расчета крутизны
    double          m_settings_slope_min;           // допуск для определения линейности
    double          m_settings_slope_max;           // допуск для определения линейности
    double          m_settings_slope_critical;      // допуск для определения линейности

    int             m_settings_warmup_time;         // время ожидания установки напряжения (sec)
    int             m_settings_acquire_time;        // время ожидания установки гамма отсчетов (sec)
    int             m_settings_samples_for_avrg;    //
    int             m_settings_gamma_acq_bucket;    // Количество усреднений в самом гамма-детекторе (в прошивке)
    double          m_settings_hot_mode_temper{};   // температура порога переключения на режим горячего теста
};

#endif // __PLATO_H_
