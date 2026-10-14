/*!
  \file   bto_gui_create_page.cpp
  \author Aleksey Lekhtin
  \brief  Формирование окна с информацией по БТО
  \date   10.05.2023
*/

#include <QMessageBox>
#include <QHBoxLayout>

#include "bto.h"
#include "dbg.h"
#include "mainwindow.h"
#include "bto_gui.h"

constexpr int ControlButtonsWidth = 200;
constexpr int ControlButtonsHeight = 48;

typedef struct
{
    QLabel     *label;
    QLineEdit  *field;
} srig_field_t;

//---------------------------------------------------------
QWidget *BTO_GUI::create_bto_page()
{
    // 1.
    auto *box_bto_control_buttons = create_poweron_buttons_box();
    QVBoxLayout *control_button_layout = new QVBoxLayout();
    control_button_layout->addStretch();
    control_button_layout->addWidget(box_bto_control_buttons);
    control_button_layout->addStretch();

    // 2.
    auto box_gamma_page = create_gamma_page();

    QHBoxLayout *layout_at_top = new QHBoxLayout();
    layout_at_top->addStretch();
    layout_at_top->addWidget(box_gamma_page);
    layout_at_top->addStretch();

    // 3.
    QHBoxLayout *layout_at_bottom = new QHBoxLayout();
    layout_at_bottom->addStretch();
    layout_at_bottom->addLayout(control_button_layout);

    auto *box_bto_current_values  = create_current_values_box();
    layout_at_bottom->addWidget(box_bto_current_values, 30);

    auto *box_bto_settings = create_settings_box();
    layout_at_bottom->addWidget(box_bto_settings, 30);
    layout_at_bottom->addStretch();

    auto fnt = box_bto_current_values->font();
    fnt.setBold(true);
    box_gamma_page->setFont(fnt);
    box_bto_current_values->setFont(fnt);
    box_bto_settings->setFont(fnt);

    // 100500. сложить все вместе
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(layout_at_top, 99);
    layout->addLayout(layout_at_bottom, 1);

    QWidget *wgt = new QWidget();
    wgt->setLayout(layout);
    return wgt;
}

//---------------------------------------------------------
QWidget *BTO_GUI::create_led_icon_box()
{
    m_bto_pixmap_red = QPixmap(":q_bto_red.png").scaledToWidth(65);
    m_bto_pixmap_green = QPixmap(":q_bto_green.png").scaledToWidth(65);
    m_bto_pixmap_light_green = QPixmap(":q_bto_light_green.png").scaledToWidth(65);
    m_bto_pixmap_yellow = QPixmap(":q_bto_yellow.png").scaledToWidth(65);

    m_bto_led = new QLabel();
    m_bto_led->setStyleSheet("border: none; color: palette(window-text);");
    m_bto_led->setPixmap(QPixmap(":q_bto_red.png").scaledToWidth(65));
    m_bto_led->setToolTip(tr("Service Unit\nUnconnected"));
    // m_bto_led->setStyleSheet("border: none; color: palette(window-text); QToolTip {color: black; background: white;}");

    auto *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addStretch();
    layout->addWidget(m_bto_led);
    layout->addStretch();
    auto *wgt = new QWidget();
    wgt->setLayout(layout);
    return wgt;
}

//// //---------------------------------------------------------
//// static QString max_length_string_chunk(const QString &str)
//// {
////     int ww = 0;
////     QString maxstr;
////     auto list = str.split("\n");
////     for (auto &s : list)
////     {
////         if (s.length() > ww)
////         {
////             ww = s.length();
////             maxstr = s;
////         }
////     }
////     return maxstr;
//// }

