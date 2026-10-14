/*!
   \file   ver.cpp
   \brief  Справка по аргументам м версии
   \author Aleksey Lekhtin
   \date   created 18.11.2014
 */

#include <QString>

#include "main.h"
#include "ver.h"

//---------------------------------------------------------
const QString APP::title() const
{
    return m_app_title;
}

//---------------------------------------------------------
const QString APP::version() const
{
    return m_version;
}

//---------------------------------------------------------
const QString APP::version_short() const
{
    return m_version_short;
}

//---------------------------------------------------------
void APP::print_version()
{
    QStringList argv = QCoreApplication::arguments();
    QFileInfo fn(argv[0]);
#ifdef Q_OS_WIN
    if (!fn.suffix().contains(".exe")) fn.setFile("");
#endif
    QString ver = QString("%1%2 %3").arg(m_app_title, fn.fileName(), m_version);
    printf("\n%s\n\n", qPrintable(ver));
    fflush(stdout);
}

//---------------------------------------------------------
void APP::print_help()
{
    QString msg("options:\n"
            #ifndef Q_OS_WIN
                "   -h  --help       - help\n"
                "   -v  --version    - about application version\n"
            #else
                "   /h           - help\n"
                "   /v           - about application version\n"
            #endif
                "\n");

#ifndef Q_OS_WIN
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    QMessageBox::about(this, tr("About Application"),
                       QString("%1\n%2\n\n%3")
                       .arg(m_app_title)
                       .arg(m_version)
                       .arg(msg));
    QApplication::restoreOverrideCursor();
#else
    printf(qPrintable(msg));
    fflush(stdout);
#endif
}
