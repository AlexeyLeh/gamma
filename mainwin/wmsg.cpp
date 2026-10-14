/*!
  \file   wmsg.cpp
  \author Aleksey Lekhtin
  \date   27.11.2024
  \brief  Window with message
*/

#include <QGridLayout>

#include "cfg.h"
#include "dbg.h"
#include "mainwindow.h"
#include "wmsg.h"

//---------------------------------------------------------
void WMSG::WMsg(APP *app, const QString &msg)
{
    WMsg(app, QMessageBox::Information, {}, msg, {});
}

//---------------------------------------------------------
void WMSG::WMsg(APP *app, const QString &title, const QString &msg)
{
    WMsg(app, QMessageBox::Information, title, msg, {});
}

//---------------------------------------------------------
void WMSG::WMsg(APP *app, QMessageBox::Icon icon, const QString &msg)
{
    WMsg(app, icon, {}, msg, {});
}

//---------------------------------------------------------
void WMSG::WMsg(APP *app, QMessageBox::Icon icon, const QString &title, const QString &msg, const QString &filename)
{
    // поиск, нет ли уже открытого окна с таким именем, чтобы избежать дублирования
    // if (this->findChild<QDialog*>("error_dialog_unique_name")) {
    //     return;
    // }

    QDialog *dlg = new QDialog(app->mw());
    //dlg->setObjectName("error_dialog_unique_name");
    if (!title.isEmpty()) {
        dlg->setWindowTitle(title);
    }

    int font_size = 0;
    if (app && app->mw()) {
        font_size = app->mw()->content_font_pointSize();
        QFont fnt = dlg->font();
        fnt.setPointSize(font_size);
        dlg->setFont(fnt);
        dlg->adjustSize();
    }

    //---------------------------------------------------------
    if (DBG::debug_level >= 3 ) {
        QString buf = QString("%1 %2").arg("MW::WMsg(...)", -DBG::FUNCNAMEWIDTH).arg(msg);
        buf.replace("\n",";");
        buf += "\n\n";
        DBG::dbg_print(DBG::debug_level, buf);
    }

    //---------------------------------------------------------
    auto *label_with_msg = new QLabel();
    int ww = 0;
    int pos = msg.indexOf("<html>");
    if (pos != -1) {
        // вырезать код только с <html>
        QString msg1 = msg.left(pos);
        QString msg2;
        int pos2 = msg.indexOf("</html>");
        if (pos2 == -1) {
            msg2 = msg.mid(pos) + "</html>";
        } else {
            int num = pos2 - pos + 7;
            msg2 = msg.mid(pos, num);
            const QString remainstr = msg.mid(pos + num);
            if (!remainstr.isEmpty()) {
                msg1 += "\n" + remainstr;
            }
        }
        label_with_msg->setText(msg1);
        //label_with_msg->setInformativeText(msg2);
    } else {
        // поиск самой длинной строки
        QStringList lst = msg.split("\n");
        if (lst.size() > 1) {
            int ln = 0;
            int idx = 0;
            for(int i = 0; i < lst.size(); ++i) {
                if (lst[i].length() > ln) {
                    ln = lst[i].length();
                    idx = i;
                }
            }
            QFontMetrics fm = dlg->fontMetrics();
            ww = fm.horizontalAdvance(lst[idx]) + 50;
        }
        label_with_msg->setText(msg);
    }

    ww = std::max(400, ww);
    ww = std::min(800, ww);
    label_with_msg->setMinimumWidth(ww);

    //-------------------------------------------
    QIcon pict_icon;
    QStyle *style = QApplication::style();
    switch (icon) {
        case QMessageBox::Warning:
            pict_icon = style->standardIcon(QStyle::SP_MessageBoxWarning);
            break;
        case QMessageBox::Critical:
            pict_icon = style->standardIcon(QStyle::SP_MessageBoxCritical);
            break;
        case QMessageBox::Question:
            pict_icon = style->standardIcon(QStyle::SP_MessageBoxQuestion);
            break;
        default:
            pict_icon = style->standardIcon(QStyle::SP_MessageBoxInformation);
            break;
    }

    int pict_size = 32;
    switch (font_size) {
        case 10:
            pict_size = 32;
            break;
        case 12:
            pict_size = 48;
            break;
        case 14:
            pict_size = 48;
            break;
        case 16:
            pict_size = 64;
            break;
        default:
            pict_size = 32;
            break;
    }
    QPixmap pixmap = pict_icon.pixmap(QSize(256, 256));
    auto *pict_label = new QLabel();
    pict_label->setPixmap(pixmap.scaledToWidth(pict_size, Qt::SmoothTransformation));
    auto *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(32, 5, 32, 32);
    hlayout->addWidget(pict_label);
    auto *icon_layout = new QVBoxLayout();
    icon_layout->addLayout(hlayout);
    icon_layout->addStretch();

    //---------------------------------------------------------
    auto *button = new QPushButton(QStringLiteral(" %1 ").arg(app->tr("Close")));
    button->setDefault(true);
    button->setAutoDefault(true);
    app->connect(button, &QPushButton::clicked, dlg, &QDialog::accept);

    QPushButton *opendir_button = {};
    if (!filename.isEmpty()) {

        QFont fnt;
        if (app && app->mw()) {
            fnt = app->mw()->font();
        }

        QFileInfo finfo(filename);
        QFontMetrics fm(fnt);
        int iconSize = fm.height() * 1.5;
        int buttonSize = iconSize + 12;

        QIcon opendir_icon(":q_opendir.svg");
        opendir_button = new QPushButton(opendir_icon, {});
        opendir_button->setToolTip(QString("%1\n%2")
                                   .arg(app->tr("Show in folder"), QDir::toNativeSeparators(finfo.fileName())));
        opendir_button->setDefault(false);
        opendir_button->setAutoDefault(false);
        opendir_button->setIconSize(QSize(iconSize, iconSize));
        opendir_button->setFixedSize(buttonSize, buttonSize);
        opendir_button->setFlat(true);
        opendir_button->setStyleSheet(
                    QString("QPushButton {"
                            "   border: none;"
                            "   background-color: transparent;"
                            "   border-radius: %1px;"
                            "}"
                            "QPushButton:hover {"
                            "   background-color: rgba(0, 0, 0, 20);"
                            "}"
                            "QPushButton:pressed {"
                            "   background-color: rgba(0, 0, 0, 40);"
                            "}").arg(buttonSize / 2));

        app->connect(opendir_button, &QPushButton::clicked, [fn = filename]() {
            QString fname = QDir::toNativeSeparators(fn);
#if defined(Q_OS_WIN)
            auto cmd = "explorer.exe";
            auto args = QStringList() << QStringLiteral("/select,") << fname;
            DBG(1, QString("%1 %2").arg(cmd).arg(args.join("")));
            QProcess::startDetached(cmd, args);
#elif defined(Q_OS_LINUX)
            QProcess::startDetached("xdg-open", QStringList() << fname);
#else
            QDesktopServices::openUrl(QUrl::fromLocalFile(fname));
#endif
        });
    }

    QHBoxLayout *button_layout = new QHBoxLayout();
    button_layout->addStretch();
    if (opendir_button) {
        button_layout->addWidget(opendir_button);
    }
    button_layout->addWidget(button);

    QHBoxLayout *msg_layout = new QHBoxLayout();
    msg_layout->addLayout(icon_layout);
    msg_layout->addWidget(label_with_msg);
    msg_layout->addStretch();

    QVBoxLayout *main_layout = new QVBoxLayout();
    main_layout->addLayout(msg_layout);
    main_layout->addSpacing(32);
    main_layout->addLayout(button_layout);

    dlg->setLayout(main_layout);
    dlg->setMinimumWidth(400);
    dlg->adjustSize();

    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    dlg->open();
}

