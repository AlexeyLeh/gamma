/*!
  \file   fw_gui_create_page_2.cpp
  \author Aleksey Lekhtin
  \brief  Формирование раскладки настроек процессора FIRMWARE
  \date   16.06.2017
*/

#include <QMessageBox>
#include <QComboBox>
#include <QMessageBox>

#include "bto.h"
#include "fw.h"
#include "fw_gui.h"
#include "gamma.h"
#include "main.h"
#include "mainwindow.h"
#include "NodeE4_ctable.h"

//---------------------------------------------------------
QWidget *FIRMWARE_GUI::create_fw_page()
{
    int current_font_size = mw()->content_font_pointSize();
    // Базовые значения для обычных мониторов
    int base_margin = 11;
    int base_spacing = 15; // отступ между колонками
    // Если шрифт увеличен (например, на 4K экранах), пропорционально увеличиваются отступы
    if (current_font_size >= 14) {
        base_margin = 22;  // Удваивание внутренних полей
        base_spacing = 30; // Делаем колонки визуально раздельными
    } else if (current_font_size >= 12) {
        base_margin = 16;
        base_spacing = 22;
    }

    // 1. Создаем каждую колонку отдельно
    QVBoxLayout *lay1 = create_survey_and_adc_column();     // Колонка 1
    QVBoxLayout *lay2 = create_ctable_column();             // Колонка 2
    QVBoxLayout *lay3 = create_ktable_column();             // Колонка 3
    QVBoxLayout *lay4 = create_settings_and_hvps_column();  // Колонка 4

    // 2. Собираем колонки в горизонтальный макет
    QHBoxLayout *fields_layout = new QHBoxLayout();
    fields_layout->setSpacing(base_spacing);
    fields_layout->setContentsMargins(base_margin, base_margin, base_margin, base_margin);

    fields_layout->addLayout(lay1, 1);
    fields_layout->addLayout(lay2, 1);
    fields_layout->addLayout(lay3, 1);
    fields_layout->addLayout(lay4, 1);
    fields_layout->addStretch();

    // 3. Создаем внутренний виджет c полями отображения
    QWidget *scrollWidget = new QWidget();
    scrollWidget->setLayout(fields_layout);

    // 4. Оборачиваем внутренний виджет в скролл-зону
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true); // Контент растягивается, если есть место
    scrollArea->setFrameShape(QFrame::NoFrame); // Убираем рамку скролла

    // НАСТРОЙКА СКРОЛЛБАРА: Показываем горизонтальную прокрутку только тогда,
    // когда окно сжали меньше минимально допустимого размера всех колонок
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // 5. Инициализируем нижние сервисные панели (Кнопки и статус)
    QWidget *box_ctrlbuttons = create_ack_status_panel();
    QHBoxLayout *button_read_layout = create_bottom_read_layout();

    clear_fw_fields();

    // 6. Главный вертикальный макет страницы
    QVBoxLayout *main_vlay = new QVBoxLayout();
    main_vlay->addWidget(scrollArea);
    main_vlay->addWidget(box_ctrlbuttons);
    main_vlay->addLayout(button_read_layout);

    QWidget *wgt = new QWidget();
    wgt->setLayout(main_vlay);
    return wgt;
}