//---------------------------------------------------------
QWidget *BTO_GUI::create_gamma_page()
{
    // 1.
    QGroupBox *main_box = new QGroupBox(tr("Gamma Sonde"));
    main_box->setAlignment(Qt::AlignCenter);

    // 2.
    m_gamma_icon = QIcon(":q_gamma_512.png");
    m_gamma_found_icon = new QLabel();
    m_gamma_found_icon->setPixmap(m_gamma_icon.pixmap(50));
    m_gamma_found_icon->setAlignment(Qt::AlignCenter);
    m_gamma_found_icon->setMinimumSize(52, 52);

    m_gamma_status = new QLabel("---");
    m_gamma_status->setAlignment(Qt::AlignCenter);

    QVBoxLayout *layout1_gamma_found = new QVBoxLayout();
    layout1_gamma_found->addStretch();
    layout1_gamma_found->addWidget(m_gamma_found_icon, 0, Qt::AlignCenter);
    layout1_gamma_found->addWidget(m_gamma_status, 0, Qt::AlignCenter);
    layout1_gamma_found->addWidget(m_button_gamma_poweron, 0, Qt::AlignCenter);
    layout1_gamma_found->addStretch();

    auto *wgt_gamma_found = new QWidget();
    ////wgt_gamma_found->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QHBoxLayout *layout2_gamma_found = new QHBoxLayout(wgt_gamma_found);
    layout2_gamma_found->addStretch();
    layout2_gamma_found->addLayout(layout1_gamma_found);
    layout2_gamma_found->addStretch();

    // 3.
    QLabel label(tr("Analog Signals"), mw());
    QFontMetrics fm = label.fontMetrics();

    // 4.
    QGridLayout *grid2 = new QGridLayout();
    grid2->setContentsMargins(5, 0, 5, 0);
    grid2->setSizeConstraint(QLayout::SetFixedSize);

    m_field_part_number   = new QLineEdit();
    m_field_gamma_voltage = new QLineEdit();
    m_field_gamma_temp    = new QLineEdit();
    m_field_gamma_gamma   = new QLineEdit();

    QVector<srig_field_t> list1 = {
        {   new QLabel(tr("Part Number")),
            m_field_part_number,
        },
        {   new QLabel(tr("High Voltage, V")),
            m_field_gamma_voltage,
        },
        {   new QLabel(tr("Temperature, °C")),
            m_field_gamma_temp,
        },
        {   new QLabel(tr("Gamma Counts, CPS")),
            m_field_gamma_gamma,
        },
    };

    int nrow = 0;
    for (auto el : list1)
    {
        el.field->setReadOnly(true);
        el.field->setAlignment(Qt::AlignRight);
        el.field->setFocusPolicy(Qt::NoFocus);
        el.label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        grid2->addWidget(el.label, nrow, 0, Qt::AlignLeft);
        grid2->addWidget(el.field, nrow, 1, Qt::AlignLeft);
        ++nrow;
    }

    /// grid2->addWidget(new QLabel(" "), nrow, 0, 1, 2, Qt::AlignLeft);
    /// ++nrow;

    auto *wgt_grid2 = new QWidget();
    auto grid2_hlayout = new QVBoxLayout(wgt_grid2);
    grid2_hlayout->addLayout(grid2);
    grid2_hlayout->addStretch();

    // 5.
    QGridLayout *grid3 = new QGridLayout();
    grid3->setContentsMargins(5, 0, 5, 0);
    grid3->setSizeConstraint(QLayout::SetFixedSize);

    m_field_gamma_15VDC   = new QLineEdit();
    m_field_gamma_5VDC    = new QLineEdit();
    m_field_gamma_DV485   = new QLineEdit();
    m_field_gamma_25Ref   = new QLineEdit();
    m_field_gamma_HV_raw  = new QLineEdit();
    m_field_gamma_TempV   = new QLineEdit();
    m_field_gamma_TempVCal= new QLineEdit();

    QVector<srig_field_t> list2 = {
        {   new QLabel(tr("+15 VDC") + "," + tr("V")),
            m_field_gamma_15VDC
        },
        {   new QLabel(tr("+5 VDC") + "," + tr("V")),
            m_field_gamma_5VDC
        },
        {   new QLabel(tr("DV485") + "," + tr("V")),
            m_field_gamma_DV485
        },
        {   new QLabel(tr("2.5V reference") + "," + tr("V")),
            m_field_gamma_25Ref
        },
        {   new QLabel(tr("HV Control") + "," + tr("V")),
            m_field_gamma_HV_raw
        },
        {   new QLabel(tr("Temp. sensor") + "," + tr("V")),
            m_field_gamma_TempV
        },
        {   new QLabel(tr("Temp. calibration") + "," + tr("V")),
            m_field_gamma_TempVCal
        }
    };

    // 6.
    nrow = 0;
    for (auto el : list2)
    {
        el.field->setReadOnly(true);
        el.field->setAlignment(Qt::AlignRight);
        el.field->setFocusPolicy(Qt::NoFocus);
        el.label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        grid3->addWidget(el.label, nrow, 0, Qt::AlignLeft);
        grid3->addWidget(el.field, nrow, 1, Qt::AlignLeft);
        ++nrow;
    }

    auto *wgt_grid3 = new QWidget();
    auto grid3_hlayout = new QVBoxLayout(wgt_grid3);
    grid3_hlayout->addLayout(grid3);
    grid3_hlayout->addStretch();

    // 7.
    QHBoxLayout *voltages_layout = new QHBoxLayout();
    voltages_layout->addWidget(wgt_grid2);
    voltages_layout->addWidget(wgt_grid3);

    // 8.
    QGridLayout *grid = new QGridLayout(main_box);
    grid->setContentsMargins(5, 0, 5, 0);
    grid->addWidget(wgt_gamma_found, 0, 0);
    grid->addLayout(voltages_layout, 0, 1);
    grid->setColumnStretch(0, 10);
    grid->setColumnStretch(1, 90);

    // 100500. сложить все вместе
    QWidget *wgt = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(wgt);
    layout->addStretch();
    layout->addWidget(main_box);
    layout->addStretch();
    return wgt;
}

