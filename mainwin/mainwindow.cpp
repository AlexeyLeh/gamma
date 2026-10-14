/*!
  \file   mainwindow.cpp
  \author Aleksey Lekhtin
  \date   created 17.11.2014
  \brief  Главное окно приложения
*/

#include <QWidget>
#include <QWindow>
#include <QMessageBox>

#include "dbg.h"
#include "gamma.h"
#include "utils.h"
#include "ver.h"

#include "api.h"
#include "api_gui.h"
#include "bto.h"
#include "bto_gui.h"
#include "comm.h"
#include "diagn.h"
#include "diagn_gui.h"
#include "main.h"
#include "mainwindow.h"
#include "NodeE4.h"
#include "plato.h"
#include "plato_gui.h"
#include "fw.h"
#include "fw_gui.h"
#include "wmsg.h"

//---------------------------------------------------------
// Main window of application
MW::MW(APP *m) :
    QMainWindow(),
    m_app(m),
    m_status_bar_label(new QLabel())
{
    setAttribute(Qt::WA_QuitOnClose, true);
    setWindowTitle(m_app->title());
    setMinimumSize(1000, 700);

    read_settings(m_app->qSettings());
    get_fonts_size();

    auto fnt = m_status_bar_label->font();
    fnt.setBold(true);
    m_status_bar_label->setFont(fnt);
    statusBar()->addPermanentWidget(m_status_bar_label);

    connect(this, qOverload<QMessageBox::Icon, const QString &>(&MW::signal_message), this,
            [this](QMessageBox::Icon icon, const QString &msg) {
        WMSG::WMsg(app(), icon, {}, UTILS::split_message(msg), {});
    }, Qt::QueuedConnection);

    connect(this, qOverload<QMessageBox::Icon, const QString &, const QString &, const QString &>(&MW::signal_message), this,
            [this](QMessageBox::Icon icon, const QString &title, const QString &msg, const QString &filename) {
        WMSG::WMsg(app(), icon, title, UTILS::split_message(msg), filename);
    }, Qt::QueuedConnection);
}

//---------------------------------------------------------
MW::~MW()
{
    DBG(4);
}

//---------------------------------------------------------
void MW::closeEvent(QCloseEvent *ev)
{
    DBG(3);
    m_shuttingdown = true;
    statusBar()->showMessage(tr("Shutdown..."));
    bto()->stop_bto_timer();
    comm()->clear_requests();
    plato()->restore_original_voltage();
    bto()->disconnect_from();
    ev->accept();
}

//---------------------------------------------------------
bool MW::shuttingdown() const
{
    return m_shuttingdown;
}

//---------------------------------------------------------
void MW::create_environment()
{
    create_action();
    create_menu();
    create_toolbar();
    create_pages();

    connect(comm(), &COMM::signal_comm_comport_connected, this, [this]() {
        DBG(3);
        showstatus_serialport();
    }, Qt::QueuedConnection);

    connect(comm(), SIGNAL(signal_comm_comport_disconnected()),
            this, SLOT(slot_comm__comport_disconnected()), Qt::QueuedConnection);

    connect(this, &MW::signal_showstatus, this, [this](const QString &msg, int sec) {
        showstatus(msg, sec);
    }, Qt::QueuedConnection);
}

//---------------------------------------------------------
QLabel *MW::status_bar_label()
{
    return m_status_bar_label;
}

//---------------------------------------------------------
void MW::set_status_bar_label_color(STATUS_BAR_LABEL mode)
{
    QString style;
    if (mode == MW::STATUS_BAR_LABEL::DARK) {
        style = QString("QLabel { background: darkred; color: white; }");
    } else if (mode == MW::STATUS_BAR_LABEL::GREEN) {
        style = QString("QLabel { background: green;   color: white; }");
    } else {
        style = QString("QLabel { background: normal;  color: normal; }");
    }
    MW::set_style_sheet(m_status_bar_label, style);
    QApplication::processEvents();
}

//---------------------------------------------------------
void MW::update_statusbar_label_text()
{
    auto pn = bto()->gamma_module_pn();
    auto pos = pn.indexOf("ES");
    if (pos != -1) {
        int ln = pn.length() - pos;
        if (ln == 5) {
            bto()->set_gamma_module_kind(BTO::MODULE_KIND::ES);
        }
    }
    if (bto()->gamma_module_variant() == BTO::MODULE_KIND::ES) {
        fw()->gui()->m_fw_but_set_hvps->setToolTip("[42] Read/Write Calibrate HVPS\n"
                                                   "[not stored in non-volatile memory]");
    }

    QString bto_code = QString("%1").arg(bto()->type(), 2, 16, QChar('0')).toUpper();
    QString bto_partnumber = QString("%1").arg(bto()->part_number()).toUpper();
    QString gamma_module_kind = " ";
    if (bto()->gamma_module_variant() == BTO::MODULE_KIND::ES) {
        gamma_module_kind = " ES ";
    }
    QString text = QString(" %1 [%2] %3%4")
                   .arg(comm()->name())
                   .arg(bto_code)
                   .arg(bto_partnumber)
                   .arg(gamma_module_kind);
    DBG(2, text.trimmed());
    status_bar_label()->setText(text);
}

