/*!
  \file   bto_gui.cpp
  \author Aleksey Lekhtin
  \brief  Отображение информации с БТО
  \date   10.05.2023
*/

#include <QWidget>
#include <QString>
#include <QTimer>

#include "bto.h"
#include "bto_gui.h"
#include "comm.h"
#include "diagn_gui.h"
#include "main.h"
#include "mainwindow.h"
#include "utils.h"

//---------------------------------------------------------
BTO_GUI::BTO_GUI(APP *app) :
    m_app(app),
    m_bto(nullptr),
    m_bar_connection(nullptr),
    m_bar_message(nullptr),
    m_bar_gamma_poweron(nullptr),
    m_bar_apply_settings(nullptr),
    m_button_bto_connect(nullptr),
    m_button_gamma_poweron(nullptr)
{
    m_gui_page = create_bto_page();
    m_gamma_status->setText(BTO_GUI::tr(module_not_powered));
}

//---------------------------------------------------------
BTO_GUI::~BTO_GUI()
{
    DBG(4);
    delete m_bar_message;
    delete m_bar_connection;
    delete m_bar_gamma_poweron;
    delete m_bar_apply_settings;
}

//---------------------------------------------------------
void BTO_GUI::set_backgr(BTO *bto)
{
    m_bto = bto;
}

//---------------------------------------------------------
void BTO_GUI::run()
{
    if (connection_type() != COMM::CONNECT_TYPE::ETHERNET) {
        return;
    }

    QFontMetrics fm = m_gamma_status->fontMetrics();
    int ww = fm.horizontalAdvance("WWWWWWWWWWWW"); // partnumber
    ww = qMax(ww, fm.horizontalAdvance(BTO_GUI::tr(module_not_powered)));
    ww = qMax(ww, fm.horizontalAdvance(BTO_GUI::tr(module_not_found)));
    ww = qMax(ww, fm.horizontalAdvance(BTO_GUI::tr(module_search)));
    m_gamma_status->adjustSize();
    m_gamma_status->setMinimumWidth(ww + 50);

    int w2 = fm.horizontalAdvance("WWWWWWWWWWWW"); // partnumber
    m_field_part_number->setMinimumWidth(w2);
    m_field_gamma_voltage->setMinimumWidth(w2);
    m_field_gamma_temp->setMinimumWidth(w2);
    m_field_gamma_gamma->setMinimumWidth(w2);

    mw()->select_bto_page(1);
    show_bto_settings();
    update_bto_settings_button();

    if (m_bto->bto_autoconnect()) {
        m_button_bto_connect->setChecked(true);
        slot_connect_button();
    }
}

//---------------------------------------------------------
COMM::CONNECT_TYPE BTO_GUI::connection_type() const
{
    return comm()->type();
}

//---------------------------------------------------------
void BTO_GUI::connected_event(const QString &msg)
{
    DBG(2, msg);
    enable_all_controls();
    set_led_icon_active();
    m_button_bto_connect->setText(tr("Disconnect from\nService Unit"));
    m_button_gamma_poweron->setEnabled(true);

    update_bto_settings_button();
    showstatus(QString("%1 %2").arg(tr("Сonnected to"), msg));
    mw()->set_status_bar_label_color(MW::STATUS_BAR_LABEL::GREEN);

    if (m_bto->gamma_autopower()) {
        QTimer::singleShot(100, this, &BTO_GUI::slot_gamma_poweron_button);
    }

    DBGNL(2);
}

//---------------------------------------------------------
void BTO_GUI::disconnected_event(int err, const QString &msg)
{
    if (mw()->shuttingdown()) return;
    // if (err == -1) => no error
    DBG(3, QString("%1 '%2'").arg(err).arg(msg).replace('\n', "; "));
    enable_all_controls();
    update_bto_settings_button();
    set_led_icon_inactive();
    update_gamma_status(BTO::GAMMA_STATUS::NOT_POWERED);

    m_button_bto_connect->setChecked(false);
    m_button_bto_connect->setText(tr("Connect to\nService Unit"));
    m_button_gamma_poweron->setDisabled(true);

    mw()->set_status_bar_label_color((err > 1 ? MW::STATUS_BAR_LABEL::DARK : MW::STATUS_BAR_LABEL::NORMAL));
    progress_bar_connection_close();
    progress_bar_message_close();
    progress_bar_apply_setting_close();
    showstatus();

    if (err > 0) {
        emit mw()->signal_message(QMessageBox::Critical, {}, UTILS::split_message(msg));
    }
}

