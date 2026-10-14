/*!
  \file   slot_fonts.cpp
  \brief  Слоты приложения
  \author Aleksey Lekhtin
  \date   30.03.2010 \n
*/

#include <QtCharts/QChartView>

#include "api_gui.h"
#include "bto_gui.h"
#include "diagn_gui.h"
#include "fw_gui.h"
#include "main.h"
#include "mainwindow.h"
#include "plato_gui.h"

//-----------------------------------------------------------
int MW::content_font_pointSize() const
{
    return m_content_font_size;
}

//-----------------------------------------------------------
void MW::get_fonts_size()
{
    // получение текущих "системных" размеров иконок и фонтов
    QIcon ic(":q_exit.png");
    QSize icon_sz = ic.actualSize(QSize(24,24), QIcon::Selected, QIcon::On);
    m_sys_icon_size = icon_sz.width();

    QFont fnt = font();
    m_sys_content_font = fnt;
    m_sys_content_font_size = fnt.pointSize();
    if (m_content_font_size == 0) {
        m_content_font_size = m_sys_content_font_size;
    }

    QMenu mn;
    fnt = mn.font();
    m_sys_menu_font_size = fnt.pointSize();

    QChartView gr;
    fnt = gr.font();
    m_sys_graph_font_size = fnt.pointSize();

    QMessageBox mbox;
    fnt = mbox.font();
    m_sys_msg_font_size = fnt.pointSize();
}