//---------------------------------------------------------
QWidget *BTO_GUI::create_poweron_buttons_box()
{
    // 1.
    m_button_bto_connect = new QPushButton(tr("Connect to\nService Unit"));
    m_button_bto_connect->setEnabled(true);
    m_button_bto_connect->setCheckable(true);
    m_button_bto_connect->setAutoDefault(false);
    m_button_bto_connect->setStyleSheet("background-color: normal;");
    m_button_bto_connect->setShortcut(QKeySequence("Ctrl+R"));
    m_button_bto_connect->setToolTip(m_button_bto_connect->text() + "\n" +
                                 m_button_bto_connect->shortcut().toString());
    //m_button_bto_connect->setMinimumSize(ControlButtonsWidth, ControlButtonsHeight);
    auto font = m_button_bto_connect->font();
    QFontMetrics fm(font);
    int min_width = fm.horizontalAdvance("WWWWWWWWWWWWWWW") + 20;
    m_button_bto_connect->setFixedWidth(min_width);

    connect(m_button_bto_connect, SIGNAL(clicked()), this, SLOT(slot_connect_button()));

    auto *box_bto_led_icon = create_led_icon_box();

    // 2.
    m_button_gamma_poweron = new QPushButton(tr(gamma_power_button_text));
    m_button_gamma_poweron->setEnabled(false);
    m_button_gamma_poweron->setCheckable(true);
    m_button_gamma_poweron->setAutoDefault(false);
    m_button_gamma_poweron->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_button_gamma_poweron->setMinimumWidth(m_button_gamma_poweron->sizeHint().width());
    m_button_gamma_poweron->setStyleSheet("background-color: normal;");
    m_button_gamma_poweron->setShortcut(QKeySequence("Ctrl+W"));
    m_button_gamma_poweron->setToolTip(m_button_gamma_poweron->shortcut().toString());
    m_button_gamma_poweron->setMinimumSize(ControlButtonsWidth, ControlButtonsHeight);
    auto tip = QString("%1\n%2").arg("Turn power On", m_button_gamma_poweron->shortcut().toString());
    m_button_gamma_poweron->setToolTip(tip);
    connect(m_button_gamma_poweron, SIGNAL(clicked()), this, SLOT(slot_gamma_poweron_button()));

    // 3.
    auto *layout = new QVBoxLayout();
    layout->setContentsMargins(10, 10, 10, 10);
    layout->addWidget(box_bto_led_icon);
    layout->addWidget(m_button_bto_connect);
    auto *hlay = new QHBoxLayout();
    hlay->addLayout(layout);
    hlay->addStretch();
    auto *wgt = new QWidget();
    wgt->setLayout(hlay);
    return wgt;
}

