/**
  \file   dbg.h
  \author Aleksey Lekhtin
  \date   2024/02/01
  \brief  debug features
*/

#ifndef DBG__H_
#define DBG__H_

#include <QDateTime>
#include <QFile>
#include <QString>
#include <QTextStream>

namespace DBG {

typedef enum TIME_FMT {
    TIMEONLY = 0,
    DATEONLY,
    DATE_AND_TIME,
    DATE_AND_TIMEMS,
} TIME_FMT;

extern int debug_level;

extern QString     QDebugOutputFile;
extern QFile       QDebug_QFile;
extern QTextStream debugTextStream;
void QDebugMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &_msg_);

void dbg_lock();
void dbg_unlock();

void dbg_print(int num, const QString &_msg_);
void dbg_print_raw(const QString &_msg_);

const int FUNCNAMEWIDTH = 52;
const char *funcname(const char *funcname, char *outbuf, int outbuflen);

//---------------------------------------------------------
#define DBGNoNL(LEVEL, ...)  \
do { \
    if (DBG::debug_level >= LEVEL) { \
        char func_name[1024]; \
        DBG::funcname(__PRETTY_FUNCTION__, func_name, sizeof(func_name)); \
        QString _b_u_f_ = QString(__VA_ARGS__); \
        if (_b_u_f_[0] == '\n' && _b_u_f_[1] != '\0') { \
            DBG::dbg_print_raw(QStringLiteral("\n"));\
            _b_u_f_ = _b_u_f_.mid(1); \
        } \
        _b_u_f_ = QString("L%1 %2%3").arg(__LINE__, 4, 10, QChar('0')).arg(func_name).arg(_b_u_f_); \
        DBG::dbg_print(LEVEL, _b_u_f_); \
    } \
} while(0)

//---------------------------------------------------------
#define DBG(LEVEL, ...)  \
    do { \
        if (DBG::debug_level >= LEVEL) { \
            DBGNoNL(LEVEL, QString(__VA_ARGS__) + QStringLiteral("\n") ); \
        } \
    } while(0)

//---------------------------------------------------------
#define DBGFNoNL(LEVEL, FORMAT, ...)  \
do { \
    if (DBG::debug_level >= LEVEL) { \
        char func_name[1024]; \
        DBG::funcname(__PRETTY_FUNCTION__, func_name, sizeof(func_name)); \
        char _b_u_f_[1024]; \
        snprintf(_b_u_f_, sizeof(_b_u_f_), FORMAT, __VA_ARGS__); \
        int _o_f_f_ = 0; \
        if (_b_u_f_[0] == '\n' && _b_u_f_[1] != '\0') { \
            DBG::dbg_print_raw(QStringLiteral("\n"));\
            _o_f_f_ = 1; \
        } \
        DBG::dbg_print(LEVEL, QString("L%1 ").arg(__LINE__, 4, 10, QChar('0')) + QString(func_name) + QString(_b_u_f_ + _o_f_f_)); \
    } \
} while(0)

//---------------------------------------------------------
#define DBGF(LEVEL, FORMAT, ...) { DBGFNoNL(LEVEL, FORMAT "\n", __VA_ARGS__); }

//---------------------------------------------------------
#define DBGNL(LEVEL) \
do { \
    if (DBG::debug_level >= (LEVEL)) { \
        DBG::dbg_lock(); \
        printf("\n"); \
        fflush(stdout); \
        DBG::dbg_unlock(); \
    } \
} while(0)


} // namespace DBG

#endif // DBG__H_
