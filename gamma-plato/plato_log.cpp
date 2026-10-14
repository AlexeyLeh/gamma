/*!
  \file   plato_log.cpp
  \brief  Протоколирование данных терминала
  \author Aleksey Lekhtin
  \date   16.05.2017
*/

#include <QString>

#include "gamma.h"
#include "dbg.h"
#include "main.h"
#include "mainwindow.h"
#include "plato.h"
#include "plato_data.h"
#include "plato_gui.h"
#include "utils.h"

#define SPC  QStringLiteral(" ")
#define DASH QStringLiteral("-")
#define EQU  QStringLiteral("=")
#define COL  QStringLiteral(":")
#define SEP  QStringLiteral("|")
#define NL   QStringLiteral("\n")
#define NBSP QStringLiteral("&nbsp;")

// метки полей в протоколе
constexpr const char *toolname            = QT_TRANSLATE_NOOP("PLATO", "Gamma Sonde");
constexpr const char *label_passed        = QT_TRANSLATE_NOOP("PLATO", "Calibration result");
constexpr const char *label_date          = QT_TRANSLATE_NOOP("PLATO", "Calibration Date");
constexpr const char *label_acqtm         = QT_TRANSLATE_NOOP("PLATO", "Sample Time");
constexpr const char *label_operator      = QT_TRANSLATE_NOOP("PLATO", "Operator");
constexpr const char *label_hv            = QT_TRANSLATE_NOOP("PLATO", "Calibration HV");
constexpr const char *label_tool_name     = QT_TRANSLATE_NOOP("PLATO", "Tool Name");
constexpr const char *label_PN            = QT_TRANSLATE_NOOP("PLATO", "Tool P/N");
constexpr const char *label_SN            = QT_TRANSLATE_NOOP("PLATO", "Tool S/N");
constexpr const char *label_temp          = QT_TRANSLATE_NOOP("PLATO", "Tool Temperature");
constexpr const char *label_fname         = QT_TRANSLATE_NOOP("PLATO", "File Name");

constexpr char const *label_hv_cold_lo    = QT_TRANSLATE_NOOP("PLATO", "Cold HV Lo");
constexpr char const *label_hv_cold_hi    = QT_TRANSLATE_NOOP("PLATO", "Cold HV Hi");
constexpr char const *label_hv_cold_sp    = QT_TRANSLATE_NOOP("PLATO", "Cold HV Spread");
constexpr char const *label_hv_hot_lo     = QT_TRANSLATE_NOOP("PLATO", "Hot  HV Lo");
constexpr char const *label_hv_hot_hi     = QT_TRANSLATE_NOOP("PLATO", "Hot  HV Hi");
constexpr char const *label_hv_hot_sp     = QT_TRANSLATE_NOOP("PLATO", "Hot  HV Spread");

constexpr char const *label_cps_cold_lo   = QT_TRANSLATE_NOOP("PLATO", "Cold CPS Lo");
constexpr char const *label_cps_cold_hi   = QT_TRANSLATE_NOOP("PLATO", "Cold CPS Hi");
constexpr char const *label_cps_cold_sp   = QT_TRANSLATE_NOOP("PLATO", "Cold CPS Spread");
constexpr char const *label_cps_hot_lo    = QT_TRANSLATE_NOOP("PLATO", "Hot  CPS Lo");
constexpr char const *label_cps_hot_hi    = QT_TRANSLATE_NOOP("PLATO", "Hot  CPS Hi");
constexpr char const *label_cps_hot_sp    = QT_TRANSLATE_NOOP("PLATO", "Hot  CPS Spread");
constexpr char const *label_cps_spread    = QT_TRANSLATE_NOOP("PLATO", "CPS Spread");

constexpr char const *label_calc_lo       = QT_TRANSLATE_NOOP("PLATO", "HV Lo Calculated");
constexpr char const *label_calc_hi       = QT_TRANSLATE_NOOP("PLATO", "HV Hi Calculated");
constexpr char const *label_calc_spread   = QT_TRANSLATE_NOOP("PLATO", "HV Spread");
constexpr char const *label_calc          = QT_TRANSLATE_NOOP("PLATO", "HV Calculated");
constexpr char const *label_orig          = QT_TRANSLATE_NOOP("PLATO", "HV Original");

constexpr char const *label_hv_lo_limit   = QT_TRANSLATE_NOOP("PLATO", "HV Lo Limit");
constexpr char const *label_hv_hi_limit   = QT_TRANSLATE_NOOP("PLATO", "HV Hi Limit");
constexpr char const *label_hv_sp_limit   = QT_TRANSLATE_NOOP("PLATO", "HV Spread Limit");

constexpr char const *label_cps_lo_limit  = QT_TRANSLATE_NOOP("PLATO", "CPS Lo Limit");
constexpr char const *label_cps_hi_limit  = QT_TRANSLATE_NOOP("PLATO", "CPS Hi Limit");
constexpr char const *label_cps_sp_limit  = QT_TRANSLATE_NOOP("PLATO", "CPS Spread Limit");

constexpr char const *label_chart_cps_lo  = QT_TRANSLATE_NOOP("PLATO", "Chart CPS Lo");
constexpr char const *label_chart_cps_hi  = QT_TRANSLATE_NOOP("PLATO", "Chart CPS Hi");
constexpr char const *label_chart_cps_tick= QT_TRANSLATE_NOOP("PLATO", "Chart CPS Ticks");

constexpr char const *label_chart_hv_lo   = QT_TRANSLATE_NOOP("PLATO", "Chart HV Lo");
constexpr char const *label_chart_hv_hi   = QT_TRANSLATE_NOOP("PLATO", "Chart HV Hi");
constexpr char const *label_chart_hv_tick = QT_TRANSLATE_NOOP("PLATO", "Chart HV Ticks");


//---------------------------------------------------------
/// Сформировать имя протокола измерений
void PLATO::protocol_filename_create()
{
    m_report_filename = {};
    const auto &data = this->data();
    if (data.date.isNull() || !data.date.isValid() || data.empty()) {
        return;
    }
    int repeat = 0;
    for(;;) {
        QString filename = QString("%1/%2/PLATO/PLATO-%3-%4-%5%6.pdf")
                           .arg(report_path(), data.toolsn)
                           .arg((true ? "PASS" : "FAIL"))
                           .arg(data.toolsn)
                           .arg(data.date.toString("yyyyMMdd-hhmmss"))
                           .arg(repeat == 0 ? QStringLiteral() : QString(" (%1)").arg(repeat));
        m_report_filename.setFile(QDir::toNativeSeparators(filename));
        if (!m_report_filename.exists()) {
            break;
        }
        ++repeat;
    }
    m_report_filename.setCaching(false);
}

//---------------------------------------------------------
const QFileInfo &PLATO::protocol_filename() const
{
    return m_report_filename;
}

