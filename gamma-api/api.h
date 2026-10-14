    /*!
  \file   api.h
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  API-тест гамма-модуля
*/

#ifndef __API_H_
#define __API_H_

#include <QObject>
#include <QSettings>
#include <QTimer>
#include <QFile>
#include <QVector>

#include "api_data.h"
#include "main.h"
#include "node.h"
#include "response.h"

class API_GUI;
class COMM;
class CTable;
class MW;
class NodeBA;
class NodeE4;

class API : public QObject
{
    Q_OBJECT;

public:
    enum api_mode_t
    {
        API_0 = (int)0,
        API_200
    };
    Q_ENUM(api_mode_t);

    enum tmode_t
    {
        COLD = (int)0,
        HOT
    };
    Q_ENUM(tmode_t);

    enum method_type_t
    {
        OLD_METHOD = (int)0,
        NEW_METHOD
    };
    Q_ENUM(method_type_t);

    enum stage_t
    {
        INIT = (int)0,
        TYPE = 1,
        SET_VOLTAGE = 2,
        WARMUP = 3,
        READ_GAMMA = 4,
        SIMULATION = 44,
        CALCULATIONS = 100,
        CANCEL = 1005,
        STOP = 1010,
    };
    Q_ENUM(stage_t);

private:
    typedef struct
    {
        stage_t stage;    // стадия теста
        int step;         // шаги в стадии
    } test_stage_t;

public:
    typedef struct
    {
        QDateTime   date;
        QString     toolname;
        QString     toolsn;
        QString     toolpn;
        QString     eskdnum;
        QString     location;
        QString     oper;
        QString     failed_message;
        int         acqtime{};
        int         operating_hv{};
        double      calibr_factor_old{};
        double      calibr_factor_new{};
        double      std_deviation_0{};              // значение квадратичного отклонения API0
        double      std_deviation_200{};            // значение квадратичного отклонения API200
        double      avrg_CPS0{};                    // среднее по гамма-отсчетам на 0 API
        double      avrg_CPS200{};                  // среднее по гамма-отсчетам на 200 API
        double      settings_calibr_min{};          // допуск кaлибровочного коэффициента API
        double      settings_calibr_max{};          // допуск кaлибровочного коэффициента API
        int         settings_calibr_old{};          // калибровочное значение коэффициента API
        int         settings_calibr_new{};          // калибровочное значение коэффициента API
        double      settings_tol_deviat0_min{};
        double      settings_tol_deviat0_max{};
        double      settings_tol_deviat200_min{};
        double      settings_tol_deviat200_max{};

        QVector<API_DATA> data;

public:

        inline size_t size() const {
            return data.size();
        }

        inline size_t empty() const {
            return data.empty();
        }

        inline void clear()
        {
            data.clear();
            date = {};
            toolname.clear();
            eskdnum.clear();
            toolsn.clear();
            toolpn.clear();
            location.clear();
            oper.clear();
            failed_message.clear();
            acqtime = {};
            calibr_factor_old = {};
            calibr_factor_new = {};
            settings_tol_deviat0_min = {};
            settings_tol_deviat0_max = {};
            settings_tol_deviat200_min = {};
            settings_tol_deviat200_max = {};
            std_deviation_0 = {};
            std_deviation_200 = {};
            avrg_CPS0 = {};
            avrg_CPS200 = {};
            settings_calibr_min = {};
            settings_calibr_max = {};
            settings_calibr_old = {};
            settings_calibr_new = {};
        }
    } api_data_t;

public:
    API(APP *m, API_GUI *gui);
    ~API();

    inline APP       *app()    const { return m_app; }
    inline MW        *mw()     const { return m_app->mw(); }
    inline COMM      *comm()   const { return m_app->comm(); }
    inline NodeE4    *nodeE4() const { return m_app->nodeE4(); }
    inline API_GUI   *gui()    const { return m_gui; }

    inline int       settings_calibr_old() const { return m_settings_calibr_old; }
    inline int       settings_calibr_new() const { return m_settings_calibr_new; }


    void                    update_gui();
    double                  hot_mode_temperature() const;
    const QString           report_path() const;
    const QVector<ODID*>    &odid_old() const;
    const API::api_data_t   &data() const;
    API::api_data_t         &data();

    int                     data_size() const;
    const QString           failed_message() const;
    bool                    passed() const;

    void                    start_api_timer();
    void                    stop_api_timer();
    void                    update_odid_table();
    void                    set_factor_version(QString str);
    const                   QString &factor_version() const;

