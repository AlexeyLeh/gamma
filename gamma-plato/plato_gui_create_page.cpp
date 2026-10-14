/*!
  \file   plato_gui_create_page.cpp
  \brief  Раскладка полей для теста линейности (Plateaue test)
  \author Aleksey Lekhtin
  \date   05.11.2016
*/

#include "mainwindow.h"
#include "plato_gui.h"

//---------------------------------------------------------
QWidget *PLATO_GUI::create_plato_page()
{
    QWidget *box_pass    = create_box1();
    QWidget *box_params  = create_box2();
    QWidget *box_graph   = create_box3_chart();
    QWidget *box_table   = create_box4_table();
    QWidget *box_buttons = create_buttons();

    QVBoxLayout *layout1 = new QVBoxLayout();
    layout1->addWidget(box_pass, 1);
    layout1->addWidget(box_table, 99);

    QVBoxLayout *layout2 = new QVBoxLayout();
    layout2->addWidget(box_params, 1);
    layout2->addWidget(box_graph, 99);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addLayout(layout1, 40);
    layout3->addLayout(layout2, 60);

    QVBoxLayout *fields_layout = new QVBoxLayout();
    fields_layout->addLayout(layout3, 99);
    //// fields_layout->addWidget(box_buttons, 1);

    //-----------------------
    QWidget *scrollWidget = new QWidget();
    scrollWidget->setLayout(fields_layout);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    //-----------------------
    // Главный макет страницы
    QVBoxLayout *main_vlay = new QVBoxLayout();
    main_vlay->addWidget(scrollArea);
    main_vlay->addWidget(box_buttons);

    QWidget *wgt = new QWidget();
    wgt->setLayout(main_vlay);
    return wgt;
}