//---------------------------------------------------------
void PLATO::protocol_filename_set(const QString &fn)
{
    m_report_filename = QFileInfo(fn);
}

//---------------------------------------------------------
void PLATO::protocol_filename_clear()
{
    m_report_filename = {};
}

//---------------------------------------------------------
QString PLATO::protocol_open(const QString &fname)
{
    // Создать файл отчета калибровки
    if (m_report_fd.isOpen()) {
        m_report_fd.close();
    }
    if (fname.isEmpty()) {
        auto errorstr = tr("The report file name is not specified");
        return errorstr;
    }

    QFileInfo finfo(fname);
    QDir dir(finfo.absoluteDir());
    dir.mkpath(finfo.absolutePath());

    m_report_fd.setFileName(fname);
    bool ok = m_report_fd.open(QIODevice::WriteOnly | QIODevice::ReadOnly);
    if (!ok)
    {
        auto err = m_report_fd.error();
        auto err_string = m_report_fd.errorString();
        auto errorstr = QString("%1\n%2\n\n(%3) %4")
                        .arg(tr("Can't create log file"), m_report_fd.fileName())
                        .arg(err)
                        .arg(err > 0 ? err_string : "");
        return errorstr;
    }

    return {};
}

//---------------------------------------------------------
void PLATO::protocol_close()
{
    if (m_report_fd.isOpen()) m_report_fd.close();
}

//---------------------------------------------------------
/// Запись строки в отчет терминала
void PLATO::protocol_write_record(const QString &str)
{
    if (str.isEmpty()) return;
    if (!m_report_fd.isOpen()) return;
    if (m_report_fd.write(str.toUtf8()) != -1)
    {
        m_report_fd.flush();
        return; // ok
    }
    int error = m_report_fd.error();
    QString error_string = m_report_fd.errorString();
    QString msg = QString("%1 - (%2) %3")
                  .arg(tr("write data error"))
                  .arg(error)
                  .arg(error > 0 ? error_string : "");
    m_report_fd.write(msg.toUtf8()); // :(
}

//---------------------------------------------------------
/// Проверка возможности создания отчета калибровки
QString PLATO::report_check_creation()
{
    auto filename = QString("%1/.stuff").arg(report_path());
    QFileInfo finfo(filename);
    QDir dir(finfo.absoluteDir());
    dir.mkpath(finfo.absolutePath());
    auto errorstr = protocol_open(filename);
    protocol_close();
    QFile qfn(filename);
    qfn.remove();
    return errorstr;
}

//---------------------------------------------------------
/**
* @brief Сформировать отчет
* @return описание ошибки (иначе пустая строка)
*/
QString PLATO::save_report()
{
    const auto &finfo = protocol_filename();
    auto filename = finfo.absoluteFilePath();
    DBG(1, filename);
    if (filename.isEmpty()) {
        auto errorstr = tr("The report file name is not specified");
        return errorstr;
    }

    const auto &data = this->data();
    if (data.date.isNull() || !data.date.isValid() || data.empty()) {
        auto errorstr = QString("%1\n\n%2")
                        .arg(tr("No measurements has been done yet."),
                             tr("Just make test from scratch or\n"
                                "load the previous report."));
        return errorstr;
    }

    QString html_header, html_page;
    QString txt_header, txt_page;

    protocol_make_header(data, txt_header, html_header);
    protocol_make_content(data, txt_page, html_page);

    QString fn;
    //---------------------------------------------------------
    // to HTML
    if (gui()->m_settings_create_html) {
        fn = filename;
        fn.replace(".pdf", ".html");
        DBG(3, fn);
        auto errorstr = protocol_open(fn);
        if (!errorstr.isEmpty()) {
            return errorstr;
        }

        protocol_write_record(html_header);
        protocol_write_record(html_page);
        protocol_close();
    }

    //---------------------------------------------------------
    // to PDF
    fn = filename;
    auto *pdf = new QPdfWriter(fn);
    pdf->setPageMargins({0, 0, 0, 0}, QPageLayout::Millimeter);
    //pdf->setPageSize(QPageSize::A4);
    pdf->setPageSize(QPagedPaintDevice::A4);
    pdf->setPageOrientation(QPageLayout::Portrait);
    pdf->setTitle(QString("%1-SN-%2").arg(tr("Gamma Sonde"), data.toolsn));
    pdf->setCreator(QString("%1 [%2 %3]")
                    .arg(tr("Gamma Sonde Calibration"),
                         QCoreApplication::applicationName(),
                         m_app->version_short()));

    auto pdfprotocol = html_header + html_page;
    pdfprotocol.replace(".html", ".pdf");
    DBG(3, fn);

    auto *doc = new QTextDocument();
    auto *cursor = new QTextCursor(doc);
    cursor->insertHtml(pdfprotocol);
    doc->print(pdf);

    delete doc;
    delete cursor;
    delete pdf;

    //---------------------------------------------------------
    // to TXT
    fn = filename;
    fn.replace(".pdf", ".rep");
    DBG(3, fn);
    auto errorstr = protocol_open(fn);
    if (!errorstr.isEmpty()) {
        return errorstr;
    }
    auto txt = txt_header + txt_page;
    protocol_write_record(txt);
    protocol_make_screenshot();
    protocol_close();

    return {};
}


