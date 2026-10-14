/*!
  \file   bto_settings.cpp
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Параметры БТО
*/

#include <QApplication>

#include <string>
#include <mutex>

#include "bto.h"
#include "bto_gui.h"
#include "dbg.h"
#include "main.h"
#include "mainwindow.h"

////static constexpr int ConnectionTimeout = 5;       // sec

//---------------------------------------------------------
QSettings *BTO::open_settings_file()
{
    m_settings_fname = QString("%1/%2.ini")
                       .arg(QCoreApplication::applicationDirPath(),
                            QCoreApplication::applicationName());
    m_settings_fname = QDir::toNativeSeparators(m_settings_fname);

    QSettings *st = new QSettings(m_settings_fname, QSettings::IniFormat, 0);
    if (st->status() == QSettings::NoError) return st;
    DBG(0, QString("%1 %2\n")
        .arg(QObject::tr("can't read settings file"), m_settings_fname));
    return nullptr;
}

//---------------------------------------------------------
void BTO::read_settings(QSettings *st)
{
    DBG(4);
    m_bto_rtime = st->value("BTO/RTime", Adam::NodeE4RTime).toInt();
    comm()->nodeBA()->set_rtime(m_bto_rtime);

    QString text = st->value("BTO/Type").toString();
    if (!text.startsWith("0x") || !text.startsWith("x")) {
        text.prepend("0x");
    }
    bool ok;
    NODECODE num = (NODECODE)text.toInt(&ok, 16);
    if (!ok) {
        num = NodeBA::CODEx7A;
    }
    if (num != NodeBA::CODExBA && num != NodeBA::CODEx7A) {
        num = NodeBA::CODEx7A;
    }
    set_type(num);

    if (num == NodeBA::CODExBA) {
        gui()->hide_amperage_limit_setup();
    } else if (num == NodeBA::CODEx7A) {
        gui()->show_amperage_limit_setup();
    }

    m_settings_bto_autoconnect  = st->value("BTO/AutoConnect", 0).toBool();
    m_settings_gamma_autopower  = st->value("BTO/AutoModulePower", false).toBool();
    m_settings_amperage_limit   = st->value("BTO/AmperageLimit", 2.0).toFloat();
    m_settings_voltage_limit    = st->value("BTO/Voltage", 25.0).toFloat();
    m_settings_power_output_index = 0;
    auto str = st->value("BTO/PowerOutput").toString().toLower();
    if (str == "external") m_settings_power_output_index = 1;
}

//---------------------------------------------------------
void BTO::save_settings(QSettings *st)
{
    DBG(4);
    QString text;
    text.setNum(type(), 16);
    st->setValue("BTO/Type", text.toUpper());
    st->setValue("BTO/RTime", m_bto_rtime);

    QString buf("Internal");
    if (m_settings_power_output_index == 1)
        buf = "External";

    st->setValue("BTO/Voltage",         m_settings_voltage_limit);
    st->setValue("BTO/AmperageLimit",   m_settings_amperage_limit);
    st->setValue("BTO/PowerOutput",     buf);
    st->setValue("BTO/AutoModulePower", m_settings_gamma_autopower);
    st->setValue("BTO/AutoConnect",     m_settings_bto_autoconnect);
}


//---------------------------------------------------------
void BTO_GUI::set_content_font_size(int sz)
{
    DBGF(3, "%d", sz);

    // глюк qt - после установки стиля нужно обновить установку фонта
    auto fnt = m_button_set_settings->font();
    QString style = QString("QPushButton { font: %1pt; }").arg(sz);
    m_button_set_settings->setStyleSheet(style);
    m_button_set_settings->setFont(fnt);

    fnt = m_button_bto_connect->font();
    m_button_bto_connect->setStyleSheet(style);
    m_button_bto_connect->setFont(fnt);

    fnt = m_button_gamma_poweron->font();
    if (m_button_gamma_poweron->isChecked()) {
        style = QString("QPushButton { font: %1pt bold; background: #BCCF97;}").arg(sz);
    }else {
        style = QString("QPushButton { font: %1pt bold; background: none;}").arg(sz);
    }
    m_button_gamma_poweron->setStyleSheet(style);
    m_button_gamma_poweron->setFont(fnt);
}

//---------------------------------------------------------
bool BTO::bto_autoconnect() const
{
    DBG(3, QString(m_settings_bto_autoconnect ? "yes" : "no"));
    return m_settings_bto_autoconnect;
}

