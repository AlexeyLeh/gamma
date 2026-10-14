/*!
 * \file   main.cpp
 * \author Aleksey Lekhtin
 * \date   21.05.2025
 * \brief  Начало.
 */

#include <unistd.h>

#include <QObject>
#include <QApplication>

#include "dbg.h"
#include "main.h"
#include "mainwindow.h"
#include "translator.h"
#include "wmsg.h"

#include "api.h"
#include "api_gui.h"
#include "bto.h"
#include "bto_gui.h"
#include "comm.h"
#include "diagn.h"
#include "diagn_gui.h"
#include "gamma.h"
#include "fw.h"
#include "fw_gui.h"
#include "NodeE4.h"
#include "plato.h"
#include "plato_gui.h"
#include "utils.h"
#include "ver.h"

//---------------------------------------------------------
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":q_app_gamma.png"));
    APP app;
    if (app.mw()) {
        app.show();
        a.exec();
    }
    return 0;
}

//---------------------------------------------------------
APP::APP() :
    qt_translator(new QTranslator()),
    app_translator(new QTranslator())
{
    m_app_title = tr("Gamma Sonde Calibration");
    if (!parse_arguments()) {
        return;
    }

    m_tr = new TRANSLATOR();
    tr_path = m_tr->tr_path();
    tr_name = m_tr->tr_name();
    m_tr->install(qt_translator, app_translator);

    m_app_title = tr("Gamma Sonde Calibration");
    read_settings();

#ifdef _WIN32
    DBG(1, QString("GetConsoleOutputCP(): %1   (65001 => UTF8)").arg(GetConsoleOutputCP()));
    DBG(1, QString("SetConsoleOutputCP(%2)").arg(m_settings_console_output_cp));
    SetConsoleOutputCP(m_settings_console_output_cp);   // CP_UTF8
    DBG(1, QString("GetConsoleOutputCP(): %1").arg(GetConsoleOutputCP()));
    set_encoding(m_settings_console_output_cp);
    //// LOG::set_encoding(m_settings_console_output_cp);
#endif
    m_mw        = new MW(this);

    m_nodeE4    = new NodeE4();
    m_nodeBA    = new NodeBA();

    m_comm      = new COMM(this);

    m_api_gui   = new API_GUI(this);
    m_api       = new API(this, m_api_gui);

    m_bto_gui   = new BTO_GUI(this);
    m_bto       = new BTO(this, m_bto_gui);

    m_diagn_gui = new DIAGN_GUI(this);
    m_diagn     = new DIAGN(this, m_diagn_gui);

    m_fw_gui    = new FIRMWARE_GUI(this);
    m_fw        = new FIRMWARE(this, m_fw_gui);

    m_plato_gui = new PLATO_GUI(this);
    m_plato     = new PLATO(this, m_plato_gui);

    m_mw->create_environment();

    /*
       QThread *workerThread = new QThread();
       m_bto->moveToThread(workerThread);
       m_comm->moveToThread(workerThread);
       // (убрать 'this' из конструкторов этих классов и настроить запуск потока)
       workerThread->start();

APP::APP()
{
    // Убираем 'this' у COMM и BTO, так как они будут жить в другом потоке.
    // m_bto_gui и m_fw_gui остаются в UI-потоке, так как это графические интерфейсы!
    m_bto_gui   = new BTO_GUI(this);
    m_fw_gui    = new FIRMWARE_GUI(this);

    m_comm      = new COMM(); // без родителя
    m_bto       = new BTO(nullptr, m_bto_gui); // без родителя
    m_fw        = new FIRMWARE(this, m_fw_gui);

    // ... ваши connect'ы ...

    // Настраиваем поток для фоновой работы
    setupWorkerThread();
}

void APP::setupWorkerThread()
{
    m_workerThread = new QThread(this); // Сам поток живет в UI-потоке

    // Переносим логику опроса и связи в новый поток
    m_comm->moveToThread(m_workerThread);
    m_bto->moveToThread(m_workerThread);

    // Важно: если внутри COMM или BTO создаются другие QObject (например, m_timer_bto_polling),
    // они должны создаваться ЛИБО внутри методов (после старта потока),
    // ЛИБО у них в конструкторе родителем должен быть указан сам BTO (this).
    // Тогда moveToThread перенесет их ВСЕХ разом.

    // Правильное удаление объектов при остановке потока
    connect(m_workerThread, &QThread::finished, m_comm, &QObject::deleteLater);
    connect(m_workerThread, &QThread::finished, m_bto, &QObject::deleteLater);

    // Запускаем поток. Теперь его внутренний цикл событий (event loop) активен!
    m_workerThread->start();
}

BTO::BTO(QObject *parent, BTO_GUI *gui) : QObject(parent) {
    // Если вы делаете так, таймер переедет в поток вместе с BTO:
    m_timer_bto_polling.setParent(this);
}
    */

    connect(m_comm, &COMM::signal_comm_showstatus, this, [mw = m_mw](const QString &msg, int sec) {
        mw->showstatus(UTILS::split_message(msg), sec);
    }, Qt::QueuedConnection);

    //// connect(m_mw, &MW::signal_message,
    ////         this, [app = this](QMessageBox::Icon icon, const QString &title, const QString &msg, const QString &filename) {
    ////     WMSG::WMsg(app, icon, title, UTILS::split_message(msg), filename);
    //// }, Qt::QueuedConnection);
    ////
    //// //// connect(m_mw, &MW::signal_message, this, [mw = m_mw](QMessageBox::Icon icon, const QString &msg) {
    //// ////     WMSG::WMsg(mw, icon, UTILS::split_message(msg));
    //// //// }, Qt::QueuedConnection);
    ////
    //// connect(m_mw, &MW::signal_message_opendir, this,
    ////         [app = this](QMessageBox::Icon icon, const QString &msg, const QString &filename) {
    ////     WMSG::WMsg(app, icon, {}, UTILS::split_message(msg), filename);
    //// }, Qt::QueuedConnection);
    ////
    //// connect(m_mw, &MW::signal_message_title,
    ////         this, [app = this](QMessageBox::Icon icon, const QString &title, const QString &msg, const QString &filename) {
    ////     WMSG::WMsg(app, icon, title, UTILS::split_message(msg), filename);
    //// }, Qt::QueuedConnection);
    ////
     connect(m_mw, &MW::signal_save_settings, this, [this]() {
         save_all_settings();
     }, Qt::QueuedConnection);

    QTimer::singleShot(50, m_mw, [this](){
        mw()->run();
        bto()->gui()->run();
    });

}

