/*!
  \file   diagn_gui_create_page.cpp
  \brief  Раскладка полей окна диагностики
  \author Aleksey Lekhtin
  \date   10.05.2023
*/

#include "dbg.h"
#include "diagn.h"
#include "diagn_gui.h"
#include "mainwindow.h"
#include "terminal.h"

// Раскладка полей терминала
QWidget *DIAGN_GUI::create_diagnostic_page()
{
    m_fld_diag_term_fname_log = new QLineEdit();
    m_fld_diag_term_fname_log->setMaxLength(1024);

    m_term_select_log_button = new QPushButton(tr("Select"));
    QString tip = tr("A button 'Select' can help select target folder");
    m_term_select_log_button->setToolTip(tip);
    connect(m_term_select_log_button, SIGNAL(clicked()), this, SLOT(slot_select_terminal_log_path()));

    m_chkbox_term_log_enabled = new QCheckBox(tr("Logging"));
    tip = tr("Check it to enable logging terminal output");
    m_chkbox_term_log_enabled->setToolTip(tip);
    m_chkbox_term_log_enabled->setChecked(false);
    connect(m_chkbox_term_log_enabled, SIGNAL(toggled(bool)), this, SLOT(slot_set_term_log_enabled(bool)));

    m_term_clear_button = new QPushButton(tr("Clear"));
    tip = tr("Clear terminal contents") + " [" + m_term_clear_button->shortcut().toString() + "]";
    m_term_clear_button->setToolTip(tip);
    connect(m_term_clear_button, SIGNAL(clicked()), this, SLOT(slot_clear_terminal()));

    //---------------------------------------------------------
    m_fld_diag_Partnumber     = new QLineEdit();
    m_fld_diag_Partnumber_cmd = new LineEdit8();
    m_fld_diag_Partnumber_ms  = new QLabel();
    m_fld_diag_Partnumber_log = new QCheckBox();
    m_fld_diag_Partnumber->setAlignment(Qt::AlignRight);
    m_fld_diag_Partnumber_ms->setAlignment(Qt::AlignRight);
    m_fld_diag_Partnumber_cmd->setAlignment(Qt::AlignCenter);
    m_fld_diag_Partnumber_cmd->setReadOnly(true);
    m_fld_diag_Partnumber_cmd->setText("FC");

    m_fld_diag_rectask     = new QLineEdit();
    m_fld_diag_rectask_cmd = new LineEdit8();
    m_fld_diag_rectask_ms  = new QLabel();
    m_fld_diag_rectask_log = new QCheckBox();
    m_fld_diag_rectask->setAlignment(Qt::AlignRight);
    m_fld_diag_rectask_ms->setAlignment(Qt::AlignRight);
    m_fld_diag_rectask_cmd->setAlignment(Qt::AlignCenter);
    m_fld_diag_rectask_cmd->setReadOnly(true);
    m_fld_diag_rectask_cmd->setText("F8");

    m_fld_diag_status     = new QLineEdit();
    m_fld_diag_status_cmd = new LineEdit8();
    m_fld_diag_status_ms  = new QLabel();
    m_fld_diag_status_log = new QCheckBox();
    m_fld_diag_status->setAlignment(Qt::AlignRight);
    m_fld_diag_status_cmd->setAlignment(Qt::AlignCenter);
    m_fld_diag_status_ms->setAlignment(Qt::AlignRight);
    m_fld_diag_status_cmd->setReadOnly(true);

    m_fld_diag_gamma_cnts        = new QLineEdit();
    m_fld_diag_gamma_cmd         = new LineEdit8();
    m_fld_diag_gamma_ms          = new QLabel();
    m_fld_diag_gamma_log         = new QCheckBox();
    m_fld_diag_gamma_cnts        ->setAlignment(Qt::AlignRight);
    m_fld_diag_gamma_ms          ->setAlignment(Qt::AlignRight);
    m_fld_diag_gamma_cmd         ->setAlignment(Qt::AlignCenter);
    m_fld_diag_gamma_cmd         ->setText("01");
    m_fld_diag_gamma_cmd         ->setReadOnly(true);

    m_fld_diag_gamma_AcqTime     = new QLineEdit();
    m_fld_diag_gamma_AcqTime_log = new QCheckBox();
    m_fld_diag_gamma_AcqTime_ms  = new QLabel();
    m_fld_diag_gamma_AcqTime_cmd = new LineEdit8();
    m_fld_diag_gamma_AcqTime     ->setAlignment(Qt::AlignRight);
    m_fld_diag_gamma_AcqTime_ms  ->setAlignment(Qt::AlignRight);
    m_fld_diag_gamma_AcqTime_cmd ->setAlignment(Qt::AlignCenter);
    m_fld_diag_gamma_AcqTime_cmd ->setReadOnly(true);
    m_fld_diag_gamma_AcqTime_cmd ->setText("13");

    m_fld_diag_accel_raw      = new QLineEdit();
    m_fld_diag_accel_raw_log  = new QCheckBox();
    m_fld_diag_accel_raw_ms   = new QLabel();
    m_fld_diag_accel_raw_cmd  = new LineEdit8();
    m_fld_diag_accel_raw      ->setAlignment(Qt::AlignRight);
    m_fld_diag_accel_raw_ms   ->setAlignment(Qt::AlignRight);
    m_fld_diag_accel_raw_cmd  ->setAlignment(Qt::AlignCenter);
    m_fld_diag_accel_raw_cmd  ->setReadOnly(true);
    m_fld_diag_accel_raw_cmd  ->setText("06");

    m_fld_diag_accel_corr     = new QLineEdit();
    m_fld_diag_accel_corr_log = new QCheckBox();
    m_fld_diag_accel_corr_ms  = new QLabel();
    m_fld_diag_accel_corr_cmd = new LineEdit8();
    m_fld_diag_accel_corr     ->setAlignment(Qt::AlignRight);
    m_fld_diag_accel_corr_ms  ->setAlignment(Qt::AlignRight);
    m_fld_diag_accel_corr_cmd ->setAlignment(Qt::AlignCenter);
    m_fld_diag_accel_corr_cmd ->setReadOnly(true);
    m_fld_diag_accel_corr_cmd ->setText("18");

    m_fld_diag_accel_incl     = new QLineEdit();
    m_fld_diag_accel_incl_log = new QCheckBox();
    m_fld_diag_accel_incl_ms  = new QLabel();
    m_fld_diag_accel_incl_cmd = new LineEdit8();
    m_fld_diag_accel_incl     ->setAlignment(Qt::AlignRight);
    m_fld_diag_accel_incl_ms  ->setAlignment(Qt::AlignRight);
    m_fld_diag_accel_incl_cmd ->setAlignment(Qt::AlignCenter);
    m_fld_diag_accel_incl_cmd ->setReadOnly(true);
    m_fld_diag_accel_incl_cmd ->setText("11");

    m_fld_diag_HV             = new QLineEdit();
    m_fld_diag_HV_log         = new QCheckBox();
    m_fld_diag_HV_ms          = new QLabel();
    m_fld_diag_HV_cmd         = new LineEdit8();
    m_fld_diag_HV             ->setAlignment(Qt::AlignRight);
    m_fld_diag_HV_ms          ->setAlignment(Qt::AlignRight);
    m_fld_diag_HV_cmd         ->setAlignment(Qt::AlignCenter);
    m_fld_diag_HV_cmd         ->setReadOnly(true);
    m_fld_diag_HV_cmd         ->setText("43");

    m_fld_diag_HV_max         = new QLineEdit();
    m_fld_diag_HV_max_log     = new QCheckBox();
    m_fld_diag_HV_max_ms      = new QLabel();
    m_fld_diag_HV_max_cmd     = new LineEdit8();
    m_fld_diag_HV_max         ->setAlignment(Qt::AlignRight);
    m_fld_diag_HV_max_ms      ->setAlignment(Qt::AlignRight);
    m_fld_diag_HV_max_cmd     ->setAlignment(Qt::AlignCenter);
    m_fld_diag_HV_max_cmd     ->setReadOnly(true);
    m_fld_diag_HV_max_cmd     ->setText("42");

    m_fld_diag_HV_corr        = new QLineEdit();
    m_fld_diag_HV_corr_log    = new QCheckBox();
    m_fld_diag_HV_corr_ms     = new QLabel();
    m_fld_diag_HV_corr_cmd    = new LineEdit8();
    m_fld_diag_HV_corr        ->setAlignment(Qt::AlignRight);
    m_fld_diag_HV_corr_ms     ->setAlignment(Qt::AlignRight);
    m_fld_diag_HV_corr_cmd    ->setAlignment(Qt::AlignCenter);
    m_fld_diag_HV_corr_cmd    ->setReadOnly(true);
    m_fld_diag_HV_corr_cmd    ->setText("44");

    m_fld_diag_HV_warm        = new QLineEdit();
    m_fld_diag_HV_warm_log    = new QCheckBox();
    m_fld_diag_HV_warm_ms     = new QLabel();
    m_fld_diag_HV_warm_cmd    = new LineEdit8();
    m_fld_diag_HV_warm        ->setAlignment(Qt::AlignRight);
    m_fld_diag_HV_warm_ms     ->setAlignment(Qt::AlignRight);
    m_fld_diag_HV_warm_cmd    ->setAlignment(Qt::AlignCenter);
    m_fld_diag_HV_warm_cmd    ->setReadOnly(true);
    m_fld_diag_HV_warm_cmd    ->setText("46");

    m_fld_diag_config_reg     = new QLineEdit();
    m_fld_diag_config_reg_log = new QCheckBox();
    m_fld_diag_config_reg_ms  = new QLabel();
    m_fld_diag_config_reg_cmd = new LineEdit8();
    m_fld_diag_config_reg     ->setAlignment(Qt::AlignRight);
    m_fld_diag_config_reg_ms  ->setAlignment(Qt::AlignRight);
    m_fld_diag_config_reg_cmd ->setAlignment(Qt::AlignCenter);
    m_fld_diag_config_reg_cmd ->setReadOnly(true);
    m_fld_diag_config_reg_cmd ->setText("1F");

    m_fld_diag_Temperature    = new QLineEdit();
    m_fld_diag_Temperature_log= new QCheckBox();
    m_fld_diag_Temperature_ms = new QLabel();
    m_fld_diag_Temperature_cmd= new LineEdit8();
    m_fld_diag_Temperature    ->setAlignment(Qt::AlignRight);
    m_fld_diag_Temperature_ms ->setAlignment(Qt::AlignRight);
    m_fld_diag_Temperature_cmd->setAlignment(Qt::AlignCenter);
    m_fld_diag_Temperature_cmd->setReadOnly(true);
    m_fld_diag_Temperature_cmd->setText("05");

    for(int i = 0; i < 8; ++i)
    {
        m_fld_diag_ADC[i]     = new QLineEdit();
        m_fld_diag_ADC_log[i] = new QCheckBox();
        m_fld_diag_ADC_ms[i]  = new QLabel();
        m_fld_diag_ADC_cmd[i] = new LineEdit8();
        m_fld_diag_ADC[i]    ->setAlignment(Qt::AlignRight);
        m_fld_diag_ADC_ms[i] ->setAlignment(Qt::AlignRight);
        m_fld_diag_ADC_cmd[i]->setAlignment(Qt::AlignCenter);
        m_fld_diag_ADC_cmd[i]->setReadOnly(true);
        m_fld_diag_ADC_cmd[i]->setText("1E");
    }

    m_fld_diag_gamma_avrg_cnts= new QSpinBox();
    m_fld_diag_gamma_avrg_cnts->setLocale(QLocale("C"));
    m_fld_diag_gamma_avrg_cnts->setValue(4);
    m_fld_diag_gamma_avrg_cnts->setPrefix(tr("avrg") + QStringLiteral(" "));
    m_fld_diag_gamma_avrg_cnts->setRange(1, 30);
    connect(m_fld_diag_gamma_avrg_cnts, SIGNAL(valueChanged(int)), this, SLOT(slot_set_gamma_counts_avrg(int)));

    m_fld_diag_poll_time= new QSpinBox();
    m_fld_diag_poll_time->setLocale(QLocale("C"));
    m_fld_diag_poll_time->setValue(2);
    m_fld_diag_poll_time->setPrefix(tr("poll time") + QStringLiteral(" "));
    m_fld_diag_poll_time->setSuffix(QStringLiteral(" ") + tr("sec"));
    m_fld_diag_poll_time->setRange(1, 10);
    connect(m_fld_diag_poll_time, SIGNAL(valueChanged(int)), this, SLOT(slot_set_poll_time(int)));

    //-----------------------------------------------------------
    QVBoxLayout *vl = new QVBoxLayout();
    vl->addWidget(m_fld_diag_poll_time);
    vl->addStretch();

    QHBoxLayout *poll_layout = new QHBoxLayout();
    poll_layout->addStretch();
    poll_layout->addLayout(vl);
    //poll_layout->addStretch();

    //-----------------------------------------------------------
    m_fld_diag_HV_set_lo = new QPushButton(QString("%1 %2").arg(tr("Set HV Low"), "[0x40]"));
    m_fld_diag_HV_set_lo->setAutoDefault(false);
    m_fld_diag_HV_set_lo->setToolTip(tr("This command to learn the lowest possible voltage of the HV.\n"
                                        "This information is need to calibrate the HV.\n"
                                        "This command sets the DAC to 0x00, the node returns a <0x01> cmd ack and\n"
                                        "the HV turns on for ten seconds.\n"
                                        "Record this voltage for future use."));
    connect(m_fld_diag_HV_set_lo, SIGNAL(clicked()), this, SLOT(slot_set_hv_low_level()));

    m_fld_diag_HV_set_hi = new QPushButton(QString("%1 %2").arg(tr("Set HV High"), "[0x41]"));
    m_fld_diag_HV_set_hi->setAutoDefault(false);
    m_fld_diag_HV_set_hi->setToolTip(
                tr(
                    "This command to learn the highest possible voltage of the HV.\n"
                    "This information is need to calibrate the HV.\n"
                    "This command sets the DAC to 0xFF, the node returns a <0x01> cmd ack and\n"
                    "the HV turns on for ten seconds.\n"
                    "Record this voltage for future use."));
    connect(m_fld_diag_HV_set_hi, SIGNAL(clicked()), this, SLOT(slot_set_hv_high_level()));

    QVBoxLayout *set_hv_buttons_layout = new QVBoxLayout();
    set_hv_buttons_layout->addWidget(m_fld_diag_HV_set_lo);
    set_hv_buttons_layout->addWidget(m_fld_diag_HV_set_hi);
    QHBoxLayout *set_hv_layout = new QHBoxLayout();
    set_hv_layout->addLayout(set_hv_buttons_layout);
    set_hv_layout->addStretch();

    //-----------------------------------------------------------
    m_diagn_led_icon = new QPushButton(m_ico_gray, "");
    m_diagn_led_icon->setFlat(true);
    m_diagn_led_icon->setAutoDefault(false);
    m_diagn_led_icon->setToolTip(tr("Tool activity status"));

    //---------------------------------------------------------
    m_diagnostic_select_all = new QCheckBox();
    m_diagnostic_select_all->setTristate(false);
    m_diagnostic_select_all->setToolTip( trUtf8("Select / Deselect All") );
    connect( m_diagnostic_select_all, &QCheckBox::clicked, this,
            [this]( bool checked )
    {
        if ( checked == true )
        {
            //m_diagnostic_select_all->setCheckState( Qt::Checked );
            set_diagnostic_fields_checked( true );
        }
        else
        {
            //m_diagnostic_select_all->setCheckState( Qt::Unchecked );
            set_diagnostic_fields_checked( false );
        }
    });

    //-----------------------------------------------------------
    m_button_diagn_start = new QPushButton(QString(" %1 ").arg(tr("Connect")));
    m_button_diagn_start->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_button_diagn_start->setCheckable(true);
    m_button_diagn_start->setAutoDefault(false);
    m_button_diagn_start->setStyleSheet("background-color: normal;");
    m_button_diagn_start->setShortcut(QKeySequence("Ctrl+R"));
    m_button_diagn_start->setToolTip(m_button_diagn_start->shortcut().toString());
    connect(m_button_diagn_start, SIGNAL(clicked()), this, SLOT(slot_run_diagnostic()));

    QHBoxLayout *button_connect_layout = new QHBoxLayout();
    button_connect_layout->addWidget(m_diagn_led_icon);
    //// button_connect_layout->addStretch();
    button_connect_layout->addWidget(m_button_diagn_start);
    button_connect_layout->addStretch();

    //-----------------------------------------------------------
    QHBoxLayout *led_layout = new QHBoxLayout();
    led_layout->addWidget(new QLabel(tr("Communication protocol")));
    led_layout->addStretch();

    QHBoxLayout *log_layout = new QHBoxLayout();
    log_layout->addWidget(m_fld_diag_term_fname_log);
    log_layout->addWidget(m_term_select_log_button);

    QHBoxLayout *clear_layout = new QHBoxLayout();
    clear_layout->addWidget(m_chkbox_term_log_enabled);
    clear_layout->addStretch();
    clear_layout->addWidget(m_term_clear_button);

    QVBoxLayout *term_layout = new QVBoxLayout();
    term_layout->addLayout(led_layout);
    term_layout->addWidget(m_terminal);
    term_layout->addLayout(clear_layout);
    term_layout->addLayout(log_layout);

    //-----------------------------------------------------------
    int nrow = 0;
    QGroupBox *box_toolinfo = new QGroupBox(tr("Info"));
    box_toolinfo->setContentsMargins(0,0,0,0);
    QGridLayout *grid_info = new QGridLayout(box_toolinfo);
    grid_info->setContentsMargins(1, 1, 1, 1);

    grid_info->addWidget(new QLabel(tr("Code")         ), nrow, 1, Qt::AlignCenter);
    grid_info->addWidget(new QLabel(tr("Obtained data")), nrow, 2);
    grid_info->addWidget(m_diagnostic_select_all,         nrow, 3, Qt::AlignCenter );
    grid_info->addWidget(new QLabel(tr("Reply\nTime")  ), nrow, 4, Qt::AlignCenter);
    ++nrow;

    m_fld_diag_Partnumber_log->setToolTip(tr("Part Number"));
    grid_info->addWidget(new QLabel(m_fld_diag_Partnumber_log->toolTip()), nrow, 0);
    grid_info->addWidget(m_fld_diag_Partnumber_cmd,     nrow, 1);
    grid_info->addWidget(m_fld_diag_Partnumber,         nrow, 2);
    grid_info->addWidget(m_fld_diag_Partnumber_log,     nrow, 3, Qt::AlignCenter);
    grid_info->addWidget(m_fld_diag_Partnumber_ms,      nrow, 4, Qt::AlignCenter);
    ++nrow;

    m_fld_diag_config_reg_log->setToolTip(tr("CONFIG"));
    grid_info->addWidget(new QLabel(m_fld_diag_config_reg_log->toolTip()),nrow, 0);
    grid_info->addWidget(m_fld_diag_config_reg_cmd,      nrow, 1);
    grid_info->addWidget(m_fld_diag_config_reg,          nrow, 2);
    grid_info->addWidget(m_fld_diag_config_reg_log,      nrow, 3, Qt::AlignCenter);
    grid_info->addWidget(m_fld_diag_config_reg_ms,       nrow, 4, Qt::AlignCenter);
    ++nrow;

    for(int i = 0; i < 8; ++i)
    {
        QString bf;
        if (i == 0)
            bf = tr("+15 VDC");
        else if (i == 1)
            bf = tr("+5 VDC");
        else if (i == 2)
            bf = tr("DV485");
        else if (i == 3)
            bf = tr("HV control");
        else if (i == 4)
            bf = tr("Temperature sensor");
        else if (i == 5)
            bf = tr("Temperature calibration");
        else if (i == 6)
            bf = tr("2.5V reference");
        else if (i == 7)
            bf = tr("Power supply amperage");

        m_fld_diag_ADC_log[i]->setToolTip(QString("[%1] %2").arg(tr("ADC")).arg(bf));
        grid_info->addWidget(new QLabel(m_fld_diag_ADC_log[i]->toolTip()), nrow, 0);
        grid_info->addWidget(m_fld_diag_ADC_cmd[i],   nrow, 1);
        grid_info->addWidget(m_fld_diag_ADC[i],       nrow, 2);
        grid_info->addWidget(m_fld_diag_ADC_log[i],   nrow, 3, Qt::AlignCenter);
        grid_info->addWidget(m_fld_diag_ADC_ms[i],    nrow, 4, Qt::AlignCenter);
        ++nrow;
    }

    int separate_row = nrow;

    m_fld_diag_rectask_log->setToolTip(tr("Record Task"));
    grid_info->addWidget(new QLabel(m_fld_diag_rectask_log->toolTip()),nrow, 0);
    grid_info->addWidget(m_fld_diag_rectask_cmd,          nrow, 1);
    grid_info->addWidget(m_fld_diag_rectask,              nrow, 2);
    grid_info->addWidget(m_fld_diag_rectask_log,          nrow, 3, Qt::AlignCenter);
    grid_info->addWidget(m_fld_diag_rectask_ms,           nrow, 4, Qt::AlignCenter);
    ++nrow;

    //-----------------------
    m_fld_diag_Temperature_log->setToolTip(tr("Temperature, °C"));
    grid_info->addWidget(new QLabel(m_fld_diag_Temperature_log->toolTip()), nrow, 0);
    grid_info->addWidget(m_fld_diag_Temperature_cmd,  nrow, 1);
    grid_info->addWidget(m_fld_diag_Temperature,      nrow, 2);
    grid_info->addWidget(m_fld_diag_Temperature_log,  nrow, 3, Qt::AlignCenter);
    grid_info->addWidget(m_fld_diag_Temperature_ms,   nrow, 4, Qt::AlignCenter);
    ++nrow;

    //-----------------------
    m_fld_diag_HV_corr->setToolTip(tr("It is the hex value being written to the HV DAC\n"
                                      "to produce a specified voltage"));
    m_fld_diag_HV_corr_log->setToolTip(tr("HV DAC"));
    QLabel *lbl = new QLabel(m_fld_diag_HV_corr_log->toolTip());
    lbl->setToolTip(m_fld_diag_HV_corr->toolTip());
    grid_info->addWidget(lbl,                        nrow, 0);
    grid_info->addWidget(m_fld_diag_HV_corr_cmd,    nrow, 1);
    grid_info->addWidget(m_fld_diag_HV_corr,        nrow, 2);
    grid_info->addWidget(m_fld_diag_HV_corr_log,    nrow, 3, Qt::AlignCenter);
    grid_info->addWidget(m_fld_diag_HV_corr_ms,     nrow, 4, Qt::AlignCenter);
    ++nrow;

    //-----------------------
    m_fld_diag_HV_log->setToolTip(tr("Operating HV, V"));
    grid_info->addWidget(new QLabel(m_fld_diag_HV_log->toolTip()), nrow, 0);
    grid_info->addWidget(m_fld_diag_HV_cmd,         nrow, 1);
    grid_info->addWidget(m_fld_diag_HV,             nrow, 2);
    grid_info->addWidget(m_fld_diag_HV_log,         nrow, 3, Qt::AlignCenter);
    grid_info->addWidget(m_fld_diag_HV_ms,          nrow, 4, Qt::AlignCenter);
    ++nrow;

    //-----------------------
    m_fld_diag_HV_max_log->setToolTip(tr("HV Min/Test/Max, V"));
    grid_info->addWidget(new QLabel(m_fld_diag_HV_max_log->toolTip()), nrow, 0);
    grid_info->addWidget(m_fld_diag_HV_max_cmd,     nrow, 1);
    grid_info->addWidget(m_fld_diag_HV_max,         nrow, 2);
    grid_info->addWidget(m_fld_diag_HV_max_log,     nrow, 3, Qt::AlignCenter);
    grid_info->addWidget(m_fld_diag_HV_max_ms,      nrow, 4, Qt::AlignCenter);
    ++nrow;

    //-----------------------
    m_fld_diag_HV_warm_log->setToolTip(tr("HV WarmUp")+"," + tr("sec"));
    grid_info->addWidget(new QLabel(m_fld_diag_HV_warm_log->toolTip()), nrow, 0);
    grid_info->addWidget(m_fld_diag_HV_warm_cmd,    nrow, 1);
    grid_info->addWidget(m_fld_diag_HV_warm,        nrow, 2);
    grid_info->addWidget(m_fld_diag_HV_warm_log,    nrow, 3, Qt::AlignCenter);
    grid_info->addWidget(m_fld_diag_HV_warm_ms,     nrow, 4, Qt::AlignCenter);
    ++nrow;

    //-----------------------
    m_fld_diag_gamma_AcqTime_log->setToolTip(tr("Operating HV, V"));
    grid_info->addWidget(new QLabel(m_fld_diag_gamma_AcqTime_log->toolTip()), nrow, 0);
    grid_info->addWidget(m_fld_diag_gamma_AcqTime_cmd, nrow, 1);
    grid_info->addWidget(m_fld_diag_gamma_AcqTime,     nrow, 2);
    grid_info->addWidget(m_fld_diag_gamma_AcqTime_log, nrow, 3, Qt::AlignCenter);
    grid_info->addWidget(m_fld_diag_gamma_AcqTime_ms,  nrow, 4, Qt::AlignCenter);
    ++nrow;

    //-----------------------
    grid_info->addWidget(new QLabel(tr("Accel Raw, V")), nrow, 0);
    grid_info->addWidget(m_fld_diag_accel_raw_cmd, nrow, 1);
    grid_info->addWidget(m_fld_diag_accel_raw,     nrow, 2);
    grid_info->addWidget(m_fld_diag_accel_raw_log, nrow, 3, Qt::AlignCenter);
    grid_info->addWidget(m_fld_diag_accel_raw_ms,  nrow, 4, Qt::AlignCenter);
    ++nrow;

    //-----------------------
    grid_info->addWidget(new QLabel(tr("Accel Corrected")), nrow, 0);
    grid_info->addWidget(m_fld_diag_accel_corr_cmd, nrow, 1);
    grid_info->addWidget(m_fld_diag_accel_corr,     nrow, 2);
    grid_info->addWidget(m_fld_diag_accel_corr_log, nrow, 3, Qt::AlignCenter);
    grid_info->addWidget(m_fld_diag_accel_corr_ms,  nrow, 4, Qt::AlignCenter);
    ++nrow;

    //-----------------------
    grid_info->addWidget(new QLabel(tr("Accel Incl, deg.")), nrow, 0);
    grid_info->addWidget(m_fld_diag_accel_incl_cmd, nrow, 1);
    grid_info->addWidget(m_fld_diag_accel_incl,     nrow, 2);
    grid_info->addWidget(m_fld_diag_accel_incl_log, nrow, 3, Qt::AlignCenter);
    grid_info->addWidget(m_fld_diag_accel_incl_ms,  nrow, 4, Qt::AlignCenter);
    ++nrow;

    //-----------------------
    QHBoxLayout *gamma_avrg_layout = new QHBoxLayout();
    gamma_avrg_layout->addWidget(new QLabel(tr("Gamma Counts")));
    gamma_avrg_layout->addStretch();
    gamma_avrg_layout->addWidget(m_fld_diag_gamma_avrg_cnts);
    m_fld_diag_gamma_avrg_cnts->setToolTip(tr("Change Gamma Counts average of \"n\" acquisitions"));

    grid_info->addLayout(gamma_avrg_layout, nrow, 0);
    grid_info->addWidget(m_fld_diag_gamma_cmd, nrow, 1);
    grid_info->addWidget(m_fld_diag_gamma_cnts,     nrow, 2);
    grid_info->addWidget(m_fld_diag_gamma_log, nrow, 3, Qt::AlignCenter);
    grid_info->addWidget(m_fld_diag_gamma_ms,  nrow, 4, Qt::AlignCenter);
    ++nrow;

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);  // QFrame::Sunken QFrame::Raised
    line->setLineWidth(1);

    //-----------------------
    nrow = separate_row+2;
    grid_info->addLayout(term_layout, 0, 5, nrow, 1);
    ++nrow;
    grid_info->addLayout(poll_layout, nrow, 5, 2, 1);
    ++nrow;
    grid_info->addWidget(line, nrow, 5, 2, 1);
    ++nrow;
    grid_info->addWidget(new QLabel(" "), nrow, 5, 2, 1);
    ++nrow;
    grid_info->addLayout(set_hv_layout, nrow, 5, 2, 1);

    //-----------------------
    grid_info->setColumnStretch(0,  1);
    grid_info->setColumnStretch(1,  1);
    grid_info->setColumnStretch(2, 40);
    grid_info->setColumnStretch(3,  1);
    grid_info->setColumnStretch(4,  1);
    grid_info->setColumnStretch(5, 60);

    //===========================================================
    QVBoxLayout *tablayout = new QVBoxLayout();
    tablayout->addWidget(box_toolinfo);
    tablayout->addLayout(button_connect_layout);
    tablayout->addStretch(100);

    QWidget *wgt = new QWidget();
    wgt->setLayout(tablayout);
    return wgt;
}