//---------------------------------------------------------
void BTO::set_bto_autoconnect(bool on)
{
    m_settings_bto_autoconnect = on;
    DBG(3, QString(m_settings_bto_autoconnect ? "yes" : "no"));
}

//---------------------------------------------------------
bool BTO::gamma_autopower() const
{
    DBG(3, QString(m_settings_gamma_autopower ? "yes" : "no"));
    return m_settings_gamma_autopower;
}

//---------------------------------------------------------
void BTO::set_gamma_autopower(bool on)
{
    m_settings_gamma_autopower = on;
    DBG(3, QString(m_settings_gamma_autopower ? "yes" : "no"));
}

//---------------------------------------------------------
void MW::slot_setup_bto()
{
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle(tr("Service Unit Setup"));
    dlg->setWindowIcon(QIcon(":q_eth.png"));
    ////QFont fnt = dlg->font();
    ////fnt.setPointSize(m_content_font_size);
    ////dlg->setFont(fnt);
    auto buttons_group = MW::make_dialog_two_button(dlg, " " + tr("&Apply") + " ", " " + tr("&Cancel") + " ");

    // QRegExp rx1("[a-z][A-Z][0-9]{1-8}");
    // QRegExp rx1("[0-9]{1-8}");
    // QRegExpValidator *validator1 = new QRegExpValidator(rx1, this);
    // QRegularExpression re("\\d{0,8}");
    // QRegularExpression re1("[a-z][A-Z][а-я][А-Я][._\\s][0-9]{1-8}");
    QRegularExpression re1("^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$|^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$");
    QRegularExpressionValidator *validator1 = new QRegularExpressionValidator(re1, this);
    QLineEdit *BTO_Host = new QLineEdit();
    BTO_Host->setValidator(validator1);
    BTO_Host->setText(comm()->host());

    QRegularExpression re2("[0-9]{4-6}");
    QRegularExpressionValidator *validator2 = new QRegularExpressionValidator(re2, this);
    QLineEdit *BTO_Port = new QLineEdit();
    BTO_Port->setValidator(validator2);
    BTO_Port->setText(QString::number(comm()->port()));

    //QRegularExpression re3("[0-9]{1-2}");
    //QRegularExpressionValidator *validator3 = new QRegularExpressionValidator(re2, this);
    //QLineEdit *BTO_RTime = new QLineEdit();
    //BTO_RTime->setValidator(validator3);
    //BTO_RTime->setText(QString::number(m_app->nodeBA()->rtime()));

    QCheckBox *BTO_AutoConnect = new QCheckBox();
    BTO_AutoConnect->setContentsMargins(0, 4, 0, 0);
    auto tip = tr("Connect to Service Unit immediately");
    BTO_AutoConnect->setToolTip(tip);
    BTO_AutoConnect->setChecked(bto()->bto_autoconnect());
    DBGF(2, "bto autoconnect: %d", (int)bto()->bto_autoconnect());
    connect(BTO_AutoConnect, &QCheckBox::toggled, this, [bto = bto()](bool on) {
        bto->set_bto_autoconnect(on);
        DBGF(2, "bto autoconnect: %d", (int)bto->bto_autoconnect());
    });

    QSize sz = BTO_AutoConnect->sizeHint();
    int checkbox_size = sz.rwidth();
    if(m_content_font_size == 8)
        checkbox_size = 14;
    else if(m_content_font_size == 10)
        checkbox_size = 16;
    else if(m_content_font_size == 12)
        checkbox_size = 20;
    else if(m_content_font_size == 14)
        checkbox_size = 22;
    else if(m_content_font_size == 16)
        checkbox_size = 24;

    DBGF(3, "font size: %d, checkbox size: %d", m_content_font_size, checkbox_size);
    const auto style = QString("QCheckBox::indicator { width: %1px; height: %2px; }")
            .arg(checkbox_size).arg(checkbox_size);
    BTO_AutoConnect->setStyleSheet(style);

    //---------------------------------------------------------
    auto textBA = QString("%1").arg(NodeBA::CODExBA, 0, 16).toUpper();
    auto text7A = QString("%1").arg(NodeBA::CODEx7A, 0, 16).toUpper();

    QComboBox *BTO_Type = new QComboBox();
    BTO_Type->setFocusPolicy(Qt::NoFocus);
    BTO_Type->setMaxVisibleItems(3);
    BTO_Type->setDuplicatesEnabled(false);
    BTO_Type->setMinimumContentsLength(12);
    BTO_Type->addItem(QString("%1 (%2)").arg(tr("Type 1")).arg(textBA));
    BTO_Type->addItem(QString("%1 (%2)").arg(tr("Type 2")).arg(text7A));
    if (bto()->type() == NodeBA::CODExBA) {
        BTO_Type->setCurrentIndex(0);
    } else if (bto()->type() == NodeBA::CODEx7A) {
        BTO_Type->setCurrentIndex(1);
    } else {
        BTO_Type->setCurrentIndex(1);
    }

    //-----------------------------------------------------------
    int nrow = 0;
    QGridLayout *grid = new QGridLayout();
    grid->setMargin(10);
    grid->addWidget(new QLabel(MW::tr("Host")), nrow, 0);
    grid->addWidget(BTO_Host, nrow, 1);
    ++nrow;
    grid->addWidget(new QLabel(MW::tr("Port")), nrow, 0);
    grid->addWidget(BTO_Port, nrow, 1);
    ++nrow;
    grid->addWidget(new QLabel(MW::tr("Type")), nrow, 0);
    grid->addWidget(BTO_Type, nrow, 1);
    ++nrow;
    // grid->addWidget(new QLabel(MW::tr("Response time")), nrow, 0);
    // grid->addWidget(BTO_RTime, nrow, 1);
    // ++nrow;
    grid->addWidget(new QLabel(MW::tr("Auto connect")), nrow, 0);
    grid->addWidget(BTO_AutoConnect, nrow, 1);
    //grid->setRowMinimumHeight(nrow, 10);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    //mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addLayout(grid);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(buttons_group[0]);

    mainLayout->setAlignment(Qt::AlignHCenter);
    mainLayout->setAlignment(grid, Qt::AlignCenter);
    mainLayout->setAlignment(buttons_group[0], Qt::AlignCenter);

    //---------------------------------------------------------
    dlg->setLayout(mainLayout);
    QFont fnt = dlg->font();
    fnt.setPointSize(content_font_pointSize());
    dlg->setFont(fnt);
    dlg->adjustSize();
    dlg->setMinimumWidth(500);

    QApplication::setOverrideCursor(Qt::ArrowCursor);
    int rc = dlg->exec();
    QApplication::restoreOverrideCursor();
    if(rc == QDialog::Accepted)
    {
        DBG(4, QString("%1:%2").arg(BTO_Host->text(), BTO_Port->text()));
        comm()->set_host(BTO_Host->text());
        comm()->set_port(BTO_Port->text().toUInt());

        NODECODE num = NodeBA::CODEx7A;
        if (BTO_Type->currentIndex() == 0) {
            num = NodeBA::CODExBA;
        }
        bto()->set_type(num);

        if (num == NodeBA::CODExBA) {
            m_app->bto_gui()->hide_amperage_limit_setup();
        } else if (num == NodeBA::CODEx7A) {
            m_app->bto_gui()->show_amperage_limit_setup();
        }

        bto()->set_part_number("");
        update_statusbar_label_text();
        comm()->push_request(m_app->nodeBA()->ReadPartNumber());

        emit signal_save_settings();
    }
    delete dlg;
}

