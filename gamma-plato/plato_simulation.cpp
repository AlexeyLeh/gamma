/*!
  \file   plato_simulation.cpp
  \author Aleksey Lekhtin
  \date   02.10.2026
  \brief  Симуляция теста линейности гамма-модуля
*/

#include "bto.h"
#include "gamma.h"
#include "mainwindow.h"
#include "plato.h"
#include "plato_gui.h"
#include "utils.h"
#include "wmsg.h"

#define SPC  QStringLiteral(" ")
#define SEP  QStringLiteral("|")
#define COL  QStringLiteral(":")
#define DASH QStringLiteral("-")
#define NL   QStringLiteral("\n")

constexpr const char *label_date        = "Calibration Date";
constexpr const char *label_PN          = "Tool P/N";
constexpr const char *label_SN          = "Tool S/N";
constexpr const char *label_operator    = "Operator";
constexpr const char *label_temp        = "Tool Temperature";
constexpr const char *label_hv          = "Operating HV";

//---------------------------------------------------------
void PLATO_GUI::simulation_save()
{
    const auto &data = m_plato->data();
    if (data.date.isNull() || !data.date.isValid() || data.empty()) {
        bool ok = WMSG::confirmation(
                      app(),
                      app()->title(),
                      QString("%1\n\n%2")
                      .arg(tr("No measurements has been done yet.\n"
                              "A predefined data set will be used."))
                      .arg(tr("Are you sure you wish to continue?")));
        if (!ok) {
            return;
        }
        m_plato->init_simulation_data(m_plato->data());
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QFileInfo out;
    int repeat = 0;
    for(;;) {
        QString filename = QString("%1/PLATO-SIM-%2-%3%4.%5")
                           .arg(m_settings_last_simulation_loaded_dir)
                           .arg(data.toolsn == GAMMA::SN_00 ? data.toolpn : data.toolsn)
                           .arg(data.date.toString("yyyyMMdd-hhmm"))
                           .arg(repeat == 0 ? QStringLiteral() : QString(" (%1)").arg(repeat))
                           .arg(PLATO_GUI::simulation_ext);
        auto fn = QFileDialog::getSaveFileName(
                      mw(),
                      tr("Save File"),
                      filename,
                      QString("%1 (*.%2)").arg(tr("Simulation")).arg(PLATO_GUI::simulation_ext));
        if (fn.isEmpty()) {
            QApplication::restoreOverrideCursor();
            return;
        }

        out.setFile(fn);
        if (!QFileInfo::exists(out.absoluteFilePath())) {
            break;
        }
        ++repeat;
    }

    m_settings_last_simulation_loaded_dir = out.absolutePath();
    DBG(3, QDir::toNativeSeparators(out.absoluteFilePath()));
    QApplication::restoreOverrideCursor();

    auto errstr = m_plato->simulation_save(data, out);
    if (!errstr.isEmpty())
    {
        QApplication::restoreOverrideCursor();
        DBG(1, errstr);
        display_error_message(errstr);
        return;
    }
    QApplication::restoreOverrideCursor();

    QString msg = tr("The simulation has been saved");
    emit mw()->signal_showstatus(QString("%1 - %2").arg(msg, QDir::toNativeSeparators(out.filePath())));
    auto s1 = UTILS::split_path(out.absolutePath());
    auto s2 = out.fileName();
    display_info_message(QMessageBox::Information, QString("%1\n\n%2\n%3").arg(msg, s1, s2), out.absoluteFilePath());
}

//---------------------------------------------------------
bool PLATO_GUI::simulation_load()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    auto fullname = QFileDialog::getOpenFileName(
                        mw(),
                        tr("Load Simulation"),
                        m_settings_last_simulation_loaded_dir,
                        QString("%1 (*.%2)").arg(tr("Simulation")).arg(PLATO_GUI::simulation_ext));
    DBG(3, QDir::toNativeSeparators(fullname));
    if (fullname.isEmpty()) {
        QApplication::restoreOverrideCursor();
        return false;
    }

    m_simulation_fileinfo.setFile(QDir::toNativeSeparators(fullname));
    m_settings_last_simulation_loaded_dir = m_simulation_fileinfo.absolutePath();

    auto errstr = m_plato->simulation_load(m_plato->data(), m_simulation_fileinfo);
    if (!errstr.isEmpty()) {
        DBG(1, errstr);
        m_simulation_file_label->clear();
        m_simulation_fileinfo = QFileInfo();
        display_error_message(errstr);
        return false;
    }
    m_simulation_file_label->setText(QDir::toNativeSeparators(m_simulation_fileinfo.fileName()));
    m_gui_page->update();
    bool ok = m_plato->set_simulation_data(m_plato->data());
    QApplication::restoreOverrideCursor();
    return ok;
}