//-----------------------------------------------------------
void MW::slot_font_settings()
{
    QFont fnt;
    auto fontdlg = new QDialog(this);
    fontdlg->setWindowTitle(tr("Setting the font size"));

    //-----------------------------------------------------------
    QLabel *font_icons_label = new QLabel(tr("Toolbar Icons"));
    QComboBox *font_icons_sizes = new QComboBox();
    font_icons_sizes->addItem(tr("System"));
    font_icons_sizes->addItem("24x24");
    font_icons_sizes->addItem("32x32");
    font_icons_sizes->addItem("48x48");

    if(m_icon_size == 24)
        font_icons_sizes->setCurrentIndex(1);
    else if(m_icon_size == 32)
        font_icons_sizes->setCurrentIndex(2);
    else if(m_icon_size == 48)
        font_icons_sizes->setCurrentIndex(3);

    int sz = m_menu_font_size;
    if(sz == 0) {
        sz = m_sys_menu_font_size;
    }
    fnt = font_icons_label->font();
    fnt.setPointSize(sz);
    font_icons_label->setFont(fnt);

    fnt = font_icons_sizes->font();
    fnt.setPointSize(sz);
    font_icons_sizes->setFont(fnt);

    connect(font_icons_sizes, &QComboBox::currentTextChanged, this, [=](const QString &str) {
        auto list = str.split("x");
        int sz = list.at(0).toUInt();
        slot_set_icons_size(sz);
        emit signal_save_settings();
    });

    //-----------------------------------------------------------
    QLabel *font_menu_label = new QLabel(tr("Menu Font"));
    QComboBox *font_menu_sizes = new QComboBox();
    font_menu_sizes->addItem(tr("System"));
    font_menu_sizes->addItem("10");
    font_menu_sizes->addItem("12");
    font_menu_sizes->addItem("14");
    font_menu_sizes->addItem("16");

    if(m_menu_font_size == 10)
        font_menu_sizes->setCurrentIndex(1);
    else if(m_menu_font_size == 12)
        font_menu_sizes->setCurrentIndex(2);
    else if(m_menu_font_size == 14)
        font_menu_sizes->setCurrentIndex(3);
    else if(m_menu_font_size == 16)
        font_menu_sizes->setCurrentIndex(4);

    sz = m_menu_font_size;
    if(sz == 0) {
        sz = m_sys_menu_font_size;
    }
    fnt = font_menu_label->font();
    fnt.setPointSize(sz);
    font_menu_label->setFont(fnt);

    fnt = font_menu_sizes->font();
    fnt.setPointSize(sz);
    font_menu_sizes->setFont(fnt);

    connect(font_menu_sizes, &QComboBox::currentTextChanged, this, [=](const QString &str) {
        auto list = str.split("x");
        int sz = list.at(0).toUInt();
        slot_set_menu_font_size(sz);
        emit signal_save_settings();
    });

    //-----------------------------------------------------------
    QLabel *font_content_label = new QLabel(tr("Content Font"));
    QComboBox *font_content_sizes = new QComboBox();
    font_content_sizes->addItem(tr("System"));
    font_content_sizes->addItem("10");
    font_content_sizes->addItem("12");
    font_content_sizes->addItem("14");
    font_content_sizes->addItem("16");

    if(m_content_font_size == 10)
        font_content_sizes->setCurrentIndex(1);
    else if(m_content_font_size == 12)
        font_content_sizes->setCurrentIndex(2);
    else if(m_content_font_size == 14)
        font_content_sizes->setCurrentIndex(3);
    else if(m_content_font_size == 16)
        font_content_sizes->setCurrentIndex(4);

    sz = m_menu_font_size;
    if(sz == 0) {
        sz = m_sys_menu_font_size;
    }
    fnt = font_content_label->font();
    fnt.setPointSize(sz);
    font_content_label->setFont(fnt);

    fnt = font_content_sizes->font();
    fnt.setPointSize(sz);
    font_content_sizes->setFont(fnt);

    connect(font_content_sizes, &QComboBox::currentTextChanged, this, [=](const QString &str) {
        auto list = str.split("x");
        int sz = list.at(0).toUInt();
        slot_set_content_font_size(sz);
        emit signal_save_settings();
    });

    //-----------------------------------------------------------
    QLabel *font_graph_label = new QLabel(tr("Graph Font"));
    QComboBox *font_graph_sizes = new QComboBox();
    font_graph_sizes->addItem(tr("System"));
    font_graph_sizes->addItem("6" );
    font_graph_sizes->addItem("8" );
    font_graph_sizes->addItem("10");
    font_graph_sizes->addItem("12");
    font_graph_sizes->addItem("14");
    font_graph_sizes->addItem("16");

    if(m_graph_font_size == 6)
        font_graph_sizes->setCurrentIndex(1);
    else if(m_graph_font_size == 8)
        font_graph_sizes->setCurrentIndex(2);
    if(m_graph_font_size == 10)
        font_graph_sizes->setCurrentIndex(3);
    else if(m_graph_font_size == 12)
        font_graph_sizes->setCurrentIndex(4);
    else if(m_graph_font_size == 14)
        font_graph_sizes->setCurrentIndex(5);
    else if(m_graph_font_size == 16)
        font_graph_sizes->setCurrentIndex(6);

    sz = m_menu_font_size;
    if(sz == 0) {
        sz = m_sys_menu_font_size;
    }
    fnt = font_graph_label->font();
    fnt.setPointSize(sz);
    font_graph_label->setFont(fnt);

    fnt = font_graph_sizes->font();
    fnt.setPointSize(sz);
    font_graph_sizes->setFont(fnt);

    connect(font_graph_sizes, &QComboBox::currentTextChanged, this, [=](const QString &str) {
        auto list = str.split("x");
        int sz = list.at(0).toUInt();
        slot_set_graph_font_size(sz);
        emit signal_save_settings();
    });

    //-----------------------------------------------------------
    QDialogButtonBox *buttons_group = new QDialogButtonBox(Qt::Horizontal);
    buttons_group->setStandardButtons(QDialogButtonBox::Close);
    buttons_group->setMinimumSize(300, 24);

    QPushButton *btn = buttons_group->button(QDialogButtonBox::Close);
    btn->setText(QCoreApplication::tr("Close"));
    btn->setDefault(true);
    btn->setAutoDefault(true);

    connect(buttons_group, SIGNAL(accepted()), fontdlg, SLOT(accept()));
    connect(buttons_group, SIGNAL(rejected()), fontdlg, SLOT(reject()));

    //-----------------------------------------------------------
    int nrow = 0;
    QGridLayout *grid = new QGridLayout();
    grid->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    if(m_toolBar)
    {
        grid->addWidget(font_icons_label, nrow, 0);
        grid->addWidget(font_icons_sizes, nrow, 1);
        ++nrow;
    }
    grid->addWidget(font_menu_label,  nrow, 0);
    grid->addWidget(font_menu_sizes,  nrow, 1);
    ++nrow;
    grid->addWidget(font_content_label, nrow, 0);
    grid->addWidget(font_content_sizes, nrow, 1);

    grid->setColumnStretch(0, 50);
    grid->setColumnStretch(1, 50);

    //-----------------------------------------------------------
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(grid);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(buttons_group);

    mainLayout->setAlignment(Qt::AlignHCenter);
    mainLayout->setAlignment(grid, Qt::AlignCenter);
    mainLayout->setAlignment(buttons_group, Qt::AlignCenter);
    fontdlg->setLayout(mainLayout);

    //-----------------------------------------------------------
    fnt = btn->font();
    fnt.setPointSize(sz);
    btn->setFont(fnt);

    fontdlg->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    fontdlg->adjustSize();
    //fontdlg->setMinimumWidth(500);
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    fontdlg->exec();
    QApplication::restoreOverrideCursor();
    delete fontdlg;
}

//-----------------------------------------------------------
void MW::slot_set_icons_size(int sz)
{
    DBGF(4, "%d", sz);
    if(sz != 24 && sz != 32 && sz != 48) sz = 0;

    m_icon_size = sz;
    if(sz == 0) sz = m_sys_icon_size;

    if(m_toolBar0) m_toolBar0->setIconSize(QSize(sz, sz));
    if(m_toolBar) m_toolBar->setIconSize(QSize(sz, sz));
}