//---------------------------------------------------------
void BTO_GUI::requests_completed(int nodecode)
{
    (void)nodecode;
    DBG(3);
    if (mw()->current_tab() != MW::PAGES::BTO_PAGE) {
        return;
    }

    progress_bar_apply_setting_close();

    if (comm()->state() == 0) {
        set_led_icon_inactive();
    } else {
        set_led_icon_light_active();
    }
}

//---------------------------------------------------------
void BTO_GUI::set_poweron_button_text()
{
    auto fnt = m_button_gamma_poweron->font();
    if (m_button_gamma_poweron->isChecked()) {
        m_button_gamma_poweron->setStyleSheet("background: #BCCF97"); // green
    }else {
        m_button_gamma_poweron->setStyleSheet("background: normal");
    }
    m_button_gamma_poweron->setFont(fnt);
    QApplication::processEvents();
}

//---------------------------------------------------------
void BTO_GUI::set_gamma_power_button_status(bool status)
{
    DBGF(3, "%s", (status ? "ON" : "OFF"));
    m_button_gamma_poweron->setChecked(status);
    set_poweron_button_text();
}

//---------------------------------------------------------
void BTO_GUI::set_enabled_gamma_power_button(bool status)
{
    DBGF(3, "%s", (status ? "ON" : "OFF"));
    m_button_gamma_poweron->setEnabled(status);
}

//---------------------------------------------------------
void BTO_GUI::update_bto_settings_button()
{
    if (!m_bto->comm()->isConnected())
    {
        DBG(4, "disabled");
        m_button_set_settings->setDisabled(true);
        m_field_set_power_output_sw->setDisabled(true);
        m_field_requested_voltage->setDisabled(true);
        m_field_requested_amperage_limit->setDisabled(true);
        return;
    }

    // если питание модуля включено, то менять уставки напряжения нельзя!
    bool yes = !m_button_gamma_poweron->isChecked();
    DBGF(4, "%s", (yes ? "enabled" : "disabled"));
    m_button_set_settings->setEnabled(yes);
    m_field_set_power_output_sw->setEnabled(yes);
    m_field_requested_voltage->setEnabled(yes);
    m_field_requested_amperage_limit->setEnabled(yes);
}

//---------------------------------------------------------
void BTO_GUI::update_poweron_button_state()
{
    DBG(3);
#if 0
    auto *node = m_bto->nodeBA();
    auto power_input_hardware_state = node->power_input_switch();
    DBGF(3, "NodeBA::PowerInputSwitch::%s",
        (power_input_hardware_state == NodeBA::PowerInputSwitch::ON ? "ON" : "OFF"));
    if (power_input_hardware_state == NodeBA::PowerInputSwitch::ON) {
        m_button_poweron->setChecked(true);
        m_button_poweron->setText(tr(poweron_button_text_off));
        m_button_poweron->setShortcut(QKeySequence("Ctrl+W"));
        auto tip = QString("%1\n%2").arg("Turn power Off").arg(m_button_poweron->shortcut().toString());
        m_button_poweron->setToolTip(tip);
    } else {
        m_button_poweron->setChecked(false);
        m_button_poweron->setText(tr(poweron_button_text_on));
        m_button_poweron->setShortcut(QKeySequence("Ctrl+W"));
        auto tip = QString("%1\n%2").arg("Turn power On").arg(m_button_poweron->shortcut().toString());
        m_button_poweron->setToolTip(tip);

        //m_field_amperage_consumption->setText(0);
        //m_field_power_output_sw->clear();
    }
#endif

#if 0 // TODO управление кнопкой питания?
    auto *node = m_bto->nodeBA();
    auto power_input_hardware_state = node->m_power_input_sw;
    auto gui_state = node->m_power_input_gui_state;
    if (gui_state == NodeBA::PowerInputSwitch::ON)
    {
        DBG(3, "NodeBA::PowerInputSwitch::ON");
        m_button_poweron->setEnabled(true);
        m_button_poweron->setChecked(true);
        //m_field_power_output_sw->setEnabled(true);
        set_power_button_text(true, BTO_GUI::tr(poweron_button_text_off));
        m_button_poweron->setShortcut(QKeySequence("Ctrl+W"));
        auto tip = QString("%1\n%2").arg("Turn power Off").arg(m_button_poweron->shortcut().toString());
        m_button_poweron->setToolTip(tip);
    }
    else
    {
        DBG(3, "NodeBA::PowerInputSwitch::OFF");
        m_button_poweron->setEnabled(true);
        m_button_poweron->setChecked(false);
        set_power_button_text(false, BTO_GUI::tr(poweron_button_text_on));
        m_button_poweron->setShortcut(QKeySequence("Ctrl+W"));
        auto tip = QString("%1\n%2").arg("Turn power On").arg(m_button_poweron->shortcut().toString());
        m_button_poweron->setToolTip(tip);
        m_field_amperage_consumption->setText(0);
        //m_field_power_output_sw->setDisabled(true);
        m_field_power_output_sw->clear();
    }

    auto fnt = m_button_poweron->font();
    if (power_input_hardware_state == NodeBA::PowerInputSwitch::OFF)
        m_button_poweron->setStyleSheet("background: normal");
    else
        m_button_poweron->setStyleSheet("background: #BCCF97"); // green
    m_button_poweron->setFont(fnt);
#endif
}

