/*!
  \file   plato.cpp
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Тест линейности гамма-модуля
*/

#include <string>
#include <math.h>

#include <QLocale>
#include <QString>
#include <QByteArray>
#include <conn.h>

#include "bto.h"
#include "bto_gui.h"
#include "comm.h"
#include "main.h"
#include "mainwindow.h"
#include "NodeE4.h"
#include "plato.h"
#include "plato_data.h"
#include "plato_gui.h"
#include "gamma.h"
#include "utils.h"

//---------------------------------------------------------
PLATO::PLATO(APP *m, PLATO_GUI *gui) :
    m_app(m),
    m_gui(gui),
    m_timer_delay(1000)
{
    read_settings(m_app->qSettings());
    m_gui->set_backgr(this);

    // расчет делается здесь, так как далее используется
    m_hv_test_initial  = m_settings_HV_Test_Lo;
    m_hv_test_final    = m_settings_HV_Test_Hi;
    m_total_steps      = (m_hv_test_final - m_hv_test_initial) / m_settings_HV_step;

    m_gui->setup_initial_values(1);
    m_gui->clear_plato_table();

    connect(comm(), SIGNAL(signal_comm_comport_disconnected()),
            this, SLOT(slot_comm__comport_disconnected()), Qt::QueuedConnection);

    connect(comm(), &COMM::signal_comm_requests_completed, this, [this](int nodecode) {
        m_gui->requests_completed(nodecode);
    });

    connect(comm(), &COMM::signal_comm_NodeE4_responded,  this, [this](byte cmd, byte ext) {
        m_gui->update_fields(cmd, ext);
    });

    connect(comm(), SIGNAL(signal_comm_not_responded(NODECODE, QString)),
            this, SLOT(slot_comm__not_responded(NODECODE, QString)));

    connect(&m_plato_timer, &QTimer::timeout, this, &PLATO::slot_plato_test);
}

//---------------------------------------------------------
PLATO::~PLATO()
{
    DBG(4);
}

//---------------------------------------------------------
const QString PLATO::report_path() const
{
    return QCoreApplication::applicationDirPath() + "/reports";
}

//---------------------------------------------------------
int PLATO::total_steps()
{
    return m_total_steps;
}

//---------------------------------------------------------
double PLATO::hot_mode_temperature() const
{
    return m_settings_hot_mode_temper;
}

//---------------------------------------------------------
const QString PLATO::failed_message() const
{
    return m_failed_message;
}

//---------------------------------------------------------
bool PLATO::passed() const
{
    return m_failed_message.isEmpty();
}

//---------------------------------------------------------
void PLATO::start_plato_timer()
{
    m_test_stage = {PLATO::INIT, 0};
    m_plato_timer.start(100);
    gui()->table()->doItemsLayout();
}

//---------------------------------------------------------
void PLATO::stop_plato_timer()
{
    DBG(3);
    comm()->clear_requests();
    m_plato_timer.stop();
    gui()->table()->doItemsLayout();
}

//---------------------------------------------------------
void PLATO::slot_comm__comport_disconnected()
{
    if (mw()->current_tab() != MW::PAGES::PLATO_PAGE) return;
    DBG(3, "got signal_comm_comport_disconnected();");
    m_gui->stop_plato_test();
    m_gui->set_stop_label();
}

//---------------------------------------------------------
void PLATO::slot_comm__not_responded(NODECODE nodeCode, QString err_string)
{
    if (m_gui->mw()->current_tab() != MW::PAGES::PLATO_PAGE) return;
    if (nodeCode != NodeE4::CODExE4) return;
    emit mw()->signal_showstatus(UTILS::split_message(err_string));
    DBG(1, "got signal_comm_not_responded");
    m_gui->stop_plato_test();
    m_gui->set_stop_label();
    emit mw()->signal_message(QMessageBox::Warning, {}, err_string);
}

//---------------------------------------------------------
void PLATO::restore_original_voltage()
{
    // Восстановление значения исходного рабочего высоковольтного напряжения
    if (!m_app->bto()->gamma_module_found()) {
        return;
    }

    DBG(1, QString("original voltages: %1 - %2 - %3, original operating HV was: %4, current operating HV: %5")
        .arg(m_HV_orig_lo).arg(m_HV_orig_tst).arg(m_HV_orig_hi).arg(m_HV_orig, nodeE4()->m_operating_hv));

    if (m_HV_orig < GAMMA::HV_Min || m_HV_orig > GAMMA::HV_Max) {
        return;
    }

    if (m_HV_orig == nodeE4()->m_operating_hv) {
        return;
    }

    auto active = m_plato_timer.isActive();
    m_plato_timer.stop();

    mw()->centralWidget()->blockSignals(true);
    auto msg = tr("Restoring Operating voltage");
    app()->bto_gui()->progress_bar_message_show(msg);
    emit mw()->signal_showstatus(msg, 2);

    int rtime = set_operating_voltage(m_HV_orig);
    mw()->delay(rtime + 500);

    app()->bto_gui()->progress_bar_message_close();
    if (active) {
        m_plato_timer.start(m_timer_delay);
    }
    mw()->centralWidget()->blockSignals(false);
}

//---------------------------------------------------------
void PLATO::update_chart(tmode_t tmode, int index)
{
    // отображение на графике
    Q_ASSERT_X(index > -1 && index < m_total_steps, __func__, "index = step-1;");
    const auto &recs = get_data_recs(tmode);
    if (index < 0 || index >= recs.size()) return;
    int    x = recs[index].m_hv;
    double y = recs[index].m_cps;
    double t = recs[index].m_temper;

    gui()->check_chart_temperature_range(tmode,  (int)t);

    if( y < gui()->axis_y2_min()) {
        gui()->set_chart_y_min(y);
        //set_chart_y_range(m_settings_graph_y_min, m_settings_graph_y_max, m_settings_graph_y_tick);
    }

    if (tmode == PLATO::COLD) {
        if (gui()->m_series_cold_CPS->count() == 0) {
            gui()->m_series_cold_CPS->append(x, y);
        }
        gui()->m_series_cold_CPS->append(x, y);
        gui()->m_series_cold_temper->append(x, t);
    } else {
        // PLATO::HOT
        if (gui()->m_series_hot_CPS->count() == 0) {
            gui()->m_series_hot_CPS->append(x, y);
        }
        gui()->m_series_hot_CPS->append(x, y);
        gui()->m_series_hot_temper->append(x, t);
    }
    DBGF(3, "[%s] [rec:%02d] hv %4d - CPS: %6.2f",
         tmode == PLATO::COLD ? "COLD" : "HOT", index + 1, x, y);
}

//---------------------------------------------------------
void PLATO::update_table(tmode_t tmode, int index, bool select_row)
{
    Q_ASSERT_X(index > -1 && index < m_total_steps, __func__, "index = step-1;");
    const auto &recs = get_data_recs(tmode);
    if (index < 0 || index >= recs.size()) return;
    auto hv     = recs[index].m_hv;
    auto cps    = recs[index].m_cps;
    auto temper = recs[index].m_temper;
    auto slope  = recs[index].m_slope;

    QStringList column_list;
    if (tmode == PLATO::COLD) {
        column_list.append(QString::number(index+1));
        column_list.append(QString::number(hv));
        column_list.append(QString::number(cps, 'f', 2));
        column_list.append(QString::number(temper, 'f', 1));
        column_list.append(QString::number(slope, 'f', 4));
    } else {
        column_list.append(QString::number(index+1));
        column_list.append(QString());
        column_list.append(QString());
        column_list.append(QString());
        column_list.append(QString());
        column_list.append(QString::number(hv));
        column_list.append(QString::number(cps, 'f', 2));
        column_list.append(QString::number(temper, 'f', 1));
        column_list.append(QString::number(slope, 'f', 4));
    }

    gui()->table_set_record(index, column_list);

    if (select_row) {
        gui()->table()->select_row(index);
        gui()->table()->deselect_row(index);
    }

    // CPS
    int ncol = 2;
    if (tmode == PLATO::HOT) {
        ncol = 6;
    }
    if (cps < m_cps_lo_limit) {         // 250
        gui()->table()->set_field_color(index, ncol, Qt::black, QColor(0xf0, 0xf0, 0xf0));
    } else if(cps >= m_cps_hi_limit) {  // 600
        gui()->table()->set_field_color(index, ncol, Qt::white, QColor(0x99, 0x00, 0x00));
    }
    DBGF(3, "[%s] [rec:%02d] hv %4d - CPS: %6.2f [%0.2f - %0.2f], spread: [%0.2f / %0.2f], slope: %.06f",
         tmode == PLATO::COLD ? "COLD" : "HOT", index + 1,
         hv, cps, m_cps_lo_limit, m_cps_hi_limit, m_cold_cps_spread, m_cold_cps_spread, slope);

    // slope
    ncol = 4;
    if (tmode == PLATO::HOT) {
        ncol = 8;
    }

    if (cps > m_cps_lo_limit && cps < m_cps_hi_limit) {
        if (slope > 0) {
            if(slope <= m_settings_slope_min)
            {
                //DBG(3, "set green color");
                gui()->table()->set_field_color(index, ncol, Qt::black,  QColor(0x99, 0xcc, 0x66));
            }
        }
    }
}

