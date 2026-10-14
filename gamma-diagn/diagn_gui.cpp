/*!
  \file   diagn_gui.cpp
  \brief  Отображение информации в окне диагностики
  \author Aleksey Lekhtin
  \date   29.11.2016
*/

#include "diagn.h"
#include "diagn_gui.h"
#include "main.h"
#include "mainwindow.h"
#include "terminal.h"

//---------------------------------------------------------
DIAGN_GUI::DIAGN_GUI(APP *app) :
    m_app(app),
    m_diagn(nullptr),
    m_ico_gray(QIcon(":q_circle_gray.png")),
    m_ico_red(QIcon(":q_circle_red.png")),
    m_ico_green(QIcon(":q_circle_green.png")),
    m_ico_yellow(QIcon(":q_circle_yellow.png"))
{
    m_terminal = new TERMINAL(m_app->mw());
    m_gui_page = create_diagnostic_page();
    read_settings(m_app->qSettings());
}

//---------------------------------------------------------
DIAGN_GUI::~DIAGN_GUI()
{
    DBG(4);
    delete m_terminal;
}

//---------------------------------------------------------
void DIAGN_GUI::set_backgr(DIAGN *diagn)
{
    m_diagn = diagn;
}

//---------------------------------------------------------
void DIAGN_GUI::showstatus(const QString& msg, int sec)
{
    emit mw()->signal_showstatus(msg, sec);
}

//---------------------------------------------------------
TERMINAL *DIAGN_GUI::terminal() const
{
    return m_terminal;
}

//---------------------------------------------------------
void DIAGN_GUI::slot_run_diagnostic()
{
    if (m_button_diagn_start->isChecked())
    {
        if (!start_diagnostic()) return;
    }
    else
    {
        stop_diagnostic();
        if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
            comm()->disconnect_from();
        }
    }
}

//---------------------------------------------------------
bool DIAGN_GUI::start_diagnostic()
{
    DBG(3);

    showstatus(tr("Start scanning..."), 5);
    if (!m_diagn->start_scan()) {
        m_button_diagn_start->setChecked(false);
        mw()->unlock_pages();
        return FAIL;
    }
    open_term_protocol();
    m_button_diagn_start->setText(QString(" %1 ").arg(tr("Stop")));
    m_button_diagn_start->setShortcut(QKeySequence("Ctrl+R"));
    auto tip = QStringLiteral("%1 [%2]").arg(tr("Stop scanning"), m_button_diagn_start->shortcut().toString());
    m_button_diagn_start->setToolTip(tip);
    set_led_icon_idle();
    mw()->lock_pages();
    return true;
}

//---------------------------------------------------------
void DIAGN_GUI::stop_diagnostic()
{
    DBG(2);
    ////QApplication::setOverrideCursor(Qt::WaitCursor);
    close_term_protocol();
    m_diagn->stop_diagn_timer();
    m_button_diagn_start->setText(QString(" %1 ").arg(tr("Connect")));
    m_button_diagn_start->setShortcut(QKeySequence("Ctrl+R"));
    auto tip = QStringLiteral("%1 [%2]").arg(tr("Start scanning"), m_button_diagn_start->shortcut().toString());
    m_button_diagn_start->setToolTip(tip);
    m_button_diagn_start->setChecked(false);
    set_led_icon_off();
    mw()->unlock_pages();
    ////QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------
void DIAGN_GUI::requests_completed(int nodecode)
{
    (void)nodecode;
    if (mw()->current_tab() != MW::PAGES::DIAGN_PAGE) {
        return;
    }

    DBG(3);
    if (m_button_diagn_start->isChecked()) {
        set_led_icon_idle();
        write_term_protocol_record("\n  ");
    }
}


//---------------------------------------------------------
void DIAGN_GUI::display_info_message(const QString &msg)
{
    emit mw()->signal_message(QMessageBox::Information, msg);
}

//---------------------------------------------------------
void DIAGN_GUI::display_error_message(const QString &msg)
{
    display_error_message_sec(msg, 60);
}

//---------------------------------------------------------
void DIAGN_GUI::display_error_message_sec(const QString &msg, int sec)
{
    showstatus(msg, sec);
    emit mw()->signal_message(QMessageBox::Critical, msg);
}
