/*!
  \file   plato_log_template.cpp
  \brief  Шаблон страницы отчета
  \author Aleksey Lekhtin
  \date   created 19.05.2017
*/

#include "api.h"

// Шаблон заголовка отчета
QString API::protocol_make_html_template_header() const
{
    auto page = QStringLiteral(
        "<!DOCTYPE HTML>\n"
        "<html>\n"
        "<head>\n"
        "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n"
        "<title>$TITLE0</title>\n"
        "<style>\n"
        "body {\n"
        "    font-family: Times New Roman; font-size: 10pt;\n"
        "    font-size: 100%;\n"
        "    max-width: 800px;\n"
        "    margin: 0 auto;\n"
        "    zoom: 0.8;\n"
        "}\n\n"
        "table {\n"
        "    border: 0;\n"
        "    border-collapse:collapse;\n"
        "    border: none !important;\n"
        "    border-spacing: 0;\n"
        "    border-padding: 0;\n"
        "}\n\n"
        ".descr {\n"
        "    font-family: Times New Roman; font-size: 11pt;\n"
        "    padding-left: 0px;\n"
        "    padding-right: 0px;\n"
        "    padding-top: 0px;\n"
        "    padding-bottom: 0px;\n"
        "    border: none;\n"
        "    border-collapse: collapse;\n"
        "    text-align: left;\n"
        "}\n\n"
        ".table_hdr {\n"
        "    font-family: Times New Roman; font-size: 12pt;\n"
        "    padding-left: 4px;\n"
        "    padding-right: 4px;\n"
        "    padding-top: 0px;\n"
        "    padding-bottom: 0px;\n"
        "    border: none;\n"
        "    border-collapse: collapse;\n"
        "    text-align: center;\n"
        "    vertical-align: top;\n"
        "}\n\n"
        ".table_data {\n"
        "    font-family: Times New Roman; font-size: 12pt;\n"
        "    padding-left: 4px;\n"
        "    padding-right: 4px;\n"
        "    padding-top: 0px;\n"
        "    padding-bottom: 0px;\n"
        "    border: none;\n"
        "    border-collapse: collapse;\n"
        "    text-align: center;\n"
        "    vertical-align: middle;\n"
        "}\n"
        "</style>\n"
        "</head>\n"
        "\n"

        "<body>\n"
        "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n"

        "<tr class=\"descr\">\n"
        "<td>$PASSED_LBL</td>\n"
        "<td>&nbsp;:&nbsp;</td>\n"
        "<td>$PASSED&nbsp;$SPCITEM</td>\n"
        "<td>$TOOLNAME_LBL</td>\n"
        "<td>&nbsp;:&nbsp;</td>\n"
        "<td>$TOOLNAME</td>\n"
        "</tr>\n"

        "<tr class=\"descr\">\n"
        "<td>$CALIBR_DATE_LBL</td>\n"
        "<td>&nbsp;:&nbsp;</td>\n"
        "<td>$CALIBR_DATE&nbsp;$SPCITEM</td>\n"
        "<td>$TOOLPN_LBL</td>\n"
        "<td>&nbsp;:&nbsp;</td>\n"
        "<td>$TOOLPN</td>\n"
        "</tr>\n"

        "<tr class=\"descr\">\n"
        "<td>$ACQMETHOD_LBL</td>\n"
        "<td>&nbsp;:&nbsp;</td>\n"
        "<td>$ACQMETHOD&nbsp;$SPCITEM</td>\n"
        "<td>$TOOLSN_LBL</td>\n"
        "<td>&nbsp;:&nbsp;</td>\n"
        "<td>$TOOLSN</td>\n"
        "</tr>\n"

        "<tr class=\"descr\">\n"
        "<td>$KF229_LBL</td>\n"
        "<td>&nbsp;:&nbsp;</td>\n"
        "<td>$KF229&nbsp;$SPCITEM</td>\n"
        "<td>$TEMP_LBL</td>\n"
        "<td>&nbsp;:&nbsp;</td>\n"
        "<td>$TEMP</td>\n"
        "</tr>\n"

        "<tr class=\"descr\">\n"
        "<td>$TOL_LBL</td>\n"
        "<td>&nbsp;:&nbsp;</td>\n"
        "<td>$TOL&nbsp;$SPCITEM</td>\n"
        "<td>$HV_LBL</td>\n"
        "<td>&nbsp;:&nbsp;</td>\n"
        "<td>$HV</td>\n"
        "</tr>\n"

        "<tr class=\"descr\">\n"
        "<td>$OPER_LBL</td>\n"
        "<td>&nbsp;:&nbsp;</td>\n"
        "<td>$OPER_NAME&nbsp;$SPCITEM</td>\n"
        "<td>&nbsp;</td>\n"
        "<td>&nbsp;</td>\n"
        "<td>&nbsp;</td>\n"
        "</tr>\n"

        "</table>\n"

        "<br>\n\n");
    return page;
}