//---------------------------------------------------------
void MW::read_settings(const QSettings *st)
{
    DBG(4);
    m_geometry = st->value("MainWindow").toByteArray();
    restoreGeometry(m_geometry);
    m_last_tab = st->value("LastTab", 0).toInt();

    m_icon_size = st->value("Font/icons_size", "32").toString().left(2).toInt();
    m_menu_font_size = st->value("Font/menu_size", "10").toString().left(2).toInt();
    m_graph_font_size = st->value("Font/graph_size", "10").toString().left(2).toInt();
    m_content_font_size = st->value("Font/content_size", "10").toString().left(2).toInt();
}

//---------------------------------------------------------
void MW::save_settings(QSettings *st)
{
    DBG(4);
    st->setValue("MainWindow", saveGeometry());
    st->setValue("LastTab", m_stacked->currentIndex());

    st->setValue("Font/icons_size",   m_icon_size);
    st->setValue("Font/menu_size",    m_menu_font_size);
    st->setValue("Font/content_size", m_content_font_size);
    st->setValue("Font/graph_size",   m_graph_font_size);
}

//---------------------------------------------------------
void MW::run()
{
    DBG(2);
    if (!m_geometry.isEmpty()) {
        restoreGeometry(m_geometry);
    } else {
        setGeometry(100, 100, 800, 600);
    }

    slot_set_icons_size(m_icon_size);
    slot_set_menu_font_size(m_menu_font_size);
    slot_set_graph_font_size(m_graph_font_size);
    slot_set_content_font_size(m_content_font_size);

    clearstatus();
    if (connection_type() == COMM::CONNECT_TYPE::ETHERNET) {
        m_action__setup_com->setEnabled(false);
        m_action__setup_bto_host->setEnabled(true);
        m_action__bto->setEnabled(true);
        m_page_bto->setEnabled(true);
        m_page_bto->show();
        m_action__sp->setEnabled(false);
        m_toolBar->removeAction(m_action__sp);
        m_toolBar->insertAction(m_action_sep_second, m_action__bto);
        m_stacked->setCurrentWidget(m_page_bto);
        m_action__bto->setChecked(true);
    } else {
        m_action__setup_com->setEnabled(true);
        m_action__setup_bto_host->setEnabled(false);
        m_action__bto->setEnabled(false);
        m_page_bto->setEnabled(false);
        m_page_bto->hide();
        m_toolBar->removeAction(m_action__bto);
        m_toolBar->insertAction(m_action_sep_second, m_action__sp);
        m_action__sp->setEnabled(false);

        if(current_tab() == PAGES::DIAGN_PAGE) {
            slot_select_diagn_page();
        } else if(current_tab() == PAGES::FIRMWARE_PAGE) {
            slot_select_firmware_page();
        } else if(current_tab() == PAGES::PLATO_PAGE) {
            slot_select_plato_page();
        } else if(current_tab() == PAGES::API_PAGE) {
            slot_select_api_page();
        } else {
            slot_select_diagn_page();
        }
    }

    update_statusbar_label_text();
    set_status_bar_label_color(MW::STATUS_BAR_LABEL::NORMAL);
}

//---------------------------------------------------------
void MW::clearstatus()
{
    statusBar()->clearMessage();
}

//---------------------------------------------------------
void MW::set_style_sheet(QWidget *wgt, const QString &style)
{
    auto fnt = wgt->font();
    wgt->setStyleSheet(style);
    wgt->setFont(fnt);
}

//---------------------------------------------------------
void MW::set_style_sheet(QWidget &wgt, const QString &style)
{
#if 0
    auto fnt = wgt.font();
    wgt.setStyleSheet(style);
    wgt.setFont(fnt);
    wgt.updateGeometry();
    wgt.repaint();
#else
    QFont fnt = wgt.font();
    QString font_style = QString("font-family: '%1'; font-size: %2pt;")
                         .arg(fnt.family())
                         .arg(fnt.pointSize());
    if (fnt.bold()) font_style += " font-weight: bold;";
    if (fnt.italic()) font_style += " font-style: italic;";
    QString final_style = style;
    if (final_style.isEmpty()) {
        final_style = QString("QWidget { %1 }").arg(font_style);
    } else {
        final_style = QString("%1 \n QWidget { %2 }").arg(style, font_style);
    }
    wgt.setStyleSheet(final_style);
#endif
}