//---------------------------------------------------------
bool WMSG::confirmation(APP *app, const QString &title, const QString &ask)
{
    int font_pnt_size = 0;
    if (app && app->mw()) {
        font_pnt_size = app->mw()->content_font_pointSize();
    }

    auto *msgBox = new QMessageBox(app->mw());
    if (title.isEmpty() && app) {
        msgBox->setWindowTitle(app->title());
    } else {
        msgBox->setWindowTitle(title);
    }
    msgBox->setText(ask);
    msgBox->setIcon(QMessageBox::Question);

    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox->setDefaultButton(QMessageBox::No);

    if (font_pnt_size >0 ) {
        QFont fnt = msgBox->font();
        fnt.setPointSize(font_pnt_size);
        msgBox->setFont(fnt);
        for (QAbstractButton *button : msgBox->buttons()) {
            button->setFont(fnt);
        }
    }

#if 1
    if (auto *textLabel = msgBox->findChild<QLabel*>("qt_msgbox_label")) {
        // textLabel->setStyleSheet("min-width: 500px;");
        textLabel->setMinimumWidth(500);
        textLabel->setWordWrap(true);
    }
#else
    if (auto *layout = qobject_cast<QGridLayout*>(msgBox->layout())) {
        auto *spacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(spacer, layout->rowCount(), 0, 1, layout->columnCount());
    }
#endif

    int ret = msgBox->exec();
    msgBox->deleteLater();

    return (ret == QMessageBox::Yes);
}

