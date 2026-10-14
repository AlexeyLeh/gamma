/*!
  \file   serial_conf.cpp
  \brief  Serial port settings form
  \author Aleksey Lekhtin
  \date   created 17.11.2014
*/

#include <QApplication>
#include <QDebug>
#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>

#include "dbg.h"
#include "serial_conf.h"

//-----------------------------------------------------------
SERIALPORT_FORM::SERIALPORT_FORM(QWidget *parent) : QWidget(parent), m_port()
{
    m_intValidator = new QIntValidator(0, 4000000, this);
}

//-----------------------------------------------------------
SERIALPORT_FORM::~SERIALPORT_FORM()
{
    DBG(4);
}

//-----------------------------------------------------------
QWidget *SERIALPORT_FORM::create_fields_layout()
{
    m_selectBox = new QGroupBox(SERIALPORT_FORM::tr("Select Serial Port"), this);
    m_serialportList = new QComboBox(m_selectBox);

    m_descriptionLabel  = new QLabel(SERIALPORT_FORM::tr("Description:"),  m_selectBox);
    m_manufacturerLabel = new QLabel(SERIALPORT_FORM::tr("Manufacturer:"), m_selectBox);
    m_serialNumberLabel = new QLabel(SERIALPORT_FORM::tr("Serial number:"),m_selectBox);
    m_locationLabel =     new QLabel(SERIALPORT_FORM::tr("Location:"),     m_selectBox);
    m_vidLabel =          new QLabel(SERIALPORT_FORM::tr("Vendor ID:"),    m_selectBox);
    m_pidLabel =          new QLabel(SERIALPORT_FORM::tr("Product ID:"),   m_selectBox);

    m_parametersBox = new QGroupBox(SERIALPORT_FORM::tr("Parameters"), this);
    m_baudRateLabel = new QLabel(SERIALPORT_FORM::tr("BaudRate:"),    m_parametersBox);
    m_dataBitsLabel = new QLabel(SERIALPORT_FORM::tr("Data bits:"),   m_parametersBox);
    m_parityLabel   = new QLabel(SERIALPORT_FORM::tr("Parity:"),      m_parametersBox);
    m_stopBitsLabel = new QLabel(SERIALPORT_FORM::tr("Stop bits:"),   m_parametersBox);
    m_flowCtrlLabel = new QLabel(SERIALPORT_FORM::tr("Flow control:"),m_parametersBox);

    m_baudRateBox = new QComboBox(m_parametersBox);
    m_dataBitsBox = new QComboBox(m_parametersBox);
    m_parityBox   = new QComboBox(m_parametersBox);
    m_stopBitsBox = new QComboBox(m_parametersBox);
    m_flowCtrlBox = new QComboBox(m_parametersBox);
    m_baudRateBox->setInsertPolicy(QComboBox::NoInsert);
    set_form_values();

    m_baudRateBox->setDisabled(true);
    m_dataBitsBox->setDisabled(true);
    m_stopBitsBox->setDisabled(true);
    m_parityBox->setDisabled(true);
    m_flowCtrlBox->setDisabled(true);

    connect(m_serialportList, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_display_port_info(int)));
    connect(m_baudRateBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_checkCustomBaudRatePolicy(int)));

    slot_get_port_list();
    update_form_fields();

    //--- раскладка ---------------------------------------------
    QGridLayout *gridLayout_1 = new QGridLayout(m_selectBox);
    gridLayout_1->addWidget(m_serialportList,   0, 0, 1, 1);
    gridLayout_1->addWidget(m_descriptionLabel, 1, 0, 1, 1);
    gridLayout_1->addWidget(m_manufacturerLabel,2, 0, 1, 1);
    gridLayout_1->addWidget(m_serialNumberLabel,3, 0, 1, 1);
    gridLayout_1->addWidget(m_locationLabel,    4, 0, 1, 1);
    gridLayout_1->addWidget(m_vidLabel,         5, 0, 1, 1);
    gridLayout_1->addWidget(m_pidLabel,         6, 0, 1, 1);

    //-----------------------------------------------------------
    QGridLayout *gridLayout_2 = new QGridLayout(m_parametersBox);
    gridLayout_2->setAlignment(Qt::AlignCenter);
    gridLayout_2->addWidget(m_baudRateLabel,   0, 0, 1, 1);
    gridLayout_2->addWidget(m_baudRateBox,     0, 1, 1, 1);
    gridLayout_2->addWidget(m_dataBitsLabel,   1, 0, 1, 1);
    gridLayout_2->addWidget(m_dataBitsBox,     1, 1, 1, 1);
    gridLayout_2->addWidget(m_stopBitsLabel,   2, 0, 1, 1);
    gridLayout_2->addWidget(m_stopBitsBox,     2, 1, 1, 1);
    gridLayout_2->addWidget(m_parityLabel,     3, 0, 1, 1);
    gridLayout_2->addWidget(m_parityBox,       3, 1, 1, 1);
    gridLayout_2->addWidget(m_flowCtrlLabel,   4, 0, 1, 1);
    gridLayout_2->addWidget(m_flowCtrlBox,     4, 1, 1, 1);

    //-----------------------------------------------------------
    m_refreshButton = new QPushButton(this);
    m_refreshButton->setIcon(QIcon(":q_refresh.png"));
    m_refreshButton->setToolTip(SERIALPORT_FORM::tr("Refresh list of available serail port"));
    connect(m_refreshButton, SIGNAL(clicked()), this, SLOT(slot_get_port_list()));

    //-----------------------------------------------------------
    QGridLayout *mainlayout = new QGridLayout(this);
    mainlayout->addWidget(m_selectBox,      0, 0, 1, 1, Qt::AlignTop);
    mainlayout->addWidget(m_parametersBox,  0, 1, 1, 1, Qt::AlignTop);
    mainlayout->addWidget(m_refreshButton,  2, 0, 1, 2, Qt::AlignRight);
    mainlayout->setRowStretch(3,100);

    QFontMetrics fm = m_selectBox->fontMetrics();
    int ww = fm.width(QString(32,'W'));
    m_selectBox->adjustSize();
    m_selectBox->setMinimumWidth(ww);

    QWidget *wgt = new QWidget();
    wgt->setLayout(mainlayout);
    return wgt;
}

