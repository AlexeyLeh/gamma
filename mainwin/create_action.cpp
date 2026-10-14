/*!
 * \file   create_action.cpp
 * \author Aleksey Lekhtin
 * \date   created 17.11.2014
 * \brief  Обработчики объектов управления
 */

#include "main.h"
#include "mainwindow.h"

#include "api.h"
#include "api_gui.h"
#include "bto.h"
#include "bto_gui.h"
#include "comm.h"
#include "diagn_gui.h"
#include "fw_gui.h"
#include "gamma.h"
#include "plato_gui.h"
#include "utils.h"
#include "wmsg.h"


//---------------------------------------------------------
void MW::create_action()
{
    m_action__exit = new QAction(QIcon(":q_exit.png"), tr("&Quit"), this);
    m_action__exit->setShortcut(Qt::Key_F10);
    m_action__exit->setToolTip(make_tip(m_action__exit));
    m_action__exit->setVisible(true);
    m_action__exit->setEnabled(true);

    connect(m_action__exit, &QAction::triggered, this, [m = app()]() {
        m->main_quit();
    });

    connect(new QShortcut(QKeySequence("Ctrl+Q"), this), &QShortcut::activated, qApp, []() {
        QApplication::closeAllWindows();
    }, Qt::QueuedConnection);

    connect(new QShortcut(QKeySequence("Alt+S"), this), &QShortcut::activated, this, [this]() {
        QRect rect = menuBar()->actionGeometry(m_settingsMenu->menuAction());
        QPoint pos = mapToParent(rect.bottomLeft());
        m_settingsMenu->popup(pos);
    });

    //-----------------------------------------------------------
    m_action__about = new QAction(QIcon(":q_app_gamma.png"), tr("&About"), this);
    m_action__about->setShortcut(QKeySequence("F1"));
    m_action__about->setToolTip(make_tip(m_action__about));
    connect(m_action__about, &QAction::triggered, this, [=]() {
        QString trmsg = tr("Translation not found.\nUsing en_US");

        QFileInfo finfo(app()->tr_name);
        auto trdir = QDir::toNativeSeparators(finfo.path());
        QStringList trlst = trdir.split(QDir::separator());
        //// QString trname;
        //// int maxlen = 35;
        //// foreach(QString item, trlst) {
        ////     trname += item;
        ////     trname += QDir::separator();
        ////     if (trname.size() > maxlen) {
        ////         trname += '\n';
        ////         maxlen += 35;
        ////     }
        //// }
        //// trname += finfo.fileName();

        QString trname = UTILS::split_path(finfo.absolutePath());
        if (!app()->tr_name.isEmpty()) {
            trmsg = QString("%1:\n%2\n%3")
                    .arg(tr("Translations"))
                    .arg(trname)
                    .arg(finfo.fileName());
        }

        QString api_factor_ver = QString("%1: %2")
                                 .arg(tr("API factor Version"))
                                 .arg(app()->api()->factor_version());

        QString msg = QString("%1\n%2\n\n%3\n\n%4")
                      .arg(app()->title(),
                           app()->version(),
                           api_factor_ver,
                           trmsg);
        //WMsg(QMessageBox::Information, tr("About Application"), msg);
        emit signal_message(QMessageBox::Information, tr("About Application"), msg);
    });

    //-----------------------------------------------------------
    m_action__fonts_size = new QAction(tr("&Font Sizes"), this);
    m_action__fonts_size->setToolTip(make_tip(m_action__fonts_size));
    connect(m_action__fonts_size, SIGNAL(triggered()), this, SLOT(slot_font_settings()));

    //-----------------------------------------------------------
    m_action__bto = new QAction(QIcon(":q_bto.png"), tr("Service &Unit"), this);
    m_action__bto->setShortcut(QKeySequence("Ctrl+1"));
    m_action__bto->setToolTip(make_tip(m_action__bto));
    m_action__bto->setCheckable(true);
    connect(m_action__bto, SIGNAL(triggered()), this, SLOT(slot_select_bto_page()));

    m_action__sp = new QAction(QIcon(":q_comport.png"), tr("Serial &Port"), this);
    m_action__sp->setToolTip(make_tip(m_action__sp));
    m_action__sp->setCheckable(false);
    m_action__sp->setDisabled(true);

    //-----------------------------------------------------------
    m_action__diagn = new QAction(QIcon(":q_terminal.png"), tr("&Diagnostic"), this);
    m_action__diagn->setShortcut(QKeySequence("Ctrl+2"));
    m_action__diagn->setToolTip(make_tip(m_action__diagn));
    m_action__diagn->setIconText(tr("&Diagnostic"));
    m_action__diagn->setCheckable(true);
    connect(m_action__diagn, SIGNAL(triggered()), this, SLOT(slot_select_diagn_page()));

    //-----------------------------------------------------------
    m_action__firmware = new QAction(QIcon(":q_chip.png"), tr("&Firmware"), this);
    m_action__firmware->setShortcut(QKeySequence("Ctrl+3"));
    m_action__firmware->setToolTip(make_tip(m_action__firmware));
    m_action__firmware->setIconText(tr("Gamma Module Firmware"));
    m_action__firmware->setCheckable(true);
    connect(m_action__firmware, SIGNAL(triggered()), this, SLOT(slot_select_firmware_page()));

    //-----------------------------------------------------------
    m_action__plato = new QAction(QIcon(":q_platotest.png"), tr("&Plateau Test"), this);
    m_action__plato->setShortcut(QKeySequence("Ctrl+4"));
    m_action__plato->setToolTip(make_tip(m_action__plato));
    m_action__plato->setIconText(tr("&Plateau Test"));
    m_action__plato->setCheckable(true);
    connect(m_action__plato, SIGNAL(triggered()), this, SLOT(slot_select_plato_page()));

    QPixmap pix(":q_platotest.png");
    pix.scaled(32, 16);
    QIcon icon(pix);
    m_action__plato->setIcon(icon);

    //-----------------------------------------------------------
    m_action__api = new QAction(QIcon(":q_api.png"), tr("&API Test"), this);
    m_action__api->setShortcut(QKeySequence("Ctrl+5"));
    m_action__api->setToolTip(make_tip(m_action__api));
    m_action__api->setIconText(tr("&API Test"));
    m_action__api->setCheckable(true);
    connect(m_action__api, SIGNAL(triggered()), this, SLOT(slot_select_api_page()));

    //-----------------------------------------------------------
    if(!app()->simulation()) {

        auto save_icon = QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton);
        m_action__save_report = new QAction(save_icon, tr("Save report"), this);
        m_action__save_report->setShortcut(QKeySequence("Ctrl+S"));
        m_action__save_report->setToolTip(make_tip(m_action__api));
        m_action__save_report->setIconText(tr("Save Report"));
        m_action__save_report->setCheckable(true);
        m_action__save_report->setDisabled(true);
        connect(m_action__save_report, &QAction::triggered, this, [this]() {
            DBG(1, "slot_save_report()");
            if (current_tab() == PAGES::PLATO_PAGE) {
                plato()->gui()->slot_save_report();
            } else if (current_tab() == PAGES::API_PAGE) {
                api()->gui()->slot_save_report();
            }
        });

        //---------------------------------------
        auto load_icon = QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton);
        m_action__load_report = new QAction(load_icon, tr("Load Report"), this);
        m_action__load_report->setShortcut(QKeySequence("Ctrl+L"));
        m_action__load_report->setToolTip(make_tip(m_action__api));
        m_action__load_report->setIconText(tr("Load Report"));
        m_action__load_report->setCheckable(true);
        m_action__load_report->setDisabled(true);
        connect(m_action__load_report, &QAction::triggered, this, [this]() {
            DBG(1, "slot_load_report()");
            if (current_tab() == PAGES::PLATO_PAGE) {
                plato()->gui()->slot_load_report();
            } else if (current_tab() == PAGES::API_PAGE) {
                ; // api()->gui()->slot_load_report();
            }
        });

        //---------------------------------------
        QIcon load_last_icon(":q_opendir.svg");
        m_action__load_last = new QAction(load_last_icon, tr("Load Last Test"), this);
        m_action__load_last->setShortcut(QKeySequence("Alt+L"));
        m_action__load_last->setToolTip(make_tip(m_action__api));
        m_action__load_last->setIconText(tr("Load Last Test"));
        m_action__load_last->setCheckable(true);
        m_action__load_last->setDisabled(true);
        connect(m_action__load_last, &QAction::triggered, this, [this]() {
            DBG(1, "slot_load_last_test()");
            if (current_tab() == PAGES::PLATO_PAGE) {
                plato()->gui()->slot_load_last_test();
            } else if (current_tab() == PAGES::API_PAGE) {
                ; // api()->gui()->slot_load_last_test();
            }
        });

    } else {

        //-----------------------------------------------------------
        auto save_sim_icon = QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton);
        m_action__save_sim = new QAction(save_sim_icon, tr("Save simulation"), this);
        m_action__save_sim->setToolTip(m_action__save_sim->text());
        m_action__save_sim->setIconText(m_action__save_sim->text());
        m_action__save_sim->setCheckable(true);
        m_action__save_sim->setDisabled(true);
        connect(m_action__save_sim, &QAction::triggered, this, [this]() {
            DBG(1, "slot_save_simulation()");
            if (current_tab() == PAGES::PLATO_PAGE) {
                plato()->gui()->simulation_save();
            } else if (current_tab() == PAGES::API_PAGE) {
                ; //api()->gui()->slot_save_simulation();
            }
        });

        //---------------------------------
        auto load_sim_icon = QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton);
        m_action__load_sim = new QAction(load_sim_icon, tr("Load simulation"), this);
        m_action__load_sim->setToolTip(m_action__load_sim->text());
        m_action__load_sim->setIconText(m_action__load_sim->text());
        m_action__load_sim->setCheckable(true);
        m_action__load_sim->setDisabled(true);
        connect(m_action__load_sim, &QAction::triggered, this, [this]() {
            DBG(1, "slot_load_simulation()");
            if (current_tab() == PAGES::PLATO_PAGE) {
                bool ok = plato()->gui()->simulation_load();
                if (ok) {
                    plato()->gui()->button_plato_run()->setText(tr("Replay Simulation"));
                }
            } else if (current_tab() == PAGES::API_PAGE) {
                ; //api()->gui()->slot_save_simulation();
            }
        });
    }

    // Настройки

    //-----------------------------------------------------------
    m_action__setup_SN = new QAction(QIcon(":q_sn.png"), tr("&Serial Number"), this);
    m_action__setup_SN->setCheckable(true);
    //m_action__setup_SN->setShortcut(Qt::ALT+'N');
    auto tip = make_tip(m_action__setup_SN) + " - " + app()->SN();
    m_action__setup_SN->setStatusTip(tip);
    connect(m_action__setup_SN, SIGNAL(triggered()), this, SLOT(slot_setup_serialno()));

    //-----------------------------------------------------------
    m_action__setup_oper = new QAction(QIcon(":q_operator.png"), tr("&Operator"), this);
    m_action__setup_oper->setCheckable(true);
    //m_action__setup_oper->setShortcut(Qt::ALT+'O');
    tip = make_tip(m_action__setup_oper) + " - " + app()->Operator();
    m_action__setup_oper->setStatusTip(tip);
    connect(m_action__setup_oper, SIGNAL(triggered()), this, SLOT(slot_setup_operator()));

    //-----------------------------------------------------------
    m_action__setup_com = new QAction(QIcon(":q_comport.png"), tr("Serial &Port Settings"), this);
    m_action__setup_com->setCheckable(true);
    m_action__setup_com->setShortcut(Qt::ALT+'P');
    m_action__setup_com->setStatusTip(make_tip(m_action__setup_com));
    connect(m_action__setup_com, SIGNAL(triggered()), this, SLOT(slot_setup_comport()));

    //-----------------------------------------------------------
    m_action__setup_bto_host = new QAction(QIcon(":q_bto.png"), tr("Service &Unit Settings"), this);
    m_action__setup_bto_host->setCheckable(true);
    m_action__setup_bto_host->setShortcut(Qt::ALT+'B');
    m_action__setup_bto_host->setStatusTip(make_tip(m_action__setup_bto_host));
    connect(m_action__setup_bto_host, SIGNAL(triggered()), this, SLOT(slot_setup_bto()));

    //-----------------------------------------------------------
    m_action__select_eth = new QAction(tr("Use Service Unit"), this);
    m_action__select_eth->setCheckable(true);
    m_action__select_eth->setShortcut(QKeySequence("Ctrl+B"));
    m_action__select_eth->setStatusTip(make_tip(m_action__select_eth));

    connect(m_action__select_eth, &QAction::triggered, this, [this]() {
        DBG(3);
        app()->change_connection(COMM::CONNECT_TYPE::ETHERNET);
        m_action__setup_bto_host->setEnabled(true);
        m_action__setup_com->setDisabled(true);
        m_action__bto->setEnabled(true);
        m_action__sp->setDisabled(true);
        m_toolBar->removeAction(m_action__sp);
        m_toolBar->insertAction(m_action_sep_second, m_action__bto);
        update_statusbar_label_text();
        set_status_bar_label_color(MW::STATUS_BAR_LABEL::NORMAL);
        m_page_bto->setEnabled(true);
        select_bto_page();
    });

    //-----------------------------------------------------------
    m_action__select_com = new QAction(tr("Use Serial port"), this);
    m_action__select_com->setCheckable(true);
    m_action__select_com->setShortcut(QKeySequence("Ctrl+P"));
    m_action__select_com->setStatusTip(make_tip(m_action__select_com));

    connect(m_action__select_com, &QAction::triggered, this, [this]() {
        DBG(3);
        app()->change_connection(COMM::CONNECT_TYPE::COMPORT);
        m_action__setup_com->setEnabled(true);
        m_action__setup_bto_host->setDisabled(true);
        m_action__bto->setDisabled(true);
        m_page_bto->setDisabled(true);
        m_action__sp->setDisabled(true);
        m_toolBar->removeAction(m_action__bto);
        m_toolBar->insertAction(m_action_sep_second, m_action__sp);
        update_statusbar_label_text();
        set_status_bar_label_color(MW::STATUS_BAR_LABEL::NORMAL);
        m_page_bto->hide();
        select_diagn_page();
    });

    m_action__select_group = new QActionGroup(this);
    m_action__select_group->addAction(m_action__select_eth);
    m_action__select_group->addAction(m_action__select_com);
    if (comm()->type() == COMM::CONNECT_TYPE::ETHERNET) {
        m_action__select_eth->setChecked(true);
        m_action__setup_bto_host->setEnabled(true);
        m_action__setup_com->setDisabled(true);
    } else if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
        m_action__select_com->setChecked(true);
        m_action__setup_com->setEnabled(true);
        m_action__setup_bto_host->setDisabled(true);
    }
}