//---------------------------------------------------------
QVBoxLayout* FIRMWARE_GUI::create_survey_and_adc_column()
{
    int current_font_size = mw()->content_font_pointSize();
    int MIN_INPUT_WIDTH = (current_font_size >= 14) ? 400 : 280;
    int MIN_INPUT_WIDTH2 = (current_font_size >= 14) ? 220 : 200;

    QLineEdit le;
    QFont fnt = le.font();
    fnt.setBold(true);
    QFontMetrics fm(fnt);
    int font_height = fm.height();
    int MIN_INPUT_HEIGHT = font_height + ((current_font_size >= 14) ? 16 : 10);

    QPalette pal_lightGray = le.palette();
    pal_lightGray.setColor(QPalette::Base, QColor::fromRgb(0xF6, 0xF6, 0xF6));

    int grid_spacing = (current_font_size >= 14) ? 14 : 7;
    int grid_margin = (current_font_size >= 14) ? 20 : 10;

    // --- Survey Data ---
    QGroupBox *box_data = new QGroupBox(tr("Survey Data"));
    QGridLayout *grid_survey = new QGridLayout(box_data);

    grid_survey->setSpacing(grid_spacing); // Расстояние между строками и колонками внутри GroupBox
    grid_survey->setContentsMargins(grid_margin, grid_margin, grid_margin, grid_margin);

    grid_survey->setColumnStretch(0, 0);
    grid_survey->setColumnStretch(1, 1);
    grid_survey->setColumnMinimumWidth(0, 100);

    auto f_setup_field = [&](QLineEdit *&field, QLabel *label, const QString &tip) {
        field = new QLineEdit();
        field->setReadOnly(true);
        field->setAlignment(Qt::AlignRight);
        field->setPalette(pal_lightGray);
        field->setFont(fnt);
        field->setToolTip(tip);
        field->setMinimumWidth(MIN_INPUT_WIDTH);
        field->setMinimumHeight(MIN_INPUT_HEIGHT);
        field->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        if (label) {
            label->setToolTip(tip);
        }
    };

    auto *lbl_toolpn = new QLabel(tr("Tool PN"));
    auto *lbl_inclin = new QLabel(tr("Inclin, deg"));
    auto *lbl_accraw = new QLabel(tr("Accel Raw, V"));
    auto *lbl_acccor = new QLabel(tr("Accel Corrected"));
    auto *lbl_gamcnt = new QLabel(tr("Gamma, counts"));
    auto *lbl_temper = new QLabel(tr("Tool Temp, °C"));

    f_setup_field(m_fld_fw_CPS, lbl_gamcnt, tr("Gamma Ray counts per second"));
    f_setup_field(m_fld_fw_Temp, lbl_temper, tr("Gamma Tool temperature"));
    f_setup_field(m_fld_fw_accel_incl, lbl_inclin, tr("Inclination"));
    f_setup_field(m_fld_fw_accel_raw, lbl_accraw, tr("Raw voltage from the 16 bit ADC that is attached to the Z axis accelerometer"));
    f_setup_field(m_fld_fw_accel_corr, lbl_acccor, tr("accelerometer Z axis data which is corrected for bias, scale factor, and temperature"));

    m_fld_fw_partnumber = new QLineEdit();
    m_fld_fw_partnumber->setAlignment(Qt::AlignCenter);
    m_fld_fw_partnumber->setReadOnly(true);
    m_fld_fw_partnumber->setPalette(pal_lightGray);
    m_fld_fw_partnumber->setFont(fnt);
    m_fld_fw_partnumber->setMinimumWidth(MIN_INPUT_WIDTH);
    m_fld_fw_partnumber->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    lbl_toolpn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    lbl_inclin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    lbl_acccor->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    lbl_accraw->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    lbl_gamcnt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    lbl_temper->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    int nrow = 0;
    grid_survey->addWidget(lbl_toolpn, nrow, 0);
    grid_survey->addWidget(m_fld_fw_partnumber, nrow, 1);
    ++nrow;
    grid_survey->addWidget(lbl_inclin, nrow, 0);
    grid_survey->addWidget(m_fld_fw_accel_incl, nrow, 1);
    ++nrow;
    grid_survey->addWidget(lbl_accraw, nrow, 0);
    grid_survey->addWidget(m_fld_fw_accel_raw, nrow, 1);
    ++nrow;
    grid_survey->addWidget(lbl_acccor, nrow, 0);
    grid_survey->addWidget(m_fld_fw_accel_corr, nrow, 1);
    ++nrow;
    grid_survey->addWidget(lbl_gamcnt, nrow, 0);
    grid_survey->addWidget(m_fld_fw_CPS, nrow, 1);
    ++nrow;
    grid_survey->addWidget(lbl_temper, nrow, 0);
    grid_survey->addWidget(m_fld_fw_Temp, nrow, 1);
    ++nrow;

    // --- A/D Raw Data ---
    QGroupBox *box_ADC = new QGroupBox(tr("A/D Raw Data"));
    QGridLayout *grid_adc = new QGridLayout(box_ADC);
    grid_adc->setColumnStretch(0, 0);
    grid_adc->setColumnStretch(1, 1);
    grid_adc->setColumnMinimumWidth(0, 100);

    for (int i = 0; i < 8; ++i) {
        m_fld_fw_ADC[i] = new QLineEdit();
        m_fld_fw_ADC[i]->setAlignment(Qt::AlignRight);
        m_fld_fw_ADC[i]->setReadOnly(true);
        m_fld_fw_ADC[i]->setPalette(pal_lightGray);
        m_fld_fw_ADC[i]->setFont(fnt);
        m_fld_fw_ADC[i]->setMinimumWidth(MIN_INPUT_WIDTH2);
        m_fld_fw_ADC[i]->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    }


    auto *lbl_adc1 = new QLabel(QString(QChar(Qt::Key_Plus)) + "15, " + tr("V"));
    auto *lbl_adc2 = new QLabel(QString(QChar(Qt::Key_Minus)) + "15, " + tr("V"));
    auto *lbl_adc3 = new QLabel("DV-485");
    auto *lbl_adc4 = new QLabel(tr("HV Input"));
    auto *lbl_adc5 = new QLabel(tr("Temp") + ", " + tr("V"));
    lbl_adc1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    lbl_adc2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    lbl_adc3->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    lbl_adc4->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    lbl_adc5->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    grid_adc->addWidget(lbl_adc1, 0, 0);
    grid_adc->addWidget(m_fld_fw_ADC[0], 0, 1);
    grid_adc->addWidget(lbl_adc2, 1, 0);
    grid_adc->addWidget(m_fld_fw_ADC[1], 1, 1);
    grid_adc->addWidget(lbl_adc3, 2, 0);
    grid_adc->addWidget(m_fld_fw_ADC[2], 2, 1);
    grid_adc->addWidget(lbl_adc4, 3, 0);
    grid_adc->addWidget(m_fld_fw_ADC[3], 3, 1);
    grid_adc->addWidget(lbl_adc5, 4, 0);
    grid_adc->addWidget(m_fld_fw_ADC[4], 4, 1);

    auto *lbl_ctemp = new QLabel(tr("Temp Calc") + "\n(0.488" + tr("V") + ")");
    lbl_ctemp->setWordWrap(false);
    lbl_ctemp->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    grid_adc->addWidget(lbl_ctemp, 5, 0);
    grid_adc->addWidget(m_fld_fw_ADC[5], 5, 1);
    grid_adc->addWidget(new QLabel("2.50" + tr("V") + " " + tr("Ref")), 6, 0);
    grid_adc->addWidget(m_fld_fw_ADC[6], 6, 1);

    auto *lbl_pwr = new QLabel(tr("Power Supply\nCurrent"));
    lbl_pwr->setWordWrap(false);
    lbl_pwr->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    grid_adc->addWidget(lbl_pwr, 7, 0);
    grid_adc->addWidget(m_fld_fw_ADC[7], 7, 1);

    QVBoxLayout *lay1 = new QVBoxLayout();
    lay1->addWidget(box_data);
    lay1->addWidget(new QLabel()); // Пустой разделитель
    lay1->addWidget(box_ADC);
    lay1->addStretch();
    return lay1;
}

