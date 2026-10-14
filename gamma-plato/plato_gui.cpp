/*!
  \file   plato_gui.cpp
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Отображение информации во вкладке PLATO
*/

#include <string>
#include <assert.h>

#include <QtDebug>
#include <QString>
#include <QTime>
#include <utils.h>

#include "bto.h"
#include "bto_gui.h"
#include "dbg.h"
#include "gamma.h"
#include "main.h"
#include "mainwindow.h"
#include "plato_gui.h"

PLATO_GUI::PLATO_GUI(APP *m) :
    m_app(m),
    m_plato(nullptr),
    m_ico_gray(QIcon(":q_circle_gray.png")),
    m_ico_red(QIcon(":q_circle_red.png")),
    m_ico_green(QIcon(":q_circle_green.png")),
    m_ico_yellow(QIcon(":q_circle_yellow.png"))
{
    m_gui_page = create_plato_page();
    read_settings(m_app->qSettings());

    if (!app()->simulation()) {
        update_part_number(GAMMA::PN_00);
        update_serial_number(app()->SN());
        update_operator(app()->Operator());
    }
}

//---------------------------------------------------------
PLATO_GUI::~PLATO_GUI()
{
    DBG(4);
}

//---------------------------------------------------------
void PLATO_GUI::showstatus(const QString &msg, int sec) const
{
    emit mw()->signal_showstatus(msg, sec);
}

//---------------------------------------------------------
const QString PLATO_GUI::serial_number() const
{
    return m_fld_SN->text();
}

//---------------------------------------------------------
void PLATO_GUI::update_serial_number(const QString &sn)
{
    m_fld_SN->setText(sn);
    m_fld_SN->setToolTip(sn);
}

//---------------------------------------------------------
void PLATO_GUI::update_operator(const QString &oper)
{
    m_fld_oper->setText(oper);
    m_fld_oper->setToolTip(oper);
}

//---------------------------------------------------------
const QString PLATO_GUI::part_number() const
{
    return m_fld_PN->text();
}

//---------------------------------------------------------
void PLATO_GUI::update_part_number(const QString &pn)
{
    m_fld_PN->setText(pn);
    m_fld_PN->setToolTip(pn);
}

//---------------------------------------------------------
void PLATO_GUI::set_backgr(PLATO *plato)
{
    m_plato = plato;
}

//---------------------------------------------------------
TABLE *PLATO_GUI::table()
{
    return m_table;
}

