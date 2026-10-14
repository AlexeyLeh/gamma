/*!
  \file   fw.h
  \author Aleksey Lekhtin
  \date   20.10.2016
  \brief  Отображение информации во вкладке FIRMWARE
*/

#ifndef __FW_H_
#define __FW_H_

#include <QObject>
#include <QSettings>
#include <QTimer>

#include <mutex>

#include "main.h"
#include "node.h"
#include "response.h"

class COMM;
class CTable;
class KTable;
class MW;
class NodeE4;
class FIRMWARE_GUI;

class FIRMWARE : public QObject
{
    Q_OBJECT;

public:
    FIRMWARE(APP *m, FIRMWARE_GUI *gui);
    ~FIRMWARE();

    inline APP          *app()    const { return m_app; }
    inline MW           *mw()     const { return m_app->mw(); }
    inline COMM         *comm()   const { return m_app->comm(); }
    inline NodeE4       *nodeE4() const { return m_app->nodeE4(); }
    inline FIRMWARE_GUI *gui()    const { return m_gui; }

    CTable              *ctable() const;
    KTable              *ktable() const;
    void                update_gui();
    int                 polling_time();
    void                set_polling_time(int sec);
    void                make_request(const Adam::Frame& frame) const;

    void                start_read_firmware();
    ////void                cancel_read();

private slots:
    void                slot_comm__comport_disconnected();
    void                slot_comm__not_responded(NODECODE nodeCode, QString err_string);

private:
    APP                *m_app;
    FIRMWARE_GUI        *m_gui;
    int                  m_scanning_time;       // msec

    std::list<Adam::Frame> m_fw_requests;

    // settings from .ini
public:
    void                read_settings(QSettings *st);
    void                save_settings(QSettings *st);
};

#endif // __FW_H_
