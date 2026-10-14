/*!
  \file   create_plato_table.cpp
  \author Aleksey Lekhtin
  \date   21.11.2016
  \brief  Формирование таблиц
*/

#include "mainwindow.h"
#include "plato_gui.h"

//-----------------------------------------------------------
void PLATO_GUI::create_table()
{
    m_table = new TABLE();
    QStringList labels;
    labels << tr("№")
           << tr("HV\n(cold)" )
           << tr("CPS" )
           << tr("Temp" )
           << tr("Slope")
           << tr("HV\n(hot)" )
           << tr("CPS" )
           << tr("Temp" )
           << tr("Slope");
    m_table->set_labels(labels);

    m_table->horizontalHeader()->setSectionsClickable(false);
    m_table->horizontalHeader()->setSortIndicatorShown(false);
    m_table->horizontalHeader()->setStretchLastSection(false);
    m_table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

    QPalette pal(m_table->horizontalHeader()->palette());
    pal.setColor(QPalette::Button, QColor(192,192,192));
    m_table->horizontalHeader()->setPalette(pal);

    QFont fnt = m_table->horizontalHeader()->font();
    fnt.setBold(true);
    m_table->horizontalHeader()->setFont(fnt);
}
