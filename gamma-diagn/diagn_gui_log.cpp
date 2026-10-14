/*!
  \file   diagnostic_log.cpp
  \brief  Протоколирование данных терминала
  \author Aleksey Lekhtin
  \date   16.05.2017
*/

#include "diagn.h"
#include "diagn_gui.h"
#include "mainwindow.h"
#include "NodeE4.h"
#include "gamma.h"

// Выбрать каталог для сохранения отчета терминала
void DIAGN_GUI::slot_select_terminal_log_path()
{
    QString fname = m_fld_diag_term_fname_log->text();
    if (fname.isEmpty())
    {
        QString pn = m_diagn->nodeE4()->partnumber();
        if (pn.isEmpty()) pn = GAMMA::SN_00;
        fname = QString("%1.log").arg(pn);
    }

    auto fn = QFileDialog::getSaveFileName(mw(),
                                           tr("Save File"),
                                           fname,
                                           QStringLiteral("Log (*.log)"));
    m_fld_diag_term_fname_log->setText(QDir::toNativeSeparators(fn));

    QString str = m_fld_diag_term_fname_log->text();
    m_fld_diag_term_fname_log->setToolTip(QDir::toNativeSeparators(str));
    QFileInfo fi(str);
    m_fld_diag_term_fname_log->setToolTip(QDir::toNativeSeparators(fi.filePath()));
}

// Разрешить протоколирование терминала
void DIAGN_GUI::slot_set_term_log_enabled(bool status)
{
    m_fld_diag_term_fname_log->setEnabled(status);
    m_term_select_log_button->setEnabled(status);
}

// Запись строки в пртокол терминала
void DIAGN_GUI::write_term_protocol_record(const QString& str, int width)
{
    if (!m_chkbox_term_log_enabled->isChecked()) return;
    if (!m_term_log_fd.isOpen()) return;
    if (str.isEmpty()) return;
    QString msg = QString("%1").arg(str, width);
    if (m_term_log_fd.write(msg.toUtf8()) != -1)
    {
        m_term_log_fd.flush();
        return; // ok
    }

    int error = m_term_log_fd.error();
    QString error_string = m_term_log_fd.errorString();
    msg = QString("%1\n\n(%2) %3")
          .arg(tr("Can't write to log file"))
          .arg(error)
          .arg(error > 0 ? error_string : "");
    qWarning() << msg;
    m_term_log_fd.write(msg.toUtf8());
}