//---------------------------------------------------------
QGroupBox *BTO_GUI::create_current_values_box()
{
    // 0.
    QGroupBox *box = new QGroupBox();
    box->setContentsMargins(5, 0, 5, 0);
    QFontMetrics fm = box->fontMetrics();
    auto str = QLabel(tr(amperage_text_consumption)).text();
    //int ww = fm.horizontalAdvance(str) + 20;
    ////  int ww = fm.width(str) + 20;

    // 1.
    m_field_required_voltage = new QLineEdit();
    m_field_required_voltage->setReadOnly(true);
    m_field_required_voltage->setAlignment(Qt::AlignRight);
    m_field_required_voltage->setFocusPolicy(Qt::NoFocus);
    auto *field_required_voltage_label = new QLabel(tr("Required Voltage"));
    field_required_voltage_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    // 2.
    m_field_output_voltage = new QLineEdit();
    m_field_output_voltage->setReadOnly(true);
    m_field_output_voltage->setAlignment(Qt::AlignRight);
    m_field_output_voltage->setFocusPolicy(Qt::NoFocus);
    auto *field_output_voltage_label = new QLabel(tr("Output voltage"));
    field_output_voltage_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    // 3.
    m_field_amperage_limit = new QLineEdit();
    m_field_amperage_limit->setReadOnly(true);
    m_field_amperage_limit->setAlignment(Qt::AlignRight);
    m_field_amperage_limit->setFocusPolicy(Qt::NoFocus);
    m_field_amperage_limit_label = new QLabel(BTO_GUI::tr(amperage_text_limit));
    m_field_amperage_limit_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    // 4.
    m_field_amperage_consumption = new QLineEdit();
    m_field_amperage_consumption->setReadOnly(true);
    m_field_amperage_consumption->setAlignment(Qt::AlignRight);
    m_field_amperage_consumption->setFocusPolicy(Qt::NoFocus);
    m_field_amperage_consumption_label = new QLabel(BTO_GUI::tr(amperage_text_consumption));
    m_field_amperage_consumption_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    // 5.
    m_field_power_output_sw = new QLineEdit();
    m_field_power_output_sw->setReadOnly(true);
    m_field_power_output_sw->setAlignment(Qt::AlignCenter);
    m_field_power_output_sw->setFocusPolicy(Qt::NoFocus);
    auto *field_power_output_sw_label = new QLabel(tr("Power supply"));
    field_power_output_sw_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    //m_field_power_output_sw->setDisabled(true);

    // 100500.
    int nrow = 0;
    QGridLayout *grid = new QGridLayout();
    grid->setContentsMargins(5, 0, 5, 0);
    //// grid->setColumnMinimumWidth(0, ww);
    grid->setSizeConstraint(QLayout::SetMinimumSize);


    grid->addWidget(field_required_voltage_label, nrow, 0);
    grid->addWidget(m_field_required_voltage, nrow, 1);
    ++nrow;
    grid->addWidget(field_output_voltage_label, nrow, 0);
    grid->addWidget(m_field_output_voltage, nrow, 1);
    ++nrow;
    grid->addWidget(m_field_amperage_consumption_label, nrow, 0);
    grid->addWidget(m_field_amperage_consumption, nrow, 1);
    ++nrow;
    grid->addWidget(m_field_amperage_limit_label, nrow, 0);
    grid->addWidget(m_field_amperage_limit, nrow, 1);
    ++nrow;
    grid->addWidget(field_power_output_sw_label, nrow, 0);
    grid->addWidget(m_field_power_output_sw, nrow, 1);

    //=========
    QHBoxLayout *layout = new QHBoxLayout(box);
    layout->addLayout(grid);
    layout->addStretch();

    return box;
}

