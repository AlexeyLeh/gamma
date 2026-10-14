#ifndef WMSG_H
#define WMSG_H

#include <QMessageBox>
#include <QString>

class APP;

class WMSG
{
public:
    static void WMsg(APP *app, const QString &msg);
    static void WMsg(APP *app, const QString &title, const QString &msg);
    static void WMsg(APP *app, QMessageBox::Icon icon, const QString &msg);
    static void WMsg(APP *app, QMessageBox::Icon icon, const QString &title, const QString &msg, const QString &filename = {});

    static bool confirmation(APP *app, const QString &title, const QString &ask);
    static const char *secs_to_date_str(uint32_t secs);
};

#endif // WMSG_H