//---------------------------------------------------------
/// Считать отчет
QString PLATO::load_report(const QString &filename, calibration_data_t &data)
{
    DBG(3);

    QFileInfo finfo(filename);
    if (!finfo.exists())
    {
        auto errorstr = QString("%1\n%2").arg(tr("File not found"), QDir::toNativeSeparators(finfo.filePath()));
        DBG(1, errorstr);
        return errorstr;
    }

    // 1.
    QFile file(filename);
    bool ok = file.open(QIODevice::ReadOnly);
    if (!ok)
    {
        auto err = file.error();
        auto err_string = file.errorString();
        auto errorstr = QString("(%1) %2\n%3 %4")
                        .arg(err)
                        .arg((err > 0 ? err_string : ""),
                             tr("Can't open report"),
                             file.fileName());
        DBG(1, errorstr);
        return errorstr;
    }

    // 2.
    auto bytes = file.readAll();
    if (file.error() > 0)
    {
        auto err = file.error();
        auto err_string = file.errorString();
        auto errorstr = QString("(%1) %2\n%3 %4")
                        .arg(err)
                        .arg((err > 0 ? err_string : ""),
                             tr("Can't read report"),
                             file.fileName());
        file.close();
        DBG(1, errorstr);
        return errorstr;
    }
    file.close();

    QString report(bytes);
    auto lines = report.replace("\r\n", "\n").split('\n', QString::SkipEmptyParts);

    // 3 - заголовок
    for(const auto &line : lines) {
        if (line.trimmed().startsWith("---")) {
            break;
        }

        QStringList chunks = line.split(SEP);
        for(const auto &chunk : chunks) {
            QStringList fields = chunk.split(COL);
            if (fields.size() < 2) continue;

            auto field = fields[0].trimmed();
            auto str = fields[1].trimmed();
            for(int i = 2; i < fields.size(); ++i) {
                str += COL + fields[i].trimmed();
            }
            DBG(3, QString("%1: %2").arg(field, -25).arg(str));

            if (field.startsWith(tr(label_tool_name))) {
                data.toolname = str;
            } else if (field.startsWith(tr(label_PN))) {
                data.toolpn = str;
            } else if (field.startsWith(tr(label_SN))) {
                data.toolsn = str;
            } else if (field.startsWith(tr(label_passed))) {
                data.failed_message.clear();
                if (str.toLower() != "passed" && str.toLower() != tr("Passed").toLower()) {
                    data.failed_message = str;
                }
            } else if (field.startsWith(tr(label_date))) {
                // 'dd.MM.yyyy hh:mm'
                data.date = QDateTime::fromString(str, "dd.MM.yyyy hh:mm");
            } else if (field.startsWith(tr(label_acqtm))) {
                auto s = str.split(" ", QString::SkipEmptyParts);
                if (s.size() > 0) {
                    data.acqtime = s[0].toInt();
                }
            } else if (field.startsWith(tr(label_hv))) {
                data.hv_accepted = str.toInt();
            } else if (field.startsWith(tr(label_operator))) {
                data.oper = str;
            } else if (field.startsWith(tr(label_temp))) {
                str.remove("°C");
                auto s = str.split(" ", QString::SkipEmptyParts);
                if (s.size() > 0) {
                    s = s[0].trimmed().split("/", QString::SkipEmptyParts);
                }
                if (s.size() > 0) {
                    data.temper_cold_avrg = s[0].toDouble();
                }
                if (s.size() > 1) {
                    data.temper_hot_avrg = s[1].toDouble();
                }
            } else if (field.startsWith(tr(label_hv_cold_lo))) {
                data.hv_cold_lo_border = str.toInt(&ok);
            } else if (field.startsWith(tr(label_hv_cold_hi))) {
                data.hv_cold_hi_border = str.toInt(&ok);
            } else if (field.startsWith(tr(label_hv_cold_sp))) {
                data.hv_cold_spread = str.toInt(&ok);
            } else if (field.startsWith(tr(label_hv_hot_lo))) {
                data.hv_hot_lo_border = str.toInt(&ok);
            } else if (field.startsWith(tr(label_hv_hot_hi))) {
                data.hv_hot_hi_border = str.toInt(&ok);
            } else if (field.startsWith(tr(label_hv_hot_sp))) {
                data.hv_hot_spread = str.toInt(&ok);
            } else if (field.startsWith(tr(label_cps_cold_lo))) {
                data.cps_cold_lo_border = str.toDouble(&ok);
            } else if (field.startsWith(tr(label_cps_cold_hi))) {
                data.cps_cold_hi_border = str.toDouble(&ok);
            } else if (field.startsWith(tr(label_cps_cold_sp))) {
                data.cps_cold_spread = str.toDouble(&ok);
            } else if (field.startsWith(tr(label_cps_hot_lo))) {
                data.cps_hot_lo_border = str.toDouble(&ok);
            } else if (field.startsWith(tr(label_cps_hot_hi))) {
                data.cps_hot_hi_border = str.toDouble(&ok);
            } else if (field.startsWith(tr(label_cps_hot_sp))) {
                data.cps_hot_spread = str.toDouble(&ok);
            } else if (field.startsWith(tr(label_calc_lo))) {
                data.hv_accepted_lo = str.toInt(&ok);
            } else if (field.startsWith(tr(label_calc_hi))) {
                data.hv_accepted_hi = str.toInt(&ok);
            } else if (field == tr(label_calc_spread)) {
                data.hv_accepted_sp = str.toInt(&ok);
            } else if (field.startsWith(tr(label_calc))) {
                data.hv_accepted = str.toInt(&ok);
            } else if (field.startsWith(tr(label_orig))) {
                data.hv_orig = str.toInt(&ok);
            } else if (field.startsWith(tr(label_hv_lo_limit))) {
                data.hv_lo_limit = str.toInt(&ok);
            } else if (field.startsWith(tr(label_hv_hi_limit))) {
                data.hv_hi_limit = str.toInt(&ok);
            } else if (field == tr(label_hv_sp_limit)) {
                data.hv_sp_limit = str.toInt(&ok);
            } else if (field.startsWith(tr(label_cps_lo_limit))) {
                data.cps_lo_limit = str.toInt(&ok);
            } else if (field.startsWith(tr(label_cps_hi_limit))) {
                data.cps_hi_limit = str.toInt(&ok);
            } else if (field.startsWith(tr(label_cps_sp_limit))) {
                data.cps_sp_limit = str.toInt(&ok);
            } else if (field.startsWith(tr(label_chart_cps_lo))) {
                data.chart_cps_lo = str.toInt(&ok);
            } else if (field.startsWith(tr(label_chart_cps_hi))) {
                data.chart_cps_hi = str.toInt(&ok);
            } else if (field.startsWith(tr(label_chart_cps_tick))) {
                data.chart_cps_tick = str.toInt(&ok);
            } else if (field.startsWith(tr(label_chart_hv_lo))) {
                data.chart_hv_lo = str.toInt(&ok);
            } else if (field.startsWith(tr(label_chart_hv_hi))) {
                data.chart_hv_hi = str.toInt(&ok);
            } else if (field.startsWith(tr(label_chart_hv_tick))) {
                data.chart_hv_tick = str.toInt(&ok);
            }
        }
    }

    // 4 - данные
    bool while_header = true;
    for(const auto &line : lines) {
        if (while_header) {
            if (line.trimmed().startsWith("---")) {
                while_header = false;
            }
            continue;
        }

        auto fields = line.trimmed().split(' ', QString::SkipEmptyParts);
        if (fields.size() < 6) {
            continue;
        }

        int num = fields[0].toInt(&ok);
        if (!ok || num == 0) {
            continue;
        }

        QDateTime dt;
        int hv;
        double temper, cps, slope;

        dt = data.date;
        dt.setTime(QTime::fromString(fields[1], "hh:mm:ss"));
        temper = fields[2].toDouble(&ok);
        if (ok) {
            hv = fields[3].toInt(&ok);
        }
        if (ok) {
            cps = fields[4].toDouble(&ok);
        }
        if (ok) {
            slope = fields[5].toDouble(&ok);
        }
        if (ok) {
            data.cold.append({dt, hv,cps, temper, slope});
        }

        if (fields.size() == 11) {
            dt = data.date;
            dt.setTime(QTime::fromString(fields[6], "hh:mm:ss"));
            temper = fields[7].toDouble(&ok);
            if (ok) {
                hv = fields[8].toInt(&ok);
            }
            if (ok) {
                cps = fields[9].toDouble(&ok);
            }
            if (ok) {
                slope = fields[10].toDouble(&ok);
            }
            if (ok) {
                data.hot.append({dt, hv,cps, temper, slope});
            }
        }
    }

    if (DBG::debug_level >= 4) {
        DBGF(4, "HV Low Limit:       %d \n", data.hv_lo_limit);
        DBGF(4, "HV High Limit:      %d \n", data.hv_hi_limit);
        DBGF(4, "HV Spread           %d \n", data.hv_sp_limit);

        DBGF(4, "CPS Low Limit:      %d \n", data.cps_lo_limit);
        DBGF(4, "CPS High Limit:     %d \n", data.cps_hi_limit);
        DBGF(4, "CPS Spread:         %d \n", data.cps_sp_limit);

        DBGF(4, "Chart HV Low:       %d \n", data.chart_hv_lo);
        DBGF(4, "Chart HV High:      %d \n", data.chart_hv_hi);
        DBGF(4, "Chart HV Ticks:     %d \n", data.chart_hv_tick);

        DBGF(4, "Chart CPS Low:      %d \n", data.chart_cps_lo);
        DBGF(4, "Chart CPS High:     %d \n", data.chart_cps_hi);
        DBGF(4, "Chart CPS Ticks:    %d \n", data.chart_cps_tick);

        DBGF(4, "COLD HV Low:        %d \n", data.hv_cold_lo_border);
        DBGF(4, "COLD HV High:       %d \n", data.hv_cold_hi_border);
        DBGF(4, "COLD HV Spread:     %d \n", data.hv_cold_spread);

        DBGF(4, "HOT HV Low:         %d \n", data.hv_hot_lo_border);
        DBGF(4, "HOT HV High:        %d \n", data.hv_hot_hi_border);
        DBGF(4, "HOT HV Spread:      %d \n", data.hv_hot_spread);

        DBGF(4, "COLD CPS Low:       %0.2f \n", data.cps_cold_lo_border);
        DBGF(4, "COLD CPS Hight:     %0.2f \n", data.cps_cold_hi_border);
        DBGF(4, "COLD CPS Spread:    %0.2f \n", data.cps_cold_spread);

        DBGF(4, "HOT CPS Low:        %0.2f \n", data.cps_hot_lo_border);
        DBGF(4, "HOT CPS Hight:      %0.2f \n", data.cps_hot_hi_border);
        DBGF(4, "HOT CPS Spread:     %0.2f \n", data.cps_hot_spread);

        DBGF(4, "HV Original:        %d \n", data.hv_orig);
        DBGF(4, "HV Low  Calculated: %d \n", data.hv_accepted_lo);
        DBGF(4, "HV High Calculated: %d \n", data.hv_accepted_hi);
        DBGF(4, "HV Calculated:      %d \n", data.hv_accepted);
        DBGF(4, "HV Spread:          %d \n", data.hv_accepted_sp);

        DBGF(4, "COLD Temper (avrg): %0.2f\n", data.temper_cold_avrg);
        DBGF(4, "HOT  Temper (avrg): %0.2f\n", data.temper_hot_avrg);

        DBGF(4, "Passed:             %d   \n", data.failed_message.isEmpty());
        DBGF(4, "FAIL msg:           '%s' \n", qPrintable(data.failed_message));
        DBGF(4, "Date:               '%s' \n", qPrintable(data.date.toString()));
        DBGF(4, "Tool name:          '%s' \n", qPrintable(data.toolname));
        DBGF(4, "Tool PN:            '%s' \n", qPrintable(data.toolpn));
        DBGF(4, "Tool SN:            '%s' \n", qPrintable(data.toolsn));
        DBGF(4, "Location:           '%s' \n", qPrintable(data.location));
        DBGF(4, "Operator:           '%s' \n", qPrintable(data.oper));
        DBGF(4, "Acquire time:        %d \n", data.acqtime);

        DBG(4, "COLD: HV     CPS    SLOPE    TEMP \n");
        int idx = 1;
        for(const auto &d : data.cold) {
            DBGF(4, "%2d: %4d %8.2f %8.6f %3.1f\n", idx++, d.m_hv, d.m_cps, d.m_slope, d.m_temper);
        }

        DBG(4, "HOT:  HV     CPS    SLOPE    TEMP \n");
        idx = 1;
        for(const auto &d : data.hot) {
            DBGF(4, "%2d: %4d %8.2f %8.6f %3.1f\n",
                 idx++, d.m_hv, d.m_cps, d.m_slope, d.m_temper);
        }
    }

    DBG(4, "done\n");
    return {};
}