//---------------------------------------------------------
QGroupBox *BTO_GUI::create_settings_box()
{
    // 1.
    m_field_set_power_output_sw = new QComboBox();
    m_field_set_power_output_sw->setFocusPolicy(Qt::NoFocus);
    m_field_set_power_output_sw->setMaxVisibleItems(3);
    m_field_set_power_output_sw->setDuplicatesEnabled(false);
    m_field_set_power_output_sw->setMinimumContentsLength(12);
    m_field_set_power_output_sw->addItem(tr("Internal"));       // "Внтуренний источник питания"
    m_field_set_power_output_sw->addItem(tr("External"));       // "Батарея"
    connect(m_field_set_power_output_sw, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slot_bto_set_power_output_sw(int)));

    // 2.
    m_field_requested_voltage = new QDoubleSpinBox();
    //m_field_requested_voltage->setFocusPolicy(Qt::NoFocus);
    m_field_requested_voltage->setAlignment(Qt::AlignRight);
    m_field_requested_voltage->setLocale(QLocale("C"));
    m_field_requested_voltage->setDecimals(1);
    m_field_requested_voltage->setSingleStep(1.0);
    m_field_requested_voltage->setValue(0.0);
    m_field_requested_voltage->setRange(0.0, 30.0);
    m_field_requested_voltage->setSuffix(tr(" V"));
    connect(m_field_requested_voltage, SIGNAL(valueChanged(double)),
            this, SLOT(slot_bto_set_voltage(double)));

    // 3.

    m_field_requested_amperage_limit_label = new QLabel(BTO_GUI::tr(amperage_text_limit));
    m_field_requested_amperage_limit = new QDoubleSpinBox();
    //m_field_requested_amperage_limit->setFocusPolicy(Qt::NoFocus);
    m_field_requested_amperage_limit->setAlignment(Qt::AlignRight);
    m_field_requested_amperage_limit->setLocale(QLocale("C"));
    m_field_requested_amperage_limit->setDecimals(1);
    m_field_requested_amperage_limit->setSingleStep(0.5);
    m_field_requested_amperage_limit->setValue(0.5);
    m_field_requested_amperage_limit->setRange(0.0, 10.0);
    m_field_requested_amperage_limit->setSuffix(" A");
    connect(m_field_requested_amperage_limit, SIGNAL(valueChanged(double)),
            this, SLOT(slot_bto_set_amperage_limit(double)));

    // 4.
    m_button_set_settings = new QPushButton(tr("Set Settings"));
    ////auto fnt = box->font();
    ////int font_size = fnt.pointSize();

    m_button_set_settings->setFocusPolicy(Qt::NoFocus);
    m_button_set_settings->setCheckable(false);
    m_button_set_settings->setAutoDefault(false);
    m_button_set_settings->setStyleSheet("background-color: normal;");
    auto tip = m_button_set_settings->shortcut().toString();
    m_button_set_settings->setToolTip(tip);
    connect(m_button_set_settings, SIGNAL(clicked()),
            this, SLOT(slot_bto_set_settings()));

    // 100500.
    int nrow = 0;
    QGridLayout *grid = new QGridLayout();
    grid->setContentsMargins(5, 0, 5, 0);

    grid->addWidget(new QLabel(tr("Power Supply")), nrow, 0);
    grid->addWidget(m_field_set_power_output_sw, nrow, 1);
    ++nrow;
    grid->addWidget(new QLabel(tr("Voltage")), nrow, 0);
    grid->addWidget(m_field_requested_voltage, nrow, 1);
    ++nrow;
    grid->addWidget(m_field_requested_amperage_limit_label, nrow, 0);
    grid->addWidget(m_field_requested_amperage_limit, nrow, 1);
    ++nrow;
    grid->addWidget(m_button_set_settings, nrow, 0, 1, 2);

    //=========
    QGroupBox *box = new QGroupBox(tr("Setup"));
    box->setContentsMargins(5, 0, 5, 0);

    QHBoxLayout *layout = new QHBoxLayout(box);
    layout->addLayout(grid);
    layout->addStretch();

    return box;
}

//---------------------------------------------------------
void BTO_GUI::show_amperage_limit_setup()
{
    m_field_amperage_limit_label->show();
    m_field_amperage_limit->show();
    m_field_requested_amperage_limit->show();
    m_field_requested_amperage_limit_label->show();
}

