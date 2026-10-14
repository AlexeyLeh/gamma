/*!
 * \file   translator.cpp
 * \author Aleksey Lekhtin
 * \date   created 09.08.2012
 * \brief  Load transaltions
 */

#include <stdio.h>

#include <QLocale>

#include "main.h"
#include "translator.h"

TRANSLATOR::TRANSLATOR() :
    m_tr_name(),
    m_tr_path()
{
    DBG(3);
    load();
}

TRANSLATOR::~TRANSLATOR()
{
    DBG(4);
}

//---------------------------------------------------------
//  Path for searching:
//  - locale/xx_XX/LC_MESSAGES/
//  - locale/xx_XX
//  - ./
//  as result:
//  - path with transaltion - successful loading
//  - empty string - nor
//
void TRANSLATOR::load()
{
    DBG(4);
    m_tr_path.clear();
    QLocale loc = QLocale::system();
    QFileInfo f_info(QApplication::applicationName());

    //--- locale for QT -----------------------------------
#if 0
    auto nm = "qt_" + QLocale::system().name();
    DBG(1, QString("locale system name: \"%1\"").arg(nm));
    auto ok = qt_translator->load(nm, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    if (!ok)
    {
        DBG(0, QString("can't load \"%1\"").arg(nm));
        return;
    }
    DBG(0, QString("%1 loaded ok").arg(nm));
    ok = qApp->installTranslator(qt_translator);
    DBG(0, QString("translator install - %1").arg(ok ? "ok" : "failed"));
    DBG(1, QString("codec name: %1").arg(QTextCodec::codecForLocale()->name().constData()));
#endif

    //-----------------------------------------------------
    QString ext = f_info.suffix();
    QString app_name = f_info.fileName();
    if (ext == "exe") {
        app_name = f_info.completeBaseName();
    }

    if (app_name.isEmpty()) {
        DBG(0, "application name not specified");
        return;
    }

    //-----------------------------------------------------
    /* getenv("PATH") */
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString env_path = env.value("PATH");
#ifdef Q_OS_WIN
    QStringList dirnames = env_path.split(";");
#else
    QStringList dirnames = env_path.split(":");
#endif

    if ( DBG::debug_level >= 3 ) {
        DBG(3, "\nPATH: ");
        for(const QString &dn : dirnames) {
            if (!dn.isEmpty()) {
                DBG::dbg_print_raw(QStringLiteral("  ") + dn + QStringLiteral("\n"));
            }
        }
    }
    DBG(3, "search translation:");

    //-----------------------------------------------------
    QString tr_name;
    QString translation_path;
    QTranslator app_translator;
    for(int i = 0; i < 5 + dirnames.size(); ++i)
    {
        tr_name = app_name;
        if (i == 0)
        {
            translation_path = ".";
        }
        else if (i == 1)
        {
            if (loc.name().isEmpty()) continue;
            translation_path = ".";
            tr_name = QString("%1.%2").arg(app_name).arg(loc.name());
        }
        else if (i == 2)
        {
            if (loc.name().isEmpty()) continue;
            translation_path = QString("locale/%1/LC_MESSAGES").arg(loc.name());
        }
        else if (i == 3)
        {
            if (loc.name().isEmpty()) continue;
            translation_path = QString("locale/%1").arg(loc.name());
        }
        else if (i == 4)
        {
            if (loc.name().isEmpty()) continue;
            translation_path = QString("../locale/%1").arg(loc.name());
        }
        else if (i == 5)
        {
            translation_path = f_info.absolutePath();
        }
        else
        {
#ifdef __linux__
            msg = "search translation in PATH";
            translation_path = dirnames.at(i-6);
#else
            break;
#endif
        }

        QString fn = translation_path + "/" + tr_name + ".qm";
        fn = QDir::cleanPath(fn);
        QFileInfo fi(fn);
        translation_path = fi.absolutePath();

        fn = fi.absolutePath() + QString(QDir::separator()) + tr_name + ".qm";
        DBG(3, QDir::toNativeSeparators(fn));

        if (app_translator.load(fn))
        {
            if (DBG::debug_level >= 3) {
                DBG::dbg_print_raw(" - FOUND\n");
            }
            m_tr_name = fn;
            m_tr_path = QDir::toNativeSeparators(translation_path);
            DBG(3, QString("translations: ") + m_tr_path);
            return ;
        }
        if (DBG::debug_level >= 3) {
            DBG::dbg_print_raw(" - not found\n");
        }
    }
    DBG(3);
}

//---------------------------------------------------------
bool TRANSLATOR::install(QTranslator *qt_translator, QTranslator *app_translator) const
{
    auto nm = "qt_" + QLocale::system().name();
    DBG(1, QString("locale system name: \"%1\"").arg(nm));
    auto ok = qt_translator->load(nm, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    if (!ok) {
        DBG(0, QString("can't load \"%1\"").arg(nm));
        return false;
    }
    DBG(1, QString("%1 loaded ok").arg(nm));
    ok = qApp->installTranslator(qt_translator);
    DBG(1, QString("qt translator: %1").arg(ok ? "ok" : "failed"));
    DBG(1, QString("codec name: %1").arg(QTextCodec::codecForLocale()->name().constData()));

    if (!m_tr_name.isEmpty()) {
        if (app_translator->load(m_tr_name)) {
            if (qApp->installTranslator(app_translator)) {
                DBG(1, QString("app translation: ok - %1").arg(QDir::toNativeSeparators(m_tr_name)));
                return true;
            }
        }
    }
    DBG(1, "setup translations is failed");
    return false;
}

//---------------------------------------------------------
const QString &TRANSLATOR::tr_path() const
{
    return m_tr_path;
}

//---------------------------------------------------------
const QString &TRANSLATOR::tr_name() const
{
    return m_tr_name;
}