//---------------------------------------------------------
QWidget *PLATO_GUI::create_buttons()
{
    if (!app()->simulation()) {
        m_button_plato_run = new QPushButton(tr("Start Plateau Test"));
        m_button_plato_run->setCheckable(true);
        m_button_plato_run->setAutoDefault(false);
        m_button_plato_run->setShortcut(QKeySequence("Ctrl+R"));
        QString tip = QStringLiteral("%1 [%2]").arg(tr("Start Plateau Test"), m_button_plato_run->shortcut().toString());
        m_button_plato_run->setToolTip(tip);
        connect(m_button_plato_run, &QPushButton::clicked, this, &PLATO_GUI::slot_run_plato_test);

        auto icon = QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton);
        m_button_write_HV = new QPushButton(icon, tr("Write HV"));
        m_button_write_HV->setDisabled(true);
        m_button_write_HV->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        m_button_write_HV->setShortcut(QKeySequence("Ctrl+F5"));
        tip = QStringLiteral("%1\n[%2]")
              .arg(tr("Writes to device the voltage calibration values\ndetermined from the linearity test"),
                   m_button_write_HV->shortcut().toString());
        m_button_write_HV->setToolTip(tip);
        connect(m_button_write_HV, SIGNAL(clicked(bool)), this, SLOT(slot_write_hpvs(bool)));

        m_plato_led_icon = new QPushButton(m_ico_gray, "");
        m_plato_led_icon->setFlat(true);
        m_plato_led_icon->setAutoDefault(false);

        m_button_save_report = new QPushButton(QString(" %1 ").arg(tr("Save Report")));
        m_button_save_report->setCheckable(false);
        tip = QStringLiteral("%1\n[%2]").arg(tr("Save Report"), "Ctrl+S");
        m_button_save_report->setToolTip(tip);
        connect(m_button_save_report, SIGNAL(clicked(bool)), SLOT(slot_save_report()));

        m_button_load_report = new QPushButton(QString(" %1 ").arg(tr("Load Report")));
        m_button_load_report->setCheckable(false);
        tip = QStringLiteral("%1\n[%2]").arg(tr("Load Report"), "Ctrl+L");
        m_button_load_report->setToolTip(tip);
        connect(m_button_load_report, &QPushButton::clicked, [this]() {
            slot_load_report();
        });

        auto *buttons_layout = new QHBoxLayout();
        buttons_layout->setContentsMargins(0, 0, 0, 0);
        buttons_layout->addWidget(m_plato_led_icon);
        buttons_layout->addWidget(m_button_plato_run);
        buttons_layout->addSpacing(64);
        buttons_layout->addWidget(m_button_write_HV);
        buttons_layout->addStretch();
        buttons_layout->addWidget(m_button_save_report);
        buttons_layout->addWidget(m_button_load_report);

        QWidget *wgt = new QWidget(mw());
        wgt->setLayout(buttons_layout);
        return wgt;
    }

    m_button_plato_run = new QPushButton(tr("Start Simulation"));
    m_button_plato_run->setCheckable(false);
    m_button_plato_run->setAutoDefault(false);
    m_button_plato_run->setShortcut(QKeySequence("Ctrl+R"));
    QString tip = QStringLiteral("%1\n[%2]").arg(tr("Start Simulation"), m_button_plato_run->shortcut().toString());
    m_button_plato_run->setToolTip(tip);
    connect(m_button_plato_run, &QPushButton::clicked, this, [this]() {
        if (m_simulation_fileinfo.fileName().isEmpty()) {
            bool ok = simulation_load();
            if (ok) {
                m_button_plato_run->setText(tr("Replay Simulation"));
            }
            return;
        }

        auto errstr = m_plato->simulation_load(m_plato->data(), m_simulation_fileinfo);
        if (!errstr.isEmpty()) {
            auto str = errstr;
            DBG(1, str.replace("\n", " "));
            display_error_message(errstr);
            return;
        }
        bool ok = m_plato->set_simulation_data(m_plato->data());
        if (ok) {
            m_button_plato_run->setText(tr("Replay Simulation"));
        }
    });

    auto *buttons_run_simulation_layout = new QHBoxLayout();
    buttons_run_simulation_layout->setContentsMargins(0, 0, 0, 0);
    buttons_run_simulation_layout->addWidget(m_button_plato_run);
    buttons_run_simulation_layout->addStretch();

    //-------------------------------------------
    m_button_load_simulation = new QPushButton(tr("Select Simulation"));
    m_button_load_simulation->setCheckable(false);
    m_button_load_simulation->setAutoDefault(false);
    m_button_load_simulation->setShortcut(QKeySequence("Ctrl+L"));
    tip = QStringLiteral("%1\n[%2]").arg(tr("Load Simulation"), m_button_load_simulation->shortcut().toString());
    m_button_load_simulation->setToolTip(tip);
    connect(m_button_load_simulation, &QPushButton::clicked, this, [this](){
        bool ok = simulation_load();
        if (ok) {
            m_button_plato_run->setText(tr("Replay Simulation"));
        }
    });

    m_simulation_file_label = new QLabel();
    m_simulation_file_label->setContentsMargins(0, 0, 0, 0);
    m_simulation_file_label->setAlignment(Qt::AlignLeft);
    QFontMetrics fm = mw()->fontMetrics();
    int ww = fm.horizontalAdvance("W") * 45;
    m_simulation_file_label->setMinimumWidth(ww);

    auto *simulation_file_layout = new QHBoxLayout();
    simulation_file_layout->addWidget(m_button_load_simulation);
    simulation_file_layout->addWidget(m_simulation_file_label);
    simulation_file_layout->addStretch();

    //-------------------------------------------
    auto *buttons_layout = new QVBoxLayout();
    buttons_layout->setContentsMargins(0, 0, 0, 0);
    buttons_layout->addLayout(buttons_run_simulation_layout);
    buttons_layout->addLayout(simulation_file_layout);

    QWidget *wgt = new QWidget(mw());
    wgt->setLayout(buttons_layout);
    return wgt;
}

