/*!
 * \file   ini.cpp
 * \author Aleksey Lekhtin
 * \date   created 06.11.2014
 * \brief  Параметры программы
 */

#include "api.h"
#include "api_gui.h"
#include "bto.h"
#include "bto_gui.h"
#include "comm.h"
#include "dbg.h"
#include "diagn.h"
#include "diagn_gui.h"
#include "fw.h"
#include "fw_gui.h"
#include "gamma.h"
#include "main.h"
#include "plato_gui.h"

//---------------------------------------------------------
// Open settings file
QSettings *APP::open_settings_file()
{
    auto fname = QString("%1/%2.ini")
            .arg(QCoreApplication::applicationDirPath())
            .arg(QCoreApplication::applicationName());
    fname = QDir::toNativeSeparators(fname);
    DBG(3, fname);
    m_sett = new QSettings(fname, QSettings::IniFormat, 0);
    if (m_sett->status() == QSettings::NoError) {
        return m_sett;
    }
    QString msg = QString("%1 %2\n").arg(tr("can't read settings file"), fname);
    DBG(0, msg);
    return nullptr;
}

//---------------------------------------------------------
QSettings *APP::qSettings() const
{
    return m_sett;
}

//---------------------------------------------------------
// Считать текущие настройки из файла
void APP::read_settings()
{
    DBG(4);
    open_settings_file();
    if (!m_sett) {
        QString msg = tr("Can't open application status file");
        QMessageBox::critical(nullptr, qApp->applicationName(), msg, QMessageBox::Close);
        return;
    }

    //
#ifdef _WIN32
    m_settings_console_output_cp = m_sett->value("ConsoleOutputCP", 1251).toInt();
#endif

    m_settings_simulation = m_sett->value("Simulation", false).toBool();
    if (DBG::debug_level == 0) {
        // 0 => в командной строке не задавался
        DBG::debug_level = m_sett->value("DebugLevel", 0).toUInt();
    }

    m_settings_ESKD = m_sett->value("Numbers/ESKDNumber", QString(GAMMA::ESKD_00)).toString();
    m_settings_oper = m_sett->value("Numbers/Operator", tr("Quality Inspector")).toString();

    auto sn = m_sett->value("Numbers/SerialNumber", QString(GAMMA::SN_00)).toString();
    int rem = GAMMA::SN_LEN - sn.length();
    QString str(GAMMA::SN_LEN, QChar('0'));
    sn.prepend(str.mid(0, rem));
    m_settings_SN = sn;
}

//---------------------------------------------------------
// Сохранить настройки
void APP::save_settings1()
{
    DBG(4);
    // [General] section
#ifdef _WIN32
    m_sett->setValue("ConsoleOutputCP", m_settings_console_output_cp);
#endif
    m_sett->setValue("Numbers/ESKDNumber", m_settings_ESKD);
    m_sett->setValue("Numbers/SerialNumber", m_settings_SN);
    m_sett->setValue("Numbers/Operator", m_settings_oper);

    m_sett->setValue("Simulation", m_settings_simulation);
    m_sett->setValue("DebugLevel", DBG::debug_level);
}

//---------------------------------------------------------
void APP::save_settings2()
{
    // DBG(3);
}

//---------------------------------------------------------
// Сохранить все настройки
void APP::save_all_settings()
{
    DBG(4);
    save_settings1();

    m_comm->save_settings(m_sett);
    m_bto->save_settings(m_sett);
    m_plato->save_settings(m_sett);
    m_plato_gui->save_settings(m_sett);
    m_api->save_settings(m_sett);
    m_api_gui->save_settings(m_sett);
    m_diagn->save_settings(m_sett);
    m_diagn_gui->save_settings(m_sett);
    m_fw->save_settings(m_sett);
    m_mw->save_settings(m_sett);

    save_settings2();
}

//---------------------------------------------------------
const QString &APP::ESKD() const
{
    return m_settings_ESKD;
}

//---------------------------------------------------------
const QString &APP::SN() const
{
    return m_settings_SN;
}

//---------------------------------------------------------
void APP::set_SN(const QString &sn)
{
    m_settings_SN = sn;
}

//---------------------------------------------------------
const QString &APP::Operator() const
{
    return m_settings_oper;
}
//---------------------------------------------------------
void APP::set_Operator(const QString &oper)
{
    m_settings_oper = oper;
}

//---------------------------------------------------------
bool APP::simulation() const
{
    return m_settings_simulation;
}
