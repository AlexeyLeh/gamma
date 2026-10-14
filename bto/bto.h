/*!
  \file   bto.h
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  БТО (bto) - Блок технического обслуживания (Service Unit)
*/

#ifndef __MAINTENANCE_UNIT_DEV_H_
#define __MAINTENANCE_UNIT_DEV_H_

#include <mutex>

#include <QTcpSocket>
#include <QTimer>
#include <QMessageBox>
#include <QMutexLocker>

#include "main.h"
#include "comm.h"
#include "response.h"
#include "NodeBA.h"
#include "NodeE4.h"

class COMM;
class BTO_GUI;
class MW;
class NODEBA;
class NodeE4;
class comport_t;

//-----------------------------------------------------
class BTO : public QObject
{
    Q_OBJECT;

public:
    enum class MODULE_KIND {
        UNKNOWN = (uint8_t)0,
        ALIEN,
        ES,
    };

    enum class GAMMA_STATUS {
        UNKNOWN = (uint8_t)0,
        NOT_POWERED = 1,
        NOT_POWERED_YET,
        NOT_FOUND,
        SEARCH,
        READPARTNUMBER,
        ALMOST_FOUND,
        ALMOST_FOUND2,
        FOUND,
    };

    enum class BTO_PERIOD {
        BtoPollPeriodFast  = 100,     // ms
        BtoPollPeriod      = 3000,    // ms
        BtoPingPeriod      = 10000,   // ms
    };


public:
    BTO(APP *m, BTO_GUI *gui);
    ~BTO();

    inline APP       *app()    const { return m_app; }
    inline MW        *mw()     const { return m_app->mw(); }
    inline COMM      *comm()   const { return m_app->comm(); }
    inline NodeBA    *nodeBA() const { return m_app->nodeBA(); }
    inline NodeE4    *nodeE4() const { return m_app->nodeE4(); }
    inline BTO_GUI   *gui()    const { return m_gui; }

    inline NODECODE           type() const { return m_bto_type; }
    inline COMM::CONNECT_TYPE connection_type() { return comm()->type(); }

    void                set_type(NODECODE num);
    QString             part_number() const;
    void                set_part_number(const QString &number);
    MODULE_KIND         gamma_module_variant() const;
    void                set_gamma_module_kind(const BTO::MODULE_KIND &kind);
    QString             gamma_module_pn() const;
    void                set_gamma_module_pn(const QString &pn);
    void                connect_to() const;
    void                disconnect_from();
    void                cancel_connection();
    bool                gamma_module_found() const;
    void                push_upload_settings_button();
    bool                push_gamma_power_button(bool on);
    bool                bto_autoconnect() const;
    void                set_bto_autoconnect(bool on);
    bool                gamma_autopower() const;
    void                set_gamma_autopower(bool on);
    void                start_bto_timer(int interval = 0);
    void                stop_bto_timer();
    void                ping();

public slots:
    void                slot_bto_polling();

private slots:
    void                slot_comm_ethernet_disconnected();
    void                slot_comm_ethernet_connected();
    void                slot_comm_ethernet_canceled();
    void                slot_comm_requests_completed(int to);
    void                slot_comm_incorrect_resonse(const Adam::Response &response, const QString &msg);
    void                slot_comm_not_responded(NODECODE nodeCode, QString err_string);
    void                slot_comm_connection_error();

private:
    bool                make_module_poweron_request(std::list<Adam::Frame> &frames);
    void                make_module_poweroff_request(std::list<Adam::Frame> &frames);
    int gamma_search_procedure();

    //-----------------------------------------------------
public:
    int                 m_settings_power_output_index;
    float               m_settings_amperage_limit;
    float               m_settings_voltage_limit;

private:
    APP                *m_app;
    BTO_GUI             *m_gui;
    QTimer              m_timer_bto_polling;
    NODECODE            m_bto_type;
    QString             m_part_number;
    int                 m_bto_rtime;
    QString             m_gamma_module_pn;
    MODULE_KIND         m_gamma_module_kind{MODULE_KIND::ALIEN};
    int                 m_initial_start{0};
    bool                m_next_request_for_gamma{false};

public:
    GAMMA_STATUS        m_module_powering_step;

    // settings
private:
    QString             m_settings_fname;
    bool                m_settings_bto_autoconnect;
    bool                m_settings_gamma_autopower;
    QSettings           *open_settings_file();
public:
    void                read_settings(QSettings *st);
    void                save_settings(QSettings *st);
};

QDebug operator<<(QDebug d, const BTO::GAMMA_STATUS & status);

#endif  // __MAINTENANCE_UNIT_DEV_H_