// //---------------------------------------------------------
// bool PLATO_GUI::eventFilter(QObject *obj, QEvent *event) {
//     if (obj == m_simulation_filename && event->type() == QEvent::MouseButtonRelease) {
//         auto *mouseEvent = static_cast<QMouseEvent*>(event);
//         if (mouseEvent->button() == Qt::LeftButton) {
//             simulation_load();
//             return true;
//         }
//     }
//     return QObject::eventFilter(obj, event); // Передаем остальные события дальше
// }

//---------------------------------------------------------
QWidget *PLATO_GUI::create_box1()
{
    m_fld_plato_pass_msg = new QLabel(QString("---"));
    m_fld_plato_pass_msg->setAlignment(Qt::AlignCenter);
    m_fld_plato_pass_msg->setFrameStyle(QFrame::Box | QFrame::Raised);
    m_fld_plato_pass_msg->setLineWidth(1);
    m_fld_plato_pass_msg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QFont fnt = m_fld_plato_pass_msg->font();
    fnt.setPointSize(20);
    fnt.setBold(true);
    m_fld_plato_pass_msg->setFont(fnt);
    m_fld_plato_pass_msg->adjustSize();
    m_fld_plato_pass_msg->setMinimumWidth(150);

    auto pass_layout = new QHBoxLayout();
    pass_layout->addWidget(m_fld_plato_pass_msg);
    pass_layout->addStretch();

    //---------------------------------------------------------
    QLineEdit in(mw());
    auto labelBoldFont = in.font();
    labelBoldFont.setPointSize(mw()->content_font_pointSize());
    labelBoldFont.setBold(true);
    QFontMetrics fm(labelBoldFont);
    //                                        62 min [12:17:33]
    int minFieldWidth = fm.horizontalAdvance("XXXXXXXXXXXXXXXXX");

    m_fld_est_time = new QLineEdit();
    m_fld_est_time->setReadOnly(true);
    m_fld_est_time->setToolTip(tr("Estimated time of calibration completion"));
    m_fld_est_time->setFont(labelBoldFont);
    m_fld_est_time->setMinimumWidth(minFieldWidth);
    m_fld_est_time->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_fld_acq_time = new QLineEdit();
    m_fld_acq_time->setReadOnly(true);
    m_fld_acq_time->setToolTip(tr("Time of one measurement"));
    m_fld_acq_time->setFont(labelBoldFont);
    m_fld_acq_time->setMinimumWidth(minFieldWidth);
    m_fld_acq_time->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

    m_fld_hv_step = new QLineEdit();
    m_fld_hv_step->setReadOnly(true);
    m_fld_hv_step->setToolTip(tr("Step of HV increase"));
    m_fld_hv_step->setFont(labelBoldFont);
    m_fld_hv_step->setMinimumWidth(minFieldWidth);
    m_fld_hv_step->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    minFieldWidth = fm.horizontalAdvance("WWWWW"); // "1800 V"
    m_fld_hv_accepted = new QLineEdit();
    m_fld_hv_accepted->setReadOnly(true);
    m_fld_hv_accepted->setAlignment(Qt::AlignCenter);
    m_fld_hv_accepted->setText("---");
    m_fld_hv_accepted->setToolTip(tr("Calibration Voltage"));
    m_fld_hv_accepted->setFont(labelBoldFont);
    m_fld_hv_accepted->setFixedWidth(minFieldWidth);
    m_fld_hv_accepted->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    //--------------------
    QFormLayout *form_acq = new QFormLayout();
    form_acq->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    form_acq->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    ////form_acq->addRow(pass_layout);
    form_acq->addRow(tr("Completion time"),  m_fld_est_time);
    form_acq->addRow(tr("One Acquire Time"), m_fld_acq_time);
    form_acq->addRow(tr("HV step"),          m_fld_hv_step);
    form_acq->addRow(tr("Calibration HV"),   m_fld_hv_accepted);
    ////form_acq->addRow(tr("Calibration HV"),   calibration_hv_layout);
    ////form_acq->addRow(m_fld_plato_pass_msg,  calibration_hv_layout);

    //--------------------
    QGroupBox *box_acqtime = new QGroupBox();
    box_acqtime->setAlignment(Qt::AlignTop);
    auto vlay = new QVBoxLayout(box_acqtime);
    //// vlay->addLayout(calibration_hv_layout);
    //// vlay->addWidget(m_fld_plato_pass_msg);
    vlay->addLayout(form_acq);

    //---------------------------------------------------------
    //                               S/N: 123456789012
    minFieldWidth = fm.horizontalAdvance("WWWWWWWWWWWW") - fm.horizontalAdvance("WW"); // чутка уменьшить-бы

    m_fld_PN = new QLineEdit();
    m_fld_PN->setReadOnly(true);
    m_fld_PN->setAlignment(Qt::AlignCenter);
    m_fld_PN->setFont(labelBoldFont);
    m_fld_PN->setMinimumWidth(minFieldWidth);

    m_fld_SN = new QLineEdit();
    m_fld_SN->setReadOnly(true);
    m_fld_SN->setAlignment(Qt::AlignCenter);
    m_fld_SN->setFont(labelBoldFont);
    m_fld_SN->setMinimumWidth(minFieldWidth);

    m_fld_oper = new QLineEdit();
    m_fld_oper->setReadOnly(true);
    m_fld_oper->setAlignment(Qt::AlignCenter);
    m_fld_oper->setFont(labelBoldFont);
    m_fld_oper->setMinimumWidth(minFieldWidth);

    //--------------------
    QGroupBox *box_sn = new QGroupBox();
    box_sn->setAlignment(Qt::AlignTop);
    QFormLayout *form_sn = new QFormLayout(box_sn);
    form_sn->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    form_sn->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    form_sn->addRow(tr("Part Number"),   m_fld_PN);
    form_sn->addRow(tr("Serial Number"), m_fld_SN);
    form_sn->addRow(tr("Operator"), m_fld_oper);
    form_sn->addRow(new QLabel(" "));
    form_sn->addRow(pass_layout);

    //======
    QHBoxLayout *layout_pass_acq = new QHBoxLayout();
    layout_pass_acq->addWidget(box_acqtime, 1);
    layout_pass_acq->addWidget(box_sn, 1);

    //-----------------------
    auto txt = QString("%1 --/--").arg(tr("Sample"));
    m_fld_samples_msg = new QLabel(txt);
    m_fld_samples_msg->setToolTip(tr("How much samples has been passed already"));
    m_fld_samples_msg->setAlignment(Qt::AlignLeft);

    m_samples_progress_bar = new QProgressBar();
    m_samples_progress_bar->setStyleSheet(QStringLiteral("QProgressBar { text-align: center; }"));

    m_progress_bar = new QProgressBar();
    auto style = QStringLiteral("QProgressBar { height: 5px; } QProgressBar::chunk { background: #7D94B0; }");
    m_progress_bar->setStyleSheet(style);
    m_progress_bar->setTextVisible(false);

    QPalette pb_palette = m_progress_bar->palette();
    pb_palette.setColor(QPalette::Highlight, QColor(0x7D, 0x94, 0xB0)); // Цвет #7D94B0 для заполнения
    m_progress_bar->setPalette(pb_palette);

    auto *hlay_samples = new QHBoxLayout();
    hlay_samples->addWidget(m_fld_samples_msg);
    hlay_samples->addWidget(m_samples_progress_bar);

    //======
    QWidget *wgt = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(wgt);
    layout->addLayout(layout_pass_acq);
    layout->addLayout(hlay_samples);
    layout->addWidget(m_progress_bar);
    layout->addStretch();
    return wgt;
}