//---------------------------------------------------------
void PLATO::update_borders()
{
    DBG(3);
    QString str;
    // результаты по HV
    if (m_cold_hv_lo_border > 0) {
        str = QString::number(m_cold_hv_lo_border);
        gui()->m_fld_cold_HV_LoBorder->setText(str);
        gui()->m_fld_cold_HV_LoBorder->setToolTip(str);
        gui()->m_series_cold_HV_lo_border->clear();
        gui()->m_series_cold_HV_lo_border->append(m_cold_hv_lo_border, 0);
        gui()->m_series_cold_HV_lo_border->append(m_cold_hv_lo_border, gui()->axis_y_max());
    }

    if (m_cold_hv_hi_border > 0) {
        str = QString::number(m_cold_hv_hi_border);
        gui()->m_fld_cold_HV_HiBorder->setText(str);
        gui()->m_fld_cold_HV_HiBorder->setToolTip(str);
        gui()->m_series_cold_HV_hi_border->clear();
        gui()->m_series_cold_HV_hi_border->append(m_cold_hv_hi_border, 0);
        gui()->m_series_cold_HV_hi_border->append(m_cold_hv_hi_border, gui()->axis_y_max());
    }

    if (m_cold_hv_spread > 0) {
        str = QString::number(m_cold_hv_spread);
        gui()->m_fld_cold_HV_Spread->setText(str);
        gui()->m_fld_cold_HV_Spread->setToolTip(str);
    }

    // hot
    if (m_hot_hv_lo_border > 0) {
        str = QString::number(m_hot_hv_lo_border);
        gui()->m_fld_hot_HV_LoBorder->setText(str);
        gui()->m_fld_hot_HV_LoBorder->setToolTip(str);
        gui()->m_series_hot_HV_lo_border->clear();
        auto value = m_hot_hv_lo_border;
        if (abs(m_hot_hv_lo_border - m_cold_hv_lo_border) < 2 || abs(m_hot_hv_lo_border - m_cold_hv_hi_border) < 2) {
            value += 2;
        }
        gui()->m_series_hot_HV_lo_border->append(value, 0);
        gui()->m_series_hot_HV_lo_border->append(value, gui()->axis_y_max());
    }

    if (m_hot_hv_hi_border > 0) {
        str = QString::number(m_hot_hv_hi_border);
        gui()->m_fld_hot_HV_HiBorder->setText(str);
        gui()->m_fld_hot_HV_HiBorder->setToolTip(str);
        gui()->m_series_hot_HV_hi_border->clear();
        auto value = m_hot_hv_hi_border;
        if (abs(m_hot_hv_hi_border - m_cold_hv_hi_border) < 2 || abs(m_hot_hv_hi_border - m_cold_hv_lo_border) < 2) {
            value += 2;
        }
        gui()->m_series_hot_HV_hi_border->append(value, 0);
        gui()->m_series_hot_HV_hi_border->append(value, gui()->axis_y_max());
    }

    if (m_hot_hv_spread > 0) {
        str = QString::number(m_hot_hv_spread);
        gui()->m_fld_hot_HV_Spread->setText(str);
        gui()->m_fld_hot_HV_Spread->setToolTip(str);
    }

    // результаты по CPS
    if (m_cold_cps_lo_border > 0) {
        str = QString::number(m_cold_cps_lo_border, 'f', 0);
        gui()->m_fld_cold_CPS_LoBorder->setText(str);
        gui()->m_fld_cold_CPS_LoBorder->setToolTip(str);
    }

    if (m_cold_cps_hi_border > 0) {
        str = QString::number(m_cold_cps_hi_border, 'f', 0);
        gui()->m_fld_cold_CPS_HiBorder->setText(str);
        gui()->m_fld_cold_CPS_HiBorder->setToolTip(str);
    }

    if (m_cold_cps_spread > 0) {
        str = QString::number(m_cold_cps_spread, 'f', 2);
        gui()->m_fld_cold_CPS_Spread->setText(str);
        gui()->m_fld_cold_CPS_Spread->setToolTip(str);
    }

    // hot
    if (m_hot_cps_lo_border > 0) {
        str = QString::number(m_hot_cps_lo_border, 'f', 0);
        gui()->m_fld_hot_CPS_LoBorder->setText(str);
        gui()->m_fld_hot_CPS_LoBorder->setToolTip(str);
    }

    if (m_hot_cps_hi_border > 0) {
        str = QString::number(m_hot_cps_hi_border, 'f', 0);
        gui()->m_fld_hot_CPS_HiBorder->setText(str);
        gui()->m_fld_hot_CPS_HiBorder->setToolTip(str);
    }

    if (m_hot_cps_spread > 0) {
        str = QString::number(m_hot_cps_spread, 'f', 2);
        gui()->m_fld_hot_CPS_Spread->setText(str);
        gui()->m_fld_hot_CPS_Spread->setToolTip(str);
    }
}

//---------------------------------------------------------
void PLATO::update_spreads()
{
    DBG(3);
    if (m_HV_AcceptedLo > 0 && m_HV_AcceptedHi > 0) {
        gui()->m_fld_HV_Accepted_Lo->setText(QString::number(m_HV_AcceptedLo));
        gui()->m_fld_HV_Accepted_Hi->setText(QString::number(m_HV_AcceptedHi));
        gui()->m_fld_HV_Accepted_Spread->setText(QString::number(m_HV_Spread_Accepted));
        if (m_HV_Spread_Accepted < m_hv_spread_limit) {
            MW::set_style_sheet(gui()->m_fld_HV_Accepted_Spread, "QLineEdit { border: 2px solid red; }");
        } else {
            MW::set_style_sheet(gui()->m_fld_HV_Accepted_Spread, "QLineEdit { border: 2px solid green; }");
        }
    }

    if (m_cold_cps_spread > 0 && m_hot_cps_spread > 0) {
        gui()->m_fld_CPS_Accepted_Spread->setText(QString::number(m_CPS_Spread_Accepted, 'f', 2));
        if (m_CPS_Spread_Accepted < m_cps_spread_limit) {  // 50
            MW::set_style_sheet(gui()->m_fld_CPS_Accepted_Spread, "QLineEdit { border: 2px solid green; }");
        } else {
            MW::set_style_sheet(gui()->m_fld_CPS_Accepted_Spread, "QLineEdit { border: 2px solid red; }");
        }
    }
}

//---------------------------------------------------------
void PLATO::update_failed_message(const QString &msg, const QString &tip)
{
    gui()->m_fld_hv_accepted->setText(msg);
    gui()->m_fld_hv_accepted->setToolTip(tip);
    auto str = tip;
    emit mw()->showstatus(str.replace("\n", ". "));
}

//---------------------------------------------------------
const PLATO::calibration_data_t &PLATO::data() const
{
    if (!app()->simulation()) {
        return m_cal_data;
    }
    return m_sim_data;
}

//---------------------------------------------------------
PLATO::calibration_data_t &PLATO::data()
{
    if (!app()->simulation()) {
        return m_cal_data;
    }
    return m_sim_data;
}

