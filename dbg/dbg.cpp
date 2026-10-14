/**
  \file   dbg.cpp
  \author Aleksey Lekhtin
  \date   2024/02/01
  \brief  debug features
*/


#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include <QDebug>

#include "dbg.h"
#include "thr.h"

#ifdef _WIN32
#define vsnprintf(buf, bufsize, fmt, ap) vsnprintf_s(buf, bufsize, bufsize, fmt, ap)
#endif

int DBG::debug_level = 0;

static pthread_mutex_t dbg_mutex = PTHREAD_MUTEX_INITIALIZER;

//---------------------------------------------------------
void DBG::dbg_lock()
{
    pthread_mutex_lock(&dbg_mutex);
}

//---------------------------------------------------------
void DBG::dbg_unlock()
{
    pthread_mutex_unlock(&dbg_mutex);
}

//---------------------------------------------------------
void DBG::dbg_print(int num, const QString &msg)
{
    DBG::dbg_lock();
    auto dt = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    printf("%s [%d] %s", qPrintable(dt), num, qPrintable(msg));
    fflush(stdout);

    if (debugTextStream.device()) {
        char buf[1024*4];
        snprintf(buf, sizeof(buf), "%s [%d] %s", qPrintable(dt), num, qPrintable(msg));
        debugTextStream << buf;
        debugTextStream.flush();
    }
    DBG::dbg_unlock();
}

//---------------------------------------------------------
void DBG::dbg_print_raw(const QString &msg)
{
    DBG::dbg_lock();
    printf("%s", qPrintable(msg));
    fflush(stdout);
    if (debugTextStream.device()) {
        debugTextStream << msg;
        debugTextStream.flush();
    }
    DBG::dbg_unlock();
}

//---------------------------------------------------------
const char *DBG::funcname(const char *funcname, char *outbuf, int outbuflen)
{
    if (!outbuf || outbuflen < 8) {
        return "";
    }
    static int swidth = DBG::FUNCNAMEWIDTH;
    snprintf(outbuf, outbuflen-4, "%s", funcname);

    const char *p = strstr(outbuf, "::");
    if (p) {
        do {
            if (p == outbuf) {
                p = nullptr;
                break;
            }
            if (*p == ' ') {
                break;
            }
            --p;
        } while(p);
        if (p) {
            memmove(outbuf, p+1, strlen(p+1)+1);
        }
    } else {
        p = strrchr(outbuf, ' ');
        if (p) {
            memmove(outbuf, p+1, strlen(p+1)+1);
        }
    }

    int sz = strlen(outbuf);
    int ln = swidth - sz;
    if (ln < 0) {
        char *p2 = strstr(outbuf, "(");
        if (p2) {
            p2[1] = ')';
            p2[2] = '\0';
        }
    }
    sz = strlen(outbuf);
    ln = swidth - sz;
    if (ln < 0) {
        ln = 0;
        outbuf[sz + 0] = ' ';
        outbuf[sz + 1] = '-';
        outbuf[sz + 2] = ' ';
        outbuf[sz + 3] = '\0';
        return outbuf;
    }

    int i;
    for(i = 0; i < ln; i++) {
        outbuf[sz + i] = ' ';
    }
    outbuf[sz + i] = ' ';
    outbuf[sz + i + 1] = '\0';
    return outbuf;
}

//----------------------------------------------------------
QString     DBG::QDebugOutputFile;
QFile       DBG::QDebug_QFile;
QTextStream DBG::debugTextStream;

//----------------------------------------------------------
void DBG::QDebugMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        //fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        if (DBG::debug_level == 3)
            fprintf(stderr, "%s \t\t(%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        else
            fprintf(stderr, "%s\n", localMsg.constData());
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        abort();
        break;
    }

    if (debugTextStream.device()) {
        debugTextStream << localMsg.constData() << endl;
    }
}
