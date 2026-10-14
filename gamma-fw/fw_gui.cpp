/*!
  \file   fw_gui.cpp
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Отображение информации во вкладке FIRMWARE
*/

#include <string>
#include <assert.h>

#include <QtDebug>
#include <QString>

#include "bto.h"
#include "fw.h"
#include "fw_gui.h"
#include "main.h"
#include "mainwindow.h"

//---------------------------------------------------------
FIRMWARE_GUI::FIRMWARE_GUI(APP *app) :
    m_app(app),
    m_fw(nullptr),
    m_ico_gray(QIcon(":q_circle_gray.png")),
    m_ico_red(QIcon(":q_circle_red.png")),
    m_ico_green(QIcon(":q_circle_green.png")),
    m_ico_yellow(QIcon(":q_circle_yellow.png"))
{
    m_gui_page = create_fw_page();
}

//---------------------------------------------------------
FIRMWARE_GUI::~FIRMWARE_GUI()
{
    DBG(4);
}

//---------------------------------------------------------
void FIRMWARE_GUI::set_backgr(FIRMWARE *fw)
{
    m_fw = fw;
}

//---------------------------------------------------------
void FIRMWARE_GUI::set_ACK_field_cmd(const QString &text, const QString &tip)
{
    m_fld_fw_ACK_cmd->setText(text);
    m_fld_fw_ACK_cmd->setToolTip(tip);
    m_fld_fw_ACK_cmd->setCursorPosition(0);
}

//---------------------------------------------------------
void FIRMWARE_GUI::set_ACK_field_code(const QString &text, const QString &tip)
{
    m_fld_fw_ACK_code->setText(text);
    m_fld_fw_ACK_code->setToolTip(tip);
    m_fld_fw_ACK_code->setCursorPosition(0);
}

//---------------------------------------------------------
void FIRMWARE_GUI::set_ACK_field_status(const QString &text, const QString &tip)
{
    m_fld_fw_ACK_status->setText(text);
    m_fld_fw_ACK_status->setToolTip(tip);
    m_fld_fw_ACK_status->setCursorPosition(0);
}

//---------------------------------------------------------
void FIRMWARE_GUI::set_ACK_field_text(const QString &text, const QString &tip)
{
    m_fld_fw_ACK_text->setText(text);
    m_fld_fw_ACK_text->setToolTip(tip);
    m_fld_fw_ACK_text->setCursorPosition(0);
}

//---------------------------------------------------------
void FIRMWARE_GUI::display_error_message(const QString &msg)
{
    showstatus(msg, 1);
    emit mw()->signal_message(QMessageBox::Critical, msg);
}

//---------------------------------------------------------
void FIRMWARE_GUI::showstatus(const QString &msg, int sec) const
{
    emit mw()->signal_showstatus(msg, sec);
}

//---------------------------------------------------------
void FIRMWARE_GUI::slot_fw_hv_oper()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (btn) showstatus(btn->text(), 5);
    if (!wish_write_data()) {
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
        comm()->connect_to();
    }
    if (!comm()->isConnected()) {
        QApplication::restoreOverrideCursor();
        auto msg = tr("No connection");
        showstatus(msg);
        emit mw()->signal_message(QMessageBox::Warning, msg);
        return;
    }

    ushort num = m_fld_fw_hv_opr->text().toUInt();
    DBGF(1, "%u", num);
    comm()->push_requests({ NodeE4::WriteOperationVoltage(num),
                            NodeE4::ReadOperationVoltage(),
                          });
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------
void FIRMWARE_GUI::slot_fw_set_hvps()
{
    QPushButton *btn = qobject_cast<QPushButton*>( sender() );
    if (btn) showstatus(btn->text(), 5);
    if (!wish_write_data()) {
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
        comm()->connect_to();
    }
    if (!comm()->isConnected()) {
        QApplication::restoreOverrideCursor();
        auto msg = tr("No connection");
        showstatus(msg);
        emit mw()->signal_message(QMessageBox::Warning, msg);
        return;
    }

    ushort value_min = m_fld_fw_hv_min->text().toUShort();
    ushort value_max = m_fld_fw_hv_max->text().toUShort();
    ushort value_tst = m_fld_fw_hv_tst->text().toUShort();
    DBGF(1, "HV min:%u test:%u max:%u", value_min, value_tst, value_max);
    comm()->push_requests({ NodeE4::WriteCalibrationVoltages(value_min, value_max, value_tst),
                            //NodeE4::ReadCalibrationVoltages(),
                          });
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------
void FIRMWARE_GUI::slot_start_fw_read()
{
    DBG(3);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_button_fw_read->setDisabled(true);
    mw()->lock_pages();
    if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
        comm()->connect_to();
    }
    if (!comm()->isConnected()) {
        QApplication::restoreOverrideCursor();
        m_button_fw_read->setEnabled(true);
        m_button_fw_read->setChecked(false);
        mw()->unlock_pages();

        auto msg = tr("No connection");
        showstatus(msg);
        emit mw()->signal_message(QMessageBox::Warning, msg);
        return;
    }
    if (!m_app->bto()->gamma_module_found()) {
        QApplication::restoreOverrideCursor();
        m_button_fw_read->setEnabled(true);
        m_button_fw_read->setChecked(false);
        mw()->unlock_pages();

        auto msg = tr("Module is not found");
        showstatus(msg);
        emit mw()->signal_message(QMessageBox::Warning, msg);
        return;
    }

    m_button_fw_read->setChecked(true);
    clear_fw_fields();
    m_fw->start_read_firmware();
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------
void FIRMWARE_GUI::firmware_read_finished()
{
    DBG(3);
    comm()->clear_requests();
    m_button_fw_read->setChecked(false);
    m_button_fw_read->setEnabled(true);
    mw()->unlock_pages();
    if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
        comm()->disconnect_from();
    }
}

//---------------------------------------------------------
void FIRMWARE_GUI::requests_completed(int nodecode)
{
    (void)nodecode;
    if (mw()->current_tab() != MW::PAGES::FIRMWARE_PAGE) {
        return;
    }

    DBGF(3, "node: %02X", nodecode);
    showstatus({});
    firmware_read_finished();
}