//---------------------------------------------------------
void PLATO_GUI::setup_initial_values(int mode)
{
    set_chart_x_range(m_settings_graph_x_min, m_settings_graph_x_max);
    set_chart_y_range(m_settings_graph_y_min, m_settings_graph_y_max);
    set_chart_y2_range(m_settings_graph_y2_min, m_settings_graph_y2_max);

    DBGF(3, "chart ticks: X:%d Y:%d Y2:%d", m_axisX->tickCount(), m_axisY->tickCount(), m_axisY2->tickCount());

    auto buf = QStringLiteral("%1 %2°C")
                   .arg(tr("The threshold for turns to hot mode is"))
                   .arg(m_plato->hot_mode_temperature());
    m_fld_temper_mode->setToolTip(buf);

    m_fld_temper_mode->setText("--");
    m_fld_HV->setText("--");
    m_fld_CPS->setText("--");
    m_fld_Temper->setText("--");

    m_fld_hv_step->clear();
    if (mode == 0) {
        m_fld_hv_step->setText(QString("%1 %2").arg(m_plato->m_settings_HV_step).arg(tr("V")));
        m_fld_hv_step->setToolTip(QString("%1\n%2 %3").arg(tr("Step of HV increase"))
                                  .arg(m_plato->m_settings_HV_step).arg(tr("V")));
    }

    m_series_HV_lo_limit->clear();
    m_series_HV_lo_limit->append(m_plato->m_hv_lo_limit, m_axisY->min());
    m_series_HV_lo_limit->append(m_plato->m_hv_lo_limit, m_axisY->max());

    m_series_HV_hi_limit->clear();
    m_series_HV_hi_limit->append(m_plato->m_hv_hi_limit, m_axisY->min());
    m_series_HV_hi_limit->append(m_plato->m_hv_hi_limit, m_axisY->max());

    m_series_CPS_lo_limit->clear();
    m_series_CPS_lo_limit->append(m_axisX->min(), m_plato->m_cps_lo_limit);
    m_series_CPS_lo_limit->append(m_axisX->max(), m_plato->m_cps_lo_limit);

    m_series_CPS_hi_limit->clear();
    m_series_CPS_hi_limit->append(m_axisX->min(), m_plato->m_cps_hi_limit);
    m_series_CPS_hi_limit->append(m_axisX->max(), m_plato->m_cps_hi_limit);

    m_fld_tol_HV_LoLimit->setText(QString::number(m_plato->m_hv_lo_limit));
    m_fld_tol_HV_LoLimit->setToolTip(m_fld_tol_HV_LoLimit->text());

    m_fld_tol_HV_HiLimit->setText(QString::number(m_plato->m_hv_hi_limit));
    m_fld_tol_HV_HiLimit->setToolTip(m_fld_tol_HV_HiLimit->text());

    m_fld_tol_HV_Spread->setText(QString::number(m_plato->m_hv_spread_limit));
    m_fld_tol_HV_Spread->setToolTip(m_fld_tol_HV_Spread->text());

    m_fld_tol_CPS_LoLimit->setText(QString::number(m_plato->m_cps_lo_limit));
    m_fld_tol_CPS_LoLimit->setToolTip(m_fld_tol_CPS_LoLimit->text());

    m_fld_tol_CPS_HiLimit->setText(QString::number(m_plato->m_cps_hi_limit));
    m_fld_tol_CPS_HiLimit->setToolTip(m_fld_tol_CPS_HiLimit->text());

    m_fld_tol_CPS_Spread->setText(QString::number(m_plato->m_cps_spread_limit));
    m_fld_tol_CPS_Spread->setToolTip(m_fld_tol_CPS_Spread->text());
}

//---------------------------------------------------------
void PLATO_GUI::show_temperaure_mode(int temperature, const QString &text)
{
    if (temperature > -1) m_fld_Temper->setText(QString::number(temperature));
    m_fld_temper_mode->setText(text);
}

//---------------------------------------------------------
void PLATO_GUI::check_chart_temperature_range(PLATO::tmode_t tmode, int temper)
{
    int tmin = axis_y2_min();
    int tmax = axis_y2_max();
    int changed = 0;
    if (tmode == PLATO::COLD) {
        auto t = (temper / 5) * 5;
        if (t <= tmin + 5)
        {
            tmin = t;
            changed = 1;
        }
        t = ceil((static_cast<double>(temper) / 10.0)) * 10;
        if (t > tmax)
        {
            tmax = t;
            changed = 1;
        }
    } else {
        auto t = ceil((static_cast<double>(temper) / 10.0)) * 10;
        if (t >= tmax - 10)
        {
            tmax = t;
            changed = 1;
        }
        t = (temper / 5) * 5;
        if (t < tmin)
        {
            tmin = t;
            changed = 1;
        }
    }

    if (changed)
    {
        //// int ticks = (tmax - tmin) / 5 + 1;
        set_chart_y2_range(tmin, tmax);
    }
}

//---------------------------------------------------------
void PLATO_GUI::setStyleSheet(QWidget *wgt, const QString &style)
{
    auto fnt = wgt->font();
    wgt->setStyleSheet(style);
    wgt->setFont(fnt);
}

//---------------------------------------------------------
void PLATO_GUI::setStyleSheet(QWidget &wgt, const QString &style)
{
    auto fnt = wgt.font();
    wgt.setStyleSheet(style);
    wgt.setFont(fnt);
}

