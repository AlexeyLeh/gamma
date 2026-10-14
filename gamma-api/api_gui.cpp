/*!
  \file   api_gui.cpp
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Отображение информации во вкладке API
*/

#include <string>
#include <assert.h>

#include <QDebug>
#include <QFileInfo>
#include <QString>

#include "api.h"
#include "api_gui.h"
#include "bto.h"
#include "gamma.h"
#include "main.h"
#include "mainwindow.h"
#include "table.h"

//---------------------------------------------------------
API_GUI::API_GUI(APP *m) :
    m_app(m),
    m_api(nullptr),
    m_ico_gray(QIcon(":q_circle_gray.png")),
    m_ico_red(QIcon(":q_circle_red.png")),
    m_ico_green(QIcon(":q_circle_green.png")),
    m_ico_yellow(QIcon(":q_circle_yellow.png"))
{
    m_gui_page = create_api_page();
    read_settings(m_app->qSettings());

    connect(m_fld_table_values_old, &QRadioButton::toggled, this, [this](bool checked) {
        DBG(3, QString("Table OLD checked: %1").arg(checked ? "on": "off"));
        if (checked && m_api) {
            m_api->update_odid_table();
        }
    });

    connect(m_fld_table_values_new, &QRadioButton::toggled, this, [this](bool checked) {
        DBG(3, QString("Table NEW checked: %1").arg(checked ? "on": "off"));
        if (checked && m_api) {
            m_api->update_odid_table();
        }
    });
}

//---------------------------------------------------------
API_GUI::~API_GUI()
{
    DBG(4);
}

//---------------------------------------------------------
void API_GUI::set_backgr(API *api)
{
    m_api = api;
    setup_initial_values();
}

//---------------------------------------------------------
QString API_GUI::version_short() const
{
    return m_app->version_short();
}

//---------------------------------------------------------
bool API_GUI::simulation()
{
    return m_app->simulation();
}

//---------------------------------------------------------
const QString API_GUI::serial_number()
{
    return m_fld_SN->text();
}

//---------------------------------------------------------
void API_GUI::update_serial_number(const QString &sn)
{
    m_fld_SN->setText(sn);
    m_fld_SN->setToolTip(sn);
}

//---------------------------------------------------------
void API_GUI::update_operator(const QString &oper)
{
    m_fld_oper->setText(oper);
    m_fld_oper->setToolTip(oper);
}

//---------------------------------------------------------
const QString API_GUI::part_number()
{
    return m_fld_PN->text();
}

//---------------------------------------------------------
void API_GUI::update_part_number(const QString &pn)
{
    m_fld_PN->setText(pn);
    m_fld_PN->setToolTip(pn);
}

//---------------------------------------------------------
void API_GUI::update_ESKD(const QString &eskd)
{
    Q_UNUSED(eskd);
    //m_fld_ESKD->setText(eskd);
}

//---------------------------------------------------------
const QString API_GUI::ESKD()
{
    //return m_fld_ESKD->text();
    return {};
}

//---------------------------------------------------------
void API_GUI::setup_titles(const API::api_data_t &data)
{
    update_part_number(data.toolpn);
    update_serial_number(data.toolsn);
    update_operator(data.oper);
    update_ESKD(data.eskdnum);

    m_api_cal_factor_label[0]->setText(QString("%1\n[%2]").arg(tr("OLD Method")).arg(m_api->settings_calibr_old()));
    m_api_cal_factor_label[1]->setText(QString("%1\n[%2]").arg(tr("NEW Method")).arg(m_api->settings_calibr_new()));

    m_fld_table_values_old->setText(QString("%1 (%2)").arg(tr("[OLD] Table Values")).arg(m_api->settings_calibr_old()));
    m_fld_table_values_new->setText(QString("%1 (%2)").arg(tr("[NEW] Table Values")).arg(m_api->settings_calibr_new()));
}