//---------------------------------------------------------
QVBoxLayout* FIRMWARE_GUI::create_ctable_column()
{
    int current_font_size = mw()->content_font_pointSize();
    int MIN_INPUT_WIDTH = (current_font_size >= 14) ? 160 : 100;

    QLineEdit le(mw());
    QPalette pal_lightGray = le.palette();
    pal_lightGray.setColor(QPalette::Base, QColor::fromRgb(0xF6, 0xF6, 0xF6));
    QFont fnt = le.font();
    fnt.setBold(true);
    QFontMetrics fm(fnt);
    int MIN_INPUT_HEIGHT = fm.height() + ((current_font_size >= 14) ? 16 : 10);

    m_ctable_accel_CRC = new QLineEdit();
    m_ctable_accel_pn = new QLineEdit();
    m_ctable_accel_scale0 = new QLineEdit();
    m_ctable_accel_scale1 = new QLineEdit();
    m_ctable_accel_scale2 = new QLineEdit();
    m_ctable_accel_scale3 = new QLineEdit();
    m_ctable_accel_bias0 = new QLineEdit();
    m_ctable_accel_bias1 = new QLineEdit();
    m_ctable_accel_bias2 = new QLineEdit();
    m_ctable_accel_bias3 = new QLineEdit();
    m_ctable_accel_SN = new QLineEdit();
    m_ctable_accel_Date = new QLineEdit();
    m_ctable_accel_Time = new QLineEdit();
    m_ctable_error = new QLineEdit();

    QList<QLineEdit*> fields16 = {
        m_ctable_accel_CRC,
        m_ctable_accel_pn,
        m_ctable_accel_scale0,
        m_ctable_accel_scale1,
        m_ctable_accel_scale2,
        m_ctable_accel_scale3,
        m_ctable_accel_bias0,
        m_ctable_accel_bias1,
        m_ctable_accel_bias2,
        m_ctable_accel_bias3,
        m_ctable_accel_SN,
        m_ctable_accel_Date,
        m_ctable_accel_Time,
    };

    for(auto *f : fields16) {
        f->setFont(fnt);
        f->setAlignment(Qt::AlignRight);
        f->setMinimumWidth(MIN_INPUT_WIDTH);
        f->setMinimumHeight(MIN_INPUT_HEIGHT);
        f->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    }
    m_ctable_error->setFont(fnt);
    m_ctable_error->setMinimumWidth(MIN_INPUT_WIDTH);
    m_ctable_error->setMinimumHeight(MIN_INPUT_HEIGHT);

    m_ctable_accel_CRC->setReadOnly(true);
    m_ctable_accel_CRC->setPalette(pal_lightGray);

    connect(m_ctable_accel_pn,     SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_ctable_accel_scale0, SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_ctable_accel_scale1, SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_ctable_accel_scale2, SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_ctable_accel_scale3, SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_ctable_accel_bias0,  SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_ctable_accel_bias1,  SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_ctable_accel_bias2,  SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_ctable_accel_bias3,  SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_ctable_accel_SN,     SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_ctable_accel_Date,   SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_ctable_accel_Time,   SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));

    QGridLayout *grid = new QGridLayout();
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setColumnStretch(0, 0);
    grid->setColumnStretch(1, 1);
    grid->setColumnMinimumWidth(0, 100);

    QStringList labels = {
        tr("Part Number"),
        tr("Scale 0"),
        tr("Scale 1"),
        tr("Scale 2"),
        tr("Scale 3"),
        tr("Bias 0"),
        tr("Bias 1"),
        tr("Bias 2"),
        tr("Bias 3"),
        tr("S/N"),
        tr("Date"),
        tr("Time"),
        tr("CRC"),
    };

    for(int i = 0; i < labels.size(); ++i) {
        grid->addWidget(new QLabel(labels[i]), i, 0);
    }

    grid->addWidget(m_ctable_accel_pn,     0, 1);
    grid->addWidget(m_ctable_accel_scale0, 1, 1);
    grid->addWidget(m_ctable_accel_scale1, 2, 1);
    grid->addWidget(m_ctable_accel_scale2, 3, 1);
    grid->addWidget(m_ctable_accel_scale3, 4, 1);
    grid->addWidget(m_ctable_accel_bias0,  5, 1);
    grid->addWidget(m_ctable_accel_bias1,  6, 1);
    grid->addWidget(m_ctable_accel_bias2,  7, 1);
    grid->addWidget(m_ctable_accel_bias3,  8, 1);
    grid->addWidget(m_ctable_accel_SN,     9, 1);
    grid->addWidget(m_ctable_accel_Date,  10, 1);
    grid->addWidget(m_ctable_accel_Time,  11, 1);
    grid->addWidget(m_ctable_accel_CRC,   12, 1);

    m_fw_but_set_ctable = new QPushButton(tr("Write"));
    m_fw_but_set_ctable->setMinimumHeight(MIN_INPUT_HEIGHT);
    m_fw_but_set_ctable->setToolTip("[50] Read/Write CTABLE");

    connect(m_fw_but_set_ctable, &QPushButton::clicked, this, [this]() {
        QPushButton *btn = qobject_cast<QPushButton *>(sender());
        if (btn) showstatus(btn->text(), 5);
        if (!wish_write_data()) return;
        const auto &ctable = m_fw->ctable();
        comm()->push_requests({ NodeE4::WriteCTable(ctable) });
    });

    QGroupBox *box_ctable = new QGroupBox(tr("Accelerometer (CTable)"));
    QVBoxLayout *vlay = new QVBoxLayout(box_ctable);
    vlay->addLayout(grid);
    vlay->addWidget(m_fw_but_set_ctable);
    vlay->addWidget(m_ctable_error);
    vlay->addStretch();

    QVBoxLayout *lay2 = new QVBoxLayout();
    lay2->addWidget(box_ctable, 70);
    lay2->addWidget(new QWidget(), 30);
    lay2->addStretch();
    return lay2;
}