//---------------------------------------------------------
void BTO_GUI::update_gamma_status(const BTO::GAMMA_STATUS status)
{
    QString s_cur_status;
    if (DBG::debug_level >= 3) {
        QDebug(&s_cur_status) << m_bto->m_module_powering_step;
    }
    m_bto->m_module_powering_step = status;

    if (status == BTO::GAMMA_STATUS::NOT_POWERED)
    {
        m_gamma_status->setText(BTO_GUI::tr(module_not_powered));
        m_bto->gamma_module_pn().clear();
        set_gamma_power_button_status(false);
    }
    else if (status == BTO::GAMMA_STATUS::SEARCH)
    {
        m_gamma_status->setText(BTO_GUI::tr(module_search));
        m_bto->gamma_module_pn().clear();
    }
    else if (status == BTO::GAMMA_STATUS::FOUND)
    {
        m_gamma_status->setText(m_bto->gamma_module_pn() + QStringLiteral("\n"));
        set_gamma_power_button_status(true);
    }
    else if (status == BTO::GAMMA_STATUS::NOT_FOUND)
    {
        m_gamma_status->setText(BTO_GUI::tr(module_not_found));
        m_bto->gamma_module_pn().clear();
        set_gamma_power_button_status(false);
    }

    if (DBG::debug_level >= 3) {
        QString s_new_status;
        QDebug(&s_new_status) << m_bto->m_module_powering_step;
        DBG(3, QString("%1 => %2").arg(s_cur_status).arg(s_new_status));
    }
}

//---------------------------------------------------------
void BTO_GUI::progress_bar_message_show(const QString &msg)
{
    DBG(3, msg);
    std::lock_guard<std::mutex> lock(m_lock_bar2);
    if (!m_bar_message) {
        m_bar_message = new QProgressDialog(msg, "Cancel", 0, 0, mw());
        QProgressBar *customBar = new QProgressBar(m_bar_message);
        customBar->setRange(0, 0);
        customBar->setTextVisible(false);
        m_bar_message->setBar(customBar);
        // if (auto *bar = m_bar_message->findChild<QProgressBar*>()) {
        //     bar->setTextVisible(true);
        // }
        m_bar_message->adjustSize();

        Qt::WindowFlags flags = m_bar_message->windowFlags();
        flags &= ~Qt::WindowContextHelpButtonHint;
        m_bar_message->setWindowFlags(flags);

        m_bar_message->setWindowModality(Qt::WindowModal);
        m_bar_message->setAutoReset(false);
        m_bar_message->setMinimumDuration(0);
        m_bar_message->setMinimumWidth(500);
        //m_bar_message->setCancelButton(nullptr);
        QFont fnt = m_bar_message->font();
        fnt.setPointSize(mw()->content_font_pointSize());
        m_bar_message->setFont(fnt);
    }
    m_bar_message->show();
    m_bar_message->setCancelButton(nullptr);
    m_bar_message->setLabelText(msg);
    m_bar_message->setValue(0);
    m_bar_message->setRange(0, 0);
    auto fnt = m_bar_message->font();
    DBGF(4, "content font point size: %d", mw()->content_font_pointSize());
    fnt.setPointSize(mw()->content_font_pointSize());
    m_bar_message->setFont(fnt);
    m_bar_message->adjustSize();
    m_bar_message->show();
    QApplication::processEvents();
}

