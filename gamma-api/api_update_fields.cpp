/*!
  \file   api_update_fields.cpp
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Обработка данных от гамма-модуля
*/

#include <set>

#include <QtDebug>
#include <QString>

#include "api.h"
#include "api_gui.h"
#include "dbg.h"
#include "main.h"
#include "mainwindow.h"
#include "NodeE4.h"
#include "table.h"
#include "utils.h"

//---------------------------------------------------------
void API_GUI::update_fields(byte cmd, byte ext)
{
    Q_UNUSED(ext);
    if (mw()->current_tab() != MW::PAGES::API_PAGE) {
        return;
    }
    DBGF(3, "cmd: %u, ext: %u", cmd, ext);
    switch (cmd)
    {
        case 0x01:                 // Read Gamma Ray Data
            update_field_0x01();
            break;
        case 0x05:                 // Read Temperature Data
            update_field_0x05();
            break;
        case 0x13:                 // Read Gamma Ray Acquisition Time
            update_field_0x13();
            break;
        case 0x43:                 // Read Voltage
            update_field_0x43();
            break;
        case 0x46:                 // Read HV Warm-up
            update_field_0x46();
            break;
        case 0xfc:                 // PartNumber
            update_field_0xfc();
            break;
        default:
            DBG(1, QString("unregistered cmd: 0x%1").arg(cmd, 2, 16, QChar('0')));
            break;
    }
}

//---------------------------------------------------------
void API_GUI::update_field_0x01()
{
    m_fld_api_CPS->setText(QString::number(m_api->nodeE4()->m_gamma_value, 'f', 2));
    m_fld_api_CPS->setToolTip(m_fld_api_CPS->text());
}

//---------------------------------------------------------
void API_GUI::update_field_0x05()
{
    auto *node = m_api->nodeE4();
    m_fld_api_Temp->setText(QString::number(node->m_temperature_value, 'f', 2));
    m_fld_api_Temp->setToolTip(QString("%1°C")
                               .arg(QString::number(node->m_temperature_value, 'f', 2)));
    m_fld_api_hot_mode->setToolTip(QStringLiteral("%1 %2°C")
                               .arg(tr("The threshold for turns to hot mode is"))
                               .arg(m_api->hot_mode_temperature()));
}

//---------------------------------------------------------
void API_GUI::update_field_0x13()
{
    m_api->m_acq_one_gamma = m_api->nodeE4()->m_gamma_acq_time;
}

//---------------------------------------------------------
void API_GUI::update_field_0x43()
{
    auto txt = QString::number(m_api->nodeE4()->m_operating_hv);
    m_fld_api_HV->setText(txt);
    m_fld_api_HV->setToolTip(QString("%1 %2").arg(txt, tr("V")));
}

//---------------------------------------------------------
void API_GUI::update_field_0x46()
{
    m_api->m_e4_warmup_time = m_api->nodeE4()->m_warmup_time;
}

//---------------------------------------------------------
void API_GUI::update_field_0xfc()
{
    QString pn = m_api->nodeE4()->partnumber();
    m_fld_PN->setText(pn);
    m_fld_PN->setToolTip(pn);
}

//---------------------------------------------------------
void API_GUI::update_field_acquire_time(int acq_period, int samples_for_avrg, int acq_total_time)
{
    auto buf = QString("%1 %2").arg(acq_period).arg(tr("sec"));
    if (acq_period > 60) {
        buf = UTILS::secs_to_minute(acq_period);
    }
    m_fld_acq_time->setText(buf);

    buf = QString("(%1x%2)+%3=%4 %5")
          .arg(acq_period)
          .arg(samples_for_avrg)
          .arg(samples_for_avrg)
          .arg(acq_total_time)
          .arg(tr("sec"));
    auto tip = QString("%1\n\n%2 - %3 %4\n%5\n%6")
               .arg(tr("Time of one gamma measurement"), tr("Total time"))
               .arg(acq_total_time)
               .arg(tr("sec"))
               .arg(tr("(One the module acquire time) x (bucket count) + cps getting time"), buf);
    m_fld_acq_time->setToolTip(tip);
}

//---------------------------------------------------------
void API_GUI::update_field_steps__done(int sample, int index, int total)
{
    int percent = (double)(index) / (double)total * 100.0;
    progress_bar_samples()->setValue(percent);

    auto txt = QString("%1 %2/%3").arg(tr("Sample")).arg(sample).arg(total);
    m_fld_samples_msg->setText(txt);

    txt = QString("%1\n%2 - %3")
          .arg(tr("How much samples has been passed already"), tr("Total steps"))
          .arg(total);
    m_fld_samples_msg->setToolTip(txt);
}