//---------------------------------------------------------
void DIAGN_GUI::set_diagnostic_fields_checked( int state )
{
    m_fld_diag_Partnumber_log->setChecked( state );
    m_fld_diag_gamma_log->setChecked( state );
    m_fld_diag_Temperature_log->setChecked( state );
    m_fld_diag_accel_raw_log->setChecked( state );
    m_fld_diag_accel_incl_log->setChecked( state );
    m_fld_diag_accel_corr_log->setChecked( state );
    m_fld_diag_gamma_AcqTime_log->setChecked( state );
    m_fld_diag_HV_max_log->setChecked( state );
    m_fld_diag_HV_log->setChecked( state );
    m_fld_diag_HV_corr_log->setChecked( state );
    m_fld_diag_HV_warm_log->setChecked( state );
    m_fld_diag_config_reg_log->setChecked( state );
    m_fld_diag_status_log->setChecked( state );
    m_fld_diag_rectask_log->setChecked( state );

    for( int i = 0; i < 8; ++i )
    {
        m_fld_diag_ADC_log[i]->setChecked( state );
    }
}

//---------------------------------------------------------
void DIAGN_GUI::set_enabled_button_set_hi_hv(bool status)
{
    m_fld_diag_HV_set_hi->setVisible(status);
}

//---------------------------------------------------------
void DIAGN_GUI::set_enabled_button_set_lo_hv(bool status)
{
    m_fld_diag_HV_set_lo->setVisible(status);
}