//---------------------------------------------------------
QString MW::make_tip(QAction *act)
{
    // QString tip = act->toolTip();
    // if (!act->shortcut().toString().isEmpty())
    //    tip = act->toolTip() + " [" + act->shortcut().toString() + "]";
    // return tip;
    QString tip = act->text();
    tip.replace("&", "");
    if (!act->shortcut().toString().isEmpty())
        tip = tip + " [" + act->shortcut().toString() + "]";
    return tip;
}

//---------------------------------------------------------
void MW::slot_setup_serialno()
{
    QDialog *dlg = new QDialog(this);
    auto buttons_group = MW::make_dialog_two_button(dlg, " " + tr("&Apply") + " ", " " + tr("&Cancel") + " ");

    // QRegExp rx1("[a-z][A-Z][0-9]{1-8}");
    // QRegExp rx1("[0-9]{1-8}");
    // QRegExpValidator *validator1 = new QRegExpValidator(rx1, this);
    // QRegularExpression re0("\\d{0,12}");
    // QRegularExpression re(QString("\\d{0,%1}").arg(GAMMA::SN_LEN));
    // QRegularExpression re(QString("[A-Za-z0-9]{0,%1}").arg(GAMMA::SN_LEN));
    QRegularExpression re(QString("[\\w]{0,%1}").arg(GAMMA::SN_LEN));
    QRegularExpressionValidator *validator1 = new QRegularExpressionValidator(re, this);
    QLineEdit *SN = new QLineEdit();
    SN->setValidator(validator1);
    SN->setText(app()->SN());
    app()->plato_gui()->update_serial_number(app()->SN());

    //-----------------------------------------------------------
    QGridLayout *grid = new QGridLayout();
    grid->addWidget(new QLabel(MW::tr("S/N")), 0, 0);
    grid->addWidget(SN, 0, 1);

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
    dlg->setWindowTitle(tr("Setup Tool Serial Number"));
    dlg->setWindowIcon(QIcon(":q_sn.png"));
    QFont fnt = dlg->font();
    fnt.setPointSize(m_content_font_size);
    dlg->setFont(fnt);
    dlg->adjustSize();
    dlg->setMinimumWidth(500);

    QApplication::setOverrideCursor(Qt::ArrowCursor);
    int rc = dlg->exec();
    QApplication::restoreOverrideCursor();
    if(rc == QDialog::Accepted)
    {
        auto sn = SN->text().trimmed();
        int rem = GAMMA::SN_LEN - sn.length();
        QString str(GAMMA::SN_LEN, QChar('0'));
        sn.prepend(str.mid(0, rem));
        app()->set_SN(sn);
        app()->api_gui()->update_serial_number(app()->SN());
        app()->plato_gui()->update_serial_number(app()->SN());
        emit signal_save_settings();
    }
    delete dlg;
}