//---------------------------------------------------------
QString PLATO::protocol_filename_last_log()
{
    if (app()->simulation()) {
        return QString("%1/.last/PLATO-SIM-LAST-LOG.log").arg(report_path());
    }

    return QString("%1/.last/PLATO-LAST-LOG.log").arg(report_path());
}

//---------------------------------------------------------
/// Записать протокол последних измерений
void PLATO::protocol_create_last_log(const calibration_data_t &data)
{
    if (data.date.isNull() || !data.date.isValid() || data.empty()) {
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);

    auto fname = protocol_filename_last_log();
    DBG(1, QDir::toNativeSeparators(fname));
    QFile fi(fname);
    fi.remove();

    auto errstr = protocol_open(fname);
    if (!errstr.isEmpty())
    {
        auto str = errstr;
        DBG(1, str.replace("\n", " "));
        QApplication::restoreOverrideCursor();
        gui()->display_error_message(errstr);
        return;
    }

    QString html_page, html_header;
    QString txt_page, txt_header;

    protocol_filename_set(fname);
    protocol_make_header(data, txt_header, html_header);
    protocol_make_content(data, txt_page, html_page);

    auto final_txt = txt_header + txt_page;
    protocol_write_record(final_txt);
    protocol_make_screenshot();
    protocol_close();
    protocol_filename_clear();
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------
/// Записать заголовок отчета
void PLATO::protocol_make_header(const calibration_data_t &data, QString &text_header, QString &html_header)
{
    auto fname = protocol_filename().fileName();

    // 1.
    QString temper_cold = QStringLiteral("----");
    if (data.temper_cold_avrg > 0) {
        temper_cold = QString::number(data.temper_cold_avrg, 'f', 1);
    }

    QString temper_hot = QStringLiteral("----");
    if (data.temper_hot_avrg > 0) {
        temper_hot = QString::number(data.temper_hot_avrg, 'f', 1);
    }

    QString temper = QStringLiteral("%1/%2 °C").arg(temper_cold, temper_hot);

    // 2. данные полей
    const char *passed   = (data.failed_message.isEmpty() ? QT_TR_NOOP("Passed") : QT_TR_NOOP("Failed"));
    //QString eskdnum    = data.eskdnum;
    QString toolsn       = data.toolsn;
    QString toolpn       = data.toolpn;
    QString calibr_date  = data.date.toString("dd.MM.yyyy hh:mm");
    QString oper         = data.oper;
    QString hv           = QString::number(data.hv_accepted);
    QString acqtime      = QString::number(data.acqtime) + SPC + tr("sec");

    // заголовок тестового отчета
    int ww  = 18;
    int ww1 = 22;
    int ww2 = 20;

    text_header += UTILS::str_left(tr(label_passed)     + COL, ww1) + UTILS::str_left(tr(passed), ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_tool_name)  + COL, ww2) + tr(toolname) + NL;

    text_header += UTILS::str_left(tr(label_date)       + COL, ww1) + UTILS::str_left(calibr_date, ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_SN)         + COL, ww2) + toolsn + NL;

    text_header += UTILS::str_left(tr(label_acqtm)      + COL, ww1) + UTILS::str_left(acqtime, ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_PN)         + COL, ww2) + toolpn + NL;

    text_header += UTILS::str_left(tr(label_operator)   + COL, ww1) + UTILS::str_left(oper, ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_temp)       + COL, ww2) + temper + NL;

    text_header += UTILS::str_left(tr(label_hv)         + COL, ww1) + UTILS::str_left(hv, ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_fname)      + COL, ww2) + fname + NL;

    text_header += NL;

    text_header += UTILS::str_left(tr(label_hv_lo_limit)  + COL, ww1) + UTILS::str_left(QString::number(data.hv_lo_limit), ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_cps_lo_limit) + COL, ww2) + UTILS::str_left(QString::number(data.cps_lo_limit), ww) + NL;

    text_header += UTILS::str_left(tr(label_hv_hi_limit)  + COL, ww1) + UTILS::str_left(QString::number(data.hv_hi_limit), ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_cps_hi_limit) + COL, ww2) + UTILS::str_left(QString::number(data.cps_hi_limit), ww) + NL;

    text_header += UTILS::str_left(tr(label_hv_sp_limit)  + COL, ww1) + UTILS::str_left(QString::number(data.hv_sp_limit), ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_cps_sp_limit) + COL, ww2) + UTILS::str_left(QString::number(data.cps_sp_limit), ww) + NL;

    text_header += NL;

    text_header += UTILS::str_left(tr(label_chart_hv_lo)  + COL, ww1) + UTILS::str_left(QString::number(data.chart_hv_lo), ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_chart_cps_lo) + COL, ww2) + UTILS::str_left(QString::number(data.chart_cps_lo), ww) + NL;

    text_header += UTILS::str_left(tr(label_chart_hv_hi)  + COL, ww1) + UTILS::str_left(QString::number(data.chart_hv_hi), ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_chart_cps_hi) + COL, ww2) + UTILS::str_left(QString::number(data.chart_cps_hi), ww) + NL;

    text_header += UTILS::str_left(tr(label_chart_hv_tick)+ COL, ww1) + UTILS::str_left(QString::number(data.chart_hv_tick), ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_chart_cps_tick)+ COL, ww2)+ UTILS::str_left(QString::number(data.chart_cps_tick), ww) + NL;

    text_header += NL;
