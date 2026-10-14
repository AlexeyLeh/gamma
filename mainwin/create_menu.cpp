/*!
 * \file   create_menu.cpp
 * \author Aleksey Lekhtin
 * \date   created 17.11.2014
 * \brief  Создание меню
 */

#include <QMenuBar>

#include "main.h"
#include "mainwindow.h"

void MW::create_menu()
{
   QMenuBar *menu_bar = new QMenuBar(nullptr);
   setMenuBar(menu_bar);
   m_fileMenu = menuBar()->addMenu(tr("&File"));
   m_fileMenu->addAction(m_action__bto);
   m_fileMenu->addAction(m_action__diagn);
   m_fileMenu->addAction(m_action__firmware);
   m_fileMenu->addAction(m_action__plato);
   m_fileMenu->addAction(m_action__api);

   m_fileMenu->addSeparator();
   if(!app()->simulation()) {
       m_fileMenu->addAction(m_action__save_report);
       m_fileMenu->addAction(m_action__load_report);
       m_fileMenu->addAction(m_action__load_last);
   } else {
       m_fileMenu->addAction(m_action__load_sim);
       m_fileMenu->addAction(m_action__save_sim);
   }

   m_fileMenu->addSeparator();
   m_fileMenu->addAction(m_action__exit);

   menuBar()->addSeparator();
   m_settingsMenu = menuBar()->addMenu(tr("&Settings"));

   m_settingsMenu->addSeparator()->setText(tr("Communications"));
   m_settingsMenu->addActions(m_action__select_group->actions());

   m_settingsMenu->addSeparator();
   m_settingsMenu->addAction(m_action__setup_bto_host);
   m_settingsMenu->addAction(m_action__setup_com);

   m_settingsMenu->addSeparator();
   m_settingsMenu->addAction(m_action__setup_SN);
   m_settingsMenu->addAction(m_action__setup_oper);

   m_settingsMenu->addSeparator();
   ////m_settingsMenu->addAction(m_action__setup_module_autopower);

   m_settingsMenu->addSeparator();
   m_settingsMenu->addAction(m_action__fonts_size);

   menuBar()->addSeparator();
   m_helpMenu = menuBar()->addMenu(tr("&Help"));
   m_helpMenu->addAction(m_action__about);
}