//---------------------------------------------------------
APP::~APP()
{
    if (!mw()) {
        return;
    }

    DBG(4, "begin");
    save_all_settings();
    mw()->delay(100);

    delete m_api;
    delete m_api_gui;
    delete m_bto_gui;
    delete m_bto;
    delete m_comm;
    delete m_diagn;
    delete m_diagn_gui;
    delete m_fw;
    delete m_fw_gui;
    delete m_nodeE4;
    delete m_nodeBA;
    delete m_plato;
    delete m_plato_gui;
    delete m_mw;
    delete m_sett;

    delete qt_translator;
    delete app_translator;
    delete m_tr;

    if (DBG::QDebug_QFile.isOpen()) {
        DBG::QDebug_QFile.close();
    }
    DBG(4, "end");
}

//-----------------------------------------------------------
bool APP::parse_arguments()
{
    m_version = QString("Ver. %1 %2\n(built %3 %4)\n\n"
                        "built with Qt %5\n"
                        "run-time Qt %6")
            .arg(VER_NUM, VER_DATE, __DATE__, __TIME__, QT_VERSION_STR)
            .arg(qVersion());

    m_version_short = QString("Ver. %1 %2").arg(VER_NUM, VER_DATE);

    QStringList argv = QCoreApplication::arguments();
    if (argv.contains("-h",     Qt::CaseInsensitive) ||
            argv.contains("--help", Qt::CaseInsensitive) ||
            argv.contains("/?",     Qt::CaseInsensitive) ||
            argv.contains("/h",     Qt::CaseInsensitive))
    {
        print_help();
        return false; // закончить работу программы
    }

    if (argv.contains("-v", Qt::CaseInsensitive) ||
            argv.contains("--version", Qt::CaseInsensitive) ||
            argv.contains("/v", Qt::CaseInsensitive))
    {
        print_version();
        return false; // закончить работу программы
    }

    int pos = argv.indexOf("-d");
    if (pos != -1 && pos < argv.size()-1 && !argv[pos+1].startsWith("-"))
    {
        ++pos;
        DBG::debug_level = argv[pos].toInt();
        DBG(0, QString("dbg: %1").arg(DBG::debug_level));
    }

    pos = argv.indexOf("--log");
    if (pos != -1) {
        if (pos < argv.size()-1 && !argv[pos+1].startsWith("-")) {
            ++pos;
            DBG::QDebugOutputFile = argv.at(pos);
        } else {
            DBG::QDebugOutputFile = qApp->applicationDirPath() + "/" + QApplication::applicationName();
        }
    }

    //
    if (!DBG::QDebugOutputFile.isEmpty()) {
        QFileInfo fi(DBG::QDebugOutputFile);
        QString ext = fi.suffix();
        if (ext.isEmpty()) {
            if (DBG::QDebugOutputFile.endsWith(".")) {
                DBG::QDebugOutputFile += "log";
            } else {
                DBG::QDebugOutputFile += ".log";
            }
        }

        QDir dir = fi.dir();
        DBG(1, QString("expected log directory: %1").arg(QDir::toNativeSeparators(fi.dir().absolutePath())));
        if (!dir.exists())  {
            DBG(0, QString("directory does not exists - %1").arg(QDir::toNativeSeparators(fi.dir().absolutePath())));
            DBG::QDebugOutputFile = QString("%1/%2").arg(qApp->applicationDirPath()).arg(fi.fileName());
        }

        DBG::QDebugOutputFile = QDir::toNativeSeparators(DBG::QDebugOutputFile);
        DBG(0, QString("log: %1").arg(DBG::QDebugOutputFile));

        DBG::QDebug_QFile.setFileName(DBG::QDebugOutputFile);
        DBG::QDebug_QFile.open(QIODevice::WriteOnly /*| QIODevice::Append*/);
        DBG::debugTextStream.setDevice(&DBG::QDebug_QFile);
    }

    return true;
}