//
    text_header += UTILS::str_left(tr(label_hv_cold_lo)  + COL, ww1) + UTILS::str_left(QString::number(data.hv_cold_lo_border), ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_cps_cold_lo) + COL, ww2) + UTILS::str_left(QString::number(data.cps_cold_lo_border, 'f', 2), ww) + NL;

    text_header += UTILS::str_left(tr(label_hv_cold_hi)  + COL, ww1) + UTILS::str_left(QString::number(data.hv_cold_hi_border), ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_cps_cold_hi) + COL, ww2) + UTILS::str_left(QString::number(data.cps_cold_hi_border, 'f', 2), ww) + NL;

    text_header += UTILS::str_left(tr(label_hv_cold_sp)  + COL, ww1) + UTILS::str_left(QString::number(data.hv_cold_spread), ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_cps_cold_sp) + COL, ww2) + UTILS::str_left(QString::number(data.cps_cold_spread, 'f', 2), ww) + NL;


    text_header += UTILS::str_left(tr(label_hv_hot_lo)   + COL, ww1) + UTILS::str_left(QString::number(data.hv_hot_lo_border), ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_cps_hot_lo)  + COL, ww2) + UTILS::str_left(QString::number(data.cps_hot_lo_border, 'f', 2), ww) + NL;

    text_header += UTILS::str_left(tr(label_hv_hot_hi)   + COL, ww1) + UTILS::str_left(QString::number(data.hv_hot_hi_border), ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_cps_hot_hi)  + COL, ww2) + UTILS::str_left(QString::number(data.cps_hot_hi_border, 'f', 2), ww) + NL;

    text_header += UTILS::str_left(tr(label_hv_hot_sp)   + COL, ww1) + UTILS::str_left(QString::number(data.hv_hot_spread), ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_cps_hot_sp)  + COL, ww2) + UTILS::str_left(QString::number(data.cps_hot_spread, 'f', 2), ww) + NL;
//
    text_header += NL;

    text_header += UTILS::str_left(tr(label_calc_hi)     + COL, ww1) + UTILS::str_left(QString::number(data.hv_accepted_hi), ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_orig )       + COL, ww2) + UTILS::str_left(QString::number(data.hv_orig),    ww) + NL;

    text_header += UTILS::str_left(tr(label_calc_lo)     + COL, ww1) + UTILS::str_left(QString::number(data.hv_accepted_lo), ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_calc_spread) + COL, ww2) + UTILS::str_left(QString::number(data.hv_accepted_sp), ww) + NL;

    text_header += UTILS::str_left(tr(label_calc)        + COL, ww1) + UTILS::str_left(QString::number(data.hv_accepted), ww);
    text_header += SEP + SPC;
    text_header += UTILS::str_left(tr(label_cps_spread) + COL, ww2) + UTILS::str_left(QString::number(data.cps_accepted_sp, 'f', 2), ww) + NL;


    // text_header += UTILS::str_left(SPC, ww1 + ww);

    // заголовок html отчета
    html_header = protocol_make_html_template_header();

    html_header.replace("$TITLE0",          QString("%1-%2").arg(tr(toolname), toolsn).replace(SPC, NBSP));
    html_header.replace("$PASSED_LBL",      tr(label_passed).replace(SPC, NBSP));
    html_header.replace("$PASSED",          tr(passed).replace(SPC, NBSP));
    html_header.replace("$CALIBR_DATE_LBL", tr(label_date).replace(SPC, NBSP));
    html_header.replace("$CALIBR_DATE",     calibr_date.replace(SPC, NBSP));
    html_header.replace("$TOOLNAME_LBL",    tr(label_tool_name).replace(SPC, NBSP));
    html_header.replace("$TOOLNAME",        tr(toolname).replace(SPC, NBSP));
    //html_header.replace("$TOOLEN_LBL",    tr(label_ESKD).replace(SPC, NBSP));
    //html_header.replace("$TOOLEN",        eskdnum.replace(SPC, NBSP));
    html_header.replace("$TOOLPN_LBL",      tr(label_PN).replace(SPC, NBSP));
    html_header.replace("$TOOLPN",          toolpn.replace(SPC, NBSP));
    html_header.replace("$TOOLSN_LBL",      tr(label_SN).replace(SPC, NBSP));
    html_header.replace("$TOOLSN",          toolsn.replace(SPC, NBSP));
    html_header.replace("$HV_LBL",          tr(label_hv).replace(SPC, NBSP));
    html_header.replace("$HV",              hv.replace(SPC, NBSP));
    html_header.replace("$ACQTIME_LBL",     tr(label_acqtm).replace(SPC, NBSP));
    html_header.replace("$ACQTIME",         acqtime.replace(SPC, NBSP));
    html_header.replace("$TEMP_LBL",        tr(label_temp).replace(SPC, NBSP));
    html_header.replace("$TEMP",            temper.replace(SPC, NBSP));
    html_header.replace("$OPER_LBL",        tr(label_operator).replace(SPC, NBSP));
    html_header.replace("$OPER_NAME",       oper.replace(SPC, NBSP));
    html_header.replace("$FILE_LBL",        tr(label_fname).replace(SPC, NBSP));
    html_header.replace("$FILE",            QString(fname).replace(SPC, NBSP));

    QString str(NBSP);
    html_header.replace("$SPCITEM", str.repeated(10));
}