//---------------------------------------------------------
QVBoxLayout* FIRMWARE_GUI::create_ktable_column()
{
    int current_font_size = mw()->content_font_pointSize();
    int MIN_INPUT_WIDTH = (current_font_size >= 14) ? 180 : 100;

    QLineEdit le(mw());
    QPalette pal_lightGray = le.palette();
    pal_lightGray.setColor(QPalette::Base, QColor::fromRgb(0xF6, 0xF6, 0xF6));
    QFont fnt = le.font();
    fnt.setBold(true);
    QFontMetrics fm(fnt);
    int MIN_INPUT_HEIGHT = fm.height() + ((current_font_size >= 14) ? 16 : 10);

    m_fld_ktable_CRC = new QLineEdit();
    m_fld_ktable_PN = new QLineEdit();
    m_fld_Short_bucket_size = new QLineEdit();
    m_fld_Medium_bucket_size = new QLineEdit();
    m_fld_Long_bucket_size = new QLineEdit();
    m_fld_Calibration_time = new QLineEdit();
    m_fld_Acquire_frequency = new QLineEdit();
    m_fld_ktable_error = new QLineEdit();

    m_fld_ktable_error->setReadOnly(true);

    QList<QLineEdit*> kfields = {
        m_fld_ktable_CRC,
        m_fld_ktable_PN,
        m_fld_Short_bucket_size,
        m_fld_Medium_bucket_size,
        m_fld_Long_bucket_size,
        m_fld_Calibration_time,
        m_fld_Acquire_frequency,
    };

    for(auto* f : kfields) {
        f->setFont(fnt);
        f->setAlignment(Qt::AlignRight);
        f->setMinimumWidth(MIN_INPUT_WIDTH);
        f->setMinimumHeight(MIN_INPUT_HEIGHT);
        f->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    }
    m_fld_ktable_error->setFont(fnt);
    m_fld_ktable_error->setAlignment(Qt::AlignLeft);
    m_fld_ktable_error->setMinimumWidth(MIN_INPUT_WIDTH);
    m_fld_ktable_error->setMinimumHeight(MIN_INPUT_HEIGHT);

    m_fld_ktable_CRC->setReadOnly(true);
    m_fld_ktable_CRC->setPalette(pal_lightGray);

    connect(m_fld_ktable_PN, SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_fld_Short_bucket_size, SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_fld_Medium_bucket_size, SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_fld_Long_bucket_size, SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_fld_Calibration_time, SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_fld_Acquire_frequency, SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));
    connect(m_fld_ktable_error, SIGNAL(editingFinished()), this, SLOT(slot_cktable_edit_finished()));

    QGridLayout *grid = new QGridLayout();
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setColumnStretch(0, 0);
    grid->setColumnStretch(1, 1);
    grid->setColumnMinimumWidth(0, 100);

    grid->addWidget(new QLabel(tr("Check sum")),          0, 0);
    grid->addWidget(new QLabel(tr("Part number")),        1, 0);
    grid->addWidget(new QLabel(tr("Short bucket size")),  2, 0);
    grid->addWidget(new QLabel(tr("Medium bucket size")), 3, 0);
    grid->addWidget(new QLabel(tr("Long bucket size")),   4, 0);
    grid->addWidget(new QLabel(tr("Calibration time")),   5, 0);
    grid->addWidget(new QLabel(tr("Acquire frequency")),  6, 0);

    grid->addWidget(m_fld_ktable_CRC, 0, 1);
    grid->addWidget(m_fld_ktable_PN, 1, 1);
    grid->addWidget(m_fld_Short_bucket_size, 2, 1);
    grid->addWidget(m_fld_Medium_bucket_size, 3, 1);
    grid->addWidget(m_fld_Long_bucket_size, 4, 1);
    grid->addWidget(m_fld_Calibration_time, 5, 1);
    grid->addWidget(m_fld_Acquire_frequency, 6, 1);

    m_fw_but_set_ktable = new QPushButton(tr("Write"));
    m_fw_but_set_ktable->setMinimumHeight(MIN_INPUT_HEIGHT);
    m_fw_but_set_ktable->setToolTip("[0C] Read/Write KTABLE");

    connect(m_fw_but_set_ktable, &QPushButton::clicked, this, [this]() {
        QPushButton *btn = qobject_cast<QPushButton *>(sender());
        if (btn) showstatus(btn->text(), 5);
        if (!wish_write_data()) return;
        const auto &ktable = m_fw->ktable();
        comm()->push_requests({ NodeE4::WriteKTable(ktable) });
    });

    QGroupBox *box_ktable = new QGroupBox(tr("KTable"));
    QVBoxLayout *vlay = new QVBoxLayout(box_ktable);
    vlay->addLayout(grid);
    vlay->addWidget(m_fw_but_set_ktable);
    vlay->addWidget(m_fld_ktable_error);
    vlay->addStretch();

    QVBoxLayout *lay3 = new QVBoxLayout();
    lay3->addWidget(box_ktable, 70);
    lay3->addWidget(new QWidget(), 30);
    lay3->addStretch();
    return lay3;
}