//---------------------------------------------------------
void PLATO::init_simulation_data(calibration_data_t &data)
{
    auto temp1 = 29.3;
    auto temp2 = 88.2;
    auto dt = QDateTime::currentDateTime();
    data.date = dt;
    data.toolname = tr("GAMMA SONDE");
    data.toolpn = "77557ES000";
    data.toolsn = "SIM000000123";
    data.oper = app()->Operator();
    data.hv_accepted = 1425;

    auto warmup_period = 1;
    auto acq_one_gamma = (int)ceil(2.5 * (double)m_settings_gamma_acq_bucket);
    auto acq_sample_time = warmup_period + (acq_one_gamma * m_settings_samples_for_avrg);
    data.acqtime = acq_sample_time;

    int kind = 1; // app()->simulation()
    if (kind == 1)
    {
        data.cold = {
            {dt,  850,  31.40, temp1, 0.0000},
            {dt,  875,  47.00, temp1, 0.0000},
            {dt,  900,  71.20, temp1, 0.0000},
            {dt,  925,  86.40, temp1, 0.0000},
            {dt,  950, 107.60, temp1, 0.7672},
            {dt,  975, 146.80, temp1, 0.9440},
            {dt, 1000, 174.20, temp1, 1.0656},
            {dt, 1025, 214.80, temp1, 1.2936},
            {dt, 1050, 250.20, temp1, 1.4128},
            {dt, 1075, 270.60, temp1, 1.2944},
            {dt, 1100, 291.00, temp1, 1.1576},
            {dt, 1125, 309.20, temp1, 0.9184},
            {dt, 1150, 310.80, temp1, 0.6392},
            {dt, 1175, 311.00, temp1, 0.4024},
            {dt, 1200, 312.00, temp1, 0.1752},
            {dt, 1225, 317.20, temp1, 0.0688},
            {dt, 1250, 324.80, temp1, 0.1368},
            {dt, 1275, 326.20, temp1, 0.1728},
            {dt, 1300, 330.00, temp1, 0.1800},
            {dt, 1325, 330.80, temp1, 0.1296},
            {dt, 1350, 325.60, temp1, 0.0248},
            {dt, 1375, 330.80, temp1, 0.0192},
            {dt, 1400, 331.40, temp1, 0.0112},
            {dt, 1425, 307.80, temp1, 0.1608},
            {dt, 1450, 332.40, temp1, 0.0376},
            {dt, 1475, 315.40, temp1, 0.1192},
            {dt, 1500, 336.20, temp1, 0.0688},
            {dt, 1525, 317.20, temp1, 0.0904},
            {dt, 1550, 331.40, temp1, 0.0008},
            {dt, 1575, 338.80, temp1, 0.1680},
            {dt, 1600, 353.60, temp1, 0.2256},
            {dt, 1625, 333.60, temp1, 0.2200},
            {dt, 1650, 345.20, temp1, 0.0896},
            {dt, 1675, 345.60, temp1, 0.0208},
            {dt, 1700, 361.40, temp1, 0.1104},
            {dt, 1725, 363.40, temp1, 0.3032},
            {dt, 1750, 368.20, temp1, 0.2552},
        };
        data.hot = {
            {dt,  850,  17.80, temp2, 0.0000},
            {dt,  875,  22.80, temp2, 0.0000},
            {dt,  900,  35.40, temp2, 0.0000},
            {dt,  925,  49.60, temp2, 0.0000},
            {dt,  950,  59.80, temp2, 0.4432},
            {dt,  975,  72.60, temp2, 0.4960},
            {dt, 1000,  91.60, temp2, 0.5416},
            {dt, 1025, 144.20, temp2, 0.8840},
            {dt, 1050, 178.20, temp2, 1.2336},
            {dt, 1075, 210.60, temp2, 1.4504},
            {dt, 1100, 233.60, temp2, 1.4016},
            {dt, 1125, 260.00, temp2, 1.1480},
            {dt, 1150, 276.80, temp2, 0.9864},
            {dt, 1175, 294.80, temp2, 0.8464},
            {dt, 1200, 293.00, temp2, 0.6144},
            {dt, 1225, 313.60, temp2, 0.4936},
            {dt, 1250, 314.20, temp2, 0.3744},
            {dt, 1275, 324.80, temp2, 0.3248},
            {dt, 1300, 314.20, temp2, 0.2144},
            {dt, 1325, 315.00, temp2, 0.0112},
            {dt, 1350, 320.60, temp2, 0.0120},
            {dt, 1375, 331.20, temp2, 0.0768},
            {dt, 1400, 318.80, temp2, 0.1016},
            {dt, 1425, 315.20, temp2, 0.0056},
            {dt, 1450, 333.00, temp2, 0.0352},
            {dt, 1475, 326.40, temp2, 0.0184},
            {dt, 1500, 330.00, temp2, 0.1344},
            {dt, 1525, 331.20, temp2, 0.1160},
            {dt, 1550, 325.40, temp2, 0.0416},
            {dt, 1575, 338.80, temp2, 0.0808},
            {dt, 1600, 332.20, temp2, 0.0480},
            {dt, 1625, 352.00, temp2, 0.1936},
            {dt, 1650, 345.40, temp2, 0.2128},
            {dt, 1675, 349.20, temp2, 0.1360},
            {dt, 1700, 356.80, temp2, 0.1856},
            {dt, 1725, 380.80, temp2, 0.2760},
            {dt, 1750, 372.40, temp2, 0.3424},
        };
    }
    else if (kind == 2)
    {
        data.cold = {
            {dt,  850,  31.40, temp1, 0.0000 },
            {dt,  875,  19.52, temp1, 0.0000 },
            {dt,  900,  32.77, temp1, 0.0000 },
            {dt,  925,  45.17, temp1, 0.0000 },
            {dt,  950,  56.85, temp1, 0.0000 },
            {dt,  975,  69.55, temp1, 0.0000 },
            {dt, 1000,  86.68, temp1, 0.0000 },
            {dt, 1025, 109.75, temp1, 0.0000 },
            {dt, 1050, 136.58, temp1, 0.0000 },
            {dt, 1075, 169.38, temp1, 0.9983 },
            {dt, 1100, 207.78, temp1, 1.2073 },
            {dt, 1125, 248.10, temp1, 1.3916 },
            {dt, 1150, 280.98, temp1, 1.4701 },
            {dt, 1175, 316.03, temp1, 1.4660 },
            {dt, 1200, 334.30, temp1, 1.2839 },
            {dt, 1225, 353.85, temp1, 1.0593 },
            {dt, 1250, 364.92, temp1, 0.8227 },
            {dt, 1275, 371.75, temp1, 0.5682 },
            {dt, 1300, 382.00, temp1, 0.4532 },
            {dt, 1325, 378.55, temp1, 0.2659 },
            {dt, 1350, 381.78, temp1, 0.1621 },
            {dt, 1375, 381.55, temp1, 0.0775 },
            {dt, 1400, 386.87, temp1, 0.0509 },
            {dt, 1425, 391.68, temp1, 0.1254 },
            {dt, 1450, 392.65, temp1, 0.1275 },
            {dt, 1475, 396.57, temp1, 0.1433 },
            {dt, 1500, 396.38, temp1, 0.0957 },
            {dt, 1525, 395.72, temp1, 0.0472 },
            {dt, 1550, 396.68, temp1, 0.0289 },
            {dt, 1575, 400.08, temp1, 0.0293 },
            {dt, 1600, 396.38, temp1, 0.0175 },
            {dt, 1625, 401.40, temp1, 0.0443 },
            {dt, 1650, 405.05, temp1, 0.0722 },
            {dt, 1675, 407.85, temp1, 0.0968 },
            {dt, 1700, 405.43, temp1, 0.0982 },
            {dt, 1750, 405.43, temp1, 0.0982 },
        };
        data.hot = {
            /*
                        {dt,  850,  17.80, temp2, 0.00000 },
                        {dt,  875,  22.80, temp2, 0.00000 },
                        {dt,  900,  35.40, temp2, 0.00000 },
                        {dt,  925,  49.60, temp2, 0.00000 },
                        {dt,  950,  59.80, temp2, 0.03734 },
                        {dt,  975,  72.60, temp2, 0.04804 },
                        {dt, 1000,  91.60, temp2, 0.05987 },
                        {dt, 1025, 144.20, temp2, 0.07310 },
                        {dt, 1050, 178.20, temp2, 0.09719 },
                        {dt, 1075, 210.60, temp2, 0.12530 },
                        {dt, 1100, 233.60, temp2, 0.15562 },
                        {dt, 1125, 260.00, temp2, 0.18390 },
                        {dt, 1150, 276.80, temp2, 0.20572 },
                        {dt, 1175, 294.80, temp2, 0.22259 },
                        {dt, 1200, 293.00, temp2, 0.23582 },
                        {dt, 1225, 313.60, temp2, 0.24250 },
                        {dt, 1250, 314.20, temp2, 0.24856 },
                        {dt, 1275, 324.80, temp2, 0.25081 },
                        {dt, 1300, 314.20, temp2, 0.25197 },
                        {dt, 1325, 315.00, temp2, 0.25036 },
                        {dt, 1350, 320.60, temp2, 0.24560 },
                        {dt, 1375, 331.20, temp2, 0.24212 },
                        {dt, 1400, 318.80, temp2, 0.23941 },
                        {dt, 1425, 315.20, temp2, 0.23558 },
                        {dt, 1450, 333.00, temp2, 0.23097 },
                        {dt, 1475, 326.40, temp2, 0.22934 },
                        {dt, 1500, 330.00, temp2, 0.22482 },
                        {dt, 1525, 331.20, temp2, 0.22288 },
                        {dt, 1550, 325.40, temp2, 0.22155 },
                        {dt, 1575, 338.80, temp2, 0.21664 },
                        {dt, 1600, 332.20, temp2, 0.21529 },
                        {dt, 1625, 352.00, temp2, 0.21218 },
                        {dt, 1650, 345.40, temp2, 0.21247 },
                        {dt, 1675, 349.20, temp2, 0.21207 },
                        {dt, 1700, 356.80, temp2, 0.21046 },
                        */
        };
    }
    else if (kind == 3)
    {
        temp1 = 29.7;
        temp2 = 117.9;
        data.cold = {
            {dt,  850,  36.94, temp1, 0.0000 },
            {dt,  875,  43.20, temp1, 0.0000 },
            {dt,  900,  53.90, temp1, 0.0000 },
            {dt,  925,  71.60, temp1, 0.0000 },
            {dt,  950,  95.06, temp1, 0.5786 },
            {dt,  975, 127.20, temp1, 0.8366 },
            {dt, 1000, 149.66, temp1, 0.9885 },
            {dt, 1025, 181.60, temp1, 1.0984 },
            {dt, 1050, 204.76, temp1, 1.0952 },
            {dt, 1075, 210.04, temp1, 0.8831 },
            {dt, 1100, 225.80, temp1, 0.7229 },
            {dt, 1125, 224.54, temp1, 0.4277 },
            {dt, 1150, 234.64, temp1, 0.2970 },
            {dt, 1175, 230.04, temp1, 0.1954 },
            {dt, 1200, 238.70, temp1, 0.1252 },
            {dt, 1225, 237.00, temp1, 0.1159 },
            {dt, 1250, 233.02, temp1, 0.0149 },
            {dt, 1275, 236.06, temp1, 0.0254 },
            {dt, 1300, 252.90, temp1, 0.1098 },
            {dt, 1325, 234.22, temp1, 0.0573 },
            {dt, 1350, 239.16, temp1, 0.0418 },
            {dt, 1375, 241.50, temp1, 0.0114 },
            {dt, 1400, 242.68, temp1, 0.0526 },
            {dt, 1425, 247.00, temp1, 0.1163 },
            {dt, 1450, 240.60, temp1, 0.0335 },
            {dt, 1475, 246.88, temp1, 0.0347 },
            {dt, 1500, 246.30, temp1, 0.0285 },
            {dt, 1525, 254.94, temp1, 0.0863 },
            {dt, 1550, 264.80, temp1, 0.2258 },
            {dt, 1575, 268.98, temp1, 0.2508 },
            {dt, 1600, 276.90, temp1, 0.3010 },
            {dt, 1625, 280.80, temp1, 0.2553 },
            {dt, 1650, 295.78, temp1, 0.2951 },
            {dt, 1675, 292.98, temp1, 0.2675 },
            {dt, 1700, 307.40, temp1, 0.2927 },
            {dt, 1750, 307.50, temp1, 0.2928 },
        };
        data.hot = {
            {dt,  850,   9.00, temp2, 0.0000 },
            {dt,  875,  13.36, temp2, 0.0000 },
            {dt,  900,  24.00, temp2, 0.0000 },
            {dt,  925,  33.48, temp2, 0.0000 },
            {dt,  950,  46.72, temp2, 0.3822 },
            {dt,  975,  57.52, temp2, 0.4442 },
            {dt, 1000,  75.68, temp2, 0.5096 },
            {dt, 1025, 100.40, temp2, 0.6512 },
            {dt, 1050, 129.26, temp2, 0.8318 },
            {dt, 1075, 154.08, temp2, 0.9868 },
            {dt, 1100, 183.10, temp2, 1.0741 },
            {dt, 1125, 194.52, temp2, 0.9683 },
            {dt, 1150, 213.16, temp2, 0.8330 },
            {dt, 1175, 224.66, temp2, 0.6849 },
            {dt, 1200, 220.94, temp2, 0.4233 },
            {dt, 1225, 222.30, temp2, 0.2534 },
            {dt, 1250, 219.40, temp2, 0.0405 },
            {dt, 1275, 225.08, temp2, 0.0028 },
            {dt, 1300, 227.60, temp2, 0.0644 },
            {dt, 1325, 240.58, temp2, 0.1790 },
            {dt, 1350, 237.48, temp2, 0.2066 },
            {dt, 1375, 235.12, temp2, 0.1198 },
            {dt, 1400, 238.20, temp2, 0.0630 },
            {dt, 1425, 235.80, temp2, 0.0354 },
            {dt, 1450, 243.54, temp2, 0.0512 },
            {dt, 1475, 245.28, temp2, 0.1026 },
            {dt, 1500, 247.90, temp2, 0.1155 },
            {dt, 1525, 249.20, temp2, 0.1246 },
            {dt, 1550, 254.84, temp2, 0.1061 },
            {dt, 1575, 263.34, temp2, 0.1722 },
            {dt, 1600, 264.02, temp2, 0.1855 },
            {dt, 1625, 257.50, temp2, 0.1031 },
            {dt, 1650, 278.92, temp2, 0.1693 },
            {dt, 1675, 291.98, temp2, 0.2887 },
            {dt, 1700, 301.10, temp2, 0.4346 },
            {dt, 1750, 301.20, temp2, 0.4346 },
        };
    }
    else if (kind == 4)
    {
        data.cold = {
            {dt,  850,  75.75, temp1, 0.0000 },
            {dt,  875,  59.55, temp1, 0.0000 },
            {dt,  900,  82.35, temp1, 0.0000 },
            {dt,  925,  92.70, temp1, 0.0000 },
            {dt,  950,  97.80, temp1, 0.3090 },
            {dt,  975, 109.35, temp1, 0.4602 },
            {dt, 1000, 126.90, temp1, 0.4230 },
            {dt, 1025, 132.00, temp1, 0.4308 },
            {dt, 1050, 132.60, temp1, 0.3690 },
            {dt, 1075, 131.55, temp1, 0.2004 },
            {dt, 1100, 138.90, temp1, 0.0942 },
            {dt, 1125, 136.20, temp1, 0.0588 },
            {dt, 1150, 145.50, temp1, 0.1218 },
            {dt, 1175, 144.45, temp1, 0.1296 },
            {dt, 1200, 134.10, temp1, 0.0054 },
            {dt, 1225, 147.90, temp1, 0.0480 },
            {dt, 1250, 149.40, temp1, 0.0450 },
            {dt, 1275, 143.10, temp1, 0.0504 },
            {dt, 1300, 147.90, temp1, 0.0912 },
            {dt, 1325, 145.80, temp1, 0.0228 },
            {dt, 1350, 143.70, temp1, 0.0348 },
            {dt, 1375, 148.35, temp1, 0.0252 },
            {dt, 1400, 154.20, temp1, 0.0606 },
            {dt, 1425, 178.65, temp1, 0.3048 },
            {dt, 1450, 179.40, temp1, 0.4068 },
            {dt, 1475, 173.40, temp1, 0.3012 },
            {dt, 1500, 177.75, temp1, 0.1674 },
            {dt, 1525, 172.65, temp1, 0.0546 },
            {dt, 1550, 174.90, temp1, 0.0390 },
            {dt, 1575, 175.20, temp1, 0.0030 },
            {dt, 1600, 196.80, temp1, 0.1626 },
            {dt, 1625, 187.50, temp1, 0.2064 },
            {dt, 1650, 191.10, temp1, 0.1788 },
            {dt, 1675, 196.80, temp1, 0.1500 },
            {dt, 1700, 192.60, temp1, 0.0036 },
            {dt, 1725, 204.60, temp1, 0.1428 },
            {dt, 1750, 194.10, temp1, 0.0552 },
        };
    }
    else if (kind == 5)
    {
        data.hot = {
            {dt,  850,  75.75, temp1, 0.0000 },
            {dt,  875,  59.55, temp1, 0.0000 },
            {dt,  900,  82.35, temp1, 0.0000 },
            {dt,  925,  92.70, temp1, 0.0000 },
            {dt,  950,  97.80, temp1, 0.3090 },
            {dt,  975, 109.35, temp1, 0.4602 },
            {dt, 1000, 126.90, temp1, 0.4230 },
            {dt, 1025, 132.00, temp1, 0.4308 },
            {dt, 1050, 132.60, temp1, 0.3690 },
            {dt, 1075, 131.55, temp1, 0.2004 },
            {dt, 1100, 138.90, temp1, 0.0942 },
            {dt, 1125, 136.20, temp1, 0.0588 },
            {dt, 1150, 145.50, temp1, 0.1218 },
            {dt, 1175, 144.45, temp1, 0.1296 },
            {dt, 1200, 134.10, temp1, 0.0054 },
            {dt, 1225, 147.90, temp1, 0.0480 },
            {dt, 1250, 149.40, temp1, 0.0450 },
            {dt, 1275, 143.10, temp1, 0.0504 },
            {dt, 1300, 147.90, temp1, 0.0912 },
            {dt, 1325, 145.80, temp1, 0.0228 },
            {dt, 1350, 143.70, temp1, 0.0348 },
            {dt, 1375, 148.35, temp1, 0.0252 },
            {dt, 1400, 154.20, temp1, 0.0606 },
            {dt, 1425, 178.65, temp1, 0.3048 },
            {dt, 1450, 179.40, temp1, 0.4068 },
            {dt, 1475, 173.40, temp1, 0.3012 },
            {dt, 1500, 177.75, temp1, 0.1674 },
            {dt, 1525, 172.65, temp1, 0.0546 },
            {dt, 1550, 174.90, temp1, 0.0390 },
            {dt, 1575, 175.20, temp1, 0.0030 },
            {dt, 1600, 196.80, temp1, 0.1626 },
            {dt, 1625, 187.50, temp1, 0.2064 },
            {dt, 1650, 191.10, temp1, 0.1788 },
            {dt, 1675, 196.80, temp1, 0.1500 },
            {dt, 1700, 192.60, temp1, 0.0036 },
            {dt, 1725, 204.60, temp1, 0.1428 },
            {dt, 1750, 194.10, temp1, 0.0552 },
        };
    }
}


