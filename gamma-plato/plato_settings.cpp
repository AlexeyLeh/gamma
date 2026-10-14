/*!
  \file   plato_settings.cpp
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Сохранение настроек теста линейности
*/

#include "dbg.h"
#include "mainwindow.h"
#include "plato.h"
#include "plato_gui.h"
#include "gamma.h"

//---------------------------------------------------------
void PLATO_GUI::read_settings(QSettings *st)
{
    DBG(4);
    // [General]
    auto state = st->value("PLATO_TableState", QByteArray()).toByteArray();
    if (!state.isEmpty()) {
        m_table->horizontalHeader()->restoreState(state);
    }

    //
    st->beginGroup("Plato");
    m_settings_graph_x_min  = st->value("Chart_HV_Min",   GAMMA::HV_Chart_Lo).toInt();
    m_settings_graph_x_max  = st->value("Chart_HV_Max",   GAMMA::HV_Chart_Hi).toInt();
    //// m_settings_graph_x_tick = st->value("Chart_HV_Ticks", GAMMA::HV_Chart_Tick).toInt();
    //// if (m_settings_graph_x_tick < 2) {
    ////     m_settings_graph_x_tick = 2;
    //// }
    //// m_axisX->setTickCount(m_settings_graph_x_tick);
    m_axisY->setTickType(QtCharts::QValueAxis::TicksDynamic);
    m_axisY->setTickAnchor(m_settings_graph_x_min);
    m_axisY->setTickInterval(50.0);
    m_axisX->setRange(m_settings_graph_x_min, m_settings_graph_x_max);

    m_settings_graph_y_min  = st->value("Chart_CPS_Min",   GAMMA::Chart_CPS_Min).toInt();
    m_settings_graph_y_max  = st->value("Chart_CPS_Max",   GAMMA::Chart_CPS_Max).toInt();
    //// m_settings_graph_y_tick = st->value("Chart_CPS_Ticks", GAMMA::Chart_CPS_Ticks).toInt();
    //// if (m_settings_graph_y_tick < 2) {
    ////     m_settings_graph_y_tick = 2;
    //// }
    m_axisY->setTickType(QtCharts::QValueAxis::TicksDynamic);
    m_axisY->setTickAnchor(m_settings_graph_y_min);
    m_axisY->setTickInterval(50.0);
    m_axisY->setRange(m_settings_graph_y_min, m_settings_graph_y_max);

    m_settings_graph_y2_min  = st->value("Chart_Temper_Min", GAMMA::Chart_Temper_Min).toInt();
    m_settings_graph_y2_max  = st->value("Chart_Temper_Max", GAMMA::Chart_Temper_Max).toInt();
    //// m_settings_graph_y2_tick = st->value("Chart_Temper_Tick",GAMMA::Chart_Temper_Tick).toInt();
    //// if (m_settings_graph_y2_tick < 2) {
    ////     m_settings_graph_y2_tick = (m_settings_graph_y2_max - m_settings_graph_y2_min) / 10 + 1;
    //// }
    m_settings_graph_y2_min = (m_settings_graph_y2_min / 10) * 10;
    m_settings_graph_y2_max = ceil((double)m_settings_graph_y2_max / 10.0) * 10;
    m_axisY2->setTickType(QtCharts::QValueAxis::TicksDynamic);
    m_axisY2->setTickAnchor(m_settings_graph_y2_min);
    m_axisY2->setTickInterval(10.0);
    m_axisY2->setRange(m_settings_graph_y2_min, m_settings_graph_y2_max);

    m_settings_create_html = st->value("ReportHtml", false).toBool();
    m_settings_last_report_dir = st->value("ReportDir", m_plato->report_path()).toString();
    m_settings_last_report_loaded_dir = st->value("ReportLoadedDir", m_plato->report_path()).toString();
    m_settings_last_simulation_loaded_dir = st->value("SimulationLoadedDir", m_plato->report_path()).toString();

    st->endGroup();
}