//---------------------------------------------------------
void PLATO_GUI::clear()
{
    clear_fields();
    clear_plato_table();
    clear_plato_chart(PLATO::COLD);
    clear_plato_chart(PLATO::HOT);
    setup_initial_values();
}

//---------------------------------------------------------
void PLATO_GUI::clear_fields()
{
    // очистка при начале тестирования
    m_fld_est_time->clear();
    m_fld_acq_time->clear();
    m_fld_PN->clear();
    m_fld_SN->clear();
    m_fld_oper->clear();

    m_fld_HV->clear();
    m_fld_hv_step->clear();
    m_fld_CPS->clear();
    m_fld_Temper->clear();
    m_fld_temper_mode->clear();

    m_fld_tol_HV_LoLimit->clear();
    m_fld_tol_HV_HiLimit->clear();
    m_fld_tol_HV_Spread->clear();
    m_fld_tol_CPS_LoLimit->clear();
    m_fld_tol_CPS_HiLimit->clear();
    m_fld_tol_CPS_Spread->clear();

    m_fld_cold_HV_LoBorder->clear();
    m_fld_cold_HV_HiBorder->clear();
    m_fld_cold_HV_Spread->clear();
    m_fld_cold_CPS_LoBorder->clear();
    m_fld_cold_CPS_HiBorder->clear();
    m_fld_cold_CPS_Spread->clear();

    m_fld_hot_HV_LoBorder->clear();
    m_fld_hot_HV_HiBorder->clear();
    m_fld_hot_HV_Spread->clear();
    m_fld_hot_CPS_LoBorder->clear();
    m_fld_hot_CPS_HiBorder->clear();
    m_fld_hot_CPS_Spread->clear();

    // QString style("QLineEdit {background:none; color:normal;} QTipLabel {color:black;}");
    // PLATO_GUI::setStyleSheet(m_fld_cold_HV_Spread, style);
    // PLATO_GUI::setStyleSheet(m_fld_cold_CPS_Spread, style);
    // PLATO_GUI::setStyleSheet(m_fld_hot_HV_Spread, style);
    // PLATO_GUI::setStyleSheet(m_fld_hot_CPS_Spread, style);

    set_empty_label();

    m_progress_bar->reset();
    m_samples_progress_bar->reset();

    m_fld_hv_accepted->clear();
    m_fld_hv_accepted->setToolTip(tr("Calibration Voltage"));

    m_fld_HV_Accepted_Lo->clear();
    m_fld_HV_Accepted_Hi->clear();
    m_fld_HV_Accepted_Spread->clear();
    MW::set_style_sheet(m_fld_HV_Accepted_Spread, "");

    //// m_fld_CPS_Accepted_Lo->clear();
    //// m_fld_CPS_Accepted_Hi->clear();
    m_fld_CPS_Accepted_Spread->clear();
    MW::set_style_sheet(m_fld_CPS_Accepted_Spread, "");

    if (m_button_write_HV) {
        m_button_write_HV->setDisabled(true);
    }

    auto txt = QString("%1 --/--").arg(tr("Sample"));
    m_fld_samples_msg->setText(txt);
}

//---------------------------------------------------------
void PLATO_GUI::clear_plato_chart(PLATO::tmode_t tmode)
{
    m_series_HV_accepted->clear();

    if (tmode == PLATO::COLD)
    {
        m_series_cold_CPS->clear();
        m_series_cold_temper->clear();
        m_series_cold_HV_lo_border->clear();
        m_series_cold_HV_hi_border->clear();
    }
    else // PLATO::HOT
    {
        m_series_hot_CPS->clear();
        m_series_hot_temper->clear();
        m_series_hot_HV_lo_border->clear();
        m_series_hot_HV_hi_border->clear();
    }
}