//---------------------------------------------------------
/// Записать данные измерений в отчет
void PLATO::protocol_make_content(const calibration_data_t &data, QString &txt_page, QString &html_page)
{
    auto app_ver = QStringLiteral("\n%1 [%2]").arg(tr("Gamma Sonde Calibration")).arg(m_app->version_short());

    auto cold = data.cold;
    auto hot  = data.hot;
    int len = std::max(cold.size(), hot.size());
    cold.resize(len);
    hot.resize(len);

    typedef struct {
        int width;
        const char *name;
    } column_t;

    QVector<column_t> columns {
        {  5, QT_TR_NOOP("№")},
        { 18, QT_TR_NOOP("Date/Time")},
        {  7, QT_TR_NOOP("Temp")},
        { 11, QT_TR_NOOP("HV")},
        { 10, QT_TR_NOOP("CPS")},
        { 12, QT_TR_NOOP("Slope")},
        { 19, QT_TR_NOOP("Date/Time")},
        {  7, QT_TR_NOOP("Temp")},
        { 11, QT_TR_NOOP("HV")},
        { 10, QT_TR_NOOP("CPS")},
        { 12, QT_TR_NOOP("Slope")},
    };

    // данные тестового отчета
    txt_page += QStringLiteral("\n%1%2%3%4%5%6%7%8%9%10%11\n")
                .arg(UTILS::str_center(tr(columns[0].name), columns[0].width))

            .arg(UTILS::str_center(tr(columns[1].name), columns[1].width))
            .arg(UTILS::str_right(tr(columns[2].name), columns[2].width))
            .arg(UTILS::str_right(tr(columns[3].name), columns[3].width))
            .arg(UTILS::str_right(tr(columns[4].name), columns[4].width))
            .arg(UTILS::str_right(tr(columns[5].name), columns[5].width))

            .arg(UTILS::str_center(tr(columns[6].name), columns[6].width))
            .arg(UTILS::str_right(tr(columns[7].name), columns[7].width))
            .arg(UTILS::str_right(tr(columns[8].name), columns[8].width))
            .arg(UTILS::str_right(tr(columns[9].name), columns[9].width))
            .arg(UTILS::str_right(tr(columns[10].name),columns[10].width));

    int ww = 0;
    for (const auto &col : columns)
    {
        ww += col.width;
    }

    txt_page += QString("-").repeated(ww) + NL;

    for (int idx = 0; idx < len; ++idx)
    {
        const auto &c = cold.at(idx);
        const auto &h = hot.at(idx);
        QString fld_dt1   = QStringLiteral("%1").arg(c.m_dt.toString("hh:mm:ss"));
        QString fld_tp1   = QStringLiteral("%1").arg(c.m_temper, 8, 'f', 1);
        QString fld_hv1   = QStringLiteral("%1").arg(c.m_hv, 4);
        QString fld_cps1  = QStringLiteral("%1").arg(c.m_cps, 8, 'f', 2);
        QString fld_slop1 = QStringLiteral("%1").arg(c.m_slope, 7, 'f', 5);
        QString fld_dt2   = QStringLiteral("%1").arg(h.m_dt.toString("hh:mm:ss"));
        QString fld_tp2   = QStringLiteral("%1").arg(h.m_temper, 8, 'f', 1);
        QString fld_hv2   = QStringLiteral("%1").arg(h.m_hv, 4);
        QString fld_cps2  = QStringLiteral("%1").arg(h.m_cps, 8, 'f', 2);
        QString fld_slop2 = QStringLiteral("%1").arg(h.m_slope, 7, 'f', 5);

        txt_page += UTILS::str_center(QStringLiteral("%1").arg(idx+1, 3, 10, QChar(' ')), columns[0].width);

        if (c.m_dt.isValid())
        {
            txt_page += QStringLiteral("%1%2%3%4%5")
                        .arg(UTILS::str_center(fld_dt1,  columns[1].width))
                    .arg(UTILS::str_right(fld_tp1,  columns[2].width))
                    .arg(UTILS::str_right(fld_hv1,  columns[3].width))
                    .arg(UTILS::str_right(fld_cps1, columns[4].width))
                    .arg(UTILS::str_right(fld_slop1,columns[5].width));
        }
        else
        {
            txt_page += QStringLiteral("%1%2%3%4%5")
                        .arg(UTILS::str_center(DASH, columns[1].width))
                    .arg(UTILS::str_right(DASH, columns[2].width))
                    .arg(UTILS::str_right(DASH, columns[3].width))
                    .arg(UTILS::str_right(DASH, columns[4].width))
                    .arg(UTILS::str_right(DASH, columns[5].width));
        }

        if (h.m_dt.isValid())
        {
            txt_page += QStringLiteral("%1%2%3%4%5")
                        .arg(UTILS::str_center(fld_dt2,  columns[6].width))
                    .arg(UTILS::str_right(fld_tp2,  columns[7].width))
                    .arg(UTILS::str_right(fld_hv2,  columns[8].width))
                    .arg(UTILS::str_right(fld_cps2, columns[9].width))
                    .arg(UTILS::str_right(fld_slop2,columns[10].width));
        }
        else
        {
            txt_page += QStringLiteral("%1%2%3%4%5")
                        .arg(UTILS::str_center(DASH, columns[6].width))
                    .arg(UTILS::str_right(DASH, columns[7].width))
                    .arg(UTILS::str_right(DASH, columns[8].width))
                    .arg(UTILS::str_right(DASH, columns[9].width))
                    .arg(UTILS::str_right(DASH, columns[10].width));
        }
        txt_page += NL;
    }

    txt_page += app_ver;
    txt_page += NL;

    // данные html отчета
    html_page = QStringLiteral(
                    "\n"
                    "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n"
                    "<tr>\n"
                    "<td  class=\"tdhdr\">$NO_LBL</td>\n"
                    "<td  class=\"tdhdr\">$DATETIME1_LBL</td>\n"
                    "<td  class=\"tdhdr\">$TEMP1_LBL</td>\n"
                    "<td  class=\"tdhdr\">$HV1_LBL</td>\n"
                    "<td  class=\"tdhdr\">$CPS1_LBL</td>\n"
                    "<td  class=\"tdhdr\">$SLOPE1_LBL</td>\n"
                    "<td  class=\"tdhdr\">&nbsp;&nbsp;</td>\n"
                    "<td  class=\"tdhdr\">$DATETIME2_LBL</td>\n"
                    "<td  class=\"tdhdr\">$TEMP2_LBL</td>\n"
                    "<td  class=\"tdhdr\">$HV2_LBL</td>\n"
                    "<td  class=\"tdhdr\">$CPS2_LBL</td>\n"
                    "<td  class=\"tdhdr\">$SLOPE2_LBL</td>\n"
                    "</tr>\n"
                    "<tr><td colspan=\"12\">&nbsp;</td>\n</tr>\n");

    html_page.replace("$NO_LBL",        QString(tr(columns[0].name)).replace(SPC, NBSP));
    html_page.replace("$DATETIME1_LBL", QString(tr(columns[1].name)).replace(SPC, NBSP));
    html_page.replace("$TEMP1_LBL",     QString(tr(columns[2].name)).replace(SPC, NBSP));
    html_page.replace("$HV1_LBL",     QString(tr(columns[3].name)).replace(SPC, NBSP));
    html_page.replace("$CPS1_LBL",      QString(tr(columns[4].name)).replace(SPC, NBSP));
    html_page.replace("$SLOPE1_LBL",    QString(tr(columns[5].name)).replace(SPC, NBSP));
    html_page.replace("$DATETIME2_LBL", QString(tr(columns[6].name)).replace(SPC, NBSP));
    html_page.replace("$TEMP2_LBL",     QString(tr(columns[7].name)).replace(SPC, NBSP));
    html_page.replace("$HV2_LBL",     QString(tr(columns[8].name)).replace(SPC, NBSP));
    html_page.replace("$CPS2_LBL",      QString(tr(columns[9].name)).replace(SPC, NBSP));
    html_page.replace("$SLOPE2_LBL",    QString(tr(columns[10].name)).replace(SPC, NBSP));

    for (int idx = 0; idx < len; ++idx)
    {
        const auto &c = cold.at(idx);
        const auto &h = hot.at(idx);

        QString fld_tm1   = QStringLiteral("%1").arg(c.m_dt.toString("hh:mm:ss"));
        QString fld_temp1 = QStringLiteral("%1").arg(c.m_temper, 5, 'f', 1).replace(SPC, NBSP);
        QString fld_hv1   = QStringLiteral("%1").arg(c.m_hv, 4).replace(SPC, NBSP);
        QString fld_cps1  = QStringLiteral("%1").arg(c.m_cps, 8, 'f', 2).replace(SPC, NBSP);
        QString fld_slop1 = QStringLiteral("%1").arg(c.m_slope, 7, 'f', 4).replace(SPC, NBSP);
        QString fld_tm2   = QStringLiteral("%1").arg(h.m_dt.toString("hh:mm:ss"));
        QString fld_temp2 = QStringLiteral("%1").arg(h.m_temper, 5, 'f', 1).replace(SPC, NBSP);
        QString fld_hv2   = QStringLiteral("%1").arg(h.m_hv, 4).replace(SPC, NBSP);
        QString fld_cps2  = QStringLiteral("%1").arg(h.m_cps, 8, 'f', 2).replace(SPC, NBSP);
        QString fld_slop2 = QStringLiteral("%1").arg(h.m_slope, 7, 'f', 4).replace(SPC, NBSP);

        html_page += QStringLiteral(
                         "<tr>"
                         "<td align=\"center\">%1</td>")
                     .arg(idx, 3, 10, QChar(' '));
        if (c.m_dt.isValid())
        {
            html_page += QStringLiteral(
                             "<td align=\"center\">%1</td>"
                             "<td align=\"center\">%2</td>"
                             "<td align=\"right\">%3</td>"
                             "<td align=\"right\">%4</td>"
                             "<td align=\"right\">%5</td>"
                             "<td>&nbsp;</td>")
                         .arg(fld_tm1)
                         .arg(fld_temp1)
                         .arg(fld_hv1)
                         .arg(fld_cps1)
                         .arg(fld_slop1);
        }
        else
        {
            html_page += QStringLiteral(
                             "<td>&nbsp;</td>"
                             "<td>&nbsp;</td>"
                             "<td>&nbsp;</td>"
                             "<td>&nbsp;</td>"
                             "<td>&nbsp;</td>"
                             "<td>&nbsp;</td>");
        }

        if (h.m_dt.isValid())
        {
            html_page += QStringLiteral(
                             "<td align=\"center\">%1</td>"
                             "<td align=\"center\">%2</td>"
                             "<td align=\"right\">%3</td>"
                             "<td align=\"right\">%4</td>"
                             "<td align=\"right\">%5</td>")
                         .arg(fld_tm2)
                         .arg(fld_temp2)
                         .arg(fld_hv2)
                         .arg(fld_cps2)
                         .arg(fld_slop2);
        }
        else
        {
            html_page += QStringLiteral(
                             "<td>&nbsp;</td>"
                             "<td>&nbsp;</td>"
                             "<td>&nbsp;</td>"
                             "<td>&nbsp;</td>"
                             "<td>&nbsp;</td>");
        }

        html_page += QStringLiteral("</tr>\n");
    }

    html_page += QStringLiteral("</table>\n\n");
    html_page += QStringLiteral("<p style=\"font-size:8pt; font-style: italic;\">%1\n</p>").arg(app_ver);
    html_page += QStringLiteral("\n</body>\n</html>\n");
}