//---------------------------------------------------------
void PLATO_GUI::save_settings(QSettings *st)
{
    DBG(4);
    // [General]
    st->setValue("PLATO_TableState",  m_table->horizontalHeader()->saveState());

    st->beginGroup("Plato");

    st->setValue("Chart_HV_Min",      m_settings_graph_x_min);
    st->setValue("Chart_HV_Max",      m_settings_graph_x_max);
    //// st->setValue("Chart_HV_Ticks",    m_settings_graph_x_tick);

    st->setValue("Chart_CPS_Min",     m_settings_graph_y_min);
    st->setValue("Chart_CPS_Max",     m_settings_graph_y_max);
    //// st->setValue("Chart_CPS_Ticks",   m_settings_graph_y_tick);

    st->setValue("Chart_Temper_Min",  m_settings_graph_y2_min);
    st->setValue("Chart_Temper_Max",  m_settings_graph_y2_max);
    //// st->setValue("Chart_Temper_Tick", m_settings_graph_y2_tick);

    st->setValue("ReportHtml",        m_settings_create_html);
    st->setValue("ReportDir",         m_settings_last_report_dir);
    st->setValue("ReportLoadedDir",   m_settings_last_report_loaded_dir);
    st->setValue("SimulationLoadedDir", m_settings_last_simulation_loaded_dir);

    st->endGroup();
}

//---------------------------------------------------------
void PLATO::read_settings(QSettings *st)
{
    DBG(3);
    st->beginGroup("Plato");
    m_settings_HV_LoLimit        = st->value("HV_LoLimit",     GAMMA::HV_Lo_Limit).toInt();
    m_settings_HV_HiLimit        = st->value("HV_HiLimit",     GAMMA::HV_Hi_Limit).toInt();
    m_settings_HV_SpreadLimit    = st->value("HV_SpreadLimit", GAMMA::HV_Spread_Limit).toInt();
    m_hv_lo_limit                = m_settings_HV_LoLimit;
    m_hv_hi_limit                = m_settings_HV_HiLimit;
    m_hv_spread_limit            = m_settings_HV_SpreadLimit;

    m_settings_HV_Test_Lo        = st->value("HV_TestLo",      GAMMA::HV_Test_Lo).toInt();
    m_settings_HV_Test_Hi        = st->value("HV_TestHi",      GAMMA::HV_Test_Hi).toInt();
    m_settings_HV_step           = st->value("HV_TestStep",    GAMMA::HV_Test_Step).toInt();
    m_settings_HV_Proportion     = st->value("HV_Prop",        GAMMA::HV_Proportionality).toDouble();

    m_settings_CPS_LoLimit       = st->value("CPS_LoLimit",    GAMMA::CPS_Lo_Limit).toInt();
    m_settings_CPS_HiLimit       = st->value("CPS_HiLimit",    GAMMA::CPS_Hi_Limit).toInt();
    m_settings_CPS_SpreadLimit   = st->value("CPS_Spread",     GAMMA::CPS_Spread_Limit).toInt();
    m_cps_lo_limit               = m_settings_CPS_LoLimit;
    m_cps_hi_limit               = m_settings_CPS_HiLimit;
    m_cps_spread_limit           = m_settings_CPS_SpreadLimit;

    m_settings_num_pnt_for_slope = st->value("Slope_Points",   GAMMA::Slope_Calc_Points).toInt();
    m_settings_slope_min         = st->value("Slope_min",      GAMMA::CPS_SlopeMin).toDouble();
    m_settings_slope_max         = st->value("Slope_max",      GAMMA::CPS_SlopeMax).toDouble();
    m_settings_slope_critical    = st->value("Slope_CutOff",   GAMMA::CPS_SlopeCutOff).toDouble();

    m_settings_warmup_time       = st->value("HV_WarmUp_sec", GAMMA::WarmUp).toInt();
    m_settings_acquire_time      = st->value("One_Acquire_sec", 0).toInt();     // 0 - использование числа из прошивки модуля
    m_settings_gamma_acq_bucket  = st->value("GammaBucket", 4).toInt();         // число усреднений в самом модуле
    m_settings_samples_for_avrg  = st->value("Samples_for_Avrg", 5).toInt();    // число усреднений в тесте
    m_settings_hot_mode_temper   = st->value("HOT_Temp", GAMMA::HOT_Mode).toDouble();

    if (m_settings_HV_Test_Lo >= m_settings_HV_Test_Hi)
    {
        std::swap(m_settings_HV_Test_Lo, m_settings_HV_Test_Hi);
    }

    st->endGroup();
    st->sync();
}