//---------------------------------------------------------
void MW::slot_setup_comport()
{
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle(tr("Serial Port Setup"));
    dlg->setWindowIcon(QIcon(":q_comport.png"));

    auto buttons_group = MW::make_dialog_two_button(dlg,  " " + tr("&Apply") + " ", " " + tr("&Cancel") + " ");

    port_t port = comm()->sp()->port_settings();
    SERIALPORT_FORM *sp_form = new SERIALPORT_FORM(dlg);
    sp_form->set_port_settings(port);

    QWidget *wgt = sp_form->create_fields_layout();

    //---------------------------------------------------------
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(wgt);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(buttons_group[0]);

    mainLayout->setAlignment(Qt::AlignHCenter);
    mainLayout->setAlignment(mainLayout, Qt::AlignCenter);
    mainLayout->setAlignment(buttons_group[0], Qt::AlignCenter);

    //---------------------------------------------------------
    dlg->setLayout(mainLayout);
    QFont fnt = dlg->font();
    fnt.setPointSize(content_font_pointSize());
    dlg->setFont(fnt);
    dlg->adjustSize();
    dlg->setMinimumWidth(500);

    QApplication::setOverrideCursor(Qt::ArrowCursor);
    int rc = dlg->exec();
    QApplication::restoreOverrideCursor();
    if(rc == QDialog::Accepted)
    {
        sp_form->flush_form_fields();
        comm()->set_port_settings(sp_form->port_settings());

        DBG(2, "emit signal_save_settings()");
        emit signal_save_settings();
    }
    delete sp_form;
    delete dlg;
}