//-----------------------------------------------------------
void MW::slot_set_menu_font_size(int sz)
{
    DBGF(4, "%d", sz);
    if(sz != 10 && sz != 12 && sz != 14 && sz != 16) {
        sz = 0;
    }
    m_menu_font_size = sz;
    if(sz == 0) sz = m_sys_menu_font_size;

    QFont fnt = menuBar()->font();
    fnt.setPointSize(sz);
    fnt.setBold(false);
    menuBar()->setFont(fnt);

    if(m_fileMenu    ) m_fileMenu->setFont(fnt);
    if(m_settingsMenu) m_settingsMenu->setFont(fnt);
    if(m_helpMenu    ) m_helpMenu->setFont(fnt);

    //-----------------------------------------------------------
    QFont fnt2 = statusBar()->font();
    fnt2.setPointSize(10);
    //fnt2.setBold(true);
    fnt2.setStyleHint(QFont::Monospace);
    fnt2.setFamily("Fixed");
    fnt2.setFixedPitch(true);
    statusBar()->setFont(fnt2);

    //-----------------------------------------------------------
    // fnt = m_label_powerset->font();
    // fnt.setPointSize(sz);
    // fnt.setStyleHint(QFont::Monospace);
    // fnt.setFixedPitch(true);
    // m_label_powerset->setFont(fnt);
    // m_label_powerout->setFont(fnt);

    //m_label_power->setFont(fnt);
    //m_label_powerin->setFont(fnt);
    // m_label_min->setFont(fnt);
    // m_label_max->setFont(fnt);

    //// //-----------------------------------------------------------
    //// if(m_fontdlg && parent())
    //// {
    ////     m_fontdlg->setFont(fnt);
    ////     // QListWidget *lst = parent()->findChild<QListWidget*>();
    ////     // for(int i = 0; i < lst->count(); ++i)
    ////     // {
    ////     //    QListWidgetItem *it = lst->item(i);
    ////     //    it->setFont(fnt);
    ////     // }
    //// }

    //slot_set_msg_font_size(text);
}

//-----------------------------------------------------------
void MW::slot_set_content_font_size(int sz)
{
    DBGF(4, "%d", sz);
    QString mono_font;
#ifdef MONO_FONT_FAMILY
    mono_font = MONO_FONT_FAMILY;
#endif

    if(sz != 10 && sz != 12 && sz != 14 && sz != 16) {
        sz = 0;
    }
    if(sz == 0) {
        sz = m_sys_content_font_size;
        m_content_font_size = sz;
    } else {
        m_content_font_size = sz;
    }

    //-----------------------------------------------------------
    QFont fnt = m_stacked->font();
    fnt.setPointSize(sz);
    fnt.setStyleHint(QFont::Monospace);
    fnt.setFamily("Fixed");
    fnt.setFixedPitch(true);
    m_stacked->setFont(fnt);

    //-----------------------------------------------------------
    statusBar()->setFont(fnt);
    setFont(fnt);

    //-----------------------------------------------------------
    m_app->bto_gui()->set_content_font_size(sz);
    m_app->fw_gui()->set_content_font_size(sz);
    m_app->diagn_gui()->set_content_font_size(sz);
    m_app->plato_gui()->set_content_font_size(sz);
    m_app->api_gui()->set_content_font_size(sz);
}

//-----------------------------------------------------------
void MW::slot_set_graph_font_size(int sz)
{
    DBGF(4, "%d", sz);
    QString mono_font;
#ifdef MONO_FONT_FAMILY
    mono_font = MONO_FONT_FAMILY;
#endif

    if (sz <  6) sz = 0;
    if (sz > 16) sz = 0;

    if(sz == 0) sz = m_sys_graph_font_size;
    m_graph_font_size = sz;

    //// TODO QFont fnt = m_plato_gui->m_chartview->font();
    //// TODO fnt.setPointSize(sz);
    //// TODO //fnt.setBold(true);
    //// TODO m_plato_gui->m_chart->setFont(fnt);
}

//-----------------------------------------------------------
void MW::slot_set_msg_font_size(int sz)
{
    if(sz < 14) sz = 0;
    if(sz == 0) sz = m_sys_msg_font_size;

    // QString c_style =
    //    QString("QMessageBox { font: %1pt; }\n"
    //             "QPushButton { font: %2pt; }  ")
    //    .arg(sz).arg(sz);

    QString c_style = QString("QMessageBox { font: %1pt; }\n").arg(sz);

    //"QPushButton { max-height: 8px; }\n"

    //qDebug() << c_style;
    qApp->setStyleSheet(c_style);
}