//---------------------------------------------------------
/**
 * @brief Запись файла симуляции
 * @param[out] out Имя созданного файла симуляции
 * @return строка с ошибкой либо пустая строка
 */
QString PLATO::simulation_save(const calibration_data_t &data, QFileInfo &out)
{
    if (data.date.isNull() || !data.date.isValid() || data.empty()) {
        return QString("505 Internal error\n\n").arg("empty simulation");
    }
    if (out.fileName().isEmpty()) {
        return tr("file not specified");
    }

    out.setCaching(false);
    DBG(1, out.absoluteFilePath());
    QDir dir = out.absoluteDir();
    gui()->m_settings_last_simulation_loaded_dir = QDir::toNativeSeparators(dir.absolutePath());

    auto ext = QString(PLATO_GUI::simulation_ext);
    auto sufx = out.suffix();
    if (sufx != ext) {
        auto new_fname = out.absoluteFilePath().replace(sufx, ext);
        out.setFile(new_fname);
    }

    //---------------------------------
    int ww1 = 22;
    QString text_header;
    text_header += UTILS::str_left(label_date     + COL, ww1) + data.date.toString("dd.MM.yyyy hh:mm") + NL;
    text_header += UTILS::str_left(label_PN       + COL, ww1) + data.toolpn + NL;
    text_header += UTILS::str_left(label_SN       + COL, ww1) + data.toolsn + NL;
    text_header += UTILS::str_left(label_operator + COL, ww1) + data.oper + NL;
    text_header += UTILS::str_left(label_hv       + COL, ww1) + QString::number(data.hv_accepted) + NL;
    text_header += UTILS::str_left(label_temp     + COL, ww1) +
                    QString::number(data.cold[0].m_temper, 'f', 1) + " / " +
                    QString::number(data.hot[0].m_temper, 'f', 1) + NL;

    //---------------------------------
    QString text_page;
    auto cold = data.cold;
    auto hot  = data.hot;
    int len = std::max(cold.size(), hot.size());
    cold.resize(len);
    hot.resize(len);

    text_page = "\n  #      HV    CPS COLD  CPS HOT\n";
    int ww = text_page.size();
    text_page += QString(ww, u'-') + NL;

    for (int idx = 0; idx < len; ++idx) {
        text_page += QStringLiteral("%1 %2 %3 %4\n")
                     .arg(UTILS::str_center(QString::number(idx+1), 6))
                     .arg(cold[idx].m_hv, 6)
                     .arg(cold[idx].m_cps, 8, 'f', 2, QChar(' '))
                     .arg(hot[idx].m_cps, 8, 'f', 2, QChar(' '));
    }

    text_page += QStringLiteral("\n%1 [%2]\n").arg("Gamma Sonde Calibration").arg(m_app->version_short());

    //---------------------------------
    auto txt = text_header + text_page;

    DBG(3, out.absoluteFilePath());
    QFile file(out.absoluteFilePath());
    bool ok = file.open(QIODevice::WriteOnly | QIODevice::ReadOnly);
    if (!ok)
    {
        auto err_string = file.errorString();
        auto errorstr = QString("%1\n%2\n\n%4")
                        .arg(tr("Can't create log file"), file.fileName())
                        .arg(err_string);
        return errorstr;
    }

    if (file.write(txt.toUtf8()) == -1)
    {
        int error = file.error();
        QString error_string = file.errorString();
        auto errorstr = QString("%1 - (%2) %3")
                        .arg(tr("write data error"))
                        .arg(error)
                        .arg(error > 0 ? error_string : "");
        file.write(errorstr.toUtf8()); // :(
        return errorstr;
    }
    file.close();
    return {};
}