//---------------------------------------------------------
void MW::slot_setup_operator()
{
    QDialog *dlg = new QDialog(this);
    auto buttons_group = MW::make_dialog_two_button(dlg, tr("&Apply"), tr("&Cancel"));

    //QRegularExpression re("\\d{0,8}");
    //QRegularExpression re("[a-z][A-Z][а-я][А-Я][._\\s][0-9]{1-8}");
    QRegularExpression re("[a-zA-Zа-яА-Я0-9 №]{1,22}");
    QRegularExpressionValidator *validator1 = new QRegularExpressionValidator(re, this);
    QLineEdit *OPER = new QLineEdit();
    OPER->setValidator(validator1);
    OPER->setText(app()->Operator());

    //-----------------------------------------------------------
    QGridLayout *grid = new QGridLayout();
    grid->addWidget(new QLabel(tr("Operator")), 0, 0);
    grid->addWidget(OPER, 0, 1);

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
    dlg->setWindowTitle(tr("Setup operator's name"));
    dlg->setWindowIcon(QIcon(":q_operator.png"));
    QFont fnt = dlg->font();
    fnt.setPointSize(m_content_font_size);
    dlg->setFont(fnt);
    dlg->adjustSize();
    dlg->setMinimumWidth(500);

    QApplication::setOverrideCursor(Qt::ArrowCursor);
    int rc = dlg->exec();
    QApplication::restoreOverrideCursor();
    if(rc == QDialog::Accepted)
    {
        app()->set_Operator(OPER->text().trimmed());
        app()->plato_gui()->update_operator(app()->Operator());
        app()->api_gui()->update_operator(app()->Operator());
        emit signal_save_settings();
    }
    delete dlg;
}


