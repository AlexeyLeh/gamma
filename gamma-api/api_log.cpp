/*!
  \file   api_log.cpp
  \brief  Протоколирование данных терминала
  \author Aleksey Lekhtin
  \date   16.05.2017
*/

#include <QPageSize>
#include <QVector>

#include "api.h"
#include "api_data.h"
#include "api_gui.h"
#include "main.h"
#include "mainwindow.h"
#include "NodeE4.h"
#include "gamma.h"
#include "utils.h"

#define SPC  QStringLiteral(" ")
#define DASH QStringLiteral(":")
#define NBSP QStringLiteral("&nbsp;")
#define NL   QStringLiteral("\n")

//---------------------------------------------------------
/// Сформировать имя протокола измерений
void API::protocol_filename_create()
{
    m_report_filename = {};
    const auto &data = this->data();
    if (data.date.isNull() || !data.date.isValid() || data.empty()) {
        return;
    }
    int repeat = 0;
    for(;;) {
        QString filename = QString("%1/%2/API/API-%3-%4-%5%6.pdf")
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
const QFileInfo &API::protocol_filename() const
{
    return m_report_filename;
}

//---------------------------------------------------------
void API::protocol_filename_set(const QString &fn)
{
    m_report_filename = QFileInfo(fn);
}

//---------------------------------------------------------
void API::protocol_filename_clear()
{
    m_report_filename = {};
}


//---------------------------------------------------------
QString API::protocol_filename_last_log()
{
    if (simulation()) {
        return QString("%1/.last/API-SIM-LAST-LOG.log").arg(report_path());
    }
    auto fname = QString("%1/.last/API-LAST-LOG.log").arg(report_path());
    return fname;
}

//---------------------------------------------------------
// Проверка возможности создания отчета калибровки
QString API::protocol_check_creation()
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
// Сформировать отчет калибровки
QString API::save_report(const QString &filename, const api_data_t &data)
{
    QString html_header, html_page;
    QString txt_header, txt_page;
    QString fn, errorstr;

    if (data.data.size() == 0) {
        m_gui->display_info_message({}, tr("Nothing to report"));
        return tr("Nothing to report");
    }

    if (filename.isEmpty()) {
        return tr("The report file name is not specified");
    }

    QFileInfo finfo(filename);
    DBG(1, QDir::toNativeSeparators(filename));
    DBG(3, QString("base name: %1").arg(finfo.baseName()));

    protocol_make_header(data, html_header, txt_header);
    protocol_make_content(data, html_page, txt_page);

    //---------------------------------------------------------
    // to HTML
    if(m_settings_create_html) {
        fn = filename;
        fn.replace(".pdf", ".html");
        DBG(3, fn);
        errorstr = protocol_open(fn);
        if (!errorstr.isEmpty()) {
            return errorstr;
        }
        protocol_write_record(html_header);
        protocol_write_record(html_page);
        protocol_close();
    }

    //-----------------------------------------------------
    // to PDF
    fn = filename;
    auto *pdf = new QPdfWriter(fn);
    pdf->setPageMargins({0, 0, 0, 0}, QPageLayout::Millimeter);
    //pdf->setPageSize(QPageSize::A4);
    pdf->setPageSize(QPagedPaintDevice::A4);
    pdf->setPageOrientation(QPageLayout::Portrait);
    pdf->setTitle(QString("%1-%2").arg(tr("Gamma Sonde"), data.toolsn));
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

#if 0
    // --- БЛОК МАСШТАБИРОВАНИЯ ---
    // Получаем реальное разрешение и размер печатной области в пикселях
    QPainter painter(pdf);
    QRect paintRect = pdf->pageLayout().paintRectPixels(pdf->resolution());
    painter.end(); // Закрываем painter, чтобы doc->print() мог открыть свой

    // Задаем процент (например, 80% от исходного размера)
    double percent = 80.0;
    double scaleFactor = percent / 100.0;

    // Искусственно увеличиваем ширину документа.
    // При печати текст и элементы ужмутся на заданный процент.
    doc->setTextWidth(paintRect.width() / scaleFactor);
    // Применяем масштабирование к самому рисовальщику через кастомную отрисовку
    QPainter printPainter(pdf);
    printPainter.scale(scaleFactor, scaleFactor);
    doc->drawContents(&printPainter);
    printPainter.end();
#else
    doc->print(pdf);
#endif
    // ----------------------------

    delete doc;
    delete cursor;
    delete pdf;

    //-----------------------------------------------------
    // to TXT
    fn = filename;
    fn.replace(".pdf", ".rep");
    DBG(3, fn);
    errorstr = protocol_open(fn);
    if (!errorstr.isEmpty()) {
        return errorstr;
    }
    auto txt = txt_header + txt_page;
    protocol_write_record(txt);
    protocol_close();

    protocol_make_screenshot();
    return {};
}

//---------------------------------------------------------
// Создать файл прокотола калибровки
QString API::protocol_open(const QString &fname)
{
    if (m_api_log_fd.isOpen()) m_api_log_fd.close();
    if (fname.isEmpty()) {
        return tr("The report name does not specified");
    }

    QFileInfo finfo(fname);
    QDir dir(finfo.absoluteDir());
    dir.mkpath(finfo.absolutePath());

    m_api_log_fd.setFileName(fname);
    bool ok = m_api_log_fd.open(QIODevice::WriteOnly | QIODevice::ReadOnly);
    if (!ok) {
        auto err = m_api_log_fd.error();
        auto err_string = m_api_log_fd.errorString();
        auto errorstr = QString("%1\n%2\n\n(%3) %4")
                        .arg(tr("Can't create log file"), m_api_log_fd.fileName())
                        .arg(err)
                        .arg(err > 0 ? err_string : "");
        return errorstr;
    }

    return {};
}

//---------------------------------------------------------
void API::protocol_close()
{
    if (m_api_log_fd.isOpen()) {
        m_api_log_fd.close();
    }
}

//---------------------------------------------------------
/// Записать протокол последних измерений
void API::protocol_create_last_log()
{
    const auto &data = this->data();
    if (data.date.isNull() || !data.date.isValid() || data.size() == 0) {
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
        DBG(1, errstr);
        QApplication::restoreOverrideCursor();
        gui()->display_error_message(tr("Can't create report"), errstr);
        return;
    }

    QString html_page, html_header;
    QString txt_page, txt_header;

    protocol_make_header(data, html_header, txt_header);
    protocol_make_content(data, html_page, txt_page);

    auto final_txt = txt_header + txt_page;
    protocol_write_record(final_txt);
    protocol_make_screenshot();
    protocol_close();
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------
// Заголовок отчета
void API::protocol_make_header(const api_data_t &data, QString &html_header, QString &text_header)
{
    // средняя температура теста
    qreal temperature_avrg = 0;
    QVector<API_DATA> arr = data.data; // copy!
    std::stable_sort(arr.begin(), arr.end(), API_DATA::temp1_less_than);
    arr.removeFirst();
    arr.removeLast();
    for(const auto &d : arr) {
        temperature_avrg += d.temper1;
    }
    temperature_avrg = temperature_avrg / data.data.size();
    QString c_temperature = QStringLiteral("----");
    if (temperature_avrg > 0) {
        c_temperature = QString::number(temperature_avrg, 'f', 1);
    }
    QString temper = QStringLiteral("%1°C").arg(c_temperature);

    // метки полей
    QString label_name      = tr("Tool Name");
    QString label_PN        = tr("Tool P/N");
    QString label_SN        = tr("Tool S/N");
    QString label_oper      = tr("Operator");
    QString label_pass      = tr("Test result");
    QString label_hv        = tr("High voltage");
    QString label_acqmet    = tr("Sample Time");
    QString label_temp      = tr("Tool Temperature");
    QString label_date      = tr("Calibration Date");
    QString label_tolfactor = tr("API/CPS Tolerance");
    QString label_calfactor = tr("Calibration Factor");

    // данные полей
    QString toolname  = tr("GAMMA SONDE");
    QString toolpn    = data.toolpn;
    QString toolsn    = data.toolsn;
    QString date      = data.date.toString("dd.MM.yyyy hh:mm");
    QString oper      = data.oper;
    QString passed    = (data.failed_message.isEmpty() ? tr("Passed") : tr("Failed"));
    QString hv        = QString::number(data.operating_hv) + SPC + tr("V");
    QString acqtime   = QString("%1 %2").arg(data.acqtime).arg(tr("sec"));
    QString calfactor = QString("%1/%2")
                        .arg(data.calibr_factor_old, 0, 'f', 3)
                        .arg(data.calibr_factor_new, 0, 'f', 3);
    QString tolfactor = QString("%1 - %2")
                        .arg(data.settings_calibr_min)
                        .arg(data.settings_calibr_max);

    // заголовок тестового отчета
    int ww  = 20;
    int ww1 = 22;
    int ww2 = 18;

    text_header += UTILS::str_left(label_pass,      ww1) + DASH + SPC + UTILS::str_left(passed, ww);
    text_header += UTILS::str_left(label_name,      ww2) + DASH + SPC + toolname;
    text_header += NL;

    text_header += UTILS::str_left(label_date,      ww1) + DASH + SPC + UTILS::str_left(date, ww);
    text_header += UTILS::str_left(label_PN,        ww2) + DASH + SPC + toolpn;
    text_header += NL;

    text_header += UTILS::str_left(label_acqmet,    ww1) + DASH + SPC + UTILS::str_left(acqtime, ww);
    text_header += UTILS::str_left(label_SN,        ww2) + DASH + SPC + toolsn;
    text_header += NL;

    text_header += UTILS::str_left(label_calfactor, ww1) + DASH + SPC + UTILS::str_left(calfactor, ww);
    text_header += UTILS::str_left(label_temp,      ww2) + DASH + SPC + temper;
    text_header += NL;

    text_header += UTILS::str_left(label_tolfactor, ww1) + DASH + SPC + UTILS::str_left(tolfactor, ww);
    text_header += UTILS::str_left(label_hv,        ww2) + DASH + SPC + hv;
    text_header += NL;

    text_header += UTILS::str_left(label_oper,      ww1) + DASH + SPC + UTILS::str_left(oper, ww);;
    text_header += SPC;
    text_header += NL;

    // заголовок html отчета
    html_header = protocol_make_html_template_header();

    html_header.replace("$TITLE0",          QString("%1-%2").arg(tr("GAMMA SONDE"), toolsn).replace(SPC, NBSP));
    html_header.replace("$PASSED_LBL",      label_pass.replace(SPC, NBSP));
    html_header.replace("$PASSED",          passed.replace(SPC, NBSP));
    html_header.replace("$CALIBR_DATE_LBL", label_date.replace(SPC, NBSP));
    html_header.replace("$CALIBR_DATE",     date.replace(SPC, NBSP));
    html_header.replace("$TOOLNAME_LBL",    label_name.replace(SPC, NBSP));
    html_header.replace("$TOOLNAME",        toolname.replace(SPC, NBSP));
    //// html_header.replace("$TOOLEN_LBL",      label_PN.replace(SPC, NBSP));
    //// html_header.replace("$TOOLEN",          toolpn.replace(SPC, NBSP));
    html_header.replace("$TOOLPN_LBL",      label_PN.replace(SPC, NBSP));
    html_header.replace("$TOOLPN",          toolpn.replace(SPC, NBSP));
    html_header.replace("$TOOLSN_LBL",      label_SN.replace(SPC, NBSP));
    html_header.replace("$TOOLSN",          toolsn.replace(SPC, NBSP));
    html_header.replace("$HV_LBL",          label_hv.replace(SPC, NBSP));
    html_header.replace("$HV",              hv.replace(SPC, NBSP));
    html_header.replace("$TEMP_LBL",        label_temp.replace(SPC, NBSP));
    html_header.replace("$TEMP",            temper.replace(SPC, NBSP));
    html_header.replace("$OPER_LBL",        label_oper.replace(SPC, NBSP));
    html_header.replace("$OPER_NAME",       oper.replace(SPC, NBSP));
    html_header.replace("$ACQMETHOD_LBL",   label_acqmet.replace(SPC, NBSP));
    html_header.replace("$ACQMETHOD",       acqtime.replace(SPC, NBSP));
    html_header.replace("$KF229_LBL",       label_calfactor.replace(SPC, NBSP));
    html_header.replace("$KF229",           calfactor.replace(SPC, NBSP));
    html_header.replace("$TOL_LBL",         label_tolfactor.replace(SPC, NBSP));
    html_header.replace("$TOL",             tolfactor.replace(SPC, NBSP));
    html_header.replace("$OPER_LBL",        label_oper.replace(SPC, NBSP));
    html_header.replace("$OPER_NAME",       oper.replace(SPC, NBSP));

    auto spcs = QString(NBSP).repeated(8);
    html_header.replace("$SPCITEM", spcs);
}

//---------------------------------------------------------
/// Данные измерений в отчет
void API::protocol_make_content(const api_data_t &data, QString &html_page, QString &txt_page)
{
    auto app_ver = QStringLiteral("\n%1 %2").arg(API::tr("Gamma Sonde Calibration")).arg(m_app->version_short());

    typedef struct {
        int width;
        QString name;
    } column_t;

    QVector<column_t> columns {
        {  5, tr("No")},
        {  8, tr("HV(V)")},
        { 13, tr("0 API(cps)")},
        { 10, tr("Temp °C")},
        { 13, tr("200 API(cps)")},
        { 10, tr("Temp °C")},
    };

    // "шапка" таблицы с данными текстового отчета
    txt_page += QStringLiteral("\n%1%2%3%4%5%6\n\n")
            .arg(UTILS::str_center(QString(columns[0].name).replace("(",",").replace(")",""), columns[0].width))
            .arg(UTILS::str_center(QString(columns[1].name).replace("(",",").replace(")",""), columns[1].width))
            .arg(UTILS::str_center(QString(columns[2].name).replace("(",",").replace(")",""), columns[2].width))
            .arg(UTILS::str_center(QString(columns[3].name).replace("(",",").replace(")",""), columns[3].width))
            .arg(UTILS::str_center(QString(columns[4].name).replace("(",",").replace(")",""), columns[4].width))
            .arg(UTILS::str_center(QString(columns[5].name).replace("(",",").replace(")",""), columns[5].width));

    // данные текстового отчета
    int idx = 0;
    for (const auto &d : data.data) {
        ++idx;
        QString fld_N    = QStringLiteral("%1").arg(idx, 3, 10, QChar(' '));
        QString fld_hv1  = QStringLiteral("%1").arg(d.hv1);
        QString fld_cps1 = QStringLiteral("%1").arg(d.cps1, 8, 'f', 2);
        QString fld_tp1  = QStringLiteral("%1").arg(d.temper1, 5, 'f', 1);
        QString fld_cps2 = QStringLiteral("%1").arg(d.cps2, 8, 'f', 2);
        QString fld_tp2  = QStringLiteral("%1").arg(d.temper2, 5, 'f', 1);

        txt_page += QStringLiteral("%1%2%3%4%5%6\n")
                .arg(UTILS::str_center(fld_N,    columns[0].width))
                .arg(UTILS::str_center(fld_hv1,  columns[1].width))
                .arg(UTILS::str_center(fld_cps1, columns[2].width))
                .arg(UTILS::str_center(fld_tp1,  columns[3].width))
                .arg(UTILS::str_center(fld_cps2, columns[4].width))
                .arg(UTILS::str_center(fld_tp2,  columns[5].width));
    }

    //-----------------------------------------------------
    const QList<QVector<ODID*>> &listOfVectors = {m_api_odid_new, m_api_odid_old};

    for(const auto &odid : listOfVectors) {
        int ncols = 4;
        int nrows = odid.size() / ncols;
        txt_page += "\n\n" + tr("Interpolation Table of pipe wall thickness") + " ";
        if (odid.at(0)->m_type == 1) {
            txt_page += QString("[%1, %2]").arg(tr("New")).arg(data.settings_calibr_new);
        } else {
            txt_page += QString("[%1, %2]").arg(tr("Old")).arg(data.settings_calibr_old);
        }
        txt_page += "\n\n";

        txt_page += QString("   OD-ID : Factor   ").repeated(ncols);
        txt_page += "\n";

        QString od_id, factor;
        for (int idx = 0; idx < nrows; ++idx) {
            for (int n = 0; n < ncols; ++n) {
                auto *el = odid.at(idx+(nrows*n));
                od_id  = QStringLiteral("%1").arg(el->m_odid, 0, 'f', 3);
                factor = QStringLiteral("%1").arg(el->m_factor, 0, 'f', 3);
                txt_page += QStringLiteral("%1 : %2 ").arg(UTILS::str_right(od_id, 8), UTILS::str_left(factor, 8));
            }
            txt_page += "\n";
        }
    }


    //-----------------------------------------------------
    txt_page += app_ver;

    //-----------------------------------------------------
    // "шапка" таблицы с данными html отчета
    html_page = QStringLiteral(
                    "\n"
                    "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"600\">\n"
                    "<tr class=\"table_hdr\" align=\"center\">\n"
                    "<td>$NO_LBL</td>\n"
                    "<td>$HV1_LBL</td>\n"
                    "<td>$MODE1_LBL</td>\n"
                    "<td>$TEMP1_LBL</td>\n"
                    "<td>$MODE2_LBL</td>\n"
                    "<td>$TEMP2_LBL</td>\n"
                    "</tr>\n"
                    "<tr><td colspan=\"12\">&nbsp;</td>\n</tr>\n");

    html_page.replace("$NO_LBL",    QString(columns[0].name).replace(SPC, NBSP).replace("(","<br>("));
    html_page.replace("$HV1_LBL",   QString(columns[1].name).replace(SPC, NBSP).replace("(","<br>("));
    html_page.replace("$MODE1_LBL", QString(columns[2].name).replace(SPC, NBSP).replace("(","<br>("));
    html_page.replace("$TEMP1_LBL", QString(columns[3].name).replace(SPC, NBSP).replace("(","<br>("));
    html_page.replace("$MODE2_LBL", QString(columns[4].name).replace(SPC, NBSP).replace("(","<br>("));
    html_page.replace("$TEMP2_LBL", QString(columns[5].name).replace(SPC, NBSP).replace("(","<br>("));

    // данные html отчета
    idx = 0;

    // 1.
    for (const API_DATA &data : data.data) {
        ++idx;
        QString fld_N     = QStringLiteral("%1").arg(idx, 3, 10, QChar(' '));
        QString fld_hv1   = QStringLiteral("%1").arg(data.hv1);
        QString fld_cps1  = QStringLiteral("%1").arg(data.cps1, 8, 'f', 2);
        QString fld_temp1 = QStringLiteral("%1").arg(data.temper1, 5, 'f', 1);
        QString fld_cps2  = QStringLiteral("%1").arg(data.cps2, 8, 'f', 2);
        QString fld_temp2 = QStringLiteral("%1").arg(data.temper2, 5, 'f', 1);
        html_page += QStringLiteral(
                         "<tr class=\"table_data\">"
                         "<td>%1</td>"
                         "<td>%2</td>"
                         "<td>%3</td>"
                         "<td>%4</td>"
                         "<td>%5</td>"
                         "<td>%6</td>"
                         "</tr>\n")
                     .arg(fld_N.replace(SPC, NBSP))
                     .arg(fld_hv1.replace(SPC, NBSP))
                     .arg(fld_cps1.replace(SPC, NBSP))
                     .arg(fld_temp1.replace(SPC, NBSP))
                     .arg(fld_cps2.replace(SPC, NBSP))
                     .arg(fld_temp2.replace(SPC, NBSP));
    }
    html_page += QStringLiteral("</table>\n\n");

    // 2. коэффициенты по толщине трубы
    for(const auto &odid : listOfVectors) {
        int ncols = 4;
        int nrows = odid.size() / ncols;

        QString caption = tr("Interpolation Table of pipe wall thickness") + " ";
        if (odid.at(0)->m_type == 1) {
            caption += QString("[%1, %2]").arg(tr("New")).arg(data.settings_calibr_new);
        } else {
            caption += QString("[%1, %2]").arg(tr("Old")).arg(data.settings_calibr_old);
        }

        //
        html_page += QStringLiteral("<table border=\"0\" cellpadding=\"0\" "
                                    "cellspacing=\"0\" width=\"600\">\n");

        html_page += QStringLiteral("<tr class=\"table_hdr\">\n");
        html_page += QStringLiteral("<td colspan=\"%1\"><br><b>%2</b><br></td>").arg(ncols).arg(caption);
        html_page += QStringLiteral("\n</tr>\n");

        html_page += QStringLiteral("<tr class=\"table_hdr\" align=\"center\">\n");
        html_page += QString("<td>OD-ID : Factor   </td>").repeated(ncols);
        html_page += QStringLiteral("\n</tr>\n");

        html_page += QStringLiteral("<tr><td colspan=\"%1\">&nbsp;</td></tr>\n").arg(ncols);

        QString od_id, factor;
        for (int idx = 0; idx < nrows; ++idx) {
            html_page += QStringLiteral("<tr class=\"table_data\">");
            for (int n = 0; n < ncols; ++n) {
                auto *el = odid.at(idx+(nrows*n));
                od_id  = QStringLiteral("%1").arg(el->m_odid, 0, 'f', 3);
                factor = QStringLiteral("%1").arg(el->m_factor, 0, 'f', 3);
                html_page += QStringLiteral("<td>%1 : %2</td>")
                             .arg(UTILS::str_right(od_id, 8), UTILS::str_left(factor, 8));
            }
            html_page += QStringLiteral("</tr>\n");
        }
        html_page += QStringLiteral("</table>\n\n");
    }

    html_page += QStringLiteral("<p style=\"font-size:8pt; font-style: italic;\">%1\n</p>").arg(app_ver);
    html_page += QStringLiteral("\n</body>\n</html>\n");
}

//---------------------------------------------------------
void API::protocol_write_record(const QString & str)
{
    if (!m_api_log_fd.isOpen()) {
        return;
    }
    if (str.isEmpty()) {
        return;
    }
    if (m_api_log_fd.write(str.toUtf8()) != -1) {
        m_api_log_fd.flush();
        return; // ok
    }
    int error = m_api_log_fd.error();
    QString error_string = m_api_log_fd.errorString();
    QString msg = QString("%1 - (%2) %3")
                  .arg(tr("write data error"))
                  .arg(error)
                  .arg(error > 0 ? error_string : "");
    m_api_log_fd.write(msg.toUtf8()); // :(
}

//---------------------------------------------------------
void API::protocol_make_screenshot()
{
    QString screenshot_fname = m_api_log_fd.fileName();
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
void API_GUI::slot_save_report()
{
    const auto &data = m_api->data();
    if (data.date.isNull() || !data.date.isValid() || data.data.size() == 0) {
        auto msg = QString("%1\n\n%2")
                       .arg(tr("No measurements has been done yet."),
                            tr("Just make test from scratch or \n"
                               "load the previous log."));
        DBG(3, msg);
        display_info_message({}, msg);
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);

    m_api->protocol_filename_create();
    const auto &finfo = m_api->protocol_filename();
    DBG(1, finfo.absoluteFilePath());
    QDir dir = finfo.absoluteDir();
    dir.cdUp();
    m_api->m_settings_last_report_dir = QDir::toNativeSeparators(dir.absolutePath());

    //QString timestamp = data.date.toString("yyyyMMdd-hhmmss");
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
    auto filename = QString("%1/%2/API/API-%3-%4-%5.pdf")
                    .arg(m_api->report_path())
                    .arg(data.toolsn)
                    .arg((true ? "PASS" : "FAIL"))
                    .arg(data.toolsn)
                    .arg(timestamp);
    DBG(3, QDir::toNativeSeparators(filename));

    auto errstr = m_api->save_report(finfo.filePath(), data);

    QApplication::restoreOverrideCursor();

    if (!errstr.isEmpty())
    {
        DBG(1, errstr);
        m_api->protocol_filename_clear();
        display_error_message(tr("Can't create report"), errstr);
        return;
    }

    QString msg = tr("The report has been saved");
    showstatus(QString("%1 - %2").arg(msg, QDir::toNativeSeparators(finfo.filePath())));

    auto s1 = UTILS::split_path(finfo.absolutePath());
    auto s2 = finfo.fileName();

    display_info_message(msg, QString("%1\n\n%2\n%3").arg(msg, s1, s2), finfo.absoluteFilePath());
    m_api->protocol_filename_clear();
}