//---------------------------------------------------------
QWidget *PLATO_GUI::create_box4_table()
{
    create_table();
    return m_table;
}

//---------------------------------------------------------
QWidget *PLATO_GUI::create_box2()
{
    QLineEdit fld(mw());
    auto boldFont = fld.font();
    boldFont.setBold(true);
    QFontMetrics fm(boldFont);
    int font_height = fm.height();
    int current_font_size = mw()->content_font_pointSize();
    int minimumFiledHeight = font_height + ((current_font_size >= 14) ? 30 : 20);

    m_fld_HV = new QLineEdit();
    m_fld_HV->setReadOnly(true);
    m_fld_HV->setAlignment(Qt::AlignCenter);
    m_fld_HV->setText(QString::number(0));
    m_fld_HV->setContentsMargins(1, 1, 1, 1);
    m_fld_HV->setFont(boldFont);
    m_fld_HV->setMinimumHeight(minimumFiledHeight);

    m_fld_CPS = new QLineEdit();
    m_fld_CPS->setReadOnly(true);
    m_fld_CPS->setAlignment(Qt::AlignCenter);
    m_fld_CPS->setText(QString::number(0, 'f', 1));
    m_fld_CPS->setContentsMargins(1, 1, 1, 1);
    m_fld_CPS->setFont(boldFont);
    m_fld_CPS->setMinimumHeight(minimumFiledHeight);

    m_fld_Temper = new QLineEdit();
    m_fld_Temper->setReadOnly(true);
    m_fld_Temper->setAlignment(Qt::AlignCenter);
    m_fld_Temper->setText(QString::number(0, 'f', 1));
    m_fld_Temper->setContentsMargins(1, 1, 1, 1);
    m_fld_Temper->setFont(boldFont);
    m_fld_Temper->setMinimumHeight(minimumFiledHeight);

    m_fld_temper_mode = new QLineEdit();
    m_fld_temper_mode->setReadOnly(true);
    m_fld_temper_mode->setAlignment(Qt::AlignCenter);
    m_fld_temper_mode->setText("---");
    m_fld_temper_mode->setContentsMargins(1, 1, 1, 1);
    m_fld_temper_mode->setFont(boldFont);
    m_fld_temper_mode->setMinimumHeight(minimumFiledHeight);

    // Выбираем, например, мягкий пастельный оттенок #D1DFF6
    // m_fld_temper_mode->setStyleSheet(
    //     "QPushButton {"
    //     "   background-color: #D1DFF6;" // Основной слегка синий фон
    //     "   color: #2F3E46;"            // Темный читаемый текст
    //     "   border: 1px solid #B2CBF2;" // Тонкая рамка чуть темнее фона
    //     "   border-radius: 4px;"        // Слегка скругленные углы
    //     "   padding: 5px 10px;"
    //     "}"
    //     "QPushButton:hover {"
    //     "   background-color: #C2D6F6;" // Цвет при наведении мыши (чуть темнее)
    //     "}"
    //     "QPushButton:pressed {"
    //     "   background-color: #A0BFF0;" // Цвет при клике (еще темнее)
    //     "}"
    // );
    // m_fld_temper_mode->setFont(boldFont);

    // QPalette pal = m_fld_temper_mode->palette();
    // pal.setColor(QPalette::Button, QColor("#D1DFF6"));
    // m_fld_temper_mode->setPalette(pal);

    /*
    TODO
    // Используем теплый янтарный оттенок #FCDCB2
    ui->highTempButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #FCDCB2;" // Основной мягкий оранжевый фон
        "   color: #3D2B1F;"            // Темно-коричневый или черный текст для контраста
        "   border: 1px solid #EBB87F;" // Тонкая рамка чуть темнее фона
        "   border-radius: 4px;"        // Скругленные углы (как у холодной кнопки)
        "   padding: 5px 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FBCB93;" // Цвет при наведении (чуть насыщеннее)
        "}"
        "QPushButton:pressed {"
        "   background-color: #E6AD67;" // Цвет при клике (еще темнее)
        "}"
    );

    QPalette pal = ui->highTempButton->palette();
    pal.setColor(QPalette::Button, QColor("#FCDCB2"));
    ui->highTempButton->setPalette(pal);
    */


    //=======
    QVBoxLayout *lay_hv = new QVBoxLayout();
    auto *label = new QLabel(tr("High Voltage, V"));
    label->setAlignment(Qt::AlignCenter);
    lay_hv->addWidget(label);
    lay_hv->addWidget(m_fld_HV);
    lay_hv->addStretch();

    QVBoxLayout *lay_cnts = new QVBoxLayout();
    label = new QLabel(tr("CPS"));
    label->setAlignment(Qt::AlignCenter);
    lay_cnts->addWidget(label);
    lay_cnts->addWidget(m_fld_CPS);
    lay_cnts->addStretch();

    QVBoxLayout *lay_temp = new QVBoxLayout();
    label = new QLabel(tr("TEMP, °C"));
    label->setAlignment(Qt::AlignCenter);
    lay_temp->addWidget(label);
    lay_temp->addWidget(m_fld_Temper);
    lay_temp->addStretch();

    QVBoxLayout *lay_mod = new QVBoxLayout();
    label = new QLabel(tr("Test Mode"));
    label->setAlignment(Qt::AlignCenter);
    lay_mod->addWidget(label);
    lay_mod->addWidget(m_fld_temper_mode);
    lay_mod->addStretch();

    QHBoxLayout *lay2 = new QHBoxLayout();
    lay2->addLayout(lay_mod,  25);
    lay2->addLayout(lay_hv,   25);
    lay2->addLayout(lay_cnts, 25);
    lay2->addLayout(lay_temp, 25);

    //======
    QGroupBox *box_realdata = new QGroupBox();
    QVBoxLayout *realdata_layout = new QVBoxLayout(box_realdata);
    realdata_layout->addLayout(lay2);

    QGroupBox *tol = create_tolerance();

    //---------------------------------------------------------
    QWidget *wgt = new QWidget();
    wgt->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *layout = new QVBoxLayout(wgt);
    layout->addWidget(tol);
    layout->addWidget(box_realdata);
    return wgt;
}