    void                    protocol_filename_create();
    const QFileInfo         &protocol_filename() const;
    void                    protocol_filename_clear();
    void                    protocol_filename_set(const QString &fn);
    QString                 protocol_check_creation();
    QString                 save_report(const QString &filename, const api_data_t &data);

private:
    bool                    temperature_control();
    int                     init_test();
    int                     set_voltage();
    int                     warmup();
    int                     read_gamma();
    void                    remember_CPS();
    void                    update_table();
    void                    calculate_cps_average();
    bool                    calculations();
    bool                    calculate_calibration_factors();
    bool                    calculate_std_deviation();
    void                    calculate_odid_table();

    bool                    simulation();                   // симуляция данных (для отладки)
    void                    init_simulation_data(api_data_t &d);
    void                    do_simulation();

    QString                 protocol_filename_last_log();
    QString                 protocol_open(const QString &fname);
    void                    protocol_close();
    void                    protocol_create_last_log();
    void                    protocol_make_header(const api_data_t &data, QString &html_header, QString &text_header);
    QString                 protocol_make_html_template_header() const;
    void                    protocol_make_content(const api_data_t &data, QString &html_page, QString &txt_page);
    void                    protocol_write_record(const QString &str);
    void                    protocol_make_screenshot();

private slots:
    void                    slot_api_test();
    void                    slot_comm__comport_disconnected();
    void                    slot_comm__not_responded(NODECODE node, QString err_string);

private:
    APP                    *m_app;
    API_GUI                 *m_gui;
    QTimer                  m_api_timer;                    // таймер опроса для чтения данных

// private:
public:
    double                  m_e4_warmup_time{};             // время ожидания установки напряжения в тестировании
    double                  m_acq_one_gamma{};              // время одного измерения гамма-отсчета (из прошивки)
    int                     m_acq_sample_time{};            // время по накоплению и расчету одного замера гаммы
    int                     m_waiting_for_acqure{};         // счетчик ожидания готовности гаммы
    int                     m_acq_counter{};                // счетчик чтения CPS
    int                     m_warmup_period{};              // время ожидания установки напряжения в тестировании
    int                     m_api_warmup_counter{};         // отсчет времени прогрева (сек)
    int                     m_acq_total_time{};             // итоговое время по накоплению и расчету всех замеров гаммы

    tmode_t                 m_api_hot_mode{COLD};           // температурный режим теста
    api_mode_t              m_api_mode{API_0};              // API текущего теста

private:
    QFile                   m_api_log_fd;                   // дескриптор файла
    QFileInfo               m_report_filename;              // имя файла отчета

    test_stage_t            m_test_stage;
    qint64                  m_prev_msec{0};
    int                     m_timer_delay;                  //
    int                     m_total_steps;                  // количество замеров CPS
    int                     m_current_step;                 // количество измеренных значений
    int                     m_progress_bar_index;           // число для индикатора

    api_data_t              m_data{};                       // результаты измерений
    api_data_t              m_sim_data{};                   // результаты измерений
    QVector<ODID*>          m_api_odid_old;                 // рассчетные коэффициенты по толщине трубы
    QVector<ODID*>          m_api_odid_new;                 // рассчетные коэффициенты по толщине трубы
    QString                 m_factor_ver;
    bool                    m_api_odid_data_valid{false};   // актуальность данных - используется при выводе в таблицу

    // settings from .ini
public:
    void                read_settings(QSettings *st);
    void                save_settings(QSettings *st);
    QString             m_settings_last_report_dir;
    bool                m_settings_create_html{};
    double              m_settings_hot_mode_temper{};       // температура порога переключения на режим горячего теста
    int                 m_settings_warmup_time{};           // время ожидания установки напряжения (sec)
    int                 m_settings_samples_for_avrg{};      // итоговое время в программе по расчету среднего значения
    int                 m_settings_acquire_time;            // время ожидания установки гамма отсчетов (sec)
    int                 m_settings_gamma_acq_bucket{};      // Количество усреднений в самом гамма-детекторе (в прошивке)
    int                 m_settings_HV_min_value{};          //
    double              m_settings_calibr_min{};            // допуск кaлибровочного коэффициента API
    double              m_settings_calibr_max{};            // допуск кaлибровочного коэффициента API
    int                 m_settings_calibr_old{};            // калибровочное значение коэффициента API
    int                 m_settings_calibr_new{};            // калибровочное значение коэффициента API
    double              m_settings_tol_deviat0_min{};
    double              m_settings_tol_deviat0_max{};
    double              m_settings_tol_deviat200_min{};
    double              m_settings_tol_deviat200_max{};
    double              m_settings_odid_beg{};              // толщина трубы начальная (наружный диаметр минус внутренний)
    double              m_settings_odid_end{};              // толщина трубы конечная
    double              m_settings_odid_stp{};              // шаг приращения толщины трубы
};

#endif // __API_H_