//---------------------------------------------------------
void PLATO::save_settings(QSettings *st)
{
    DBG(4);
    st->beginGroup("Plato");

    st->setValue("HV_TestLo",         m_settings_HV_Test_Lo);
    st->setValue("HV_TestHi",         m_settings_HV_Test_Hi);
    st->setValue("HV_LoLimit",        m_settings_HV_LoLimit);
    st->setValue("HV_HiLimit",        m_settings_HV_HiLimit);
    st->setValue("HV_SpreadLimit",    m_settings_HV_SpreadLimit);
    st->setValue("HV_Prop",           m_settings_HV_Proportion);
    st->setValue("CPS_LoLimit",       m_settings_CPS_LoLimit);
    st->setValue("CPS_HiLimit",       m_settings_CPS_HiLimit);
    st->setValue("CPS_Spread",        m_settings_CPS_SpreadLimit);

    st->setValue("Slope_Points",      m_settings_num_pnt_for_slope);
    st->setValue("Slope_min",         m_settings_slope_min);
    st->setValue("Slope_max",         m_settings_slope_max);
    st->setValue("Slope_CutOff",      m_settings_slope_critical);

    st->setValue("HV_WarmUp_sec",     m_settings_warmup_time);
    st->setValue("One_Acquire_sec",   m_settings_acquire_time);
    st->setValue("Samples_for_Avrg",  m_settings_samples_for_avrg);
    st->setValue("GammaBucket",       m_settings_gamma_acq_bucket);
    st->setValue("HOT_Temp",          m_settings_hot_mode_temper);

    st->endGroup();
}

//---------------------------------------------------------
static void __set_fixed_font(QFont& fnt, int sz, bool bold)
{
    fnt.setStyleHint(QFont::Monospace);
    fnt.setFamily("Fixed");
    fnt.setFixedPitch(true);
    fnt.setPointSize(sz);
    if (bold) {
        fnt.setBold(true);
    }
}

//---------------------------------------------------------
void PLATO_GUI::set_content_font_size(int sz)
{
    DBG(4);

    auto fnt = m_button_plato_run->font();
    fnt.setPointSize(mw()->content_font_pointSize());
    QFontMetrics fm(fnt);
    int minFieldWidth = fm.horizontalAdvance(tr("Start Plateau Test"));
    minFieldWidth = std::max(minFieldWidth, fm.horizontalAdvance(tr("Stop Plateau Test")));
    minFieldWidth += 10;

    std::list<QPushButton*> buttons {
        m_button_plato_run,
        m_button_load_simulation,
    };

    for (auto but : buttons)
    {
        if (but) {
            but->setMinimumWidth(minFieldWidth);
            auto style = QString("QPushButton { font: %1pt; }").arg(sz);
            MW::set_style_sheet(but, style);
        }
    }

    auto psz = mw()->content_font_pointSize();
    psz += 4;
    fnt = m_fld_HV->font();
    fnt.setPointSize(psz);

    m_fld_HV->setFont(fnt);
    m_fld_CPS->setFont(fnt);
    m_fld_Temper->setFont(fnt);
    m_fld_temper_mode->setFont(fnt);

    fm = QFontMetrics(fnt);
    int font_height = fm.height();
    int current_font_size = mw()->content_font_pointSize();
    int minimumFiledHeight = font_height + ((current_font_size >= 14) ? 30 : 20);

    m_fld_HV->setMinimumHeight(minimumFiledHeight);
    m_fld_CPS->setMinimumHeight(minimumFiledHeight);
    m_fld_Temper->setMinimumHeight(minimumFiledHeight);
    m_fld_temper_mode->setMinimumHeight(minimumFiledHeight);

    QLineEdit in(mw());
    auto labelBoldFont = in.font();
    labelBoldFont.setBold(true);
    fm = QFontMetrics(labelBoldFont);
    //                                        62 min [12:17:33]
    minFieldWidth = fm.horizontalAdvance("XXXXXXXXXXXXXXXXX");
    m_fld_est_time->setMinimumWidth(minFieldWidth);
    m_fld_acq_time->setMinimumWidth(minFieldWidth);
    m_fld_hv_step->setMinimumWidth(minFieldWidth);
    m_fld_hv_accepted->setMinimumWidth(minFieldWidth);

    for (auto table : {m_table})
    {
        QFont fnt = table->font();
        fnt = table->font();
        __set_fixed_font(fnt, sz, false);
        table->setFont(fnt);

        fnt = table->horizontalHeader()->font();
        __set_fixed_font(fnt, sz, false);
        table->horizontalHeader()->setFont(fnt);

        auto *item = (TableItem*)(table->item(0, 0));
        if (item) {
            __set_fixed_font(fnt, sz, true);

            int nrows = table->rowCount();
            int ncols = table->columnCount();
            for (int i = 0; i < nrows; ++i)
            {
                for (int j = 0; j < ncols; ++j)
                {
                    item = (TableItem*)(table->item(i, j));
                    if (item) item->setFont(fnt);
                }
            }
        }
    }
}