//---------------------------------------------------------
void DIAGN_GUI::slot_set_poll_time(int sec)
{
    DBG(3, QString("set sec: %1").arg(sec));
    m_diagn->set_polling_time(sec);
    m_diagn->restart_scan();
}

//---------------------------------------------------------
void DIAGN_GUI::set_poll_time(int sec)
{
    auto status = m_fld_diag_poll_time->blockSignals(true);
    m_fld_diag_poll_time->setValue(sec);
    m_fld_diag_poll_time->blockSignals(status);
}

//---------------------------------------------------------
void DIAGN_GUI::slot_clear_terminal()
{
    m_terminal->clear();
    clear_terminal_fields(1);
}

//---------------------------------------------------------
void DIAGN_GUI::clear_terminal_fields(int mode)
{
    // mode - режим очистки терминала
    // 1 - очистить поля с кодами запросов

    m_fld_diag_Partnumber->clear();
    m_fld_diag_Partnumber_ms->clear();
    m_fld_diag_gamma_cnts->clear();
    m_fld_diag_gamma_ms->clear();
    m_fld_diag_HV_max->clear();
    m_fld_diag_HV_max_ms->clear();
    m_fld_diag_HV_corr->clear();
    m_fld_diag_HV_corr_ms->clear();
    m_fld_diag_HV_warm->clear();
    m_fld_diag_HV_warm_ms->clear();
    m_fld_diag_HV->clear();
    m_fld_diag_HV_ms->clear();
    m_fld_diag_rectask->clear();
    m_fld_diag_rectask_ms->clear();
    m_fld_diag_config_reg->clear();
    m_fld_diag_config_reg_ms->clear();
    m_fld_diag_Temperature->clear();
    m_fld_diag_Temperature_ms->clear();
    m_fld_diag_gamma_AcqTime->clear();
    m_fld_diag_gamma_AcqTime_ms->clear();
    m_fld_diag_accel_raw->clear();
    m_fld_diag_accel_raw_ms->clear();
    m_fld_diag_accel_corr->clear();
    m_fld_diag_accel_corr_ms->clear();
    m_fld_diag_accel_incl->clear();
    m_fld_diag_accel_incl_ms->clear();

    //// memset(&m_Gz_raw, 0, sizeof(m_Gz_raw));
    //// memset(&m_Gz_corrected, 0, sizeof(m_Gz_corrected));
    //// memset(&m_Gz_inclination, 0, sizeof(m_Gz_inclination));

    for(int i = 0; i < 8; ++i)
    {
        m_fld_diag_ADC[i]->clear();
        m_fld_diag_ADC_ms[i]->clear();
    }

    (void)mode;
    /*
    if (mode == 1) // Очистить коды запросов
    {
        m_fld_diag_Partnumber_cmd->clear();
        m_fld_diag_accel_raw_cmd->clear();
        m_fld_diag_accel_corr_cmd->clear();
        m_fld_diag_accel_incl_cmd->clear();
        m_fld_diag_gamma_cmd->clear();
        m_fld_diag_HV_max_cmd->clear();
        m_fld_diag_HV_corr_cmd->clear();
        m_fld_diag_HV_warm_cmd->clear();
        m_fld_diag_HV_cmd->clear();
        m_fld_diag_rectask_cmd->clear();
        m_fld_diag_config_reg_cmd->clear();
        m_fld_diag_Temperature_cmd->clear();
        m_fld_diag_gamma_AcqTime_cmd->clear();
        for(int i = 0; i < 8; ++i)
        {
            m_fld_diag_ADC_cmd[i]->clear();
        }
    }
    */
}

//---------------------------------------------------------
void DIAGN_GUI::set_led_icon_off()
{
    m_diagn_led_icon->setIcon(m_ico_gray);
}

//---------------------------------------------------------
void DIAGN_GUI::set_led_icon_active()
{
    m_diagn_led_icon->setIcon(m_ico_green);
}

//---------------------------------------------------------
void DIAGN_GUI::set_led_icon_alarm()
{
    m_diagn_led_icon->setIcon(m_ico_red);
}

//---------------------------------------------------------
void DIAGN_GUI::set_led_icon_idle()
{
    m_diagn_led_icon->setIcon(m_ico_yellow);
}

