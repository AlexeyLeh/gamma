/*!
  \file   fw_settings.cpp
  \author Aleksey Lekhtin
  \date   20.10.2016
  \brief  Сохранение настроек прошивки гамма-модуля
*/


#include "dbg.h"
#include "fw.h"
#include "fw_gui.h"
#include "mainwindow.h"

void FIRMWARE::read_settings(QSettings *st)
{
    DBG(4);
    st->beginGroup("Firmware");
    // TODO - чтение настроек для Firmware (их пока нет)
    st->endGroup();
}

void FIRMWARE::save_settings(QSettings *st)
{
    DBG(4);
    st->beginGroup("Firmware");
    // TODO - запись настроек для Firmware (их пока нет)
    st->endGroup();
}

void FIRMWARE_GUI::set_content_font_size(int sz)
{
    DBG(4);

    std::list<QPushButton*> buttons {
        m_button_fw_read,
    };

    for (auto but : buttons)
    {
        auto style = QString("QPushButton { font: %1pt; }").arg(sz);
        but->setStyleSheet(style);
    }
}
