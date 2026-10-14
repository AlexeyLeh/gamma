/*!
  \file   main.h
  \author Aleksey Lekhtin
  \date   21.05.2025
  \brief  Главный объект
*/

#ifndef _MAIN_H_
#define _MAIN_H_

#include <cstdbool>

#include <QtWidgets>

#include "comm.h"
#include "dbg.h"

#ifndef OK
#define OK   true
#define FAIL false
#define YES  true
#define NO   false
#endif

class MNG;
class BTO;
class BTO_GUI;
class FIRMWARE;
class FIRMWARE_GUI;
class DIAGN;
class DIAGN_GUI;
class MW;
class PLATO;
class PLATO_GUI;
class API;
class API_GUI;
class Node;
class NodeE4;
class NodeE4;
class TOOLS;
class TRANSLATOR;

class APP : public QObject
{
    Q_OBJECT;

public:
    explicit APP();
    ~APP();


    void                show();
    void                main_quit();
    const QString       root_path() const;
    const QString       title() const;
    const QString       version() const;
    const QString       version_short() const;
    const QString       &ESKD() const;
    const QString       &SN() const;
    void                set_SN(const QString &sn);
    const QString       &Operator() const;
    void                set_Operator(const QString &oper);
    bool                simulation() const;
    QSettings           *qSettings() const;

    inline MW           *mw()     const { return m_mw; }
    inline NodeBA       *nodeBA() const { return m_nodeBA; }
    inline NodeE4       *nodeE4() const { return m_nodeE4; }
    inline COMM         *comm()   const { return m_comm; }
    void                change_connection(const COMM::CONNECT_TYPE type);

    API                 *api() const;
    API_GUI             *api_gui() const;
    BTO                 *bto() const;
    BTO_GUI             *bto_gui() const;
    DIAGN               *diagn() const;
    DIAGN_GUI           *diagn_gui() const;
    FIRMWARE            *fw() const;
    FIRMWARE_GUI        *fw_gui() const;
    PLATO               *plato() const;
    PLATO_GUI           *plato_gui() const;

    void                ping_bto();

private:
    bool                parse_arguments();
    void                print_help();
    void                print_version();
    void                read_settings();
    void                save_settings1();
    void                save_settings2();
    void                save_all_settings();
    QSettings           *open_settings_file();
    void                set_encoding(int enc);

public:
    QString             tr_path;                    // путь к выбранным переводам
    QString             tr_name;                    // файл с переводами

private:
    TRANSLATOR          *m_tr;
    QSettings           *m_sett{};
    MW                  *m_mw{};
    QString             m_app_title;                // название приложения
    QString             m_version;                  // о программе
    QString             m_version_short;            // о программе (сокращенно)

private:
    API                 *m_api{};
    API_GUI             *m_api_gui{};
    BTO                 *m_bto{};
    BTO_GUI             *m_bto_gui{};
    COMM                *m_comm{};
    FIRMWARE            *m_fw{};
    FIRMWARE_GUI        *m_fw_gui{};
    DIAGN               *m_diagn{};
    DIAGN_GUI           *m_diagn_gui{};
    NodeBA              *m_nodeBA{};
    NodeE4              *m_nodeE4{};
    PLATO               *m_plato{};
    PLATO_GUI           *m_plato_gui{};

    QTranslator         *qt_translator;     // translator sybsystem for QT
    QTranslator         *app_translator;    // translator sybsystem for application

    // settings from .ini
private:
    int                 m_settings_console_output_cp;
    bool                m_settings_simulation;
    QString             m_settings_SN;
    QString             m_settings_ESKD;
    QString             m_settings_oper;
};

#endif // _MAIN_H_