//---------------------------------------------------------
QVBoxLayout* FIRMWARE_GUI::create_settings_and_hvps_column()
{

    QLineEdit lineedt(mw());
    QFont fnt = lineedt.font();
    fnt.setBold(true);
    QFontMetrics fm(fnt);
    int current_font_size = mw()->content_font_pointSize();
    int MIN_INPUT_HEIGHT = fm.height() + ((current_font_size >= 14) ? 16 : 10);
    auto inFont = lineedt.font();
    fm = QFontMetrics(inFont);
    int minFieldWidth6 = fm.horizontalAdvance("WWWWWW");
    int minFieldWidth8 = fm.horizontalAdvance("WWWWWWWW");

    // --- Gravity ---
    m_fld_fw_grav_field = new QDoubleSpinBox();
    m_fld_fw_grav_field->setAlignment(Qt::AlignCenter);
    m_fld_fw_grav_field->setRange(0, 2);
    m_fld_fw_grav_field->setDecimals(6);
    m_fld_fw_grav_field->setSingleStep(0.1);
    m_fld_fw_grav_field->setLocale(QLocale("C"));
    m_fld_fw_grav_field->setFont(fnt);
    m_fld_fw_grav_field->setMinimumHeight(MIN_INPUT_HEIGHT);
    m_fld_fw_grav_field->setMinimumWidth(minFieldWidth8);
    m_fld_fw_grav_field->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_fw_but_set_grav = new QPushButton(tr("Write"));
    m_fw_but_set_grav->setMinimumHeight(MIN_INPUT_HEIGHT);
    connect(m_fw_but_set_grav, SIGNAL(clicked()), this, SLOT(slot_fw_write_gravity()));

    QWidget *box_grav = new QWidget();
    QLabel *lbl_grav = new QLabel(tr("Local Gravity\nField") + "[1.00]");
    lbl_grav->setWordWrap(false);
    QHBoxLayout *hlay = new QHBoxLayout();
    hlay->addWidget(lbl_grav);
    hlay->addWidget(m_fld_fw_grav_field);
    QVBoxLayout *vlay = new QVBoxLayout(box_grav);
    vlay->addLayout(hlay);
    vlay->addWidget(m_fw_but_set_grav);

    // --- WarmUp Time ---
    m_fld_fw_warmuptime = new QSpinBox();
    m_fld_fw_warmuptime->setAlignment(Qt::AlignCenter);
    m_fld_fw_warmuptime->setRange(0, 10);
    m_fld_fw_warmuptime->setSingleStep(1);
    m_fld_fw_warmuptime->setLocale(QLocale("C"));
    m_fld_fw_warmuptime->setFont(fnt);
    m_fld_fw_warmuptime->setMinimumWidth(minFieldWidth6);
    m_fld_fw_warmuptime->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_fw_but_set_warmuptime = new QPushButton(tr("Write"));
    m_fw_but_set_warmuptime->setMinimumHeight(MIN_INPUT_HEIGHT);
    connect(m_fw_but_set_warmuptime, SIGNAL(clicked()), this, SLOT(slot_fw_write_warmup_time()));

    m_fld_fw_warmuptime->setToolTip(QString("(1s-10s)"));
    QWidget *box_warmup = new QWidget();
    QLabel *lbl_warm = new QLabel(tr("WarmUp Time") + "\n[1-10]");
    lbl_warm->setWordWrap(false);
    hlay = new QHBoxLayout();
    hlay->addWidget(lbl_warm);
    hlay->addWidget(m_fld_fw_warmuptime);
    vlay = new QVBoxLayout(box_warmup);
    vlay->addLayout(hlay);
    vlay->addWidget(m_fw_but_set_warmuptime);

    // --- Gamma Acq Time ---
    m_fld_fw_acqtime = new QSpinBox();
    m_fld_fw_acqtime->setAlignment(Qt::AlignCenter);
    m_fld_fw_acqtime->setRange(0, 600);
    m_fld_fw_acqtime->setSingleStep(1);
    m_fld_fw_acqtime->setLocale(QLocale("C"));
    m_fld_fw_acqtime->setFont(fnt);
    m_fld_fw_acqtime->setMinimumWidth(minFieldWidth6);
    m_fld_fw_acqtime->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_fw_but_set_acqtime = new QPushButton(tr("Write"));
    m_fw_but_set_acqtime->setMinimumHeight(MIN_INPUT_HEIGHT);
    connect(m_fw_but_set_acqtime, SIGNAL(clicked()), this, SLOT(slot_fw_write_acqtime()));

    m_fld_fw_acqtime->setToolTip(QString("(1s-60s) x10"));
    QWidget *box_acqtime = new QWidget();
    QLabel *lbl_acq = new QLabel(tr("Gamma Acq Time") + "\n[10-600]");
    lbl_acq->setWordWrap(false);
    hlay = new QHBoxLayout();
    hlay->addWidget(lbl_acq);
    hlay->addWidget(m_fld_fw_acqtime);
    vlay = new QVBoxLayout(box_acqtime);
    vlay->addLayout(hlay);
    vlay->addWidget(m_fw_but_set_acqtime);

    // --- HVPS ---
    m_fld_fw_hv_min = new QLineEdit();
    m_fld_fw_hv_min->setFont(fnt);
    m_fld_fw_hv_min->setAlignment(Qt::AlignCenter);
    m_fld_fw_hv_min->setMinimumWidth(minFieldWidth6);
    m_fld_fw_hv_min->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_fld_fw_hv_max = new QLineEdit();
    m_fld_fw_hv_max->setFont(fnt);
    m_fld_fw_hv_max->setAlignment(Qt::AlignCenter);
    m_fld_fw_hv_max->setMinimumWidth(minFieldWidth6);
    m_fld_fw_hv_max->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_fld_fw_hv_tst = new QLineEdit();
    m_fld_fw_hv_tst->setFont(fnt);
    m_fld_fw_hv_tst->setAlignment(Qt::AlignCenter);
    m_fld_fw_hv_tst->setMinimumWidth(minFieldWidth6);
    m_fld_fw_hv_tst->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_fw_but_set_hvps = new QPushButton(tr("Write"));
    m_fw_but_set_hvps->setMinimumHeight(MIN_INPUT_HEIGHT);
    m_fw_but_set_hvps->setToolTip("[42] Read/Write Calibrate HVPS");
    connect(m_fw_but_set_hvps, SIGNAL(clicked()), this, SLOT(slot_fw_set_hvps()));

    auto ranges = QString(" [%1-%2]").arg(GAMMA::HV_Min).arg(GAMMA::HV_Max);

    QGridLayout *grid1 = new QGridLayout();
    grid1->setContentsMargins(0, 0, 0, 0);
    grid1->setColumnStretch(0, 0);
    grid1->setColumnStretch(1, 100);
    //grid1->setColumnMinimumWidth(0, 100);

    grid1->addWidget(new QLabel(tr("Min")), 0, 0);
    grid1->addWidget(m_fld_fw_hv_min, 0, 1);
    grid1->addWidget(new QLabel(tr("Max")), 1, 0);
    grid1->addWidget(m_fld_fw_hv_max, 1, 1);
    grid1->addWidget(new QLabel(tr("Test")), 2, 0);
    grid1->addWidget(m_fld_fw_hv_tst, 2, 1);

    QGroupBox *box_hvps = new QGroupBox(tr("Calibrate HVPS") + ranges);
    vlay = new QVBoxLayout(box_hvps);
    vlay->addSpacing(10);
    vlay->addLayout(grid1);
    vlay->addWidget(m_fw_but_set_hvps);
    vlay->addSpacing(30);
    vlay->addStretch();

    // --- Operating HVPS ---
    m_fld_fw_hv_opr = new QLineEdit();
    m_fld_fw_hv_opr->setFont(fnt);
    m_fld_fw_hv_opr->setAlignment(Qt::AlignCenter);
    m_fld_fw_hv_opr->setMinimumWidth(minFieldWidth6);
    m_fld_fw_hv_opr->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_fw_but_set_hvps_oper = new QPushButton(tr("Write"));
    m_fw_but_set_hvps_oper->setMinimumHeight(MIN_INPUT_HEIGHT);
    m_fw_but_set_hvps_oper->setToolTip("[43] Read/Write Voltage");
    connect(m_fw_but_set_hvps_oper, SIGNAL(clicked()), this, SLOT(slot_fw_hv_oper()));

    QGridLayout *grid2 = new QGridLayout();
    grid2->setContentsMargins(0, 0, 0, 0);
    grid2->setColumnStretch(0, 0);
    grid2->setColumnStretch(1, 1);
    grid2->addWidget(new QLabel(tr("HVPS")), 0, 0);
    grid2->addWidget(m_fld_fw_hv_opr, 0, 1);
    grid2->setColumnStretch(0, 0);
    grid2->setColumnStretch(1, 100);
    //grid2->setColumnMinimumWidth(0, 100);

    QGroupBox *box_hvps_oper = new QGroupBox(tr("Operating HVPS") + ranges);
    vlay = new QVBoxLayout(box_hvps_oper);
    vlay->addSpacing(10);
    vlay->addLayout(grid2);
    vlay->addWidget(m_fw_but_set_hvps_oper);
    vlay->addSpacing(30);
    vlay->addStretch();

    //-----------------------
    QWidget *box3_warm = new QWidget();
    vlay = new QVBoxLayout(box3_warm);
    vlay->setContentsMargins(0, 0, 0, 0);
    vlay->addWidget(box_grav);
    vlay->addWidget(box_warmup);
    vlay->addWidget(box_acqtime);
    vlay->addStretch();

    //-----------------------
    QWidget *box3_hvps = new QWidget();
    vlay = new QVBoxLayout(box3_hvps);
    vlay->setContentsMargins(0, 0, 0, 0);
    vlay->addWidget(box_hvps);
    vlay->addWidget(box_hvps_oper);
    vlay->addStretch();

    //-----------------------
    QVBoxLayout *lay4 = new QVBoxLayout();
    lay4->addWidget(box3_warm);
    lay4->addWidget(box3_hvps);
    lay4->addStretch();
    return lay4;
}