//---------------------------------------------------------
void API_GUI::update_field_steps__reset(int total)
{
    progress_bar_samples()->setValue(0);

    auto txt = QString("%1 %2/%3").arg(tr("Sample")).arg(0).arg(total);
    m_fld_samples_msg->setText(txt);

    txt = QString("%1\n%2 - %3")
          .arg(tr("How much samples has been passed already"), tr("Total steps"))
          .arg(total);
    m_fld_samples_msg->setToolTip(txt);
}

//---------------------------------------------------------
void API_GUI::update_field_estimated_time(int remain_sec, int total_sec)
{
    //// QLocale loc(QLocale::system());
    //// auto txt = loc.toDateTime(dt.toString(), QLocale::ShortFormat).toString();

    QDateTime dt = QDateTime::currentDateTime();
    dt = dt.addSecs(remain_sec);
    QString estimated_end__time = dt.toString("hh:mm:ss");

    auto txt = QString("%1\n[%2]").arg(UTILS::secs_to_minute(total_sec)).arg(estimated_end__time);
    m_fld_est_time->setText(txt);
    m_fld_est_time->setToolTip(QString("%1 %2").arg(tr("Estimated time of completion"), txt));
}

//---------------------------------------------------------
void API_GUI::update_field_estimated_time2(const QString &cdate)
{
    DBG(4);
    m_fld_est_time->setText(cdate);
    m_fld_est_time->setToolTip(QString("%1\n%2").arg(tr("Date of calibration"), cdate));
}

//---------------------------------------------------------
void API_GUI::update_field_CPS(const QString &text)
{
    m_fld_api_CPS->setText(text);
}

//---------------------------------------------------------
void API_GUI::update_field_test_mode(const QString &text)
{
    m_fld_api_hot_mode->setText(text);
}

//---------------------------------------------------------
void API_GUI::clear_fields()
{
    // очистка информационных полей
    m_fld_est_time->clear();
    m_fld_acq_time->clear();
    m_fld_api_mode->setText("--");
    m_fld_api_hot_mode->setText("--");
    m_fld_api_HV->clear();
    m_fld_api_CPS->clear();
    m_fld_api_Temp->clear();

    MW::set_style_sheet(m_fld_api_pass_msg, "QLabel {background: none;}");
    m_fld_api_pass_msg->setText("");

    auto txt = QString("%1 --/--").arg(tr("Sample"));
    m_fld_samples_msg->setText(txt);
    progress_bar_gamma()->reset();
    progress_bar_samples()->reset();

    auto style = QString("QLineEdit {background: white; color: black;}");
    for (int idx : {0, 1})
    {
        MW::set_style_sheet(m_fld_api_cal_factor[idx], style);
        m_fld_api_cal_factor[idx]->clear();

        MW::set_style_sheet(m_fld_api_std_deviat0[idx], style);
        m_fld_api_std_deviat0[idx]->clear();

        MW::set_style_sheet(m_fld_api_std_deviat200[idx], style);
        m_fld_api_std_deviat200[idx]->clear();
    }

    // очистка таблиц
    //

    QApplication::setOverrideCursor(Qt::WaitCursor);

    m_table1->model()->blockSignals(true);
    m_table1->blockSignals(true);
    auto sorts = m_table1->isSortingEnabled();
    if (sorts) {
        m_table1->setSortingEnabled(false);
    }

    for (int ncol : {1, 2, 3, 4, 5, 6})
    {
        for (int nrow = 0; nrow < m_table1->rowCount(); ++nrow)
            m_table1->set_field_text(nrow, ncol, {});
    }
    m_table1->select_row(0);
    m_table1->deselect_row(0);

    m_table1->model()->blockSignals(false);
    m_table1->blockSignals(false);
    m_table1->setSortingEnabled(sorts);
    m_table1->doItemsLayout();

    //
    m_table2->model()->blockSignals(true);
    m_table2->blockSignals(true);
    sorts = m_table2->isSortingEnabled();
    if (sorts) {
        m_table2->setSortingEnabled(false);
    }

    for (int ncol = 1; ncol < m_table2->columnCount(); ncol+=2)
    {
        for (int nrow = 0; nrow < m_table2->rowCount(); ++nrow)
            m_table2->set_field_text(nrow, ncol, {});
    }
    m_table2->select_row(0);
    m_table2->deselect_row(0);

    m_table2->model()->blockSignals(false);
    m_table2->blockSignals(false);
    m_table2->setSortingEnabled(sorts);
    m_table2->doItemsLayout();

    QApplication::restoreOverrideCursor();

}