//---------------------------------------------------------
QVector<PLATO_DATA> &PLATO::get_data_recs(tmode_t tmode)
{
    if (!app()->simulation()) {
        if (tmode == PLATO::COLD) {
            return m_cal_data.cold;
        }
        return m_cal_data.hot;
    }

    if (tmode == PLATO::COLD) {
        return m_sim_data.cold;
    }
    return m_sim_data.hot;
}

//---------------------------------------------------------
double PLATO::calculate_slope(tmode_t tmode, int index)
{
    Q_ASSERT_X(index > -1 && index < m_total_steps, __func__, "index = step-1;");
    auto &recs = get_data_recs(tmode);

    int m = m_settings_num_pnt_for_slope;
    if ((index + 1) < m) {
        // еще мало данных для расчетов
        DBGF(3, "[%s] [rec:%02d] hv %4d - MNK::slope - Insufficient data",
             tmode == PLATO::COLD ? "COLD" : "HOT", index + 1, recs[index].m_hv);
        return 0.0;
    }

    int begin = (index+1) - m;
    int end   = index;
    if (begin < 0) {
        return 0.0;
    }

    // MNK
    double sx    = 0.0;
    double sy    = 0.0;
    double sx2   = 0.0;
    double sxy   = 0.0;

    for(int idx = begin; idx <= end; idx++) {
        double x = recs[idx].m_hv;
        double y = recs[idx].m_cps;

        sx  +=  x;
        sy  +=  y;
        sx2 +=  x * x;
        sxy +=  x * y;
    }
    double denominator = m * sx2 - sx * sx;
    if (qFuzzyIsNull(denominator)) {
        recs[index].m_slope = 0.0;
        return 0.0;
    }

    double slope = (m * sxy - sx * sy) / denominator;
    slope = std::abs(slope);
    recs[index].m_slope = slope;
    DBGF(3, "[%s] [rec:%02d] hv %4d - MNK::slope: %0.5f",
         tmode == PLATO::COLD ? "COLD" : "HOT", index + 1, recs[index].m_hv, slope);
    return slope;
}

//---------------------------------------------------------
bool PLATO::calculate_borders(tmode_t tmode, int index)
{
    // ** GAMCFG32 DATA **
    //    ----------------------------------------------------------------------------
    //   | SlopePts | LoOffset | HiOffset | Percent  | LoSlope  | HiSlope  |  CutOff  |
    //   |        5 |        3 |        4 |     0.65 |   0.2000 |   0.5500 |   2.3000 |
    //    ----------------------------------------------------------------------------

    Q_ASSERT_X(index > -1 && index < m_total_steps, __func__, "index = step-1;");
    if (index < 3 || index < m_settings_num_pnt_for_slope) {
        // еще нечего рассчитывать
        return true;
    }

    const auto &recs = get_data_recs(tmode);
    Q_ASSERT_X(!recs.empty(), __func__, "get_data_recs(tmode) empty");
    if (recs.size() == 0) {
        return true;
    }

    int    *HV_low_border   = &m_cold_hv_lo_border;
    int    *HV_high_border  = &m_cold_hv_hi_border;
    int    *HV_spread       = &m_cold_hv_spread;
    double *CPS_low_border  = &m_cold_cps_lo_border;
    double *CPS_high_border = &m_cold_cps_hi_border;
    double *CPS_spread      = &m_cold_cps_spread;

    if (tmode == PLATO::HOT) {
        HV_low_border   = &m_hot_hv_lo_border;
        HV_high_border  = &m_hot_hv_hi_border;
        HV_spread       = &m_hot_hv_spread;
        CPS_low_border  = &m_hot_cps_lo_border;
        CPS_high_border = &m_hot_cps_hi_border;
        CPS_spread      = &m_hot_cps_spread;
    }

    auto hv    = recs.at(index).m_hv;
    auto hv_1  = recs.at(index-1).m_hv;
    auto hv_2  = recs.at(index-2).m_hv;
    auto cps   = recs.at(index).m_cps;
    auto cps_1 = recs.at(index-1).m_cps;
    auto cps_2 = recs.at(index-2).m_cps;
    auto slope = recs.at(index).m_slope;

    QString msg;
    if (DBG::debug_level > 0) {
        msg = QString("[%1] [rec:%2] hv %3 - cps: %4, slope: %5")
              .arg((tmode == PLATO::COLD ? "COLD" : "HOT"))
              .arg(index + 1, 2, 10, QChar('0'))
              .arg(hv, 4)
              .arg(cps, 5, 'f', 2, QChar(u'0'))
              .arg(slope, 7, 'f', 5, QChar(u'0'));
    }

    if (hv < m_hv_test_initial) {           // 850
        DBGF(3, "%s - HV [%d < %d]", qPrintable(msg), hv, m_hv_test_initial);
        return true;                        // нечего еще считать
    }
    if (hv < m_hv_lo_limit) {               // 1000
        DBGF(3, "%s - HV [%d < %d]", qPrintable(msg), hv, m_hv_lo_limit);
        return true;                        // нечего еще считать
    }
    if (hv >= m_hv_hi_limit) {      // 1700
        if (*HV_low_border == 0) {
            m_failed_message = QString("%1 [%2]\n\n%3\n\n[%4 > %5]")
                               .arg(tr("Test Mode"))
                               .arg(tmode == PLATO::COLD ? "COLD" : "HOT")
                               .arg(tr("Low voltage was not detected"))
                               .arg(hv).arg(m_hv_hi_limit);
            auto str = m_failed_message;
            DBG(2, QString("[1] failed message: %1").arg(str.replace("\n", ". ")));
            return false;                   // ошибка, прекращаем тестирование
        }
        if (*HV_high_border == 0) {
            *HV_high_border = hv_1;
            *CPS_high_border = cps_1;
            *HV_spread = *HV_high_border - *HV_low_border;
            *CPS_spread = *CPS_high_border - *CPS_low_border;
            DBGF(1, "[%s] [rec:%02d] hv %4d - [ACCEPTED HIGH HV: %4d] borders: HV[%d - %d], "
                    "CPS[%0.2f - %0.2f  %0.2f]",
                 tmode == PLATO::COLD ? "COLD" : "HOT", index + 1, hv,
                 *HV_high_border, *HV_low_border, *HV_high_border,
                 *CPS_low_border, *CPS_high_border, *CPS_spread);
        }
        return true;                        // идем дальше
    }
    if (hv > m_hv_test_final) {             // 1750
        m_failed_message = QString("%1\n\n[%2 > %3]")
                           .arg(tr("Voltage above high boundary"))
                           .arg(hv).arg(m_hv_test_final);
        auto str = m_failed_message;
        DBG(2, QString("[2] failed message: %1").arg(str.replace("\n", ". ")));
        return false;                       // ошибка, прекращаем тестирование
    }

    if (cps > m_cps_hi_limit) {             // 600
        m_failed_message = QString("%1 [%2 >= %3]\n\n%4 %5 %6")
                           .arg(tr("CPS critically above high limit"))
                           .arg(cps)
                           .arg(m_cps_hi_limit)
                           .arg(tr("Voltage"))
                           .arg(hv).arg(tr("v"));
        auto str = m_failed_message;
        DBG(2, QString("[3] failed message: %1").arg(str.replace("\n", ". ")));
        return false;                       // ошибка, прекращаем тестирование
    }

    //-------------------------------------------
    if (*HV_low_border == 0) {

        if (slope <= m_settings_slope_min) {    // 0.2
            // запомнить начальное граничное значение
            int i;
            for (i = 1; i <= 3; ++i) { // see LoOffset
                if (recs.at(index-i).m_cps < m_cps_lo_limit || recs.at(index-i).m_cps >= m_cps_hi_limit) {
                    DBGF(2, "[%s] [rec:%02d] hv %4d - [HV: %4d CPS: %0.2f] OUT OF BORDERS [%0.2f - %6.2f]",
                         tmode == PLATO::COLD ? "COLD" : "HOT", index + 1, hv,
                         recs[index-i].m_hv, recs[index-i].m_cps, m_cps_hi_limit, m_cps_lo_limit);
                    break;
                }
            }
            if (i == 4) {
                if (hv_2 > m_hv_lo_limit) {
                    *HV_low_border = hv_1;
                    *CPS_low_border = cps_1;
                    DBGF(1, "[%s] [rec:%02d] hv %4d - [ACCEPTED LOW  HV: %4d] borders: HV[%d - %4d], "
                            "CPS[%0.2f - %6.2f  %0.2f]",
                         tmode == PLATO::COLD ? "COLD" : "HOT", index + 1, hv,
                         *HV_low_border, *HV_low_border, *HV_high_border,
                         *CPS_low_border, *CPS_high_border, *CPS_spread);
                }
            }
        }

    } else if (*HV_high_border == 0) {

        if (slope >= m_settings_slope_critical) { // 2.3
            // 2.3 - наклон сильно наклонился
            *HV_high_border  = hv_1;
            *CPS_high_border = cps_1;
            *HV_spread = *HV_high_border - *HV_low_border;
            *CPS_spread = *CPS_high_border - *CPS_low_border;
            DBGF(1, "[%s] [rec:%02d] hv %4d - [ACCEPTED HIGH HV: %4d] borders: HV[%d - %d], "
                    "CPS[%0.2f - %0.2f  %0.2f]",
                 tmode == PLATO::COLD ? "COLD" : "HOT", index + 1, hv,
                 *HV_high_border, *HV_low_border, *HV_high_border,
                 *CPS_low_border, *CPS_high_border, *CPS_spread);
            m_failed_message = QString("%1\n\n[%2 >= %3]\n%4 %5 %6")
                               .arg(tr("Slope is critically above high limit"))
                               .arg(cps)
                               .arg(m_settings_slope_critical)
                               .arg(tr("Voltage"))
                               .arg(hv).arg(tr("v"));
            auto str = m_failed_message;
            DBG(2, QString("[4] failed message: %1").arg(str.replace("\n", ". ")));
            return false;                       // ошибка, прекращаем тестирование
        }

        if (slope >= m_settings_slope_max) {    // 0.55
            // 0.55 - наклон превысил макс. порог
            if (hv_1 > *HV_low_border) {
                *HV_high_border = hv_1;
                *CPS_high_border = cps_1;
            } else {
                *HV_high_border = hv;
                *CPS_high_border = cps;
            }
            *HV_spread = *HV_high_border - *HV_low_border;
            *CPS_spread = *CPS_high_border - *CPS_low_border;
            DBGF(1, "[%s] [rec:%02d] hv %4d - [ACCEPTED HIGH HV: %4d] borders: HV[%d - %d], "
                    "CPS[%0.2f - %0.2f  %0.2f]",
                 tmode == PLATO::COLD ? "COLD" : "HOT", index + 1, hv,
                 *HV_high_border, *HV_low_border, *HV_high_border,
                 *CPS_low_border, *CPS_high_border, *CPS_spread);
            m_failed_message = QString("%1 [%2 > %3]\n%4 %5 %6").arg(tr("Slope is above the limit"))
                               .arg(slope, 0, 'f', 2, QChar(' '))
                               .arg(m_settings_slope_max, 0, 'f', 2, QChar(' '))
                               .arg(tr("Voltage"))
                               .arg(hv).arg(tr("v"));
            auto str = m_failed_message;
            DBG(2, QString("[5] failed message: %1").arg(str.replace("\n", ". ")));
            return false;                   // ошибка, прекращаем тестирование
        }

        if(slope > m_settings_slope_min) {  // 0.2
            // запомнить конечное граничное значение
            if (hv_2 > *HV_low_border) {
                *HV_high_border  = hv_2;
                *CPS_high_border = cps_2;
            }
            else if (hv_1 > *HV_low_border) {
                *HV_high_border  = hv_1;
                *CPS_high_border = cps_1;
            } else {
                *HV_high_border  = hv;
                *CPS_high_border = cps;
            }
            *HV_spread = *HV_high_border - *HV_low_border;
            *CPS_spread = *CPS_high_border - *CPS_low_border;
            DBGF(1, "[%s] [rec:%02d] hv %4d - [ACCEPTED HIGH HV: %4d] borders: HV[%d - %d], "
                    "CPS[%0.2f - %0.2f]  %0.2f",
                 tmode == PLATO::COLD ? "COLD" : "HOT", index + 1, hv,
                 *HV_high_border, *HV_low_border, *HV_high_border,
                 *CPS_low_border, *CPS_high_border, *CPS_spread);
        }
    }

    return true; // идем дальше
}

