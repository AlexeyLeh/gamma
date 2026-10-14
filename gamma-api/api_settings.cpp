/*!
  \file   api_settings.cpp
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Сохранение настроек API-теста
*/

#include "api.h"
#include "api_gui.h"
#include "dbg.h"
#include "mainwindow.h"
#include "gamma.h"
#include "table.h"

//---------------------------------------------------------
void API_GUI::read_settings(QSettings *st)
{
    DBG(4);
    QByteArray state = st->value("API_Table2State", QByteArray()).toByteArray();
    m_table1->horizontalHeader()->restoreState(state);

    state.clear();
    state = st->value("API_Table3State", QByteArray()).toByteArray();
    m_table2->horizontalHeader()->restoreState(state);
}

//---------------------------------------------------------
void API_GUI::save_settings(QSettings *st)
{
    DBG(4);
    // [General]
    QByteArray state = m_table1->horizontalHeader()->saveState();
    st->setValue("API_Table2State", state);

    state.clear();
    state = m_table2->horizontalHeader()->saveState();
    st->setValue("API_Table3State", state);
}

//---------------------------------------------------------
void API::read_settings(QSettings *st)
{
    DBG(4);
    st->beginGroup("API");

    m_settings_last_report_dir  = st->value("ReportDir", report_path()).toString();
    m_settings_create_html      = st->value("ReportHtml", false).toBool();
    m_settings_hot_mode_temper  = st->value("HOTTemp", GAMMA::HOT_Mode).toDouble();
    m_settings_warmup_time      = st->value("HVWarmUpSec", 30).toInt();
    m_settings_HV_min_value     = st->value("HV_LoLimit", GAMMA::HV_Lo_Limit).toInt();

    m_settings_acquire_time      = st->value("OneAcquiresec", 0).toInt();   // 0 - использование числа из прошивки модуля
    m_settings_gamma_acq_bucket = st->value("GammaBucket", 4).toInt();      // число усреднений в самом модуле
    m_settings_samples_for_avrg = st->value("SamplesForAvrg", 10).toInt();  // число усреднений в тесте

    m_settings_calibr_min       = st->value("CalibrKoefMin",   GAMMA::API_Calibr_Koef_Min).toDouble();
    m_settings_calibr_max       = st->value("CalibrKoefMax",   GAMMA::API_Calibr_Koef_Max).toDouble();

    m_settings_calibr_old       = st->value("CalibrKoefOld").toInt();
    m_settings_calibr_new       = st->value("CalibrKoefNew").toInt();

    if (m_settings_calibr_old == 0) m_settings_calibr_old = GAMMA::API_Calibr_Koef_Old;
    if (m_settings_calibr_new == 0) m_settings_calibr_new = GAMMA::API_Calibr_Koef_New;

    m_settings_tol_deviat0_min   = st->value("TolDeviat0Min",   GAMMA::API_Tol_Deviat0_Min).toDouble();
    m_settings_tol_deviat0_max   = st->value("TolDeviat0Max",   GAMMA::API_Tol_Deviat0_Max).toDouble();
    m_settings_tol_deviat200_min = st->value("TolDeviat200Min", GAMMA::API_Tol_Deviat200_Min).toDouble();
    m_settings_tol_deviat200_max = st->value("TolDeviat200Max", GAMMA::API_Tol_Deviat200_Max).toDouble();

    m_settings_odid_beg         = st->value("ODIDBeg",         GAMMA::ODID_Beg).toDouble();
    m_settings_odid_end         = st->value("ODIDEnd",         GAMMA::ODID_End).toDouble();
    m_settings_odid_stp         = st->value("ODIDStep",        GAMMA::ODID_Step).toDouble();
    st->endGroup();
}

//---------------------------------------------------------
void API::save_settings(QSettings *st)
{
    DBG(4);
    st->beginGroup("API");
    st->setValue("ReportDir",        m_settings_last_report_dir);
    st->setValue("ReportHtml",       m_settings_create_html);
    st->setValue("HOTTemp",          m_settings_hot_mode_temper);
    st->setValue("HVWarmUpSec",      m_settings_warmup_time);
    st->setValue("GammaBucket",      m_settings_gamma_acq_bucket);
    st->setValue("SamplesForAvrg",   m_settings_samples_for_avrg);
    st->setValue("HV_LoLimit",       m_settings_HV_min_value);
    st->setValue("CalibrKoefMin",    m_settings_calibr_min);
    st->setValue("CalibrKoefMax",    m_settings_calibr_max);
    st->setValue("CalibrKoefOld",    m_settings_calibr_old);
    st->setValue("CalibrKoefNew",    m_settings_calibr_new);
    st->setValue("TolDeviat0Min",    m_settings_tol_deviat0_min);
    st->setValue("TolDeviat0Max",    m_settings_tol_deviat0_max);
    st->setValue("TolDeviat200Min",  m_settings_tol_deviat200_min);
    st->setValue("TolDeviat200Max",  m_settings_tol_deviat200_max);
    st->setValue("ODIDBeg",          m_settings_odid_beg);
    st->setValue("ODIDEnd",          m_settings_odid_end);
    st->setValue("ODIDStep",         m_settings_odid_stp);

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
void API_GUI::set_content_font_size(int sz)
{
    DBGF(3, "%d", sz);

    std::list<QPushButton*> buttons {
        m_button_api_run,
    };

    for (auto but : buttons)
    {
        auto style = QString("QPushButton { font: %1pt; }").arg(sz);
        but->setStyleSheet(style);
    }

    auto psz = mw()->content_font_pointSize();
    psz += 4;
    auto fnt = m_fld_api_HV->font();
    fnt.setPointSize(psz);

    m_fld_api_mode->setFont(fnt);
    m_fld_api_hot_mode->setFont(fnt);
    m_fld_api_HV->setFont(fnt);
    m_fld_api_CPS->setFont(fnt);
    m_fld_api_Temp->setFont(fnt);

    QFontMetrics fm(fnt);
    int font_height = fm.height();
    int current_font_size = mw()->content_font_pointSize();
    int minimumFiledHeight = font_height + ((current_font_size >= 14) ? 30 : 20);

    m_fld_api_mode->setMinimumHeight(minimumFiledHeight);
    m_fld_api_hot_mode->setMinimumHeight(minimumFiledHeight);
    m_fld_api_HV->setMinimumHeight(minimumFiledHeight);
    m_fld_api_CPS->setMinimumHeight(minimumFiledHeight);
    m_fld_api_Temp->setMinimumHeight(minimumFiledHeight);

    QLineEdit in(mw());
    auto labelBoldFont = in.font();
    labelBoldFont.setBold(true);
    fm = QFontMetrics(labelBoldFont);
    //                                         62 min 20 sec [12:17:33]
    int minFieldWidth = fm.horizontalAdvance("WWWWWWWWWWWWWWWWWWWWWWWW") + 10; // самое широкое наполнение
    m_fld_est_time->setMinimumWidth(minFieldWidth);
    m_fld_acq_time->setMinimumWidth(minFieldWidth);

    for (auto table : {m_table1, m_table2})
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