//-----------------------------------------------------------
void SERIALPORT_FORM::set_form_values()
{
    m_baudRateBox->addItem(QStringLiteral("1200"),  QSerialPort::Baud1200 ); // 1200
    m_baudRateBox->addItem(QStringLiteral("2400"),  QSerialPort::Baud2400 ); // 2400
    m_baudRateBox->addItem(QStringLiteral("4800"),  QSerialPort::Baud4800 ); // 4800
    m_baudRateBox->addItem(QStringLiteral("9600"),  QSerialPort::Baud9600 ); // 9600
    m_baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200); // 19200
    m_baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400); // 38400
    m_baudRateBox->addItem(QStringLiteral("115200"),QSerialPort::Baud115200); // 115200
    m_baudRateBox->addItem(QStringLiteral("Custom"));
    m_baudRateBox->setCurrentIndex(3);

    //-----------------------------------------------------------
    m_dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5); // 5
    m_dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6); // 6
    m_dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7); // 7
    m_dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8); // 8
    m_dataBitsBox->addItem(QStringLiteral("9"), 9                 ); // 9
    m_dataBitsBox->setCurrentIndex(3);

    //-----------------------------------------------------------
    m_parityBox->addItem(QStringLiteral("None"), QSerialPort::NoParity  );      // 0
    m_parityBox->addItem(QStringLiteral("Even"), QSerialPort::EvenParity);      // 2
    m_parityBox->addItem(QStringLiteral("Odd"),  QSerialPort::OddParity );      // 3
    m_parityBox->addItem(QStringLiteral("Space"),QSerialPort::SpaceParity);     // 4
    m_parityBox->addItem(QStringLiteral("Mark"), QSerialPort::MarkParity);      // 5
    m_parityBox->addItem(QStringLiteral("Auto"), QSerialPort::UnknownParity);   // -1

    //-----------------------------------------------------------
    m_stopBitsBox->addItem(QStringLiteral("1"),  QSerialPort::OneStop);       // 1
#ifdef Q_OS_WIN
    m_stopBitsBox->addItem(QStringLiteral("1.5"),QSerialPort::OneAndHalfStop);// 2
#endif
    m_stopBitsBox->addItem(QStringLiteral("2"),  QSerialPort::TwoStop);       // 3

    //-----------------------------------------------------------
    m_flowCtrlBox->addItem(QStringLiteral("None"), QSerialPort::NoFlowControl);    // 0
    m_flowCtrlBox->addItem(QStringLiteral("RTS/CTS"), QSerialPort::HardwareControl);  // 1
    m_flowCtrlBox->addItem(QStringLiteral("XON/XOFF"), QSerialPort::SoftwareControl);  // 2
}

//---------------------------------------------------------
void SERIALPORT_FORM::update_form_fields()
{
    m_serialportList->setCurrentIndex(-1);
    int pos = m_serialportList->findText(m_port.sName);
    if(pos != -1)
    {
        m_serialportList->setCurrentIndex(pos);

        pos = m_baudRateBox->findText(m_port.sBaudRate);
        if(pos > -1) {
            m_baudRateBox->setCurrentIndex(pos);
        } else {
            pos = m_baudRateBox->count()-1;
            m_baudRateBox->setCurrentIndex(pos);
            m_baudRateBox->setItemText(pos, m_port.sBaudRate);
            m_baudRateBox->setItemData(pos, m_port.sBaudRate.toInt(), Qt::UserRole);
        }

        pos = m_dataBitsBox->findText(m_port.sDataBits);
        if(pos > -1) m_dataBitsBox->setCurrentIndex(pos);

        pos = m_stopBitsBox->findText(m_port.sStopBits);
        if(pos > -1) m_stopBitsBox->setCurrentIndex(pos);

        pos = m_parityBox->findText(m_port.sParity);
        if(pos > -1) m_parityBox->setCurrentIndex(pos);

        pos = m_flowCtrlBox->findText(m_port.sFlowCtrl);
        if(pos > -1) m_flowCtrlBox->setCurrentIndex(pos);
    }
}