//---------------------------------------------------------
void PLATO::check_borders()
{
    if (m_cold_hv_lo_border > 0 && m_cold_hv_hi_border > 0) {
        m_cold_hv_spread = m_cold_hv_hi_border - m_cold_hv_lo_border;
    }
    if (m_hot_hv_lo_border > 0 && m_hot_hv_hi_border > 0) {
        m_hot_hv_spread = m_hot_hv_hi_border - m_hot_hv_lo_border;
    }

    if (m_cold_cps_lo_border > 0 && m_cold_cps_hi_border > 0) {
        m_cold_cps_spread = m_cold_cps_hi_border - m_cold_cps_lo_border;
    }

    if (m_hot_cps_lo_border > 0 && m_hot_cps_hi_border > 0) {
        m_hot_cps_spread = m_hot_cps_hi_border - m_hot_cps_lo_border;
    }

    if (DBG::debug_level > 0) {
        const char *_hv_spread_status = "ok";
        const char *_cps_spread_status = "ok";
        if (m_cold_hv_spread < m_hv_spread_limit) {
            _hv_spread_status = "FAILED";
        }
        if (m_cold_cps_spread < m_cps_spread_limit) {
            _cps_spread_status = "FAILED";
        }
        DBGF(1, "HV SPREAD: %d (%s), CPS SPREAD: %0.2f (%s)",
             m_cold_hv_spread, _hv_spread_status,
             m_cold_cps_spread, _cps_spread_status);
    }


    //-------------------------------------------
    // горячее нижнее больше холодного верхнего
    if (m_hot_hv_lo_border > 0 && m_cold_hv_hi_border > 0) {
        if (m_hot_hv_lo_border >= m_cold_hv_hi_border) {
            if (m_failed_message.isEmpty()) {
                m_failed_message = tr("The boundaries between cold and hot do not cross");
                DBGF(1, "HV [%d > %d] - %s", m_hot_hv_lo_border, m_cold_hv_hi_border,  qPrintable(m_failed_message));
            }
        }
    }
    // холодное нижнее больше горячего верхнего
    if (m_hot_hv_hi_border > 0 && m_cold_hv_lo_border > 0) {
        if (m_cold_hv_lo_border > m_hot_hv_hi_border) {
            if (m_failed_message.isEmpty()) {
                m_failed_message = tr("The boundaries between cold and hot do not cross");
                DBGF(1, "HV [%d > %d] - %s", m_cold_hv_lo_border, m_hot_hv_hi_border, qPrintable(m_failed_message));
            }
        }
    }

    //-------------------------------------------
    m_HV_Spread_Accepted = 0;
    m_CPS_Spread_Accepted = 0.0;

    if (m_cold_cps_spread > 0 && m_hot_cps_spread > 0) {
        m_CPS_Spread_Accepted = std::max(m_cold_cps_spread, m_hot_cps_spread);
    }

    // HV Accepted cold
    if (m_cold_hv_lo_border > 0 && m_hot_hv_lo_border > 0) {
        m_HV_AcceptedLo = std::max(m_cold_hv_lo_border, m_hot_hv_lo_border);
    }

    // HV Accepted hot
    if (m_cold_hv_hi_border > 0 && m_hot_hv_hi_border > 0) {
        m_HV_AcceptedHi = std::min(m_cold_hv_hi_border, m_hot_hv_hi_border);
    }

    if (m_HV_AcceptedLo > 0 && m_HV_AcceptedHi > 0) {
        m_HV_Spread_Accepted = m_HV_AcceptedHi - m_HV_AcceptedLo;
    }

    update_spreads();

    if (!m_failed_message.isEmpty()) {
        return;
    }

    //-------------------------------------------
    m_HV_Accepted = 0;

    if (m_HV_Spread_Accepted == 0) {
        m_failed_message = tr("Unable to determine the calibration voltage");
        return;

    }
    if (m_HV_Spread_Accepted < m_hv_spread_limit) {
        m_failed_message = QString("%1\n\n%2 [%3 - %4]\n%5 %6\n\n%7 %8%9\n[%10 < %11]")
                           .arg(tr("Operating voltages are below the spread"))
                           .arg(tr("Accepted voltages"))
                           .arg(m_HV_AcceptedLo)
                           .arg(m_HV_AcceptedHi)
                           .arg(tr("Accepted voltage spread"))
                           .arg(m_HV_Spread_Accepted)
                           .arg(tr("Minimal spread limit"))
                           .arg(m_hv_spread_limit)
                           .arg(tr("V"))
                           .arg(m_HV_Spread_Accepted)
                           .arg(m_hv_spread_limit);
        return;
    }

    // OK
    m_HV_Accepted = m_HV_AcceptedLo + (m_HV_Spread_Accepted * m_settings_HV_Proportion);
    DBGF(1, "HV Accepted: %d (Lo:%d, Hi:%d, Spread: %d) ",
         m_HV_Accepted, m_HV_AcceptedLo, m_HV_AcceptedHi, m_HV_Spread_Accepted);

    if (m_HV_Accepted > 0) {
        auto str = QString("%1 %2").arg(m_HV_Accepted).arg(("V"));
        auto tip = QString("%1 %2 %3\n%4 %5%6\n%7 %8%9")
                   .arg(tr("Calibration Voltage"))
                   .arg(m_HV_Accepted).arg(tr("V"))
                   .arg(tr("Spread"))
                   .arg(m_HV_Spread_Accepted)
                   .arg(tr("V"))
                   .arg(tr("Spread limit"))
                   .arg(m_hv_spread_limit)
                   .arg(tr("V"));
        gui()->m_fld_hv_accepted->setText(str);
        gui()->m_fld_hv_accepted->setToolTip(tip);
        gui()->m_series_HV_accepted->clear();
        gui()->m_series_HV_accepted->append(m_HV_Accepted, 0);
        gui()->m_series_HV_accepted->append(m_HV_Accepted, gui()->axis_y_max());
        return;
    }

    if (m_HV_Accepted == 0)
    {
        if (m_failed_message.isEmpty())
        {
            auto msg  = tr("HV boundaries not detected");
            m_failed_message = msg;
            update_failed_message(QStringLiteral("---"), msg);
        }
    }
}