//---------------------------------------------------------
QString PLATO::simulation_load(calibration_data_t &data, const QFileInfo &finfo)
{
    DBG(3);
    if (!finfo.exists())
    {
        auto errorstr = QString("%1\n\n%2").arg(tr("File not found"), QDir::toNativeSeparators(finfo.filePath()));
        auto str = errorstr;
        DBG(1, str.replace("\n", " "));
        return errorstr;
    }

    QFile file(finfo.absoluteFilePath());
    bool ok = file.open(QIODevice::ReadOnly);
    if (!ok)
    {
        auto err_string = file.errorString();
        auto errorstr = QString("%1\n%2\n\n%3")
                        .arg(tr("Can't read simulation file"),
                             QDir::toNativeSeparators(file.fileName()),
                             err_string);
        auto str = errorstr;
        DBG(1, str.replace("\n", " "));
        return errorstr;
    }

    auto bytes = file.readAll();
    if (file.error() > 0)
    {
        auto err_string = file.errorString();
        auto errorstr = QString("%1\n%2\n\n%3")
                        .arg(tr("Can't read simulation file"),
                             QDir::toNativeSeparators(file.fileName()),
                             err_string);
        file.close();
        auto str = errorstr;
        DBG(1, str.replace("\n", " "));
        return errorstr;
    }
    file.close();

    QString report(bytes);
    auto lines = report.replace("\r\n", "\n").split('\n', QString::SkipEmptyParts);

    // заголовок
    data = {};
    data.temper_cold_avrg = 29.0;
    data.temper_hot_avrg = 88.0;

    for(const auto &line : lines) {
        if (line.trimmed().startsWith("---")) {
            break;
        }

        QStringList fields = line.split(COL);
        if (fields.size() < 2) continue;

        auto field = fields[0].trimmed();
        auto str = fields[1].trimmed();
        for(int i = 2; i < fields.size(); ++i) {
            str += COL + fields[i].trimmed();
        }
        DBG(3, QString("%1: %2").arg(field, -25).arg(str));

        if (field.startsWith(label_PN)) {
            data.toolpn = str;
        } else if (field.startsWith(label_SN)) {
            data.toolsn = str;
        } else if (field.startsWith(label_date)) {
            data.date = QDateTime::fromString(str, "dd.MM.yyyy hh:mm");
        } else if (field.startsWith(label_operator)) {
            data.oper = str;
        } else if (field.startsWith(label_hv)) {
            data.hv_accepted = str.toInt();
        } else if (field.startsWith(label_temp)) {
            auto fields = str.trimmed().split('/', QString::SkipEmptyParts);
            if (fields.size() > 0) {
                data.temper_cold_avrg = fields[0].toFloat();
            }
            if (fields.size() > 1) {
                data.temper_hot_avrg = fields[1].toFloat();
            }
        }
    }

    // данные
    bool while_header = true;
    for(const auto &line : lines) {
        if (while_header) {
            if (line.trimmed().startsWith("---")) {
                while_header = false;
            }
            continue;
        }

        auto fields = line.trimmed().split(' ', QString::SkipEmptyParts);
        if (fields.size() < 4) {
            continue;
        }

        int num = fields[0].toInt(&ok);
        if (!ok || num == 0) {
            continue;
        }

        int hv;
        double cps1, cps2;

        hv = fields[1].toInt(&ok);
        if (ok) {
            cps1 = fields[2].toDouble(&ok);
        }
        if (ok) {
            cps2 = fields[3].toDouble(&ok);
        }
        if (ok) {
            data.cold.append({data.date, hv, cps1, data.temper_cold_avrg, {}});
            data.hot.append({data.date, hv, cps2, data.temper_hot_avrg, {}});
        }
    }

    if (DBG::debug_level >= 3) {
        DBGF(4, "Date:                     %s", qPrintable(data.date.toString()));
        DBGF(4, "Tool PN:                  %s", qPrintable(data.toolpn));
        DBGF(4, "Tool SN:                  %s", qPrintable(data.toolsn));
        DBGF(4, "Operator:                 %s", qPrintable(data.oper));

        DBG(4, "COLD: HV     CPS    SLOPE    TEMP");
        int idx = 1;
        for(const auto &d : data.cold) {
            DBGF(4, "%2d: %4d %8.2f %8.6f %3.1f", idx++, d.m_hv, d.m_cps, d.m_slope, d.m_temper);
        }

        DBG(4, "HOT:  HV     CPS    SLOPE    TEMP");
        idx = 1;
        for(const auto &d : data.hot) {
            DBGF(4, "%2d: %4d %8.2f %8.6f %3.1f",
                 idx++, d.m_hv, d.m_cps, d.m_slope, d.m_temper);
        }
    }

    DBG(4, "done");
    return {};
}