//---------------------------------------------------------
void PLATO::protocol_make_screenshot()
{
    QString screenshot_fname = m_report_fd.fileName();
    auto pos = screenshot_fname.lastIndexOf('.');
    if (pos != -1) {
        screenshot_fname = screenshot_fname.mid(0, pos);
    }
    screenshot_fname += ".png";
    DBG(1, QDir::toNativeSeparators(screenshot_fname));

    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        auto winid = mw()->winId();
        DBG(2, QString("winId(): %1").arg(winid));
        QPixmap originalPixmap = screen->grabWindow(winid);
        if (!originalPixmap.save(screenshot_fname)) {
            DBG(0, QString("%1 ; %2")
                .arg(tr("Can't save screenshot about test result"))
                .arg(screenshot_fname));
        }
    }
}

//---------------------------------------------------------
void PLATO::set_data(const calibration_data_t &data)
{
    DBG(3);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    gui()->clear();
    clear_plato_values(PLATO::COLD);
    clear_plato_values(PLATO::HOT);
    m_failed_message.clear();

    nodeE4()->m_partnumber = data.toolpn;
    m_acq_sample_time    = data.acqtime;

    m_hv_lo_limit        = data.hv_lo_limit;
    m_hv_hi_limit        = data.hv_hi_limit;
    m_hv_spread_limit    = data.hv_sp_limit;

    m_cps_lo_limit       = data.cps_lo_limit;
    m_cps_hi_limit       = data.cps_hi_limit;
    m_cps_spread_limit   = data.cps_sp_limit;

    m_HV_AcceptedLo      = data.hv_accepted_lo;
    m_HV_AcceptedHi      = data.hv_accepted_hi;
    m_HV_Accepted        = data.hv_accepted;
    m_HV_orig            = data.hv_orig;

    if (!data.cold.empty()) {
        m_cold_hv_lo_border  = data.hv_cold_lo_border;
        m_cold_hv_hi_border  = data.hv_cold_hi_border;
        m_cold_hv_spread     = data.hv_cold_spread;
        m_cold_cps_lo_border = data.cps_cold_lo_border;
        m_cold_cps_hi_border = data.cps_cold_hi_border;
        m_cold_cps_spread    = data.cps_cold_spread;
    }

    if (!data.hot.empty()) {
        m_hot_hv_lo_border   = data.hv_hot_lo_border;
        m_hot_hv_hi_border   = data.hv_hot_hi_border;
        m_hot_hv_spread      = data.hv_hot_spread;
        m_hot_cps_lo_border  = data.cps_hot_lo_border;
        m_hot_cps_hi_border  = data.cps_hot_hi_border;
        m_hot_cps_spread     = data.cps_hot_spread;
    }

    if (!data.cold.empty() && !data.hot.empty()) {
        m_HV_Accepted     = data.hv_accepted;
        m_HV_AcceptedHi   = data.hv_accepted_hi;
        m_HV_AcceptedLo   = data.hv_accepted_lo;
        m_HV_Spread_Accepted = data.hv_accepted_sp;
        m_CPS_Spread_Accepted = data.cps_accepted_sp;
    } else {
        m_failed_message  = tr("The test is not complete");
    }

    gui()->setup_initial_values();
    gui()->update_field__acquire_time(m_acq_sample_time);
    gui()->update_field__estimated_time(data.date.toString("dd.MM.yyyy hh:mm"));

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
            update_chart(tmode, index);
            update_table(tmode, index, false);
        }

        nodeE4()->m_gamma_value = recs.last().m_cps;
        nodeE4()->m_operating_hv = recs.last().m_hv;
        nodeE4()->m_temperature_value = recs.last().m_temper;
    }
    gui()->update_field__steps_done(m_total_steps, m_total_steps);

    gui()->table()->blockSignals(false);
    gui()->table()->model()->blockSignals(false);
    gui()->table()->setSortingEnabled(sorts);
    gui()->table()->select_row(0);
    gui()->table()->deselect_row(0);
    gui()->table()->doItemsLayout();

    gui()->update_fields(0x01);
    gui()->update_fields(0x05);
    gui()->update_fields(0x43);
    gui()->update_fields(0xfc);

    update_borders();
    gui()->set_passed();

    DBG(1, "done.");
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------
void PLATO_GUI::slot_save_report()
{
    const auto &data = m_plato->data();
    if (data.date.isNull() || !data.date.isValid() || data.empty()) {
        auto msg = QString("%1\n\n%2")
                   .arg(tr("No measurements has been done yet."),
                        tr("Just make test from scratch or \n"
                           "load the previous log."));
        DBG(3, msg);
        display_info_message(QMessageBox::Information, msg);
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);

    m_plato->protocol_filename_create();
    const auto &finfo = m_plato->protocol_filename();
    DBG(1, finfo.absoluteFilePath());
    QDir dir = finfo.absoluteDir();
    dir.cdUp();
    m_settings_last_report_dir = QDir::toNativeSeparators(dir.absolutePath());

    auto errstr = m_plato->save_report();

    QApplication::restoreOverrideCursor();

    if (!errstr.isEmpty())
    {
        auto str = errstr;
        DBG(1, str.replace("\n", " "));
        m_plato->protocol_filename_clear();
        display_error_message(errstr);
        return;
    }

    QString msg = tr("The report has been saved");
    emit mw()->signal_showstatus(QString("%1 - %2").arg(msg, QDir::toNativeSeparators(finfo.filePath())));
    auto s1 = UTILS::split_path(finfo.absolutePath());
    auto s2 = finfo.fileName();
    display_info_message(QMessageBox::Information, QString("%1\n\n%2\n%3").arg(msg, s1, s2), finfo.absoluteFilePath());
    m_plato->protocol_filename_clear();
}