//---------------------------------------------------------
void PLATO::store_data(tmode_t tmode, int index)
{
    // запоминание значений
    Q_ASSERT_X(index > -1 && index < m_total_steps, __func__, "index = step-1;");
    PLATO_DATA el {
        QDateTime::currentDateTime(),
                nodeE4()->m_operating_hv,
                //nodeE4()->m_gamma_value,
                m_gamma_avrg,
                nodeE4()->m_temperature_value,
                0,
    };
    auto &recs = get_data_recs(tmode);
    recs.append(el);
    DBGF(1, "[%s], recs[%dV].slope: %f",
         (tmode == PLATO::COLD ? "COLD" : "HOT"), recs.last().m_hv, recs.last().m_slope);
}

//---------------------------------------------------------
void PLATO::create_summary(tmode_t tmode, calibration_data_t &data)
{
    bool empty = false;
    if (tmode == PLATO::COLD) {
        empty = data.cold.empty();
    } else {
        empty = data.hot.empty();
    }

    auto avrg_temper = 0.0;
    if (!empty) {
        // making a deep copy especially for sorting!
        QVector<PLATO_DATA> recs;
        if (tmode == PLATO::COLD) {
            recs = data.cold;
        } else {
            recs = data.hot;
        }

        // средняя температура теста
        std::stable_sort(recs.begin(), recs.end(), PLATO_DATA::temp_less_than);
        double sum = 0.0;
        int len = recs.size();
        for(int i = 0; i < len; ++i) {
            sum += recs.at(i).m_temper;
        }
        avrg_temper = sum / len;

        if (recs.last().m_dt > data.date) {
            data.date = recs.last().m_dt;
        }
    }

    if (tmode == PLATO::COLD)
    {
        data.temper_cold_avrg   = avrg_temper;
        data.hv_cold_lo_border  = m_cold_hv_lo_border;
        data.hv_cold_hi_border  = m_cold_hv_hi_border;
        data.hv_cold_spread     = m_cold_hv_spread;
        data.cps_cold_lo_border = m_cold_cps_lo_border;
        data.cps_cold_hi_border = m_cold_cps_hi_border;
        data.cps_cold_spread    = m_cold_cps_spread;
    }
    else
    {
        data.temper_hot_avrg   = avrg_temper;
        data.hv_hot_lo_border  = m_hot_hv_lo_border;
        data.hv_hot_hi_border  = m_hot_hv_hi_border;
        data.hv_hot_spread     = m_hot_hv_spread;
        data.cps_hot_lo_border = m_hot_cps_lo_border;
        data.cps_hot_hi_border = m_hot_cps_hi_border;
        data.cps_hot_spread    = m_hot_cps_spread;
    }
}

//---------------------------------------------------------
int PLATO::set_operating_voltage(int value)
{
    DBGF(1, "set operating HV to %d", value);
    gui()->set_led_icon_active();
    int rtime = comm()->push_request(NodeE4::WriteOperationVoltage(value));
    return rtime;
}

//---------------------------------------------------------
int PLATO::set_working_voltage(int operation_value, int min_value, int tst_value, int max_value)
{
    DBG(1, QString("oper:%1 [min:%2 - tst:%3 - max:%4]")
        .arg(operation_value).arg(min_value).arg(tst_value).arg(max_value));

    gui()->set_led_icon_active();
    int rtime = comm()->push_requests({ NodeE4::WriteCalibrationVoltages(min_value, max_value, tst_value),
                                        NodeE4::WriteOperationVoltage(operation_value),
                                        NodeE4::WriteRecordTask({0x01, 0x25, 0x50, 0x00, 0x04, 0x00}),
                                      });
    return rtime;
}

//---------------------------------------------------------
void PLATO::clear_plato_values(PLATO::tmode_t tmode)
{
    m_HV_AcceptedLo = 0;
    m_HV_AcceptedHi = 0;
    m_HV_Accepted = 0;
    m_HV_Spread_Accepted = 0;
    if (tmode == PLATO::COLD)
    {
        m_cold_hv_lo_border = 0;
        m_cold_hv_hi_border = 0;
        m_cold_hv_spread = 0;
        m_cold_cps_lo_border = 0;
        m_cold_cps_hi_border = 0;
        m_cold_cps_spread = 0.0;
    }
    else
    {
        m_hot_hv_lo_border = 0;
        m_hot_hv_hi_border = 0;
        m_hot_hv_spread = 0;
        m_hot_cps_lo_border = 0;
        m_hot_cps_hi_border = 0;
        m_hot_cps_spread = 0.0;
    }
}

//---------------------------------------------------------
void PLATO::clear_data_recs(tmode_t tmode, calibration_data_t &data)
{
    if (tmode == PLATO::COLD)
        data.cold.clear();
    else
        data.hot.clear();
}

//---------------------------------------------------------
void PLATO::create_summary()
{
    auto &data = this->data();

    data.failed_message = m_failed_message;
    data.toolname       = QStringLiteral("Gamma-Sonde");
    data.acqtime        = m_acq_sample_time;
    data.location       = "";

    // data.toolpn = nodeE4()->partnumber();
    // data.toolsn      = data.toolsn;
    // data.oper        = data.oper;

    data.hv_lo_limit    = m_hv_lo_limit;
    data.hv_hi_limit    = m_hv_hi_limit;
    data.hv_sp_limit    = m_hv_spread_limit;

    data.cps_lo_limit   = m_cps_lo_limit;
    data.cps_hi_limit   = m_cps_hi_limit;
    data.cps_sp_limit   = m_cps_spread_limit;

    data.chart_cps_lo   = gui()->axis_y_min();
    data.chart_cps_hi   = gui()->axis_y_max();
    data.chart_cps_tick = gui()->axis_y_ticks();

    data.chart_hv_lo    = gui()->axis_x_min();
    data.chart_hv_hi    = gui()->axis_x_max();
    data.chart_hv_tick  = gui()->axis_x_ticks();

    create_summary(PLATO::COLD, data);
    create_summary(PLATO::HOT, data);

    data.hv_orig         = m_HV_orig;
    data.hv_accepted_lo  = m_HV_AcceptedLo;
    data.hv_accepted_hi  = m_HV_AcceptedHi;
    data.hv_accepted     = m_HV_Accepted;
    data.hv_accepted_sp  = m_HV_Spread_Accepted;
    data.cps_accepted_sp = m_CPS_Spread_Accepted;
}