//---------------------------------------------------------
QWidget* FIRMWARE_GUI::create_ack_status_panel()
{
    QLineEdit le;
    QFont fnt = le.font();
    fnt.setBold(true);

    m_fld_fw_ACK_cmd = new QLineEdit();
    m_fld_fw_ACK_cmd->setReadOnly(true); m_fld_fw_ACK_cmd->setFont(fnt); m_fld_fw_ACK_cmd->setAlignment(Qt::AlignCenter); m_fld_fw_ACK_cmd->setFixedWidth(70);

    m_fld_fw_ACK_status = new QLineEdit();

    m_fld_fw_ACK_status->setReadOnly(true);
    m_fld_fw_ACK_status->setFont(fnt);
    m_fld_fw_ACK_status->setAlignment(Qt::AlignCenter);
    m_fld_fw_ACK_status->setFixedWidth(70);
    m_fld_fw_ACK_code = new QLineEdit();
    m_fld_fw_ACK_code->setReadOnly(true);
    m_fld_fw_ACK_code->setFont(fnt);
    m_fld_fw_ACK_code->setAlignment(Qt::AlignCenter);
    m_fld_fw_ACK_code->setFixedWidth(70);
    m_fld_fw_ACK_text = new QLineEdit();
    m_fld_fw_ACK_text->setReadOnly(true);
    m_fld_fw_ACK_text->setFont(fnt);
    m_fld_fw_ACK_text->setAlignment(Qt::AlignLeft);
    m_fld_fw_ACK_text->setMinimumWidth(150);
    QGroupBox *box_ack_status = new QGroupBox();
    QHBoxLayout *hlay = new QHBoxLayout(box_ack_status);
    hlay->addWidget(new QLabel(tr("ACK")));
    hlay->addWidget(m_fld_fw_ACK_code);
    hlay->addWidget(new QLabel(tr("Status")));
    hlay->addWidget(m_fld_fw_ACK_status);
    hlay->addWidget(new QLabel(tr("Cmd")));
    hlay->addWidget(m_fld_fw_ACK_cmd);
    hlay->addWidget(m_fld_fw_ACK_text, 1);
    QWidget *box_ctrlbuttons = new QWidget();
    hlay = new QHBoxLayout(box_ctrlbuttons);
    hlay->addStretch();
    hlay->addWidget(box_ack_status);
    return box_ctrlbuttons;
}

//---------------------------------------------------------
QHBoxLayout* FIRMWARE_GUI::create_bottom_read_layout()
{
    m_button_fw_read = new QPushButton(QString(" %1 ").arg(FIRMWARE::tr("Read firmware")));
    m_button_fw_read->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_button_fw_read->setCheckable(true);
    m_button_fw_read->setAutoDefault(false);
    m_button_fw_read->setStyleSheet("background-color: normal;");

    m_button_fw_read->setShortcut(QKeySequence("Ctrl+R"));
    m_button_fw_read->setToolTip(m_button_fw_read->shortcut().toString());

    connect(m_button_fw_read, &QPushButton::clicked, this, [this]() {
        QPushButton *btn = qobject_cast<QPushButton*>(sender());
        if (btn) {
            showstatus(btn->text() + QStringLiteral("..."), 5);
        }
        slot_start_fw_read();
    });


    QHBoxLayout *button_read_layout = new QHBoxLayout();
    button_read_layout->addWidget(m_button_fw_read);
    button_read_layout->addStretch();
    return button_read_layout;
}