//---------------------------------------------------------
void PLATO_GUI::slot_load_report()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    auto fullname = QFileDialog::getOpenFileName(
                        mw(),
                        tr("Load Report"),
                        m_settings_last_report_loaded_dir,
                        tr("Report (PLATO-*.rep)"));
    DBG(3, QDir::toNativeSeparators(fullname));
    QApplication::restoreOverrideCursor();
    if (fullname.isEmpty()) {
        return;
    }

    QFileInfo finfo(QDir::toNativeSeparators(fullname));
    m_settings_last_report_loaded_dir = finfo.absolutePath();

    PLATO::calibration_data_t report_data{};
    auto errstr = m_plato->load_report(fullname, report_data);
    if (!errstr.isEmpty()) {
        auto str = errstr;
        DBG(1, str.replace("\n", " "));
        display_error_message(errstr);
        return;
    }

    m_plato->set_data(report_data);
}

//---------------------------------------------------------
void PLATO_GUI::slot_load_last_test()
{
    DBG(4);
    auto fname = m_plato->protocol_filename_last_log();
    QFileInfo finfo(fname);
    if (!finfo.exists())
    {
        auto errstr = QString("%1\n\n%2")
                      .arg(tr("Last measurements not found"), QDir::toNativeSeparators(finfo.filePath()));
        auto str = errstr;
        DBG(1, str.replace("\n", " "));
        display_error_message(errstr);
        return;
    }

    PLATO::calibration_data_t report_data{};
    auto errstr = m_plato->load_report(fname, report_data);
    if (!errstr.isEmpty())
    {
        auto str = errstr;
        DBG(1, str.replace("\n", " "));
        display_error_message(errstr);
        return;
    }
    m_plato->set_data(report_data);
}


