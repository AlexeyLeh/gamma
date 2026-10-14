/*!
  \file   api_gui_create_page.cpp
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Раскладка полей для API-теста
*/

#include <QStringList>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "api.h"
#include "api_gui.h"
#include "dbg.h"
#include "gamma.h"
#include "main.h"
#include "mainwindow.h"
#include "NodeE4.h"
#include "table.h"

//---------------------------------------------------------
QWidget *API_GUI::create_api_page()
{
    QWidget *box_pass    = create_box_pass();
    QWidget *box_data    = create_box_real_data();
    QWidget *box_results = create_box_result_table();
    QWidget *box_table2  = create_box_CPS_table();
    QWidget *box_table3  = create_box_interpolation_table();
    QWidget *box_buttons = create_buttons();

    QVBoxLayout *layout_left = new QVBoxLayout();
    layout_left->addWidget(box_pass, 1);
    layout_left->addWidget(box_results, 30);
    layout_left->addWidget(box_table2, 70);
    layout_left->addStretch();

    QVBoxLayout *layout_right = new QVBoxLayout();
    layout_right->addWidget(box_data, 1);
    layout_right->addWidget(box_table3, 98);

    QHBoxLayout *layout_main = new QHBoxLayout();
    layout_main->addLayout(layout_left,  40);
    layout_main->addLayout(layout_right, 60);

    //-----------------------
    QWidget *scrollWidget = new QWidget();
    scrollWidget->setLayout(layout_main);

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
QWidget *API_GUI::create_buttons()
{
    m_button_api_run = new QPushButton(tr("Start API Test"));
    m_button_api_run->setCheckable(true);
    m_button_api_run->setAutoDefault(false);
    m_button_api_run->setStyleSheet({});
    m_button_api_run->setShortcut(QKeySequence("Ctrl+R"));
    QString tip = QStringLiteral("%1 [%2]").arg(tr("Start API Test"), m_button_api_run->shortcut().toString());
    m_button_api_run->setToolTip(tip);
    connect(m_button_api_run, &QPushButton::clicked, this, &API_GUI::slot_run_api_test);

    m_api_led_icon = new QPushButton(m_ico_gray, "");
    m_api_led_icon->setFlat(true);
    m_api_led_icon->setAutoDefault(false);

    if (!app()->simulation()) {
        m_button_save_report = new QPushButton(tr("Save Report"));
        m_button_save_report->setCheckable(false);
        tip = QStringLiteral("%1\n[%2]").arg(tr("Save Report"), "Ctrl+S");
        m_button_save_report->setToolTip(tip);
        connect(m_button_save_report, &QPushButton::clicked, this, &API_GUI::slot_save_report);
    }

    //======
    QWidget *wgt = new QWidget();
    auto *hlay = new QHBoxLayout(wgt);
    hlay->setContentsMargins(0, 0, 0, 0);
    hlay->addWidget(m_api_led_icon);
    hlay->addWidget(m_button_api_run);
    hlay->addStretch();
    if (m_button_save_report) {
        hlay->addWidget(m_button_save_report);
    }
    return wgt;
}

//---------------------------------------------------------
QWidget *API_GUI::create_box_pass()
{
    m_fld_api_pass_msg = new QLabel(QString("---"));
    m_fld_api_pass_msg->setAlignment(Qt::AlignCenter);
    m_fld_api_pass_msg->setFrameStyle(QFrame::Box | QFrame::Raised);
    m_fld_api_pass_msg->setLineWidth(1);
    QFont fnt = m_fld_api_pass_msg->font();
    fnt.setPointSize(20);
    fnt.setBold(true);
    m_fld_api_pass_msg->setFont(fnt);
    m_fld_api_pass_msg->adjustSize();
    m_fld_api_pass_msg->setMinimumWidth(150);

     auto pass_layout = new QHBoxLayout();
     pass_layout->addWidget(m_fld_api_pass_msg);
     pass_layout->addStretch();

    //---------------------------------------------------------
    QLineEdit in(mw());
    auto labelBoldFont = in.font();
    labelBoldFont.setBold(true);
    QFontMetrics fm(labelBoldFont);
    //                                        62 min 20 sec [12:17:33]
    int minFieldWidth = fm.horizontalAdvance("WWWWWWWWWWWWWWWWWWWWWWWW") + 10; // самое широкое наполнение

    m_fld_est_time = new QLineEdit();
    m_fld_est_time->setReadOnly(true);
    m_fld_est_time->setToolTip(tr("Estimated time of API test completion"));
    m_fld_est_time->setFont(labelBoldFont);
    m_fld_est_time->setMinimumWidth(minFieldWidth);

    m_fld_acq_time = new QLineEdit();
    m_fld_acq_time->setReadOnly(true);
    m_fld_acq_time->setToolTip(tr("Time of one measurement"));
    m_fld_acq_time->setFont(labelBoldFont);
    m_fld_acq_time->setMinimumWidth(minFieldWidth);

    //--------------------
    QGroupBox *box_acqtime = new QGroupBox();
    box_acqtime->setAlignment(Qt::AlignTop);
    QFormLayout *form_acq = new QFormLayout(box_acqtime);
    form_acq->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    form_acq->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    //// form_acq->addRow(pass_layout);
    form_acq->addRow(tr("Completion time"),  m_fld_est_time);
    form_acq->addRow(tr("One Acquire Time"), m_fld_acq_time);
    //form_acq->addRow(new QLabel(" "));
    form_acq->addRow(pass_layout);

    //---------------------------------------------------------
    minFieldWidth = fm.horizontalAdvance("WWWWWWWWWWWWWWW") + 10;

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

    //======
    QHBoxLayout *layout_pass_acq = new QHBoxLayout();
    ////layout_pass_acq->addLayout(pass_layout, 1);
    layout_pass_acq->addWidget(box_acqtime, 1);
    layout_pass_acq->addWidget(box_sn, 1);

    //-----------------------
    auto txt = QString("%1 --/--").arg(tr("Sample"));
    m_fld_samples_msg = new QLabel(txt);
    m_fld_samples_msg->setToolTip(tr("How much samples has been passed already"));
    m_fld_samples_msg->setAlignment(Qt::AlignLeft);


    QPalette pb_palette = progress_bar_gamma()->palette();
    pb_palette.setColor(QPalette::Highlight, QColor(0x7D, 0x94, 0xB0)); // Цвет #7D94B0 для заполнения
    progress_bar_gamma()->setPalette(pb_palette);

    auto *hlay_samples = new QHBoxLayout();
    hlay_samples->addWidget(m_fld_samples_msg);
    hlay_samples->addWidget(progress_bar_samples());

    //======
    QWidget *wgt = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(wgt);
    layout->addLayout(layout_pass_acq);
    layout->addLayout(hlay_samples);
    layout->addWidget(progress_bar_gamma());
    layout->addStretch();
    return wgt;
}

//---------------------------------------------------------
QWidget *API_GUI::create_box_real_data()
{
    QLabel qlabel;
    QFont LabelBoldFont = qlabel.font();
    LabelBoldFont.setBold(true);

    QLineEdit ed(mw());
    QFont boldFont = ed.font();
    boldFont.setBold(true);
    QFontMetrics fm(boldFont);
    int font_height = fm.height();
    int current_font_size = mw()->content_font_pointSize();
    int minimumFiledHeight = font_height + ((current_font_size >= 14) ? 30 : 20);

    //
    m_fld_api_mode = new QLineEdit("--");
    m_fld_api_mode->setReadOnly(true);
    m_fld_api_mode->setAlignment(Qt::AlignCenter);
    m_fld_api_mode->setFont(boldFont);
    m_fld_api_mode->setToolTip(tr("The test with background radiation source - 0 or 200 API"));

    QLabel *label = new QLabel(tr("API Mode"));
    label->setFont(LabelBoldFont);
    QGridLayout *glay = new QGridLayout();
    glay->setContentsMargins(0, 0, 0, 0);
    glay->addWidget(label, 0, 0, Qt::AlignCenter);
    glay->addWidget(m_fld_api_mode, 1, 0, Qt::AlignCenter);
    QHBoxLayout *lay_api_mode = new QHBoxLayout();
    lay_api_mode->addLayout(glay);

    //
    m_fld_api_hot_mode = new QLineEdit("--");
    m_fld_api_hot_mode->setReadOnly(true);
    m_fld_api_hot_mode->setAlignment(Qt::AlignCenter);
    m_fld_api_hot_mode->setFont(boldFont);
    m_fld_api_hot_mode->setToolTip(QStringLiteral("%1 - %2 °C")
                                    .arg(tr("The threshold for turns to hot mode is"))
                                    .arg(GAMMA::HOT_Mode));
    m_fld_api_hot_mode->setMinimumHeight(minimumFiledHeight);

    label = new QLabel(tr("Cold/Hot"));
    label->setFont(LabelBoldFont);
    glay = new QGridLayout();
    glay->addWidget(label, 0, 0, Qt::AlignCenter);
    glay->addWidget(m_fld_api_hot_mode, 1, 0, Qt::AlignCenter);
    QHBoxLayout *lay_temper_mode = new QHBoxLayout();
    lay_temper_mode->addLayout(glay);

    //
    m_fld_api_HV = new QLineEdit();
    m_fld_api_HV->setReadOnly(true);
    m_fld_api_HV->setAlignment(Qt::AlignCenter);
    m_fld_api_HV->setText(QString::number(0));
    m_fld_api_HV->setFont(boldFont);
    m_fld_api_HV->setMinimumHeight(minimumFiledHeight);

    label = new QLabel(tr("HV, V"));
    label->setFont(LabelBoldFont);
    glay = new QGridLayout();
    glay->addWidget(label, 0, 0, Qt::AlignCenter);
    glay->addWidget(m_fld_api_HV, 1, 0, Qt::AlignCenter);
    QHBoxLayout *lay_hv = new QHBoxLayout();
    lay_hv->addLayout(glay);

    //
    m_fld_api_CPS = new QLineEdit();
    m_fld_api_CPS->setReadOnly(true);
    m_fld_api_CPS->setAlignment(Qt::AlignCenter);
    m_fld_api_CPS->setText(QString::number(0, 'f', 1));
    m_fld_api_CPS->setFont(boldFont);
    m_fld_api_CPS->setMinimumHeight(minimumFiledHeight);

    label = new QLabel(tr("CPS"));
    label->setFont(LabelBoldFont);
    glay = new QGridLayout();
    glay->addWidget(label, 0, 0, Qt::AlignCenter);
    glay->addWidget(m_fld_api_CPS, 1, 0, Qt::AlignCenter);
    QHBoxLayout *lay_cps = new QHBoxLayout();
    lay_cps->addLayout(glay);

    //
    m_fld_api_Temp = new QLineEdit();
    m_fld_api_Temp->setReadOnly(true);
    m_fld_api_Temp->setAlignment(Qt::AlignCenter);
    m_fld_api_Temp->setText(QString::number(0, 'f', 1));
    m_fld_api_Temp->setFont(boldFont);
    m_fld_api_Temp->setMinimumHeight(minimumFiledHeight);

    label = new QLabel(tr("TEMP, °C"));
    label->setFont(LabelBoldFont);
    glay = new QGridLayout();
    glay->addWidget(label, 0, 0, Qt::AlignCenter);
    glay->addWidget(m_fld_api_Temp, 1, 0, Qt::AlignCenter);
    QHBoxLayout *lay_temper = new QHBoxLayout();
    lay_temper->addLayout(glay);

    //=========

    QGroupBox *box_realdata = new QGroupBox(tr("RealTime Data"));
    box_realdata->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *layout_realdata = new QHBoxLayout(box_realdata);
    layout_realdata->addStretch();
    layout_realdata->addLayout(lay_api_mode);
    layout_realdata->addStretch();
    layout_realdata->addLayout(lay_temper_mode);
    layout_realdata->addStretch();
    layout_realdata->addLayout(lay_hv);
    layout_realdata->addStretch();
    layout_realdata->addLayout(lay_cps);
    layout_realdata->addStretch();
    layout_realdata->addLayout(lay_temper);
    layout_realdata->addStretch();

    //======
    QWidget *wgt = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(wgt);
    layout->addWidget(box_realdata);
    layout->addStretch();
    return wgt;
}

//---------------------------------------------------------
QWidget *API_GUI::create_box_interpolation_table()
{
    m_fld_table_values_old = new QRadioButton(tr("[OLD] Table Values"));
    m_fld_table_values_old->setShortcut(QKeySequence("Alt+O"));
    m_fld_table_values_old->setToolTip(m_fld_table_values_old->shortcut().toString());

    m_fld_table_values_new = new QRadioButton(tr("[NEW] Table Values"));
    m_fld_table_values_new->setShortcut(QKeySequence("Alt+N"));
    m_fld_table_values_new->setToolTip(m_fld_table_values_new->shortcut().toString());
    m_fld_table_values_new->setChecked(true);

    QGroupBox *box_radio_buttons = new QGroupBox(tr("Interpolation Table of pipe wall thickness"));
    QHBoxLayout *hbox = new QHBoxLayout(box_radio_buttons);
    hbox->addWidget(m_fld_table_values_new);
    hbox->addWidget(m_fld_table_values_old);

    m_table2 = create_interpolation_table();

    //======
    QWidget *wgt = new QWidget();
    QVBoxLayout *layout_interplation_table = new QVBoxLayout(wgt);
    layout_interplation_table->addWidget(box_radio_buttons, 0);
    layout_interplation_table->addWidget(m_table2, 100);
    return wgt;
}

//---------------------------------------------------------
QWidget *API_GUI::create_box_result_table()
{
    QLineEdit ed(mw());
    QFont LineEditBoldFont = ed.font();
    LineEditBoldFont.setBold(true);
    QFontMetrics fm(LineEditBoldFont);
    // 0000.00
    int minFieldWidth = fm.horizontalAdvance("WWWWWW") + 10; // самое широкое наполнение

    for (int i = 0; i < 4; i++)
    {
        m_fld_api_cal_factor[i] = new QLineEdit();
        m_fld_api_cal_factor[i]->setAlignment(Qt::AlignRight);
        m_fld_api_cal_factor[i]->setReadOnly(true);
        m_fld_api_cal_factor[i]->setFont(LineEditBoldFont);
        m_fld_api_cal_factor[i]->setMinimumWidth(minFieldWidth);
    }
    for (int i = 0; i < 4; i++)
    {
        m_fld_api_std_deviat0[i] = new QLineEdit();
        m_fld_api_std_deviat0[i]->setAlignment(Qt::AlignRight);
        m_fld_api_std_deviat0[i]->setReadOnly(true);
        m_fld_api_std_deviat0[i]->setFont(LineEditBoldFont);
        m_fld_api_std_deviat0[i]->setMinimumWidth(minFieldWidth);
    }
    for (int i = 0; i < 4; i++)
    {
        m_fld_api_std_deviat200[i] = new QLineEdit();
        m_fld_api_std_deviat200[i]->setAlignment(Qt::AlignRight);
        m_fld_api_std_deviat200[i]->setReadOnly(true);
        m_fld_api_std_deviat200[i]->setFont(LineEditBoldFont);
        m_fld_api_std_deviat200[i]->setMinimumWidth(minFieldWidth);
    }

    auto *label = new QLabel();
    QGroupBox *box_table1 = new QGroupBox();
    QGridLayout *grid = new QGridLayout(box_table1);
    grid->addWidget(label, 0, 0);

    //-------------------------------------------
    int nrow = 0;
    m_api_cal_factor_label[0] = new QLabel(tr("OLD Method"));
    m_api_cal_factor_label[0]->setAlignment(Qt::AlignCenter);
    grid->addWidget(m_api_cal_factor_label[0], nrow, 1, Qt::AlignCenter);

    m_api_cal_factor_label[1] = new QLabel(tr("NEW Method"));
    m_api_cal_factor_label[1]->setAlignment(Qt::AlignCenter);
    grid->addWidget(m_api_cal_factor_label[1], nrow, 2, Qt::AlignCenter);

    label = new QLabel(tr("TOL Min"));
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget(label, nrow, 3, Qt::AlignCenter);

    label = new QLabel(tr("TOL Max"));
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget(label, nrow, 4, Qt::AlignCenter);

    label = new QLabel(tr("Units"));
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget(label, nrow, 5, Qt::AlignCenter);
    ++nrow;

    //-------------------------------------------
    label = new QLabel(tr("Calibration\nFactor"));
    label->setToolTip(tr("The calibration factor"));
    label->setFont(LineEditBoldFont);
    grid->addWidget(label, nrow, 0, Qt::AlignLeft);
    grid->addWidget(m_fld_api_cal_factor[0], nrow, 1, Qt::AlignCenter);
    grid->addWidget(m_fld_api_cal_factor[1], nrow, 2, Qt::AlignCenter);
    grid->addWidget(m_fld_api_cal_factor[2], nrow, 3, Qt::AlignCenter);
    grid->addWidget(m_fld_api_cal_factor[3], nrow, 4, Qt::AlignCenter);
    grid->addWidget(new QLabel(tr("API/CPS")), nrow, 5, Qt::AlignLeft);
    ++nrow;

    //-------------------------------------------
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    //line->setStyleSheet("background-color: black; height: 1px;");
    grid->addWidget(line, nrow, 0, 1, grid->columnCount());
    ++nrow;

    //-------------------------------------------
    label = new QLabel(tr("TOL Min"));
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget(label, nrow, 2, Qt::AlignCenter);

    label = new QLabel(tr("TOL Max"));
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget(label, nrow, 3, Qt::AlignCenter);
    ++nrow;

    label = new QLabel(QString("%1 API %2").arg(0, 3).arg(tr("StdDev")));
    label->setToolTip(tr("The standard deviation for blank source testing mode"));
    label->setFont(LineEditBoldFont);
    grid->addWidget(label, nrow, 0, Qt::AlignLeft);
    grid->addWidget(m_fld_api_std_deviat0[0], nrow, 1, Qt::AlignCenter);
    grid->addWidget(m_fld_api_std_deviat0[2], nrow, 2, Qt::AlignCenter);
    grid->addWidget(m_fld_api_std_deviat0[3], nrow, 3, Qt::AlignCenter);
    grid->addWidget(new QLabel(tr("CPS")), nrow, 4, Qt::AlignLeft);
    ++nrow;

    label = new QLabel(QString("%1 API %2").arg(200, 3).arg(tr("StdDev")));
    label->setToolTip(tr("The standard deviation for gamma source testing mode"));
    label->setFont(LineEditBoldFont);
    grid->addWidget(label, nrow, 0, Qt::AlignLeft);
    grid->addWidget(m_fld_api_std_deviat200[0], nrow, 1, Qt::AlignCenter);
    grid->addWidget(m_fld_api_std_deviat200[2], nrow, 2, Qt::AlignCenter);
    grid->addWidget(m_fld_api_std_deviat200[3], nrow, 3, Qt::AlignCenter);
    grid->addWidget(new QLabel(tr("CPS")), nrow, 4, Qt::AlignLeft);
    ++nrow;

    //-------------------------------------------
    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->addWidget(box_table1);
    vlayout->addStretch();

    //-------------------------------------------
    QWidget *wgt = new QWidget();
    wgt->setLayout(vlayout);
    return wgt;
}

//---------------------------------------------------------
QWidget *API_GUI::create_box_CPS_table()
{
    m_table1 = create_CPS_table();
    QGroupBox *box_table2 = new QGroupBox(tr("Gamma counts (CPS) -- 0/200 API"), new QWidget());
    QVBoxLayout *layout_table2 = new QVBoxLayout(box_table2);
    layout_table2->addWidget(m_table1);
    return m_table1; // ->parentWidget();
}

//---------------------------------------------------------
TABLE *API_GUI::create_CPS_table()
{
    auto table2 = new TABLE();
    // 1.
    QStringList labels;
    labels << "#"
           << tr("0 API (CPS)")
           << tr("Temp, °C")
           //// << tr("Status")
           << tr("200 API (CPS)")
           << tr("Temp, °C");
           //// << tr("Status");
    table2->set_labels(labels);
    table2->setRowCount(2 + GAMMA::API_Points);

    // 2.
    table2->horizontalHeader()->setSectionsClickable(false);
    table2->horizontalHeader()->setSortIndicatorShown(false);
    table2->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    table2->horizontalHeader()->setStyleSheet("QHeaderView::section { background: #d4d0c8; }");
    table2->horizontalHeader()->setStretchLastSection(false);
    // table2->resizeColumnsToContents();

    QFont header_fnt = table2->horizontalHeader()->font();
    header_fnt.setBold(true);
    table2->horizontalHeader()->setFont(header_fnt);

    auto *item = table2->horizontalHeaderItem(1);
    TABLE::set_field_font(item, header_fnt);
    item = table2->horizontalHeaderItem(4);
    TABLE::set_field_font(item,header_fnt);

    // 4.
    // table2->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    // table2->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    // table2->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);

    // 5.
    table2->set_record(0, {tr("StdDev")}, {}, tr("The standard deviation"));
    table2->set_record(1, {tr("Mean")}, {}, tr("The average of values"));
    QStringList str;
    for(int i = 0; i < table2->columnCount(); ++i) str << "";
    for(int nrow = 2; nrow < table2->rowCount(); ++nrow)
    {
        str[0] = QString::number(nrow-1);
        table2->set_record(nrow, str, {Qt::AlignCenter});
    }

    QPalette pal = table2->palette();
    pal.setColor(QPalette::Base, QColor("darkBlue"));
    pal.setColor(QPalette::Text, QColor("white"));
    for (int nrow = 0; nrow < 2; ++nrow)
    {
        for (int ncol = 0; ncol < table2->columnCount(); ++ncol)
        {
            table2->set_field_color(nrow, ncol, pal.brush(QPalette::Text), pal.brush(QPalette::Base));
            table2->set_field_alignment(nrow, ncol, Qt::AlignCenter);
        }
    }

    pal = table2->palette();
    pal.setColor(QPalette::Base, QColor(255, 255, 225)); // светло-желтый #ffffe1
    pal.setColor(QPalette::Text, Qt::black);
    for (int nrow = 2; nrow < table2->rowCount(); ++nrow)
    {
        for (int ncol = 0; ncol < table2->columnCount(); ++ncol)
        {
            table2->set_field_color(nrow, ncol, pal.brush(QPalette::Text), pal.brush(QPalette::Base));
        }
    }

    return table2;
}

//---------------------------------------------------------
TABLE *API_GUI::create_interpolation_table()
{
    auto table3 = new TABLE();
    // 1.
    QStringList labels;
    labels << tr("OD-ID")
           << tr("Factor")
           << tr("OD-ID")
           << tr("Factor")
           << tr("OD-ID")
           << tr("Factor")
           << tr("OD-ID")
           << tr("Factor");
    table3->set_labels(labels);

    // 2.
    table3->horizontalHeader()->setSectionsClickable(false);
    table3->horizontalHeader()->setSortIndicatorShown(false);
    table3->horizontalHeader()->setDefaultAlignment( Qt::AlignCenter);
    table3->horizontalHeader()->setStyleSheet("QHeaderView::section { background: #d4d0c8; }");
    table3->horizontalHeader()->setStretchLastSection(false);

    // 3.
    QFont fnt = table3->horizontalHeader()->font();
    fnt.setBold(true);
    table3->horizontalHeader()->setFont(fnt);

    for (int n = 0; n < table3->columnCount(); ++n)
    {
        auto *item = table3->horizontalHeaderItem(n);
        if (item)
        {
            item->setTextAlignment(Qt::AlignCenter);
            item->setFont(fnt);
        }
    }

    return table3;
}

//---------------------------------------------------------
void API_GUI::update_box_interpolation_table()
{
    //int num = ceil(m_api->m_settings_odid_end - m_api->m_settings_odid_beg) / m_api->m_settings_odid_stp;
    //m_table3->setRowCount(num / 4);
    double range = m_api->m_settings_odid_end - m_api->m_settings_odid_beg;
    int num = qCeil(range / m_api->m_settings_odid_stp);
    m_table2->setRowCount(qCeil(static_cast<double>(num) / 4.0));

    QPalette pal = m_gui_page->palette();
    pal.setColor(QPalette::Base, QColor(0xff, 0xff, 0xe1));
    pal.setColor(QPalette::Text, Qt::black);

    int nrow = 0;
    int ncol = 0;
    for (const auto *odid : m_api->odid_old())
    {
        if (nrow >= m_table2->rowCount())
        {
            nrow = 0;
            ncol += 2;
        }
        if (ncol + 1 >= m_table2->columnCount()) {
            break;
        }
        auto *item = m_table2->set_field_text(nrow, ncol, QString::number(odid->m_odid, 'f', 3));
        if (item)
        {
            m_table2->set_field_color(nrow, ncol, pal.brush(QPalette::Text), pal.brush(QPalette::Base));
            m_table2->set_field_alignment(nrow, ncol, Qt::AlignCenter);

            item = m_table2->set_field_text(nrow, ncol+1, {});
            if (item) {
                m_table2->set_field_color(nrow, ncol, pal.brush(QPalette::Text), pal.brush(QPalette::Base));
                m_table2->set_field_alignment(nrow, ncol, Qt::AlignCenter);
            }
        }
        ++nrow;
    }
}

//---------------------------------------------------------
TABLE *API_GUI::table1() const
{
    return m_table1;
}

//---------------------------------------------------------
TABLE *API_GUI::table2() const
{
    return m_table2;
}

//---------------------------------------------------------
bool API_GUI::is_selected_old_table() const
{
    return m_fld_table_values_old->isChecked();
}

//---------------------------------------------------------
bool API_GUI::is_selected_new_table() const
{
    return m_fld_table_values_new->isChecked();
}

//---------------------------------------------------------
QProgressBar *API_GUI::progress_bar_gamma()
{
    if (!m_gamma_progress_bar) {
        m_gamma_progress_bar = new QProgressBar();
        auto style = QStringLiteral("QProgressBar { height: 5px; } QProgressBar::chunk { background: #7D94B0; }");
        m_gamma_progress_bar->setStyleSheet(style);
        m_gamma_progress_bar->setTextVisible(false);
    }
    return m_gamma_progress_bar;
}

//---------------------------------------------------------
void API_GUI::progress_bar_gamma_close()
{
    if (m_gamma_progress_bar) {
        m_gamma_progress_bar->reset();
        delete m_gamma_progress_bar;
    }
}

//---------------------------------------------------------
QProgressBar *API_GUI::progress_bar_samples()
{
    if (!m_samples_progress_bar) {
        m_samples_progress_bar = new QProgressBar();
        m_samples_progress_bar->setStyleSheet(QStringLiteral("QProgressBar { text-align: center; }"));
    }
    return m_samples_progress_bar;
}

//---------------------------------------------------------
void API_GUI::progress_bar_samples_close()
{
    if (m_samples_progress_bar) {
        m_samples_progress_bar->reset();
        delete m_samples_progress_bar;
    }
}

//---------------------------------------------------------
void API_GUI::set_led_icon_off()
{
    m_api_led_icon->setIcon(m_ico_gray);
}

//---------------------------------------------------------
void API_GUI::set_led_icon_active()
{
    m_api_led_icon->setIcon(m_ico_green);
}

//---------------------------------------------------------
void API_GUI::set_led_icon_alarm()
{
    m_api_led_icon->setIcon(m_ico_red);
}

//---------------------------------------------------------
void API_GUI::set_led_icon_idle()
{
    m_api_led_icon->setIcon(m_ico_yellow);
}