//---------------------------------------------------------
void APP::show()
{
    mw()->show();
}

//---------------------------------------------------------
void APP::set_encoding(int enc)
{
    if (!DBG::debugTextStream.device()) {
        return;
    }

    if (enc == 1251)
    {
        DBG::debugTextStream.setCodec(QTextCodec::codecForLocale());
    }
    else if (enc == 65001)
    {
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        DBG::debugTextStream.setCodec(codec);
    }
}

//---------------------------------------------------------
void APP::main_quit()
{
    DBG(3);
    QApplication::closeAllWindows();
}

//---------------------------------------------------------
const QString APP::root_path() const
{
    return QCoreApplication::applicationDirPath();
}

//---------------------------------------------------------
void APP::change_connection(const COMM::CONNECT_TYPE type)
{
    DBG(3);
    if (type == COMM::CONNECT_TYPE::ETHERNET) {
        comm()->set_type(COMM::CONNECT_TYPE::ETHERNET);
        comm()->save_settings(qSettings());
    } else if (type == COMM::CONNECT_TYPE::COMPORT) {
        comm()->set_type(COMM::CONNECT_TYPE::COMPORT);
        comm()->save_settings(qSettings());
    }

    delete m_comm;
    m_comm = new COMM(this);
    connect(m_comm, &COMM::signal_comm_showstatus, this, [mw = m_mw](const QString &msg, int sec) {
        mw->showstatus(UTILS::split_message(msg), sec);
    }, Qt::QueuedConnection);

    QString text;
    QDebug(&text) << m_comm;
    DBG(3, text);
}

//---------------------------------------------------------
API *APP::api() const
{
    return m_api;
}

//---------------------------------------------------------
API_GUI *APP::api_gui() const
{
    return m_api_gui;
}

//---------------------------------------------------------
BTO *APP::bto() const
{
    return m_bto;
}

//---------------------------------------------------------
BTO_GUI *APP::bto_gui() const
{
    return m_bto_gui;
}

//---------------------------------------------------------
void APP::ping_bto()
{
    bto()->ping();
}

//---------------------------------------------------------
DIAGN *APP::diagn() const
{
    return m_diagn;
}

//---------------------------------------------------------
DIAGN_GUI *APP::diagn_gui() const
{
    return m_diagn_gui;
}

//---------------------------------------------------------
FIRMWARE *APP::fw() const
{
    return m_fw;
}

//---------------------------------------------------------
FIRMWARE_GUI *APP::fw_gui() const
{
    return m_fw_gui;
}

//---------------------------------------------------------
PLATO *APP::plato() const
{
    return m_plato;
}

//---------------------------------------------------------
PLATO_GUI *APP::plato_gui() const
{
    return m_plato_gui;

}
