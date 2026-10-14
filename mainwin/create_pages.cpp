/*!
  \file   select_page.cpp
  \brief  Выбор активной страницы для отображения
  \author Aleksey Lekhtin
  \date   10.05.2023
*/

#include "main.h"
#include "mainwindow.h"

#include "api.h"
#include "api_gui.h"
#include "bto.h"
#include "bto_gui.h"
#include "comm.h"
#include "diagn.h"
#include "diagn_gui.h"
#include "fw.h"
#include "fw_gui.h"
#include "plato.h"
#include "plato_gui.h"

//-----------------------------------------------------------
void MW::create_pages()
{
    m_page_bto    = app()->bto_gui()->page();
    m_page_fw     = app()->fw_gui()->page();
    m_page_diagn  = app()->diagn_gui()->page();
    m_page_plato  = app()->plato_gui()->page();
    m_page_api    = app()->api_gui()->page();

    m_stacked = new QStackedWidget(this);
    setCentralWidget(m_stacked);

    m_stacked->addWidget(m_page_bto);
    m_stacked->addWidget(m_page_diagn);
    m_stacked->addWidget(m_page_fw);
    m_stacked->addWidget(m_page_plato);
    m_stacked->addWidget(m_page_api);
    m_stacked->setCurrentIndex(m_last_tab);
}

//-----------------------------------------------------------
void MW::slot_select_bto_page()
{
    DBG(3);
    select_bto_page();
}

//-----------------------------------------------------------
void MW::select_bto_page(int mode)
{
    DBG(3, QString::number(mode));
    clearstatus();
    if (mode == 1)
    {
        m_stacked->setCurrentWidget(m_page_bto);
        m_action__bto->setChecked(true);
        return;
    }
    m_stacked->setCurrentWidget(m_page_bto);
    m_action__bto->setChecked(true);
    m_action__firmware->setChecked(false);
    m_action__diagn->setChecked(false);
    m_action__plato->setChecked(false);
    m_action__api->setChecked(false);
    if(!app()->simulation()) {
        m_action__load_report->setEnabled(false);
        m_action__save_report->setEnabled(false);
        m_action__load_last->setEnabled(false);
    } else {
        m_action__save_sim->setEnabled(false);
        m_action__load_sim->setEnabled(false);
    }

    bto()->slot_bto_polling();
    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

//-----------------------------------------------------------
void MW::slot_select_firmware_page()
{
    DBG(3);
    select_firmware_page();
}

//-----------------------------------------------------------
void MW::select_firmware_page()
{
    DBG(3);
    comm()->clear_requests();
    clearstatus();
    m_stacked->setCurrentWidget(m_page_fw);
    m_action__bto->setChecked(false);
    m_action__firmware->setChecked(true);
    m_action__diagn->setChecked(false);
    m_action__plato->setChecked(false);
    m_action__api->setChecked(false);
    if(!app()->simulation()) {
        m_action__load_report->setEnabled(false);
        m_action__save_report->setEnabled(false);
        m_action__load_last->setEnabled(false);
    } else {
        m_action__save_sim->setEnabled(false);
        m_action__load_sim->setEnabled(false);
    }
    fw()->update_gui();
    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

//---------------------------------------------------------
void MW::slot_select_diagn_page()
{
    DBG(3);
    select_diagn_page();
}

//---------------------------------------------------------
void MW::select_diagn_page()
{
    DBG(3);
    clearstatus();
    m_stacked->setCurrentWidget(m_page_diagn);
    m_action__bto->setChecked(false);
    m_action__firmware->setChecked(false);
    m_action__diagn->setChecked(true);
    m_action__plato->setChecked(false);
    m_action__api->setChecked(false);
    if(!app()->simulation()) {
        m_action__load_report->setEnabled(false);
        m_action__save_report->setEnabled(false);
        m_action__load_last->setEnabled(false);
    } else {
        m_action__save_sim->setEnabled(false);
        m_action__load_sim->setEnabled(false);
    }
    diagn()->update_gui();
    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

//-----------------------------------------------------------
void MW::slot_select_plato_page()
{
    DBG(3);
    comm()->clear_requests();
    clearstatus();
    m_stacked->setCurrentWidget(m_page_plato);
    m_action__bto->setChecked(false);
    m_action__firmware->setChecked(false);
    m_action__diagn->setChecked(false);
    m_action__plato->setChecked(true);
    m_action__api->setChecked(false);
    if(!app()->simulation()) {
        m_action__load_report->setEnabled(true);
        m_action__save_report->setEnabled(true);
        m_action__load_last->setEnabled(true);
    } else {
        m_action__save_sim->setEnabled(true);
        m_action__load_sim->setEnabled(true);
    }
    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    DBGNL(3);
}

//-----------------------------------------------------------
void MW::slot_select_api_page()
{
    DBG(3);
    comm()->clear_requests();
    clearstatus();
    m_stacked->setCurrentWidget(m_page_api);
    m_action__bto->setChecked(false);
    m_action__firmware->setChecked(false);
    m_action__diagn->setChecked(false);
    m_action__plato->setChecked(false);
    m_action__api->setChecked(true);
    if(!app()->simulation()) {
        m_action__load_report->setEnabled(false);
        m_action__save_report->setEnabled(true);
        m_action__load_last->setEnabled(false);
    } else {
        m_action__save_sim->setEnabled(false);
        m_action__load_sim->setEnabled(false);
    }
    api()->update_gui();
    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    DBGNL(3);
}

//-----------------------------------------------------------
MW::PAGES MW::current_tab() const
{
    if (!m_stacked) return PAGES::UNKNOWN;
    if (m_stacked->currentWidget() == m_page_bto) {
        return PAGES::BTO_PAGE;
    }
    if (m_stacked->currentWidget() == m_page_fw) {
        return PAGES::FIRMWARE_PAGE;
    }
    if (m_stacked->currentWidget() == m_page_diagn) {
        return PAGES::DIAGN_PAGE;
    }
    if (m_stacked->currentWidget() == m_page_plato) {
        return PAGES::PLATO_PAGE;
    }
    if (m_stacked->currentWidget() == m_page_api) {
        return PAGES::API_PAGE;
    }

    return PAGES::UNKNOWN;
}

#if 0
//-----------------------------------------------------------
// Действия по смене вкладки
void MW::slot_stacked_changed(int index)
{
    (void)index;
    if (!m_stacked->isVisible()) {
        return;
    }
    QWidget *wgt = m_stacked->currentWidget();
    if (wgt == m_page_bto)
    {
        //// LOG::dbgf2("MW::%s() selected bto page\n", __FUNCTION__);
    }
    else if (wgt == m_page_fw)
    {
        //// LOG::dbgf2("MW::%s() selected srig page\n", __FUNCTION__);
    }
    else if (wgt == m_page_diagn)
    {
        //// LOG::dbgf2("MW::%s() selected srig diagn terminal\n", __FUNCTION__);
    }
    else if (wgt == m_page_plato)
    {
        //// LOG::dbgf2("MW::%s() selected srig plato test\n", __FUNCTION__);
    }
}
#endif
