/*!
 * \file   create_toolbar.cpp
 * \author Aleksey Lekhtin
 * \date   created 17.11.2014
 * \brief  Создание полосы с кнопками инструментов
 */

#include <QtGui>
#include "mainwindow.h"

#include "comm.h"

/*----------------------------------------------------------*/
void MW::create_toolbar()
{
    m_toolBar = new QToolBar("ToolBar1", this);
    m_toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_toolBar->setMovable(false);
    m_toolBar->setFloatable(false);

    m_toolBar0 = new QToolBar("ToolBar0", this);
    m_toolBar0->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_toolBar0->setMovable(false);
    m_toolBar0->setFloatable(false);
    m_toolBar0->setLayoutDirection(Qt::RightToLeft);

    //-----------------------------------------------------------
    addToolBar(Qt::TopToolBarArea, m_toolBar);
    addToolBar(Qt::TopToolBarArea, m_toolBar0);

    //-----------------------------------------------------------
    // toolbar 0
    m_toolBar0->addAction(m_action__exit);

    //-----------------------------------------------------------
    // toolbar 1
    m_action_sep_first = new QAction(QString(""), this);
    m_action_sep_first->setEnabled(false);
    m_toolBar->addAction(m_action_sep_first);
    m_toolBar->addAction(m_action__bto);

    m_action_sep_second = new QAction(QString(""), this);
    m_action_sep_second->setEnabled(false);
    m_toolBar->addAction(m_action_sep_second);
    m_toolBar->addAction(m_action__diagn);

    auto sep = new QAction(QString(""), this);
    sep->setEnabled(false);
    m_toolBar->addAction(sep);
    m_toolBar->addAction(m_action__firmware);

    sep = new QAction(QString(""), this);
    sep->setEnabled(false);
    m_toolBar->addAction(sep);
    m_toolBar->addAction(m_action__plato);

    sep = new QAction(QString(""), this);
    sep->setEnabled(false);
    m_toolBar->addAction(sep);
    m_toolBar->addAction(m_action__api);
}

//-----------------------------------------------------------
QToolBar *MW::toolBar()
{
    return m_toolBar;
}