bool DIAGN_GUI::open_term_protocol()
{
    if (m_term_log_fd.isOpen()) m_term_log_fd.close();
    QString fname = m_fld_diag_term_fname_log->text();
    if (!m_chkbox_term_log_enabled->isChecked())
        return true;

    if (fname.isEmpty()) return true;
    m_term_log_fd.setFileName(fname);
    bool ok = m_term_log_fd.open(QIODevice::Append);
    if (!ok)
    {
        int err = m_term_log_fd.error();
        QString err_string = m_term_log_fd.errorString();

        QString msg = QString("%1%2\n%3\n\n(%4) %5")
                      .arg(tr("Can't create log file"))
                      .arg(QString(40,' '))
                      .arg(m_term_log_fd.fileName())
                      .arg(err)
                      .arg(err > 0 ? err_string : "");
        QMessageBox::critical(mw(), __FUNCTION__, msg, QMessageBox::Close);
        m_chkbox_term_log_enabled->setChecked(false);
        return false;
    }

    //-----------------------------------------------------
    QString fmt = QStringLiteral("dd.MM.yy hh:mm:ss");
    QString msg = QString(QStringLiteral("\n#%1\n# %2\n# %3\n# "))
                  .arg(QString(40, '-'))
                  .arg(fname)
                  .arg(QDateTime::currentDateTime().toString(fmt));
    write_term_protocol_record(msg);

    byte ext = 0;
    int  ww = -12;
    msg = "";

    for (const auto& req : m_diagn->requests())
    {
        if (req.cmd() == 0xFC && m_fld_diag_Partnumber_log->isChecked())
            msg += QString("%1").arg(tr("PN"), ww);
        // else if (req.cmd() == 0xFB && m_fld_diag_status_log->isChecked())
        //     msg += QString("%1").arg(tr("Status"), ww);
        else if (req.cmd() == 0xF8 && m_fld_diag_rectask_log->isChecked())
            msg += QString("%1").arg(tr("Task"), ww);
        else if (req.cmd() == 0x1F && m_fld_diag_config_reg_log->isChecked())
            msg += QString("%1").arg(tr("Config Reg"), ww);
        else if (req.cmd() == 0x05 && m_fld_diag_Temperature_log->isChecked())
            msg += QString("%1").arg(tr("Temp,C"), ww);
        else if (req.cmd() == 0x42 && m_fld_diag_HV_max_log->isChecked())
            msg += QString("%1").arg(tr("HV Min/Max/Test"), -18);
        else if (req.cmd() == 0x44 && m_fld_diag_HV_corr_log->isChecked())
            msg += QString("%1").arg(tr("HV Corr"), ww);
        else if (req.cmd() == 0x46 && m_fld_diag_HV_warm_log->isChecked())
            msg += QString("%1").arg(tr("HV WarmUp"), ww);
        else if (req.cmd() == 0x43 && m_fld_diag_HV_log->isChecked())
            msg += QString("%1").arg(tr("HV,V"), ww);
        else if (req.cmd() == 0x13 && m_fld_diag_gamma_AcqTime_log->isChecked())
            msg += QString("%1").arg(tr("Gamma-Time,sec"), ww);
        else if (req.cmd() == 0x01 &&  m_fld_diag_gamma_log->isChecked())
        {
            msg += QString("%1").arg(tr("Gamma-Avrg"), ww);
            msg += QString("%1").arg(tr("Gamma-CPS"), ww);
        }
        else if (req.cmd() == 0x06 &&  m_fld_diag_accel_raw_log->isChecked())
        {
            auto bytes = req.payload();
            if (bytes.size() > 0) ext = bytes[0];
            QString str = QString("%1-%2").arg(tr("Accel Raw")).arg(ext);
            msg += QString("%1").arg(str, ww);
        }
        else if (req.cmd() == 0x18 &&  m_fld_diag_accel_corr_log->isChecked())
        {
            auto bytes = req.payload();
            if (bytes.size() > 0) ext = bytes[0];
            QString str = QString("%1-%2").arg(tr("Accel Corr")).arg(ext);
            msg += QString("%1").arg(str, ww);
        }
        else if (req.cmd() == 0x11 &&  m_fld_diag_accel_incl_log->isChecked())
        {
            auto bytes = req.payload();
            if (bytes.size() > 0) ext = bytes[0];
            QString str = QString("%1-%2").arg(tr("Accel Incl")).arg(ext);
            msg += QString("%1").arg(str, ww);
        }
        else
        {
            if (req.cmd() == 0x1E)
            {
                auto bytes = req.payload();
                if (bytes.size() > 0)
                {
                    int ext = bytes[0];
                    if (ext >= 0 && ext <= 7)
                    {
                        if (m_fld_diag_ADC_log[ext]->isChecked())
                        {
                            //// QString bf = QString("%1[%2]").arg(tr("ADC")).arg(ext);
                            QString bf = QString("%1-%2").arg(tr("ADC"), NodeE4::ADC_name_short(ext));
                            msg += QString("%1").arg(bf, ww);
                        }
                    }
                }
            }
        }
    }

    write_term_protocol_record(msg);
    write_term_protocol_record("\n#\n  ");

    return true;
}

void DIAGN_GUI::close_term_protocol()
{
    if (m_term_log_fd.isOpen()) {
        write_term_protocol_record("\n");
        m_term_log_fd.close();
    }
}