//-----------------------------------------------------------
void SERIALPORT_FORM::flush_form_fields()
{
    int pos, val;
    m_port.sName.clear();
    if (m_serialportList->currentIndex() != -1) {
        m_port.sName = m_serialportList->currentText();
    }

    if(m_baudRateBox->currentIndex() == m_baudRateBox->count()-1) {
        m_port.baudRate = m_baudRateBox->currentText().toInt();
    } else {
        pos = m_baudRateBox->currentIndex();
        val = m_baudRateBox->itemData(pos, Qt::UserRole).toInt();
        m_port.baudRate = static_cast<QSerialPort::BaudRate>(val);
    }

    m_port.sBaudRate = QString::number(m_port.baudRate);

    pos = m_dataBitsBox->currentIndex();
    val = m_dataBitsBox->itemData(pos, Qt::UserRole).toInt();
    m_port.dataBits = static_cast<QSerialPort::DataBits>(val);
    m_port.sDataBits = m_dataBitsBox->currentText();

    pos = m_stopBitsBox->currentIndex();
    val = m_stopBitsBox->itemData(pos, Qt::UserRole).toInt();
    m_port.stopBits = static_cast<QSerialPort::StopBits>(val);
    m_port.sStopBits = m_stopBitsBox->currentText();

    pos = m_parityBox->currentIndex();
    val = m_parityBox->itemData(pos, Qt::UserRole).toInt();
    m_port.parity = static_cast<QSerialPort::Parity>(val);
    m_port.sParity = m_parityBox->currentText();

    pos = m_flowCtrlBox->currentIndex();
    val = m_flowCtrlBox->itemData(pos, Qt::UserRole).toInt();
    m_port.flowCtrl = static_cast<QSerialPort::FlowControl>(val);
    m_port.sFlowCtrl = m_flowCtrlBox->currentText();

    //m_port_settings.localEcho = m_localEchoCheckBox->isChecked();
}

//-----------------------------------------------------------
void SERIALPORT_FORM::slot_get_port_list()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    static const QString blankString = SERIALPORT_FORM::tr("N/A");
    m_serialportList->clear();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        QString description  = info.description();
        QString manufacturer = info.manufacturer();
        QString serialNumber = info.serialNumber();
        QString loc = info.systemLocation();
        loc.replace("\\\\", "");
        list << info.portName()
             << (!description.isEmpty()  ? description  : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << loc
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(),16):blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(),16):blankString);

        m_serialportList->addItem(list.first(), list);
    }
    m_serialportList->setCurrentIndex(0);
    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------
port_t SERIALPORT_FORM::port_settings()
{
    return m_port;
}

//-----------------------------------------------------------
void SERIALPORT_FORM::set_port_settings(const port_t & port)
{
    m_port = port;
}

//-----------------------------------------------------------
void SERIALPORT_FORM::slot_display_port_info(int index)
{
    if(index < 0) return;
    QStringList list = m_serialportList->itemData(index).toStringList();
    m_descriptionLabel->setText( QString("%1 %2").arg(tr("Description:")).arg(list.at(1)));
    m_manufacturerLabel->setText(QString("%1 %2").arg(tr("Manufacturer:")).arg(list.at(2)));
    m_serialNumberLabel->setText(QString("%1 %2").arg(tr("Serial number:")).arg(list.at(3)));
    m_locationLabel->setText(QString("%1 %2").arg(tr("Location:")).arg(list.at(4)));
    m_vidLabel->setText(QString("%1 %2").arg(tr("Vendor Identifier :")).arg(list.at(5)));
    m_pidLabel->setText(QString("%1 %2").arg(tr("Product Identifier:")).arg(list.at(6)));
}

//-----------------------------------------------------------
// Переключение на ручной ввод скорости порта
// \ param[in] index - номер выбраного параметра
//
void SERIALPORT_FORM::slot_checkCustomBaudRatePolicy(int index)
{
    bool isCustomBaudRate = !m_baudRateBox->itemData(index).isValid();
    m_baudRateBox->setEditable(isCustomBaudRate);
    if(isCustomBaudRate)
    {
        m_baudRateBox->clearEditText();
        QLineEdit *edit = m_baudRateBox->lineEdit();
        edit->setValidator(m_intValidator);
    }
}