//---------------------------------------------------------
void API_GUI::setup_initial_values()
{
    QString str;
    if (!m_api) return;

    // постоянные коэффициенты
    str = QString("%1 - %2").arg(tr("Calibration factor for OLD Method"))
          .arg(QString::number(m_api->m_settings_calibr_old, 'f', 1));
    m_fld_api_cal_factor[0]->setToolTip(str);

    str = QString("%1 - %2").arg(tr("Calibration factor for NEW Method"))
          .arg(QString::number(m_api->m_settings_calibr_new, 'f', 1));
    m_fld_api_cal_factor[1]->setToolTip(str);

    str = QString::number(m_api->m_settings_calibr_min, 'f', 3);
    m_fld_api_cal_factor[2]->setText(str);
    m_fld_api_cal_factor[2]->setToolTip(str);

    str = QString::number(m_api->m_settings_calibr_max, 'f', 3);
    m_fld_api_cal_factor[3]->setText(str);
    m_fld_api_cal_factor[3]->setToolTip(str);

    str = QString::number(m_api->m_settings_tol_deviat0_min, 'f', 2);
    m_fld_api_std_deviat0[2]->setText(str);
    m_fld_api_std_deviat0[2]->setToolTip(str);

    str = QString::number(m_api->m_settings_tol_deviat0_max, 'f', 2);
    m_fld_api_std_deviat0[3]->setText(str);
    m_fld_api_std_deviat0[3]->setToolTip(str);

    str = QString::number(m_api->m_settings_tol_deviat200_min, 'f', 2);
    m_fld_api_std_deviat200[2]->setText(str);
    m_fld_api_std_deviat200[2]->setToolTip(str);

    str = QString::number(m_api->m_settings_tol_deviat200_max, 'f', 2);
    m_fld_api_std_deviat200[3]->setText(str);
    m_fld_api_std_deviat200[3]->setToolTip(str);

    update_box_interpolation_table();
}

//---------------------------------------------------------
void API_GUI::update_field__calibration_factor(bool ok)
{
    QString style;
    if (ok)
        style = QStringLiteral("QLineEdit {background: darkgreen; color: white;}");
    else
        style = QStringLiteral("QLineEdit {background: darkred; color: white;}");

    // 1.  old
    auto str = QString::number(m_api->data().calibr_factor_old, 'f', 3);
    m_fld_api_cal_factor[0]->setText(str);
    //// m_fld_api_cal_factor[0]->setToolTip(str);
    MW::set_style_sheet(m_fld_api_cal_factor[0], style);

    // 2.  new
    str = QString::number(m_api->data().calibr_factor_new, 'f', 3);
    m_fld_api_cal_factor[1]->setText(str);
    //// m_fld_api_cal_factor[1]->setToolTip(str);
    MW::set_style_sheet(m_fld_api_cal_factor[1], style);
}

//---------------------------------------------------------
void API_GUI::update_field__standard_deviation(bool ok)
{
    QString style;
    if (ok)
        style = QStringLiteral("QLineEdit {background: darkgreen; color: white;}");
    else
        style = QStringLiteral("QLineEdit {background: darkred; color: white;}");

    // 1.
    auto str = QString::number(m_api->data().std_deviation_0, 'f', 2);
    m_fld_api_std_deviat0[0]->setText(str);
    m_fld_api_std_deviat0[0]->setToolTip(str);
    MW::set_style_sheet(m_fld_api_std_deviat0[0], style);

    // 2.
    m_fld_api_std_deviat200[0]->setText(str);
    m_fld_api_std_deviat200[0]->setToolTip(str);
    MW::set_style_sheet(m_fld_api_std_deviat200[0], style);

    // 3.
    table1()->set_field_text(0, 1, QString::number(m_api->data().std_deviation_0, 'f', 2));
    table1()->set_field_text(0, 4, QString::number(m_api->data().std_deviation_200, 'f', 2));
}

//---------------------------------------------------------
void API_GUI::showstatus(const QString &msg, int sec) const
{
    emit mw()->signal_showstatus(msg, sec);
}

//---------------------------------------------------------
void API_GUI::slot_run_api_test()
{
    DBGNL(3);
    DBG(3);
    bool checked = m_button_api_run->isChecked();
    if (checked)
    {
        auto errorstr = m_api->protocol_check_creation();
        if (!errorstr.isEmpty())
        {
            m_button_api_run->setChecked(false);
            display_error_message(tr("Can't create report"), errorstr);
            return;
        }

        if (!start_api_test()) {
            return;
        }
        showstatus(tr("Test started"), 5);
    }
    else
    {
        stop_api_test();
        if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
            comm()->disconnect_from();
        }
        set_stop_label();
        auto msg = tr("Test stopped");
        showstatus(msg);
        display_info_message(msg, msg);
    }
}