//---------------------------------------------------------
void PLATO_GUI::clear_plato_table()
{
    m_table->model()->blockSignals(true);
    m_table->blockSignals(true);
    auto sorts = m_table->isSortingEnabled();
    if (sorts) {
        m_table->setSortingEnabled(false);
    }

    QPalette pal = m_table->palette();
    pal.setColor(QPalette::Base, QColor(0xff, 0xff, 0xe1));
    pal.setColor(QPalette::Text, QColor("black"));

    m_table->setRowCount(m_plato->total_steps());
    for(int nrow = 0; nrow < m_table->rowCount(); ++nrow)
    {
        for(int ncol = 0; ncol < m_table->columnCount(); ++ncol)
        {
            auto *item = new TableItem();
            if (ncol == 0) {
               item->setText(QString::number(nrow + 1));
            }
            item->setForeground(pal.brush(QPalette::Text));
            item->setBackground(pal.brush(QPalette::Base));
            item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
            auto fnt = item->font();
            fnt.setBold(true);
            item->setFont(fnt);
            m_table->setItem(nrow, ncol, item);
        }
    }

    m_table->select_row(0);
    m_table->deselect_row(0);

    m_table->blockSignals(false);
    m_table->model()->blockSignals(false);
    m_table->setSortingEnabled(sorts);
    m_table->doItemsLayout();
}

//---------------------------------------------------------
void PLATO_GUI::clear_plato_table(PLATO::tmode_t tmode)
{
    m_table->model()->blockSignals(true);
    m_table->blockSignals(true);
    auto sorts = m_table->isSortingEnabled();
    if (sorts) {
        m_table->setSortingEnabled(false);
    }

    std::vector<int> cols;
    if (tmode == PLATO::COLD)
    {
        cols = {1, 2, 3, 4};
    }
    else // PLATO::HOT
    {
        cols = {5, 6, 7, 8};
    }

    QPalette pal = m_table->palette();
    pal.setColor(QPalette::Base, QColor("white"));
    pal.setColor(QPalette::Text, QColor("black"));

    for(int nrow = 0; nrow < m_table->rowCount(); ++nrow)
    {
        for (int ncol : cols)
        {
            auto *item = m_table->item(nrow, ncol);
            if (item)
            {
                item->setText({});
                item->setToolTip({});
                item->setForeground(pal.brush(QPalette::Text));
                item->setBackground( pal.brush(QPalette::Base));
                item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
            }
        }
    }

    m_table->select_row(0);
    m_table->deselect_row(0);

    m_table->model()->blockSignals(false);
    m_table->blockSignals(false);
    m_table->setSortingEnabled(sorts);
    m_table->doItemsLayout();
}

//---------------------------------------------------------
void PLATO_GUI::slot_run_plato_test()
{
    DBG(3);
    if (m_button_plato_run->isChecked()) {
        auto errorstr = m_plato->report_check_creation();
        if (!errorstr.isEmpty()) {
            m_button_plato_run->setChecked(false);
            display_error_message(errorstr);
            return;
        }
        bool ok = start_plato_test();
        if (!ok) {
            stop_plato_test();
            return;
        }
        showstatus(tr("Test started"), 5);
    } else {
        stop_plato_test();
        m_plato->restore_original_voltage();
        if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
            comm()->disconnect_from();
        }
        set_stop_label();
        auto msg = tr("Test stopped");
        showstatus(msg);
        display_info_message(QMessageBox::Information, msg);
    }
}

//---------------------------------------------------------
bool PLATO_GUI::start_plato_test()
{
    DBG(1);
    if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
        comm()->connect_to();
    }
    if (!app()->simulation()) {
        if (!comm()->isConnected()) {
            auto msg = tr("No connection");
            showstatus(msg);
            emit mw()->signal_message(QMessageBox::Warning, msg);
            return FAIL;
        }
        if (!app()->bto()->gamma_module_found()) {
            auto msg = tr("Module is not found");
            showstatus(msg);
            emit mw()->signal_message(QMessageBox::Warning, msg);
            return FAIL;
        }
    }

    m_plato->start_plato_timer();
    m_button_plato_run->setChecked(true);
    m_button_plato_run->setText(tr("Stop Test"));
    m_button_plato_run->setShortcut(QKeySequence("Ctrl+R"));
    QString tip = QStringLiteral("%1 [%2]").arg(tr("Stop Test"), m_button_plato_run->shortcut().toString());
    m_button_plato_run->setToolTip(tip);

    set_led_icon_idle();
    mw()->lock_pages();
    return OK;
}

