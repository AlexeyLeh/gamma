/*!
  \file   diagn.h
  \author Aleksey Lekhtin
  \date   20.10.2016
  \brief  Работа с гамма-модулем
*/

#ifndef __DIAGN_H_
#define __DIAGN_H_

#include <QObject>
#include <QSettings>
#include <QString>
#include <QTimer>

#include "main.h"
#include "node.h"
#include "response.h"

class COMM;
class DIAGN_GUI;
class MW;
class NodeE4;

class DIAGN : public QObject
{
    Q_OBJECT;

public:
    // Режим работы с устройствами
    enum mode_t
    {
        MODE_UNDEF = (quint8)0, // неопределенный режим
        MODE_PLATO,             // тест линейности
        MODE_DIAGNOSTIC,        // диагностика
        MODE_WRITE              // запись данных в устройство
    };

    Q_ENUM(mode_t);

public:
    DIAGN(APP *m, DIAGN_GUI *gui);
    ~DIAGN();

    inline APP       *app()    const { return m_app; }
    inline MW        *mw()     const { return m_app->mw(); }
    inline COMM      *comm()   const { return m_app->comm(); }
    inline NodeE4    *nodeE4() const { return m_app->nodeE4(); }
    inline DIAGN_GUI *gui()    const { return m_gui; }

    int                 polling_time();
    void                set_polling_time(int sec);
    void                set_E4_CPS_avrg(int counts);
    const QString       report_path() const;

    bool                start_scan();
    void                restart_scan();
    void                stop_diagn_timer();
    void                update_gui();

    void                make_request(const Adam::Frame& frame) const;
    const std::list<Adam::Frame> &requests() const;

private slots:
    void                slot_diagn_scanning();
    void                slot_comm__comport_disconnected();
    void                slot_comm__not_responded(NODECODE nodeCode, QString err_string);

private:
    APP                *m_app;
    DIAGN_GUI           *m_gui;
    QTimer              m_timer_diagn;     // таймер опроса для чтения данных
    int                 m_diagn_scanning_time;      // msec
    int                 m_gamma_counts_avrg;        // количество усреднений отсчетов для процессора

    std::list<Adam::Frame> m_diagn_requests;

    // settings from .ini
public:
    void                read_settings(QSettings *st);
    void                save_settings(QSettings *st);

};

#endif // __DIAGN_H_