//---------------------------------------------------------
void BTO_GUI::progress_bar_message_close()
{
    DBG(4);
    std::lock_guard<std::mutex> lock(m_lock_bar2);
    if (!m_bar_message) {
        return;
    }
    m_bar_message->cancel();
    delete m_bar_message;
    m_bar_message = nullptr;
}

//---------------------------------------------------------
void BTO_GUI::progress_bar_message_settext(const QString &msg)
{
    DBG(4);
    std::lock_guard<std::mutex> lock(m_lock_bar2);
    if (!m_bar_message) {
        return;
    }
    m_bar_message->setLabelText(msg);
}

//// //---------------------------------------------------------
//// void BTO_GUI::progress_bar_gamma_poweron_show()
//// {
////     DBG(3, "Powering the module");
////     if (!m_bar_gamma_poweron) {
////         m_bar_gamma_poweron = new QProgressDialog(tr("Powering the module"), "Cancel", 0, 0, mw());
////         m_bar_gamma_poweron->setWindowModality(Qt::WindowModal);
////         //m_poweron_bar->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint);
////         m_bar_gamma_poweron->setWindowFlags(Qt::Window | Qt::WindowTitleHint);
////         m_bar_gamma_poweron->installEventFilter(this);
////         m_bar_gamma_poweron->setAutoReset(false);
////         m_bar_gamma_poweron->setMinimumDuration(0);
////         m_bar_gamma_poweron->setMinimumWidth(500);
////         m_bar_gamma_poweron->setValue(0);
////         m_bar_gamma_poweron->setRange(0,0);
////
////         QPushButton *cancelButton = m_bar_gamma_poweron->findChild<QPushButton *>();
////         qDebug() << cancelButton;
////         if (cancelButton) {
////             cancelButton->hide(); // убираем кнопку и освобождаем место
////         }
////         //m_poweron_bar->setCancelButton(nullptr);
////
////         QFont fnt = m_bar_gamma_poweron->font();
////         fnt.setPointSize(mw()->content_font_pointSize());
////         m_bar_gamma_poweron->setFont(fnt);
////         //// connect(m_poweron_bar, SIGNAL(canceled()), this, SLOT(progress_bar_poweron_cancel()));
////     }
////     m_bar_gamma_poweron->setValue(0);
////     m_bar_gamma_poweron->setRange(0,0);
////     //m_poweron_bar->setRange(0, comm()->timeout()/10);
////     m_bar_gamma_poweron->show();
////     QApplication::processEvents();
//// }
////
//// //---------------------------------------------------------
//// void BTO_GUI::progress_bar_gamma_poweron_close()
//// {
////     DBG(3);
////     if (m_bar_gamma_poweron) {
////         m_bar_gamma_poweron->cancel();
////     }
//// }

//---------------------------------------------------------
void BTO_GUI::progress_bar_apply_setting_show()
{
    DBG(3, "Applying settings");
    if (!m_bar_apply_settings) {
        m_bar_apply_settings = new QProgressDialog(tr("Applying settings"), "", 0, 0, mw());
        QProgressBar *customBar = new QProgressBar(m_bar_apply_settings);
        customBar->setRange(0, 0);
        customBar->setTextVisible(false);
        m_bar_apply_settings->setBar(customBar);
        // if (auto *bar = m_bar_apply_settings->findChild<QProgressBar*>()) {
        //     bar->setTextVisible(true);
        // }
        m_bar_apply_settings->adjustSize();

        Qt::WindowFlags flags = m_bar_apply_settings->windowFlags();
        flags &= ~Qt::WindowContextHelpButtonHint;
        m_bar_apply_settings->setWindowFlags(flags);

        m_bar_apply_settings->setWindowModality(Qt::WindowModal);
        m_bar_apply_settings->setAutoReset(false);
        m_bar_apply_settings->setMinimumDuration(0);
        m_bar_apply_settings->setMinimumWidth(500);
        m_bar_apply_settings->setCancelButton(nullptr);
        QFont fnt = m_bar_apply_settings->font();
        fnt.setPointSize(mw()->content_font_pointSize());
        m_bar_apply_settings->setFont(fnt);
    }
    m_bar_apply_settings->setValue(0);
    m_bar_apply_settings->setRange(0, 0);
    m_bar_apply_settings->show();
    QApplication::processEvents();
}