//---------------------------------------------------------
void PLATO_GUI::stop_plato_test()
{
    DBG(2);
    m_plato->stop_plato_timer();
    if (app()->simulation()) {
        m_button_plato_run->setEnabled(true);
    } else {
        m_button_plato_run->setChecked(false);
        m_button_plato_run->setText(tr("Start Plateau Test"));
        m_button_plato_run->setShortcut(QKeySequence("Ctrl+R"));
        QString tip = QStringLiteral("%1 [%2]").arg(tr("Start Plateau Test"), m_button_plato_run->shortcut().toString());
        m_button_plato_run->setToolTip(tip);
    }
    m_chart->setAnimationOptions(QChart::NoAnimation);
    m_progress_bar->reset();
    set_led_icon_off();
    app()->bto()->start_bto_timer();
    mw()->unlock_pages();
}

//---------------------------------------------------------
void PLATO_GUI::set_empty_label(const QString &text)
{
    auto style = QString("QLabel { background: none; color: normal; }"
                       "QToolTip { color: black; background: white; }");
    PLATO_GUI::setStyleSheet(m_fld_plato_pass_msg, style);
    if (!text.isEmpty()) {
        m_fld_plato_pass_msg->setText(QStringLiteral(" %1 ").arg(text));
    }else {
        m_fld_plato_pass_msg->setText("---");
    }
    m_fld_plato_pass_msg->setToolTip({});
    m_fld_plato_pass_msg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

//---------------------------------------------------------
void PLATO_GUI::set_init_label()
{
    auto style = QString("QLabel { background: darkblue; color: white; }"
                       "QToolTip { color: black; background: white; }");
    PLATO_GUI::setStyleSheet(m_fld_plato_pass_msg, style);
    m_fld_plato_pass_msg->setText(QStringLiteral(" %1 ").arg(tr("INIT")));
    m_fld_plato_pass_msg->setToolTip({});
    m_fld_plato_pass_msg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

//---------------------------------------------------------
void PLATO_GUI::set_run_label()
{
    auto style = QString("QLabel { background: darkgreen; color: white; }"
                       "QToolTip { color: black; background: white; }");
    PLATO_GUI::setStyleSheet(m_fld_plato_pass_msg, style);
    m_fld_plato_pass_msg->setText(QStringLiteral(" %1 ").arg(tr("RUN")));
    m_fld_plato_pass_msg->setToolTip({});
    m_fld_plato_pass_msg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

//---------------------------------------------------------
void PLATO_GUI::set_stop_label(const QString &text, const QString &tip)
{
    auto style = QString("QLabel { background: yellow; color: black; }"
                       "QToolTip { color: black; background: white; }");
    PLATO_GUI::setStyleSheet(m_fld_plato_pass_msg, style);
    if (text.isEmpty()) {
        m_fld_plato_pass_msg->setText(QStringLiteral(" %1 ").arg(tr("STOP")));
    }else {
        m_fld_plato_pass_msg->setText(QStringLiteral(" %1 ").arg(text));
    }
    if (!tip.isEmpty()) {
        m_fld_plato_pass_msg->setToolTip(tip);
    } else {
        m_fld_plato_pass_msg->setToolTip({});
    }
    m_fld_plato_pass_msg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

//---------------------------------------------------------
void PLATO_GUI::set_err_label()
{
    auto style = QString("QLabel { background: red; color: white; }"
                       "QToolTip { color: black; background: white; }");
    PLATO_GUI::setStyleSheet(m_fld_plato_pass_msg, style);
    m_fld_plato_pass_msg->setText(QStringLiteral(" %1 ").arg(tr("ERR")));
    m_fld_plato_pass_msg->setToolTip({});
    m_fld_plato_pass_msg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

//---------------------------------------------------------
void PLATO_GUI::set_pass_label()
{
    auto style = QString("QLabel { background: darkgreen; color: white; }"
                       "QToolTip { color: black; background: white; }");
    PLATO_GUI::setStyleSheet(m_fld_plato_pass_msg, style);
    m_fld_plato_pass_msg->setText(QStringLiteral(" %1 ").arg(tr("PASS")));
    m_fld_plato_pass_msg->setToolTip({});
    m_fld_plato_pass_msg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

//---------------------------------------------------------
void PLATO_GUI::set_failed_label(const QString &tip)
{
    auto style = QString("QLabel { background: red; color: white; }"
                       "QToolTip { color: black; background: white; }");
    PLATO_GUI::setStyleSheet(m_fld_plato_pass_msg, style);
    m_fld_plato_pass_msg->setText(QStringLiteral(" %1 ").arg(tr("FAILED")));
    if (!tip.isEmpty()) {
        m_fld_plato_pass_msg->setToolTip(tip);
    } else {
        m_fld_plato_pass_msg->setToolTip({});
    }
    m_fld_plato_pass_msg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

//---------------------------------------------------------
void PLATO_GUI::set_passed()
{
    if (m_plato->passed())
    {
        set_pass_label();
        if (m_button_write_HV) {
            m_button_write_HV->setEnabled(true);
        }
        emit mw()->showstatus(tr("PASS"));
        DBG(1, "PASS");
    }
    else
    {
        set_failed_label(m_plato->failed_message());
        if (m_button_write_HV) {
            m_button_write_HV->setEnabled(false);
        }
        display_error_message(m_plato->failed_message(), 0);
        DBG(1, "FAILED");
    }
}

//---------------------------------------------------------
void PLATO_GUI::finishing_test(bool ok)
{
    stop_plato_test();
    m_plato->restore_original_voltage();
    if (ok) {
        set_passed();
        if (!m_plato->passed())
        {
            auto msg = tr("The test has been failed");
            DBG(1, msg);
            display_error_message(msg);
            return;
        }

        auto msg = tr("The test has been passed");
        DBG(1, msg);
        display_info_message(QMessageBox::Information, msg);
    } else {
        set_stop_label();
        auto msg = tr("Test canceled");
        DBG(1, msg);
        showstatus(msg);
    }
    const auto &data = m_plato->data();
    update_field__estimated_time(data.date.toString("dd.MM.yyyy hh:mm"));
    if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
        comm()->disconnect_from();
    }
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------
void PLATO_GUI::requests_completed(int nodecode)
{
    (void)nodecode;
    if (mw()->current_tab() != MW::PAGES::PLATO_PAGE) {
        return;
    }

    DBG(3);
    if (m_button_plato_run->isChecked()) {
        set_led_icon_idle();
    }
    //// QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------
void PLATO_GUI::display_info_message(QMessageBox::Icon icon, const QString &msg, const QString &filename)
{
    if (filename.isEmpty()) {
        emit mw()->signal_message(icon, msg);
        return;
    }
    emit mw()->signal_message(icon, {}, msg, filename);
}

//---------------------------------------------------------
void PLATO_GUI::display_error_message(const QString &msg, int sec)
{
    showstatus(msg, sec);
    emit mw()->signal_message(QMessageBox::Critical, msg);
}

//---------------------------------------------------------
void PLATO_GUI::slot_write_hpvs(bool)
{
    DBG(3);
    if (!comm()->isConnected()) {
        m_button_write_HV->setChecked(false);
        auto msg = tr("No connection");
        showstatus(msg);
        emit mw()->signal_message(QMessageBox::Warning, msg);
        return;
    }
    if (!m_app->bto()->gamma_module_found()) {
        m_button_write_HV->setChecked(false);
        auto msg = tr("Module is not found");
        showstatus(msg);
        emit mw()->signal_message(QMessageBox::Warning, msg);
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_button_write_HV->setChecked(false);
    set_led_icon_active();
    int rtime = comm()->push_request(NodeE4::ReadOperationVoltage());
    mw()->delay(rtime + 500);

    QDialog *dlg = new QDialog(mw());
    dlg->setWindowTitle(tr("Write Operation Voltage"));
    QDialogButtonBox *buttons_group = new QDialogButtonBox(Qt::Horizontal);
    buttons_group->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel);

    QPushButton *btn = buttons_group->button(QDialogButtonBox::Apply);
    connect(btn, SIGNAL(clicked()), dlg, SLOT(accept()));

    btn = buttons_group->button(QDialogButtonBox::Cancel);
    connect(btn, SIGNAL(clicked()), dlg, SLOT(reject()));
    btn->setDefault(true);
    btn->setAutoDefault(true);

    //---------------------------------------------------------
    QLabel *fld_current = new QLabel();
    fld_current->setText(QString::number(m_plato->nodeE4()->m_operating_hv, 10));
    //fld_current->setFrameStyle(QFrame::Box | QFrame::Plain);
    fld_current->setFrameStyle(QFrame::Panel | QFrame::Raised);
    fld_current->setMinimumWidth(10 * 16);

    QSpinBox *fld_wrk = new QSpinBox();
    fld_wrk->setRange(m_plato->m_hv_test_initial, m_plato->m_hv_test_final);
    fld_wrk->setSingleStep(25);
    fld_wrk->setValue(m_plato->m_HV_Accepted);

    //---------------------------------------------------------
    int nrow = 0;
    auto *box1 = new QGroupBox(dlg);
    QGridLayout *grid1 = new QGridLayout(box1);
    //grid1->setColumnStretch(0, 30);
    //grid1->setColumnStretch(1, 70);
    grid1->addWidget(new QLabel(tr("Current Operation Voltage")), nrow, 0);
    grid1->addWidget(fld_current, nrow, 2);
    ++nrow;
    grid1->addWidget(new QFrame(), nrow, 0, 2, 3);
    grid1->setRowMinimumHeight(nrow, 10);
    ++nrow;
    grid1->addWidget(new QLabel(tr("New Operation Voltage")), nrow, 0);
    grid1->addWidget(fld_wrk, nrow, 2);
    ++nrow;
    grid1->addWidget(new QFrame(), nrow, 0, 1, 3);
    grid1->setRowMinimumHeight(nrow, 30);
    ++nrow;
    grid1->addWidget(new QLabel(tr("Are you sure you wish to setup new voltage?")), nrow, 0, 1, 3, Qt::AlignCenter);
    ++nrow;
    grid1->addWidget(new QFrame(), nrow, 0, 1, 3);
    grid1->setRowMinimumHeight(nrow, 20);
    ++nrow;
    grid1->addWidget(buttons_group, nrow, 0, 1, 3);

    //=================================
    auto *layout = new QVBoxLayout();
    layout->addWidget(box1);
    //layout->addWidget(box2);

    //---------------------------------
    QFont fnt = dlg->font();
    fnt.setPointSize(mw()->content_font_pointSize());
    dlg->setFont(fnt);

    dlg->setLayout(layout);
    dlg->adjustSize();
    dlg->setMinimumWidth(400);

    QApplication::restoreOverrideCursor();
    int rc = dlg->exec();
    if (rc != QDialog::Accepted)
    {
        delete dlg;
        return;
    }

    mw()->centralWidget()->blockSignals(true);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    app()->bto_gui()->progress_bar_message_show(tr("Setup new Operating Voltage"));

    ushort value_opr = fld_wrk->value();
    delete dlg;

    set_led_icon_active();
    rtime = comm()->push_requests({
        NodeE4::WriteOperationVoltage(value_opr),
        NodeE4::ReadOperationVoltage(),
        NodeE4::WriteCalibrationVoltages(GAMMA::HV_Min, GAMMA::HV_Max, value_opr),
        NodeE4::ReadCalibrationVoltages(),
    });

    mw()->delay(rtime + 500);

    app()->bto_gui()->progress_bar_message_close();
    mw()->centralWidget()->blockSignals(false);

    m_plato->m_HV_orig = 0; // сбросить для дальнешего заполения по команде 0x43
    int value = m_plato->nodeE4()->m_operating_hv;
    auto msg = QString("%1\n\n%2%3")
              .arg(tr("The operating voltage is set."))
              .arg(value).arg("V");

    QApplication::restoreOverrideCursor();
    display_info_message(QMessageBox::Information, msg);
}

//---------------------------------------------------------
void PLATO_GUI::table_set_record(int nrow, const QStringList &data)
{
    auto data_size = data.size();
    for(int ncol = 0; ncol < m_table->columnCount(); ++ncol)
    {
        if (ncol >= data_size) break;
        if (data.at(ncol).isNull()) continue;
        m_table->set_field_text(nrow, ncol, data.at(ncol));
    }
    m_table->set_field_alignment(nrow, 0, Qt::AlignCenter | Qt::AlignVCenter);

    m_table->set_field_alignment(nrow, 1, Qt::AlignCenter | Qt::AlignVCenter);
    m_table->set_field_alignment(nrow, 2, Qt::AlignRight  | Qt::AlignVCenter);
    m_table->set_field_alignment(nrow, 3, Qt::AlignCenter | Qt::AlignVCenter);
    m_table->set_field_alignment(nrow, 4, Qt::AlignRight  | Qt::AlignVCenter);

    m_table->set_field_alignment(nrow, 5, Qt::AlignCenter | Qt::AlignVCenter);
    m_table->set_field_alignment(nrow, 6, Qt::AlignRight  | Qt::AlignVCenter);
    m_table->set_field_alignment(nrow, 7, Qt::AlignCenter | Qt::AlignVCenter);
    m_table->set_field_alignment(nrow, 8, Qt::AlignRight  | Qt::AlignVCenter);
}

//---------------------------------------------------------
bool PLATO_GUI::ask_about_incorrect_test_voltage(const QString &title, const QString &msg)
{
    QDialog *dlg = new QDialog(mw());
    dlg->setWindowTitle(title);

    QDialogButtonBox *buttons_group = new QDialogButtonBox(Qt::Horizontal);
    buttons_group->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel);

    QPushButton *btn = buttons_group->button(QDialogButtonBox::Apply);
    connect(btn, SIGNAL(clicked()), dlg, SLOT(accept()));
    btn->setText(tr("Continue"));
    btn->setDefault(true);
    btn->setAutoDefault(true);

    btn = buttons_group->button(QDialogButtonBox::Cancel);
    connect(btn, SIGNAL(clicked()), dlg, SLOT(reject()));

    QGridLayout *lay = new QGridLayout();
    lay->addWidget(new QLabel(msg), 0, 0);
    lay->addWidget(new QLabel(),    1, 0);
    lay->addWidget(buttons_group,   2, 0);

    //---------------------------------
    QFont fnt = dlg->font();
    fnt.setPointSize(mw()->content_font_pointSize());
    dlg->setFont(fnt);

    dlg->setLayout(lay);
    dlg->adjustSize();
    dlg->setMinimumWidth(600);

    QApplication::setOverrideCursor(Qt::ArrowCursor);
    int rc = dlg->exec();
    QApplication::restoreOverrideCursor();
    delete dlg;
    if (rc == QDialog::Accepted) return true;
    return false;
}