//---------------------------------------------------------
int PLATO::init_test()
{
    // Подготовка к тестированию
    if (m_test_stage.step == 0) {
        emit mw()->signal_showstatus({});
        DBGF(1, "stage[%d/%d]: Getting Tool Hardware Settings",
             m_test_stage.stage, m_test_stage.step);
        app()->bto_gui()->progress_bar_message_show(tr("Getting Tool Hardware Settings"));

        m_failed_message.clear();
        m_cps_lo_limit     = m_settings_CPS_LoLimit;
        m_cps_hi_limit     = m_settings_CPS_HiLimit;
        m_cps_spread_limit = m_settings_CPS_SpreadLimit;
        m_hv_lo_limit      = m_settings_HV_LoLimit;
        m_hv_hi_limit      = m_settings_HV_HiLimit;
        m_hv_spread_limit  = m_settings_HV_SpreadLimit;

        gui()->clear();  // очистка полей отображения
        gui()->set_init_label();

        auto &data = this->data();
        data = {};
        data.toolsn = app()->SN();
        data.oper = app()->Operator();
        gui()->update_serial_number(data.toolsn);
        gui()->update_operator(data.oper);

        // запрос уставок
        gui()->set_led_icon_active();
        auto requests = std::list<Adam::Frame> {
                        NodeE4::ReadTemperature(),                  // 0x05
                        NodeE4::ReadOperationVoltage(),             // 0x43
                        NodeE4::ReadCalibrationVoltages(),          // 0x42
                        NodeE4::ReadHVWarmUp(),                     // 0x46
                        NodeE4::ReadGammaRayAcqusitionTime(),       // 0x13
                        NodeE4::ReadPartNumber(),                   // 0xFC
    };
        int rtime = comm()->push_requests(requests);
        m_timer_delay = (rtime + 500);
        m_test_stage.step = 1;
        DBGF(1, "stage[%d/%d]: step: %d waiting for module settings for about %lldms or earlier...",
             m_test_stage.stage, m_test_stage.step, m_total_steps, m_timer_delay);
        return 0;
    }

    if (m_test_stage.step == 1) {
        m_current_step = 0;

        auto &data = this->data();
        data.toolpn = nodeE4()->partnumber();
        gui()->update_part_number(data.toolpn);

        // получение уставок и измеренных значений

        // 1. m_warmup_time - время установки напряжения в гамма-модуле
        //    нужно, чтобы подождать, чтобы напряжение "установилось", гамма-отсчеты отсчитались
        int warmup_module = int(ceil(nodeE4()->m_warmup_time));
        int warmup_ini = m_settings_warmup_time; // ручная установка из .ini
        m_warmup_period = std::max(warmup_module, warmup_ini);

        // 2. m_acq_one_gamma - время одного измерения гамма-отсчетов в гамма-модуле (из прошивки)
        //    нужно, чтобы подождать, пока устройство будет подсчитывать гамма-отсчеты
        if (m_settings_acquire_time > 0) {
            m_acq_one_gamma = m_settings_acquire_time; // ручная установка из .ini
        } else {
            m_acq_one_gamma = int(ceil(nodeE4()->m_gamma_acq_time * m_settings_gamma_acq_bucket));
        }
        m_acq_one_gamma = std::max(m_acq_one_gamma, (int)ceil(2.5 * (double)m_settings_gamma_acq_bucket));

        // 3. время по накоплению и расчету среднего значения отсчетов в процессе одного замера
        // измерение в модуле * кол-во измерений + кол-во изм * сек на каждое считывание из модуля
        m_acq_sample_time = m_warmup_period + (m_acq_one_gamma * m_settings_samples_for_avrg);
        m_acq_total_time = m_acq_sample_time * m_total_steps;

        // 4. тип теста cold/hot
        double temper = nodeE4()->m_temperature_value;
        if (temper < m_settings_hot_mode_temper) {
            m_temper_mode = PLATO::COLD;
            gui()->show_temperaure_mode(temper, tr("COLD MODE"));
        } else {
            m_temper_mode = PLATO::HOT;
            gui()->show_temperaure_mode(temper, tr("HOT MODE"));
        }

        gui()->update_field__acquire_time_details(m_warmup_period, m_settings_gamma_acq_bucket,
                                           m_acq_one_gamma, m_settings_samples_for_avrg);
        gui()->update_field__steps_done(0, m_total_steps);

        clear_plato_values(m_temper_mode);
        gui()->clear_plato_chart(m_temper_mode);
        gui()->clear_plato_table(m_temper_mode);
        gui()->set_run_label();

        if (DBG::debug_level >= 3) {
            DBGF(1, "stage[%d/%d]: Initial HV:        %d",
                 m_test_stage.stage, m_test_stage.step, m_hv_test_initial);
            DBGF(1, "stage[%d/%d]: Operating voltage: %d",
                 m_test_stage.stage, m_test_stage.step, nodeE4()->m_operating_hv);
            DBGF(1, "stage[%d/%d]: Test voltage range: %dV - %dV, samples: %d",
                 m_test_stage.stage, m_test_stage.step, m_hv_test_initial, m_hv_test_final, m_total_steps);
            DBGF(1, "stage[%d/%d]: WarmUp (from module):   %d sec",
                 m_test_stage.stage, m_test_stage.step, warmup_module);
            DBGF(1, "stage[%d/%d]: WarmUp time from ini [PLATO]::HV_WarmUp_sec - %d sec",
                 m_test_stage.stage, m_test_stage.step, warmup_ini);
            DBGF(1, "stage[%d/%d]: Finnaly WarmUp: %d sec",
                 m_test_stage.stage, m_test_stage.step, m_warmup_period);
            DBGF(1, "stage[%d/%d]: Acq time (from module): %0.2f",
                 m_test_stage.stage, m_test_stage.step, nodeE4()->m_gamma_acq_time);
            if (m_settings_acquire_time > 0) {
                DBGF(1, "stage[%d/%d]: Acq time from ini [PLATO]::One_Acquire_sec - %d sec "
                        "(will using because greater than 0)",
                     m_test_stage.stage, m_test_stage.step, m_acq_one_gamma);
            } else {
                DBGF(1, "stage[%d/%d]: Acq time with %d bucket(s): %0.2f, rounded to %d sec",
                     m_test_stage.stage, m_test_stage.step, m_settings_gamma_acq_bucket,
                     nodeE4()->m_gamma_acq_time * m_settings_gamma_acq_bucket, m_acq_one_gamma);
            }
            DBGF(1, "stage[%d/%d]: Finally one gamma acquire time: %d sec",
                 m_test_stage.stage, m_test_stage.step, m_acq_one_gamma);
            DBGF(1, "stage[%d/%d]: samples for average: %d",
                 m_test_stage.stage, m_test_stage.step, m_settings_samples_for_avrg);
            DBGF(1, "stage[%d/%d]: Acquire time for one voltage: %d + (%d x %d(avrg)) = %d sec",
                 m_test_stage.stage, m_test_stage.step, m_warmup_period, m_acq_one_gamma, m_settings_samples_for_avrg,
                 m_acq_sample_time);
            DBGF(1, "stage[%d/%d]: Total steps: %d",
                 m_test_stage.stage, m_test_stage.step, m_total_steps);
            DBGF(1, "stage[%d/%d]: Total acquire time: %d x %d = %d sec (%s)",
                 m_test_stage.stage, m_test_stage.step, m_acq_sample_time, m_test_stage.step, m_acq_total_time,
                 qPrintable(UTILS::secs_to_minute(m_acq_total_time)));
            DBGF(3, "stage[%d/%d]: TEMP MODE: %f < %f",
                 m_test_stage.stage, m_test_stage.step, temper, m_settings_hot_mode_temper);
        }

        m_test_stage.step = 2;
        m_timer_delay = 0;
        return 0;
    }

    if (m_test_stage.step == 2) {
        DBGF(1, "\nstage[%d/%d]", m_test_stage.stage, m_test_stage.step);
        if (m_app->bto()->gamma_module_variant() == BTO::MODULE_KIND::ES) {
            m_app->bto_gui()->progress_bar_message_close();
            m_timer_delay = 0;
            m_test_stage.step = 100;
            return 0;
        }

        int init_delay = m_acq_one_gamma * 10; // tenths of sec  (default: 100)
        int period = (m_warmup_period + m_acq_one_gamma) * 10; // tenths of sec (default: 100)
        int max_record = 0; // 0 - no maximum (run forever)
        int log_mode = 0;   // default: 0
        auto gamma_task = QString("01 %1 %2 %3 %4 %5")          // "01 0078 0064 00 04 00"
                          .arg(init_delay, 4, 16, QChar(u'0'))
                          .arg(period, 4, 16, QChar(u'0'))
                          .arg(max_record, 2, 16, QChar(u'0'))
                          .arg(m_settings_gamma_acq_bucket, 2, 16, QChar(u'0'))
                          .arg(log_mode, 2, 16, QChar(u'0'));
        auto ba = QByteArray::fromHex(QByteArray(qPrintable(gamma_task)));
        const std::vector<byte> gamma_task_bytes(ba.begin(), ba.end());

        auto temper_task = QString("05 000A 000A 00 01 00");    // "05 003C 0032 00 01 00"
        ba = QByteArray::fromHex(QByteArray(qPrintable(temper_task)));
        const std::vector<byte> temper_task_bytes(ba.begin(), ba.end());

        int rtime = comm()->push_requests({ NodeE4::WriteRecordTask(gamma_task_bytes),
                                            NodeE4::WriteRecordTask(temper_task_bytes),
                                          });

        m_app->bto_gui()->progress_bar_message_close();
        m_timer_delay = (rtime + 500);
        m_test_stage.step = 100;
        return 0;
    }

    if (m_test_stage.step == 100) {
        ++m_current_step;
        DBGF(1, "\nstage[%d/%d] current step: %d/%d",
             m_test_stage.stage, m_test_stage.step, m_current_step, m_total_steps);
        if (m_current_step > m_total_steps)
        {
            m_test_stage = {PLATO::FINISH, 0};            // закончили
            m_timer_delay = 0;
            return 0;
        }

        int remain = m_acq_sample_time * (m_total_steps - m_current_step);
        gui()->update_field__remain_time(remain, m_acq_total_time);
        gui()->update_field__steps_done(m_current_step, m_total_steps);

        m_progress_bar_total = m_progress_bar_index;
        if (m_progress_bar_total == 0) {
            m_progress_bar_total = m_warmup_period + (m_acq_one_gamma + 1) * m_settings_samples_for_avrg;
        }
        m_progress_bar_index = 0;
        gui()->progress_bar()->reset();
        gui()->progress_bar()->setRange(0, m_progress_bar_total);
        gui()->progress_bar()->setValue(0);

        return 1; // done
    }

    return 1; // done
}