//---------------------------------------------------------
void BTO_GUI::progress_bar_apply_setting_close()
{
    DBG(3);
    if (m_bar_apply_settings) {
        m_bar_apply_settings->cancel();
        delete m_bar_apply_settings;
        m_bar_apply_settings = nullptr;
    }
}

//---------------------------------------------------------
void BTO_GUI::progress_bar_connection_show()
{
    DBG(4);
    std::lock_guard<std::mutex> lock(m_lock_bar1);
    if (!m_bar_connection) {
        m_bar_connection = new QProgressDialog(tr("Connection to"), tr("Cancel"), 0, 0, mw());

        QProgressBar *customBar = new QProgressBar(m_bar_connection);
        customBar->setRange(0, 0);
        customBar->setTextVisible(false);
        m_bar_connection->setBar(customBar);
        // if (auto *bar = m_bar_connection->findChild<QProgressBar*>()) {
        //     bar->setTextVisible(true);
        // }
        m_bar_connection->adjustSize();

        Qt::WindowFlags flags = m_bar_connection->windowFlags();
        flags &= ~Qt::WindowContextHelpButtonHint;
        m_bar_connection->setWindowFlags(flags);

        m_bar_connection->setWindowModality(Qt::WindowModal);
        m_bar_connection->setAutoReset(false);
        //m_connection_bar->setMinimumDuration(0);
        m_bar_connection->setMinimumWidth(500);
        m_bar_connection->setValue(0);
        m_bar_connection->setRange(0, 0);
        QFont fnt = m_bar_connection->font();
        fnt.setPointSize(mw()->content_font_pointSize());
        m_bar_connection->setFont(fnt);
    }

    disconnect(m_bar_connection);
    disconnect(&progress_bar_timer);

    connect(m_bar_connection, SIGNAL(canceled()),
            this, SLOT(slot_progress_bar_connection_cancel()));

    connect(&progress_bar_timer, SIGNAL(timeout()),
            this, SLOT(slot_progress_bar_connection_action()));
    DBGF(2, "connection timeout: %d", comm()->timeout());
    auto text = QString("%1 %2").arg(tr("Connection to")).arg(comm()->host());
    m_bar_connection->setLabelText(text);
    m_bar_connection->setCancelButtonText(tr("Cancel"));
    m_connection_bar_value = 0;
    m_bar_connection->show();
    progress_bar_timer.setInterval(10);
    progress_bar_timer.start(10);
    QApplication::processEvents();
}

//---------------------------------------------------------
void BTO_GUI::progress_bar_connection_set_max()
{
    DBG(4);
    std::lock_guard<std::mutex> lock(m_lock_bar1);
    if (m_bar_connection) {
        int num = m_bar_connection->maximum();
        m_bar_connection->setRange(0, num);
        m_bar_connection->setValue(num);
        QApplication::processEvents();
    }
}

//---------------------------------------------------------
void BTO_GUI::progress_bar_connection_close()
{
    DBG(3);
    std::lock_guard<std::mutex> lock(m_lock_bar1);
    progress_bar_timer.stop();
    if (m_bar_connection) {
        DBGF(0, "bar %d, max: %d, interval: %s %d",
             m_connection_bar_value,
             m_bar_connection->maximum(),
             progress_bar_timer.isActive() ? "active" : "stoped",
             progress_bar_timer.interval());
        m_bar_connection->cancel();
        DBGF(0, "bar %d, max: %d, interval: %s %d",
             m_connection_bar_value,
             m_bar_connection->maximum(),
             progress_bar_timer.isActive() ? "active" : "stoped",
             progress_bar_timer.interval());
        delete m_bar_connection;
        m_bar_connection = nullptr;
    }
}