//// //---------------------------------------------------------
//// void WMSG::WMsg(APP *app, const QString &title, const QString &msg, QMessageBox::Icon icon)
//// {
////     if (DBG::debug_level >= 3 ) {
////         QString buf = QString("%1 %2").arg("WMSG::WMsg(...)", -DBG::FUNCNAMEWIDTH).arg(msg);
////         buf.replace("\n",";");
////         buf += "\n\n";
////         DBG::dbg_print(DBG::debug_level, buf);
////     }
////     int ww = 0;
////     auto *msg_box = new QMessageBox(parent);
////     msg_box->setIcon(icon);
////     msg_box->setStandardButtons(QMessageBox::Close);
////     msg_box->setDefaultButton(QMessageBox::Close);
////     if (!title.isEmpty()) msg_box->setWindowTitle(title);
////
////     int pos = msg.indexOf("<html>");
////     if (pos == -1) {
////         QStringList lst = msg.split("\n");
////         if (lst.size() > 1) {
////             int ln = 0;
////             int idx = 0;
////             for(int i = 0; i < lst.size(); ++i) {
////                 if (lst[i].length() > ln) {
////                     ln = lst[i].length();
////                     idx = i;
////                 }
////             }
////             QFontMetrics fm = msg_box->fontMetrics();
////             ww = fm.horizontalAdvance(lst.at(idx)) + 50;
////         }
////         msg_box->setText(msg);
////     } else {
////         QString msg1 = msg.left(pos);
////         QString msg2;
////         int pos2 = msg.indexOf("</html>");
////         if (pos2 == -1) {
////             msg2 = msg.mid(pos) + "</html>";
////         } else {
////             int num = pos2 - pos + 7;
////             msg2 = msg.mid(pos, num);
////             const QString remainstr = msg.mid(pos + num);
////             if (!remainstr.isEmpty()) {
////                 msg1 += "\n" + remainstr;
////             }
////         }
////         msg_box->setText(msg1);
////         msg_box->setInformativeText(msg2);
////     }
////
////     auto *layout = (QGridLayout*)msg_box->layout();
////     ww = (ww < 350 ? 350 : ww);
////     ww = (ww > 800 ? 800 : ww);
////     msg_box->adjustSize();
////     layout->setColumnMinimumWidth(2, ww);
////
////     msg_box->adjustSize();
////     msg_box->setMinimumWidth(800);
////
////     ////QApplication::setOverrideCursor(Qt::ArrowCursor);
////     msg_box->exec();
////     ////QApplication::restoreOverrideCursor();
////     delete msg_box;
//// }