//---------------------------------------------------------
int PLATO::set_voltage()
{
    // Установить высоковольтное напряжение
    if (m_test_stage.step == 0) {
        int hv = m_hv_test_initial + (m_settings_HV_step * (m_current_step-1));
        if (hv < GAMMA::HV_Min || hv > GAMMA::HV_Max) {
            DBGF(1, "stage[%d/%d]: [%s] step[%d]: HV: %d OUT OF LIMITS %d - %d - finishing",
                 m_test_stage.stage, m_test_stage.step,
                 (m_temper_mode == PLATO::COLD ? "COLD" : "HOT"),
                 m_current_step, hv, GAMMA::HV_Min, GAMMA::HV_Max);
            m_failed_message = tr("Requested voltage is outside the limits");
            QString tip = QString("%1\n\n%2%3\n\n[%4-%5]")
                          .arg(tr("Requested voltage is outside the limits"))
                          .arg(hv).arg("v")
                          .arg(m_hv_test_initial)
                          .arg(m_hv_test_final);
            auto str = m_failed_message;
            DBG(2, QString("[7] failed message: %1").arg(str.replace("\n", ". ")));
            update_failed_message(QStringLiteral("---"), tip);
            m_test_stage = {PLATO::STOP, 0};
            m_timer_delay = 0;
            return 0;
        }

        DBGF(1, "stage[%d/%d]: [%s] step: %d, set %d (%d-%d)",
             m_test_stage.stage, m_test_stage.step,
             m_temper_mode == PLATO::COLD ? "COLD" : "HOT",
             m_current_step, hv, GAMMA::HV_Min, GAMMA::HV_Max);

        gui()->showstatus(QString("[%1 %2] %3").arg(hv).arg(tr("V")).arg(tr("Set HV")), 10);

        gui()->set_led_icon_active();
        int rtime = comm()->push_request(NodeE4::WriteOperationVoltage(hv));
        m_timer_delay = (rtime + 100);
        m_test_stage.step = 1;
        return 0;
    }

    if (m_test_stage.step == 1) {
        gui()->set_led_icon_active();
        int rtime = comm()->push_request(NodeE4::ReadOperationVoltage());
        m_timer_delay = (rtime + 100);
        m_test_stage.step = 2;
        return 0;
    }

    if (m_test_stage.step == 2) {
        return 1; // done
    }

    return 1; // done
}

//---------------------------------------------------------
int PLATO::warmup()
{
    // Выдержка паузы для "прогрева" устройства
    if (m_test_stage.step == 0) {
        // подготовка
        DBGF(1, "stage[%d/%d]: sample: %d, warmup period: %d",
             m_test_stage.stage, m_test_stage.step, m_current_step, m_warmup_period);

        auto style = "QProgressBar {height: 5px;} QProgressBar::chunk {background: #eb8000;}"; // оранж
        gui()->progress_bar()->setStyleSheet(style);

        m_warmup_counter = 0; // sec
        m_test_stage.step = 1;
    }

    if (m_test_stage.step == 1) {
        // ожидание "стабилизации напряжения"
        if (m_warmup_counter < m_warmup_period)
        {
            emit mw()->signal_showstatus(QString("[%1 %2] %3 [%4/%5 %6]")
                                         .arg(nodeE4()->m_operating_hv)
                                         .arg(tr("V"), tr("High Voltage Stabilization"))
                                         .arg(m_warmup_counter + 1)
                                         .arg(m_warmup_period)
                                         .arg(tr("sec")));
            gui()->progress_bar()->setValue(++m_progress_bar_index);
            gui()->set_led_icon_active();
            comm()->push_request(NodeE4::ReadOperationVoltage()); // 0x43

            ++m_warmup_counter;
            m_timer_delay = 1000;
            return 0;
        }

        return 1;
    }

    return 1;
}