//---------------------------------------------------------
void BTO_GUI::progress_bar_connection_settext_only(const QString &msg)
{
    DBG(3);
    std::lock_guard<std::mutex> lock(m_lock_bar1);
    progress_bar_timer.stop();
    if (m_bar_connection) {
        m_bar_connection->setCancelButton(nullptr);
        m_bar_connection->setRange(0, 0);
        m_bar_connection->setLabelText(msg);
        m_bar_connection->show();
        QApplication::processEvents();
    }
}

//---------------------------------------------------------
void BTO_GUI::slot_progress_bar_connection_cancel()
{
    DBG(3);
    std::lock_guard<std::mutex> lock(m_lock_bar1);
    progress_bar_timer.stop();
    m_bto->cancel_connection();
    m_bar_connection->cancel();
}

//---------------------------------------------------------
void BTO_GUI::slot_progress_bar_connection_action()
{
    DBG(4);
    std::lock_guard<std::mutex> lock(m_lock_bar1);
    if (m_bar_connection) {
        if (m_connection_bar_value < 2) {
            ++m_connection_bar_value;
            return;
        }
        if (m_bar_connection->maximum() == 0) {
            m_bar_connection->setRange(0, comm()->timeout()/10);
        }
        DBGF(0, "bar %d, max: %d, interval: %s %d",
             m_connection_bar_value,
             m_bar_connection->maximum(),
             progress_bar_timer.isActive() ? "active" : "stoped",
             progress_bar_timer.interval());
        if (m_connection_bar_value >= m_bar_connection->maximum()) {
            m_bar_connection->setRange(0, 0);
            return;
        }
        ++m_connection_bar_value;
        m_bar_connection->setValue(m_connection_bar_value);
    }
}

//---------------------------------------------------------
void BTO_GUI::slot_connect_button()
{
    DBG(1);
    if (m_button_bto_connect->isChecked()) {
        disable_all_controls();
        showstatus("");
        m_bto->set_gamma_module_pn("");
        clear_NodeBA_fields();
        clear_nodeE4_fields();
        m_bto->set_gamma_module_kind(BTO::MODULE_KIND::UNKNOWN);
        mw()->update_statusbar_label_text();
        mw()->set_status_bar_label_color(MW::STATUS_BAR_LABEL::NORMAL);
        progress_bar_connection_show();
        m_bto->connect_to();
        return;
    }
    DBG(1, "\n");
    m_bto->disconnect_from();
    mw()->set_status_bar_label_color(MW::STATUS_BAR_LABEL::NORMAL);
    m_app->diagn_gui()->set_enabled_button_set_hi_hv(true);
    m_app->diagn_gui()->set_enabled_button_set_lo_hv(true);
}

//---------------------------------------------------------
void BTO_GUI::slot_gamma_poweron_button()
{
    DBG(1, "\n");
    showstatus();
    m_bto->push_gamma_power_button(m_button_gamma_poweron->isChecked());
}

//---------------------------------------------------------
void BTO_GUI::slot_bto_set_power_output_sw(int index)
{
    if (index < 0) return;
    m_bto->m_settings_power_output_index = index;
}

//---------------------------------------------------------
void BTO_GUI::slot_bto_set_amperage_limit(double value)
{
    m_bto->m_settings_amperage_limit = value;
}

//---------------------------------------------------------
void BTO_GUI::slot_bto_set_voltage(double value)
{
    m_bto->m_settings_voltage_limit = value;
}

//---------------------------------------------------------
void BTO_GUI::slot_bto_set_settings()
{
    bool status = m_button_set_settings->isEnabled();
    m_button_set_settings->setDisabled(true);
    mw()->clearstatus();
    m_bto->push_upload_settings_button();
    m_button_set_settings->setEnabled(status);
    if (comm()->isConnected()) {
        progress_bar_apply_setting_show();
    }
}

//---------------------------------------------------------
bool BTO_GUI::eventFilter(QObject *obj, QEvent *event)
{
    // TODO if (event->type() == QEvent::KeyPress) {
    //          QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    //          if (keyEvent->key() == Qt::Key_Escape) {
    //              return true; // Возвращаем true, чтобы остановить дальнейшую обработку Esc
    //          }
    //      }
    return QObject::eventFilter(obj, event); // Для остальных событий стандартный ход
}