//---------------------------------------------------------
COMM::CONNECT_TYPE MW::connection_type() const
{
    return comm()->type();
}

//---------------------------------------------------------
void MW::showstatus_serialport()
{
    bool status = (comm()->state() ? true : false);
    DBG(3, QString("%1 state:%2, status:%3")
        .arg(comm()->name()).arg(comm()->state()).arg(comm()->state_string()));
    //m_action__sp->setChecked(comm()->state());
    m_action__sp->setEnabled(comm()->state());

    const port_t & port = comm()->sp()->port_settings();
    QString port_descr = QString("%1 %2").arg(port.sName).arg(port.baudRate);
    QString tip = QString("%1\n%2")
                  .arg(port_descr).arg(status ? tr("connected") : tr("disconnected"));
    m_action__sp->setToolTip(tip);
    tip.replace("\n", " - ");
    showstatus(tip, 60);
    QApplication::processEvents();

    if(comm()->error() > 0)
    {
        QString msg = QString("%1\n%2\n%3\n%4")
                      .arg(tr("Serial Port error"))
                      .arg(port_descr)
                      .arg(comm()->errorString())
                      .arg(comm()->errorReason());
    }
}

//---------------------------------------------------------
void MW::slot_comm__comport_disconnected()
{
    DBG(3, "got signal_comm_comport_disconnected();");
    // if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
    // }
}

//---------------------------------------------------------
/// Пауза c блокировкой интерфейса (без "замораживания" GUI)
void MW::delay(int ms)
{
    if (ms <= 0) return;

    // 1. Блокируем весь пользовательский ввод на уровне ОС для этого окна,
    // но оставляем его способным перерисовываться и обрабатывать сетевые ответы.
    centralWidget()->blockSignals(true);
    //// QApplication::setOverrideCursor(Qt::WaitCursor);

    // 2. Создаем локальный, изолированный цикл обработки событий
    QEventLoop loop;

    // 3. Привязываем таймер: когда он сработает, локальный цикл завершится
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);

    // 4. Запускаем цикл. Программа "замрет" на этой строке,
    // но процессор будет отдыхать (0% нагрузки), а Qt продолжит
    // отправлять/принимать ответы от железа в фоновом режиме!
    loop.exec();

    // 5. Возвращаем всё в исходное состояние
    //// QApplication::restoreOverrideCursor();
    centralWidget()->blockSignals(false);
}

//---------------------------------------------------------
API *MW::api() const
{
    return m_app->api();
}

//---------------------------------------------------------
BTO *MW::bto() const
{
    return m_app->bto();
}

//---------------------------------------------------------
DIAGN *MW::diagn() const
{
    return m_app->diagn();
}

//---------------------------------------------------------
FIRMWARE *MW::fw() const
{
    return m_app->fw();
}

//---------------------------------------------------------
PLATO *MW::plato() const
{
    return m_app->plato();
}

//---------------------------------------------------------
void MW::lock_pages()
{
    DBG(3);
    if (m_app->simulation()) {
        return;
    }

    if (connection_type() == COMM::CONNECT_TYPE::ETHERNET) {
        m_action__bto->setEnabled(false);
    }

    m_action__firmware->setEnabled(false);
    m_action__diagn->setEnabled(false);
    m_action__plato->setEnabled(false);
    if(!app()->simulation()) {
        m_action__save_sim->setEnabled(false);
        m_action__load_sim->setEnabled(false);
    } else {
        m_action__api->setEnabled(false);
        m_action__save_report->setEnabled(false);
        m_action__load_report->setEnabled(false);
        m_action__load_last->setEnabled(false);
    }
}

//---------------------------------------------------------
void MW::unlock_pages()
{
    DBG(4);
    if (connection_type() == COMM::CONNECT_TYPE::ETHERNET) {
        m_action__bto->setEnabled(true);
    }

    m_action__firmware->setEnabled(true);
    m_action__diagn->setEnabled(true);
    m_action__plato->setEnabled(true);
    m_action__api->setEnabled(true);

    if (current_tab() == PAGES::PLATO_PAGE) {
        if (m_action__save_report) {
            m_action__save_report->setEnabled(true);
            m_action__load_report->setEnabled(true);
            m_action__load_last->setEnabled(true);
        }
        if (m_action__save_sim) {
            m_action__save_sim->setEnabled(true);
            m_action__load_sim->setEnabled(true);
        }
    } else if (current_tab() == PAGES::API_PAGE) {
        if (m_action__save_report) {
            m_action__save_report->setEnabled(true);
            m_action__load_report->setEnabled(false);
            m_action__load_last->setEnabled(false);
        }
        if (m_action__save_sim) {
            m_action__save_sim->setEnabled(false);
            m_action__load_sim->setEnabled(false);
        }
    }
}