//---------------------------------------------------------
QWidget *PLATO_GUI::create_box3_chart()
{
    create_chart();
    return m_chartview;
}

//---------------------------------------------------------
QGroupBox *PLATO_GUI::create_tolerance()
{
    QVector<LineEdit12**> fields {
        &m_fld_tol_HV_LoLimit,
        &m_fld_tol_HV_HiLimit,
        &m_fld_tol_HV_Spread,
        &m_fld_tol_CPS_LoLimit,
        &m_fld_tol_CPS_HiLimit,
        &m_fld_tol_CPS_Spread,
        &m_fld_cold_HV_LoBorder,
        &m_fld_cold_HV_HiBorder,
        &m_fld_cold_HV_Spread,
        &m_fld_cold_CPS_LoBorder,
        &m_fld_cold_CPS_HiBorder,
        &m_fld_cold_CPS_Spread,
        &m_fld_hot_HV_LoBorder,
        &m_fld_hot_HV_HiBorder,
        &m_fld_hot_HV_Spread,
        &m_fld_hot_CPS_LoBorder,
        &m_fld_hot_CPS_HiBorder,
        &m_fld_hot_CPS_Spread,
        &m_fld_HV_Accepted_Lo,
        &m_fld_HV_Accepted_Hi,
        &m_fld_HV_Accepted_Spread,
        //// &m_fld_CPS_Accepted_Lo,
        //// &m_fld_CPS_Accepted_Hi,
        &m_fld_CPS_Accepted_Spread
    };

    for(auto **fld : fields)
    {
        *fld = new LineEdit12();
        (*fld)->setReadOnly(true);
        (*fld)->setAlignment(Qt::AlignCenter);
    }

    //---------------------------------------------------------
    QLabel label;
    QFont fnt = label.font();
    fnt.setBold(true);

    QGroupBox *box = new QGroupBox(tr("Calibration Data"));
    box->setFont(fnt);
    QGridLayout *grid = new QGridLayout(box);
    grid->setAlignment(Qt::AlignCenter);

    grid->addWidget(new QLabel(tr("High Power Voltage (HVPS)")),  0, 1, 1, 3, Qt::AlignCenter);
    grid->addWidget(new QLabel(tr("TOLERANCES")), 2, 0, Qt::AlignCenter);
    grid->addWidget(new QLabel(tr("COLD MODE")),  3, 0, Qt::AlignCenter);
    grid->addWidget(new QLabel(tr("HOT MODE")),   4, 0, Qt::AlignCenter);
    grid->addWidget(new QLabel(tr("Calculated")), 5, 0, Qt::AlignCenter);

    //
    grid->addWidget(new QLabel(tr("Low")),   1, 1, Qt::AlignCenter);
    grid->addWidget(new QLabel(tr("High")),  1, 2, Qt::AlignCenter);
    grid->addWidget(new QLabel(tr("Spread")),1, 3, Qt::AlignCenter);

    grid->addWidget(m_fld_tol_HV_LoLimit,    2, 1);
    grid->addWidget(m_fld_tol_HV_HiLimit,    2, 2);
    grid->addWidget(m_fld_tol_HV_Spread,     2, 3);

    grid->addWidget(m_fld_cold_HV_LoBorder,  3, 1);
    grid->addWidget(m_fld_cold_HV_HiBorder,  3, 2);
    grid->addWidget(m_fld_cold_HV_Spread,    3, 3);

    grid->addWidget(m_fld_hot_HV_LoBorder,   4, 1);
    grid->addWidget(m_fld_hot_HV_HiBorder,   4, 2);
    grid->addWidget(m_fld_hot_HV_Spread,     4, 3);

    grid->addWidget(m_fld_HV_Accepted_Lo,      5, 1);
    grid->addWidget(m_fld_HV_Accepted_Hi,      5, 2);
    grid->addWidget(m_fld_HV_Accepted_Spread,  5, 3);

    //// grid->addWidget(m_fld_CPS_Accepted_Lo,     5, 5);
    //// grid->addWidget(m_fld_CPS_Accepted_Hi,     5, 6);
    grid->addWidget(m_fld_CPS_Accepted_Spread, 5, 7);

    //
    grid->setColumnMinimumWidth(4, 10);

    //
    grid->addWidget(new QLabel(tr("Counts Pre Seconds (CPS)")), 0, 5, 1, 3, Qt::AlignCenter);
    grid->addWidget(new QLabel(tr("LoSlope")), 1, 5, Qt::AlignCenter);
    grid->addWidget(new QLabel(tr("HiSlope")), 1, 6, Qt::AlignCenter);
    grid->addWidget(new QLabel(tr("Spread")),  1, 7, Qt::AlignCenter);

    grid->addWidget(m_fld_tol_CPS_LoLimit,   2, 5);
    grid->addWidget(m_fld_cold_CPS_LoBorder, 3, 5);
    grid->addWidget(m_fld_hot_CPS_LoBorder,  4, 5);

    grid->addWidget(m_fld_tol_CPS_HiLimit,   2, 6);
    grid->addWidget(m_fld_cold_CPS_HiBorder, 3, 6);
    grid->addWidget(m_fld_hot_CPS_HiBorder,  4, 6);

    grid->addWidget(m_fld_tol_CPS_Spread,    2, 7);
    grid->addWidget(m_fld_cold_CPS_Spread,   3, 7);
    grid->addWidget(m_fld_hot_CPS_Spread,    4, 7);

    return box;
}

//---------------------------------------------------------
QProgressBar *PLATO_GUI::progress_bar()
{
    return m_progress_bar;
}

//---------------------------------------------------------
void PLATO_GUI::set_led_icon_off()
{
    if (m_plato_led_icon) {
        m_plato_led_icon->setIcon(m_ico_gray);
    }
}

//---------------------------------------------------------
void PLATO_GUI::set_led_icon_active()
{
    if (m_plato_led_icon) {
        m_plato_led_icon->setIcon(m_ico_green);
    }
}

//---------------------------------------------------------
void PLATO_GUI::set_led_icon_alarm()
{
    if (m_plato_led_icon) {
        m_plato_led_icon->setIcon(m_ico_red);
    }
}

//---------------------------------------------------------
void PLATO_GUI::set_led_icon_idle()
{
    if (m_plato_led_icon) {
        m_plato_led_icon->setIcon(m_ico_yellow);
    }
}