//---------------------------------------------------------
bool API_GUI::start_api_test()
{
    DBG(2);
    if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
        comm()->connect_to();
    }

    if (!simulation()) {
        if (!comm()->isConnected()) {
            auto msg = tr("No connection");
            showstatus(msg);
            emit mw()->signal_message(QMessageBox::Warning, msg);
            return false;
        }
        if (!app()->bto()->gamma_module_found()) {
            auto msg = tr("Module is not found");
            showstatus(msg);
            emit mw()->signal_message(QMessageBox::Warning, msg);
            return false;
        }
    }

    m_api->start_api_timer();
    m_button_api_run->setChecked(true);
    m_button_api_run->setText(tr("Stop Test"));
    m_button_api_run->setShortcut(QKeySequence("Ctrl+R"));
    QString tip = QStringLiteral("%1 [%2]").arg(tr("Stop Test"), m_button_api_run->shortcut().toString());
    m_button_api_run->setToolTip(tip);
    set_led_icon_idle();
    mw()->lock_pages();
    return true;
}

//---------------------------------------------------------
void API_GUI::stop_api_test()
{
    m_api->stop_api_timer();
    DBG(2);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    //m_fld_api_mode->setText("--");
    m_button_api_run->setChecked(false);
    m_button_api_run->setText(tr("Start API Test"));
    m_button_api_run->setShortcut(QKeySequence("Ctrl+R"));
    QString tip = QStringLiteral("%1 [%2]").arg(tr("Start API Test"), m_button_api_run->shortcut().toString());
    m_button_api_run->setToolTip(tip);
    progress_bar_gamma()->reset();
    set_led_icon_off();
    mw()->unlock_pages();
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------
bool API_GUI::message_about_selected_api_source(API::api_mode_t api_mode) const
{
    QString askmsg;
    if (api_mode == API::api_mode_t::API_0) {
        askmsg = tr("Check the gamma module is in a box\n"
                    "<font size='10'><b>Blank source (0 API)</b></font>");
    }
    else { // API::api_mode_t::API_200
        askmsg = tr("Check the gamma module is in a box\n"
                    "<font size='10'><b>Gamma source (200 API)</b></font>");
    }
    askmsg.replace("\n","<br>");

    //---------------------------------------------------------
    QScopedPointer<QDialog> dlg(new QDialog(mw()));
    dlg->setWindowTitle(tr("Select and confirm"));
    auto buttons_group = MW::make_dialog_two_button(
                             dlg.get(),
                             QString(" %1 ").arg(tr("Confirm test running")),
                             QString(" %1 ").arg(tr("Cancel")));
    QIcon icon = QApplication::style()->standardIcon(QStyle::SP_MessageBoxQuestion);
    //// QPixmap pixmap = icon.pixmap(QSize(32, 32));
    //// auto *ask_icon = new QLabel();
    //// ask_icon->setPixmap(pixmap);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(new QLabel(askmsg));
    layout->addSpacing(10);
    layout->addWidget(buttons_group[0]);

    layout->setAlignment(Qt::AlignHCenter);
    layout->setAlignment(buttons_group[0], Qt::AlignCenter);

    //---------------------------------------------------------
    dlg->setLayout(layout);
    dlg->setWindowIcon(icon);
    QFont fnt = dlg->font();
    fnt.setPointSize(mw()->content_font_pointSize());
    dlg->setFont(fnt);
    dlg->adjustSize();
    dlg->setMinimumWidth(500);

    QApplication::setOverrideCursor(Qt::ArrowCursor);
    int ret = dlg->exec();
    QApplication::restoreOverrideCursor();
    if (ret == QDialog::Accepted)
    {
        auto str = (api_mode == API::api_mode_t::API_0 ? tr("0 API") : tr("200 API"));
        m_fld_api_mode->setText(str);
    } else {
        m_fld_api_mode->setText("--");
    }
    return (ret == QDialog::Accepted ? true : false);
}

//---------------------------------------------------------
void API_GUI::requests_completed(int nodecode)
{
    Q_UNUSED(nodecode);
    if (mw()->current_tab() != MW::PAGES::API_PAGE) {
        return;
    }

    DBG(3);
    if (m_button_api_run->isChecked()) {
        set_led_icon_idle();
    }
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------
void API_GUI::finish_message(bool ok)
{
    if (ok) {
        set_passed();
        if (m_api->passed()) {
            auto msg = tr("The test has been passed");
            DBG(3, msg);
            display_info_message(msg, msg);
        } else {
            auto msg = m_api->data().failed_message;
            DBG(1, msg);
            display_error_message(tr("The test has been failed"), msg);
        }
    } else {
        set_stop_label();
        DBG(3, "canceled");
    }
    const auto &data = m_api->data();
    update_field_estimated_time2(data.date.toString("dd.MM.yyyy hh:mm"));
    if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
        comm()->disconnect_from();
    }
}

//---------------------------------------------------------
void API_GUI::set_init_label()
{
    auto style = QString("QLabel { background: darkblue; color: white; }"
                         "QToolTip { color: black; background: white; }");
    MW::set_style_sheet(m_fld_api_pass_msg, style);
    m_fld_api_pass_msg->setText(QString(" %1 ").arg(tr("INIT")));
    m_fld_api_pass_msg->setToolTip(tr("INIT"));
    m_fld_api_pass_msg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

//---------------------------------------------------------
void API_GUI::set_run_label()
{
    auto style = QString("QLabel { background: darkgreen; color: white; }"
                         "QToolTip { color: black; background: white; }");
    MW::set_style_sheet(m_fld_api_pass_msg, style);
    m_fld_api_pass_msg->setText(QString(" %1 ").arg(tr("RUN")));
    m_fld_api_pass_msg->setToolTip(tr("RUN"));
    m_fld_api_pass_msg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

//---------------------------------------------------------
void API_GUI::set_stop_label()
{
    auto style = QString("QLabel { background: yellow; color: black; }"
                         "QToolTip { color: black; background: white; }");
    MW::set_style_sheet(m_fld_api_pass_msg, style);
    m_fld_api_pass_msg->setText(QString(" %1 ").arg(tr("STOP")));
    m_fld_api_pass_msg->setToolTip(tr("STOP"));
    m_fld_api_pass_msg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

//---------------------------------------------------------
void API_GUI::set_err_label()
{
    auto style = QString("QLabel { background: red; color: white; }"
                         "QToolTip { color: black; background: white; }");
    MW::set_style_sheet(m_fld_api_pass_msg, style);
    m_fld_api_pass_msg->setText(QString(" %1 ").arg(tr("ERR")));
    m_fld_api_pass_msg->setToolTip(tr("ERROR"));
    m_fld_api_pass_msg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

//---------------------------------------------------------
void API_GUI::set_pass_label()
{
    auto style = QString("QLabel { background: darkgreen; color: white; }"
                         "QToolTip { color: black; background: white; }");
    MW::set_style_sheet(m_fld_api_pass_msg, style);
    m_fld_api_pass_msg->setText(QString(" %1 ").arg(tr("PASS")));
    m_fld_api_pass_msg->setToolTip(tr("PASSED"));
    m_fld_api_pass_msg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

//---------------------------------------------------------
void API_GUI::set_failed_label()
{
    auto style = QString("QLabel { background: red; color: white; }"
                         "QToolTip { color: black; background: white; }");
    MW::set_style_sheet(m_fld_api_pass_msg, style);
    m_fld_api_pass_msg->setText(QString(" %1 ").arg(tr("FAILED")));
    m_fld_api_pass_msg->setToolTip(tr("FAILED"));
    m_fld_api_pass_msg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

//---------------------------------------------------------
void API_GUI::set_passed()
{
    if (m_api->data().failed_message.isEmpty())
    {
        set_pass_label();
        showstatus(tr("The test has been passed"), 60);
        m_fld_api_pass_msg->setToolTip(tr("The test has been passed"));
        DBG(1, "PASS");
    }
    else
    {
        set_failed_label();
        showstatus(tr("The test has been failed"), 60);
        m_fld_api_pass_msg->setToolTip(m_api->data().failed_message);
        DBG(1, "FAILED");
    }
}

//---------------------------------------------------------
void API_GUI::display_info_message(const QString &title, const QString &msg, const QString &filename)
{
    if (filename.isEmpty()) {
        emit mw()->signal_message(QMessageBox::Information,{},  msg);
        return;
    }
    emit mw()->signal_message(QMessageBox::Information, title, msg, filename);
}

//---------------------------------------------------------
void API_GUI::display_error_message(const QString &title, const QString &msg, const QString &status_msg)
{
    showstatus(status_msg, 60);
    emit mw()->signal_message(QMessageBox::Critical, title, msg, {});
}