//---------------------------------------------------------
bool PLATO::set_simulation_data(const calibration_data_t &data)
{
    DBG(3);
    bool ok = true;
    if (data.date.isNull() || !data.date.isValid()) {
        ok = false;
    }
    if(data.empty()) {
        ok = false;
    }
    if (!ok) {
        m_failed_message  = tr("The test is not complete");
        return false;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);

    gui()->clear();
    clear_plato_values(PLATO::COLD);
    clear_plato_values(PLATO::HOT);
    m_failed_message.clear();

    mw()->delay(250);   // чтобы было видно, что обновлется...

    m_cps_lo_limit      = m_settings_CPS_LoLimit;
    m_cps_hi_limit      = m_settings_CPS_HiLimit;
    m_cps_spread_limit  = m_settings_CPS_SpreadLimit;
    m_hv_lo_limit       = m_settings_HV_LoLimit;
    m_hv_hi_limit       = m_settings_HV_HiLimit;
    m_hv_spread_limit   = m_settings_HV_SpreadLimit;
    m_HV_Accepted       = data.hv_accepted;
    m_warmup_period     = 1;

    m_acq_one_gamma = (int)ceil(2.5 * (double)m_settings_gamma_acq_bucket);
    m_acq_sample_time = m_warmup_period + (m_acq_one_gamma * m_settings_samples_for_avrg);

    nodeE4()->m_operating_hv = data.hv_accepted;
    nodeE4()->m_warmup_time = m_warmup_period;
    nodeE4()->m_temperature_value = data.temper_cold_avrg;
    nodeE4()->m_partnumber = data.toolpn;
    app()->bto()->set_gamma_module_kind(BTO::MODULE_KIND::ES);

    gui()->update_part_number(data.toolpn);
    gui()->update_serial_number(data.toolsn);
    gui()->update_operator(data.oper);
    gui()->update_field__estimated_time(data.date.toString("dd.MM.yyyy hh:mm"));
    gui()->update_field__acquire_time_details(m_warmup_period, m_settings_gamma_acq_bucket,
                                       m_acq_one_gamma, m_settings_samples_for_avrg);

    gui()->table()->model()->blockSignals(true);
    gui()->table()->blockSignals(true);
    auto sorts = gui()->table()->isSortingEnabled();
    if (sorts) {
        gui()->table()->setSortingEnabled(false);
    }

    m_total_steps = 0;
    for(auto tmode : {PLATO::COLD, PLATO::HOT})
    {
        DBG(1,  QString("[%1]").arg(tmode == PLATO::COLD ? "COLD" : "HOT"));
        const auto &recs = get_data_recs(tmode);
        if (recs.empty()) {
            continue;
        }
        nodeE4()->m_gamma_value = recs.last().m_cps;
        nodeE4()->m_operating_hv = recs.last().m_hv;
        nodeE4()->m_temperature_value = recs.last().m_temper;
        gui()->show_temperaure_mode(recs.last().m_temper,
                                    tmode == PLATO::COLD ? tr("COLD MODE") : tr("HOT MODE"));
        auto steps = recs.size();
        m_total_steps = std::max(m_total_steps, steps);
        for(int index = 0; index < steps; ++index)
        {
            calculate_slope(tmode, index);
            bool ok = calculate_borders(tmode, index);
            update_chart(tmode, index);
            update_table(tmode, index, false);
            update_borders();
            if (!ok) {
                //break;
            }
        }

        nodeE4()->m_gamma_value = recs.last().m_cps;
        nodeE4()->m_operating_hv = recs.last().m_hv;
        nodeE4()->m_temperature_value = recs.last().m_temper;
    }

    check_borders();
    gui()->update_field__steps_done(m_total_steps, m_total_steps);

    gui()->table()->blockSignals(false);
    gui()->table()->model()->blockSignals(false);
    gui()->table()->setSortingEnabled(sorts);
    gui()->table()->select_row(0);
    gui()->table()->deselect_row(0);
    gui()->table()->doItemsLayout();

    if (!m_failed_message.isEmpty()) {
        auto str = m_failed_message;
        DBG(2, QString("\n[100500] failed message: %1").arg(str.replace('\n', ' ')));
    }
    gui()->set_passed();

    gui()->update_fields(0x01);     // cps
    gui()->update_fields(0x05);     // temp
    gui()->update_fields(0x43);     // hv

    create_summary();
    protocol_create_last_log(data);

    DBG(3, "done.");
    QApplication::restoreOverrideCursor();
    return true;
}