//---------------------------------------------------------
void BTO_GUI::hide_amperage_limit_setup()
{
    m_field_amperage_limit_label->hide();
    m_field_amperage_limit->hide();
    m_field_requested_amperage_limit->hide();
    m_field_requested_amperage_limit_label->hide();
}


//---------------------------------------------------------
void BTO_GUI::disable_all_controls()
{
    m_bto_led->setDisabled(true);
    m_button_bto_connect->setDisabled(true);
    if (connection_type() == COMM::CONNECT_TYPE::ETHERNET)
    {
        m_button_gamma_poweron->setDisabled(true);
        m_field_set_power_output_sw->setDisabled(true);
        m_field_requested_amperage_limit->setDisabled(true);
        m_field_requested_voltage->setDisabled(true);
        m_button_set_settings->setDisabled(true);

        // current values
        m_field_required_voltage->clear();
        m_field_output_voltage->clear();
        m_field_amperage_limit->clear();
        m_field_amperage_consumption->clear();
        m_field_power_output_sw->clear();

        //m_field_required_voltage->setDisabled(true);
        //m_field_output_voltage->setDisabled(true);
        //m_field_amperage_limit->setDisabled(true);
        //m_field_amperage_consumption->setDisabled(true);
        //m_field_power_output_sw->setDisabled(true);
    }
}

//---------------------------------------------------------
void BTO_GUI::enable_all_controls()
{
    m_bto_led->setEnabled(true);
    m_button_bto_connect->setEnabled(true);
    if (connection_type() == COMM::CONNECT_TYPE::ETHERNET)
    {
        m_button_gamma_poweron->setEnabled(true);
        m_field_set_power_output_sw->setEnabled(true);
        m_field_requested_amperage_limit->setEnabled(true);
        m_field_requested_voltage->setEnabled(true);

        // current values
        m_field_required_voltage->setEnabled(true);
        m_field_output_voltage->setEnabled(true);
        m_field_amperage_limit->setEnabled(true);
        m_field_amperage_consumption->setEnabled(true);
        //m_field_power_output_sw->setDisabled(true);
    }
}

//---------------------------------------------------------
void BTO_GUI::show_bto_settings()
{
    auto status = m_field_requested_voltage->blockSignals(true);
    m_field_requested_voltage->setValue(m_bto->m_settings_voltage_limit);
    m_field_requested_voltage->blockSignals(status);

    status = m_field_requested_amperage_limit->blockSignals(true);
    m_field_requested_amperage_limit->setValue(m_bto->m_settings_amperage_limit);
    m_field_requested_amperage_limit->blockSignals(status);

    m_field_set_power_output_sw->setCurrentIndex(m_bto->m_settings_power_output_index);
}

//---------------------------------------------------------
void BTO_GUI::showstatus(const QString &msg, int sec) const
{
    if (msg.isEmpty())
        mw()->clearstatus();
    else
        mw()->showstatus(msg, sec);
}

//---------------------------------------------------------
NodeBA::PowerOutputType BTO_GUI::get_power_output_switch_from_settings() const
{
    auto index = m_field_set_power_output_sw->currentIndex();
    if (index == 0)
        return NodeBA::PowerOutputType::INT;
    if (index == 1)
        return NodeBA::PowerOutputType::EXT;
    return NodeBA::PowerOutputType::OFF;
}

//---------------------------------------------------------
int BTO_GUI::power_output_switch_to_index(NodeBA::PowerOutputType sw)
{
    if (sw == NodeBA::PowerOutputType::INT)
        return 0;
    if (sw == NodeBA::PowerOutputType::EXT)
        return 1;
    return -1; // NodeBA::PowerOutput::OFF
}

//---------------------------------------------------------
void BTO_GUI::set_led_icon_inactive()
{
    DBG(4);
    m_bto_led->setPixmap(m_bto_pixmap_red);
}

//---------------------------------------------------------
void BTO_GUI::set_led_icon_active()
{
    DBG(4);
    m_bto_led->setPixmap(m_bto_pixmap_light_green);
}

//---------------------------------------------------------
void BTO_GUI::set_led_icon_light_active()
{
    DBG(4);
    m_bto_led->setPixmap(m_bto_pixmap_green);
}