//---------------------------------------------------------
int PLATO::read_gamma()
{
    if (m_test_stage.step == 0) {
        DBGF(3, "stage[%d/%d]: sample: %d hv %dV, acq period: %d",
             m_test_stage.stage, m_test_stage.step, m_current_step, nodeE4()->m_operating_hv, m_acq_one_gamma);
        auto style = "QProgressBar {height: 5px;} QProgressBar::chunk {background: #7D94B0;}"; // серо-синий
        gui()->progress_bar()->setStyleSheet(style);

        m_acq_counter = 0;
        m_gamma_avrg = 0.0;
        m_waiting_for_acqure = 0;

        DBGF(3, "stage[%d/%d]: acq counter: %d/%d",
             m_test_stage.stage, m_test_stage.step,
             m_acq_counter, m_settings_samples_for_avrg);
        m_test_stage.step = 1;
        m_timer_delay = 0;
        return 0;
    }

    if (m_test_stage.step == 1) {
        // задержка на время считывания гамма-отсчета в гамма-модуле
        if (m_waiting_for_acqure < m_acq_one_gamma) {
            DBGF(3, "stage[%d/%d]: acq_one_gamma: %d/%d",
                 m_test_stage.stage, m_test_stage.step,
                 m_waiting_for_acqure, m_acq_one_gamma);

            gui()->progress_bar()->setValue(++m_progress_bar_index);
            int acqtime = (m_acq_one_gamma - m_waiting_for_acqure);
            emit mw()->signal_showstatus(QString("[%1 %2] Sample: %3/%4  %5 [%6]")
                                         .arg(nodeE4()->m_operating_hv)
                                         .arg(tr("V"))
                                         .arg(m_acq_counter + 1)
                                         .arg(m_settings_samples_for_avrg)
                                         .arg(tr("Waiting for acquire gamma sample"))
                                         .arg(UTILS::secs_to_minute(acqtime)));

            bool status = temperature_control(m_temper_mode);
            if (status == false) {
                m_test_stage = {PLATO::STOP, 0}; // не-нормальное завершение
                m_timer_delay = 0;
                return 0;
            }

            ++m_waiting_for_acqure;
            gui()->set_led_icon_active();
            comm()->push_requests({ NodeE4::ReadTemperature(),      // 0x05
                                    NodeE4::ReadOperationVoltage(), // 0x43
                                  });
            m_test_stage.step = 1;
            m_timer_delay = 1000; // точная секунда тут нужна...
            return 0;
        }

        gui()->progress_bar()->setValue(++m_progress_bar_index);
        m_waiting_for_acqure = 0;
        emit mw()->signal_showstatus(QString("[%1 %2] %3")
                                     .arg(nodeE4()->m_operating_hv)
                                     .arg(tr("V"), tr("Getting gamma sample")));
        DBGF(3, "stage[%d/%d]: getting gamma sample",
             m_test_stage.stage, m_test_stage.step);

        // считывание гамма-отсчета
        auto rtime = 0;
        gui()->set_led_icon_active();
        rtime = comm()->push_request(NodeE4::ReadGammaCPS(byte(m_settings_gamma_acq_bucket))); // 0x01
        m_test_stage.step = 2;
        m_timer_delay = (rtime + 100);
        return 0;
    }

    if (m_test_stage.step == 2) {
        ++m_acq_counter;
        if (m_acq_counter < m_settings_samples_for_avrg) {
            DBGF(3, "stage[%d/%d]: acq counter: %d/%d (gamma sum: %0.2f)",
                 m_test_stage.stage, m_test_stage.step,
                 m_acq_counter, m_settings_samples_for_avrg, m_gamma_avrg);
            m_timer_delay = 0;
            m_test_stage.step = 1;     // переход на считывание гаммы - для накопления, для расчета среднего
            return 0;
        }

        // шаги для накопления и расчета среднего гамма-отсчетов
        m_gamma_avrg += nodeE4()->m_gamma_value;

        // закончили считывание гамма-отсчетов
        DBGF(3, "stage[%d/%d]: acq counter: %d/%d (gamma sum: %0.2f)",
             m_test_stage.stage, m_test_stage.step,
             m_acq_counter, m_settings_samples_for_avrg, m_gamma_avrg);
        DBGF(3, "stage[%d/%d]: gamma avrg: %0.2f = %0.2f / %d",
             m_test_stage.stage, m_test_stage.step,
             m_gamma_avrg/ m_acq_counter, m_gamma_avrg, m_acq_counter);
        m_gamma_avrg /= m_acq_counter;

        m_acq_counter = 0;
        m_test_stage.step = 3;
        m_timer_delay = 0;
        return 0;
    }

    if (m_test_stage.step == 3) {

        int index = m_current_step-1;

        DBGF(1, "stage[%d/%d]", m_test_stage.stage, m_test_stage.step);
        gui()->progress_bar()->setValue(gui()->progress_bar()->maximum());

        // расчеты
        store_data(m_temper_mode, index);
        calculate_slope(m_temper_mode, index);
        bool ok = calculate_borders(m_temper_mode, index);
        update_chart(m_temper_mode, index);
        update_table(m_temper_mode, index);
        update_borders();
        if (!ok) {
            // прекращаем тестирование
            m_test_stage = {PLATO::FINISH, 0};
            return 0;
        }
        check_borders();
        return 1;
    }

    return 1;
}

//---------------------------------------------------------
bool PLATO::temperature_control(tmode_t tmode)
{
    auto value = nodeE4()->m_temperature_value;
    DBGF(3, "stage[%d/%d]: sample: %d, temperature: %0.2f",
         m_test_stage.stage, m_test_stage.step, m_current_step, value);

    QString str1, str2;
    if (tmode == PLATO::COLD) {
        if (value > m_settings_hot_mode_temper) {
            str1 = QString("%1 - %2").arg(tr("Test Mode")).arg(tr("COLD MODE"));
            str2 = QString("%1°C > %2°C")
                   .arg(value, 0, 'f', 1)
                   .arg(m_settings_hot_mode_temper, 0, 'f', 1);

        }
    } else {
        // PLATO::HOT
        if (value < m_settings_hot_mode_temper) {
            str1 = QString("%1 - %2")
                   .arg(tr("Test Mode")).arg(tr("HOT MODE"));
            str2 = QString("%1°C < %2°C")
                   .arg(value, 0, 'f', 1)
                   .arg(m_settings_hot_mode_temper, 0, 'f', 1);
        }
    }

    if (!str1.isEmpty()) {
        auto msg = QString("%1\n\n%2\n%3").arg(tr("WRONG TEMPERATURE MODE"), str1, str2);
        m_failed_message = msg;
        auto str = m_failed_message;
        DBG(2, QString("[8] failed message: %1").arg(str.replace("\n", ". ")));
        update_failed_message(QStringLiteral("---"), msg);
        return false;
    }

    return true;
}

//---------------------------------------------------------
void PLATO::slot_plato_test()
{
    qint64 msec = QDateTime::currentMSecsSinceEpoch();
    qint64 passed_ms = (msec - m_prev_msec);
    if (passed_ms < m_timer_delay) {
        return;
    }

    m_timer_delay = 1000; // default

    if (comm()->number_of_requests() > 0) {
        DBGF(3, "number of requests: %d > 0", comm()->number_of_requests());
        return;
    }

    if (!comm()->isConnected()) {
        if (m_test_stage.stage == 0) {
            DBG(1, "--- device is not connected");
            QApplication::restoreOverrideCursor();
            m_test_stage = {PLATO::STOP, 0};
            m_timer_delay = 0;
            m_prev_msec = QDateTime::currentMSecsSinceEpoch();
            return;
        }
    }

    switch (m_test_stage.stage)
    {
        case PLATO::INIT:
        {
            auto done = init_test();
            if (done) {
                m_test_stage = {PLATO::SET_VOLTAGE, 0};
                m_timer_delay = 0;
            }
            break;
        }

        case PLATO::SET_VOLTAGE:
        {
            bool status = temperature_control(m_temper_mode);
            if (status == false) {
                m_timer_delay = 0;
                m_test_stage = {PLATO::STOP, 0}; // не-нормальное завершение
                break;
            }

            auto done = set_voltage();
            if (done) {
                m_test_stage = {PLATO::WARMUP, 0};
                m_timer_delay = 0;
            }
            break;
        }

        case PLATO::WARMUP:
        {
            auto done = warmup();
            if (done) {
                m_test_stage = {PLATO::READ_GAMMA, 0};
                m_timer_delay = 0;
            }
            break;
        }

        case PLATO::READ_GAMMA:
        {
            auto done = read_gamma();
            if (done) {
                m_test_stage = {PLATO::INIT, 100};    // переход на следующее напряжение
                m_timer_delay = 0;
            }
            break;
        }

        case PLATO::FINISH:  // нормальное завершение
        {
            DBGF(1, "stage[%d/%d]: finishing test", m_test_stage.stage, m_test_stage.step);
            create_summary();
            protocol_create_last_log(m_cal_data);
            gui()->finishing_test();
            break;
        }

        case PLATO::STOP:           // не-нормальное завершение
        {
            DBGF(1, "stage[%d/%d]: stop test", m_test_stage.stage, m_test_stage.step);
            create_summary();
            protocol_create_last_log(m_cal_data);
            gui()->finishing_test();
            break;
        }

    } // end of switch

    m_prev_msec = QDateTime::currentMSecsSinceEpoch();
}
