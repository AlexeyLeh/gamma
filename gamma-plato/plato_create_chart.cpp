/*!
  \file   create_plato_graph.cpp
  \brief  Создание графиков
  \author Aleksey Lekhtin
  \date   08.11.2016
*/

#include <QtCharts/QCategoryAxis>
#include <QtCharts/QLegendMarker>

#include "mainwindow.h"
#include "plato_gui.h"

//---------------------------------------------------------
void PLATO_GUI::create_chart()
{
    m_chart = new QChart();
    m_chart->setMargins(QMargins(0,0,0,0));
    m_chart->setTitle(QString("%1").arg(tr("PLATEAU DATA")));
    m_chart->setDropShadowEnabled(true);
    m_chart->setAcceptHoverEvents(true);
    m_chart->setMargins(QMargins(0, 0, 0, 0));
    m_chart->setAnimationOptions(QChart::NoAnimation);
    //m_chart->setAnimationOptions(QChart::AllAnimations);
    //m_chart->setAnimationOptions(QChart::GridAxisAnimations);
    //m_chart->setAnimationOptions(QChart::SeriesAnimations);

    //---------------------------------------------------------
    m_chartview = new QChartView(m_chart);
    //m_chartview->setRenderHint(QPainter::Antialiasing);
    m_chartview->setRubberBand(QChartView::NoRubberBand /*QChartView::RectangleRubberBand*/);

    //---------------------------------------------------------
    m_axisX = new QValueAxis();
    m_axisX->setTitleText(tr("HV (volts)"));
    m_axisX->setLabelFormat("%4d");
    //// m_axisX->setTickCount((m_settings_graph_x_max - m_settings_graph_x_min)/50 + 1);
    m_axisX->setTickType(QtCharts::QValueAxis::TicksDynamic);
    m_axisX->setTickAnchor(m_settings_graph_x_min);
    m_axisX->setTickInterval(50.0);
    m_axisX->setMinorTickCount(1);
    m_axisX->setMinorGridLineVisible(true);
    m_axisX->setRange(m_settings_graph_x_min, m_settings_graph_x_max);
    m_chart->addAxis(m_axisX, Qt::AlignBottom);

    //---------------------------------------------------------
    m_axisY = new QValueAxis();
    m_axisY->setTitleText(QString("%1 (%2)").arg(tr("Gamma"), tr("CPS")));
    m_axisY->setLabelFormat("%4d");
    ////m_axisY->setTickCount(m_settings_graph_y_tick);
    m_axisY->setTickType(QtCharts::QValueAxis::TicksDynamic);
    m_axisY->setTickAnchor(m_settings_graph_y_min);
    m_axisY->setTickInterval(50.0);
    m_axisY->setRange(m_settings_graph_y_min, m_settings_graph_y_max);
    m_axisY->setMinorTickCount(1);
    m_axisY->setMinorGridLineVisible(true);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    //---------------------------------------------------------
    m_axisY2 = new QValueAxis();
    m_axisY2->setTitleText(QString("%1°C").arg(tr("Temperature")));
    m_axisY2->setLabelFormat("%4d");
    //// m_axisY2->setTickCount(m_settings_graph_y2_tick);
    m_axisY2->setTickType(QtCharts::QValueAxis::TicksDynamic);
    m_axisY2->setTickAnchor(m_settings_graph_y2_min);
    m_axisY2->setTickInterval(10.0);
    m_axisY2->setRange(m_settings_graph_y2_min, m_settings_graph_y2_max);
    m_axisY2->setGridLineVisible(false);
    m_axisY2->setMinorGridLineVisible(false);
    m_chart->addAxis(m_axisY2, Qt::AlignRight);

    //---------------------------------------------------------
    QPen dotted_pen;
    // dotted_pen.setStyle(Qt::DotLine);
    dotted_pen.setStyle(Qt::DashDotLine);
    dotted_pen.setWidth(2);
    dotted_pen.setColor(Qt::black);

    m_series_HV_lo_limit = new QLineSeries();
    m_chart->addSeries(m_series_HV_lo_limit);
    m_series_HV_lo_limit->setName(tr("HV Low Limit"));
    m_series_HV_lo_limit->attachAxis(m_axisX);
    m_series_HV_lo_limit->attachAxis(m_axisY);
    m_series_HV_lo_limit->setPointsVisible(false);
    m_series_HV_lo_limit->setPen(dotted_pen);
    //// m_series_HV_lo_limit->setColor(Qt::red);

    m_series_HV_hi_limit = new QLineSeries();
    m_chart->addSeries(m_series_HV_hi_limit);
    m_series_HV_hi_limit->setName(tr("HV High Limit"));
    m_series_HV_hi_limit->attachAxis(m_axisX);
    m_series_HV_hi_limit->attachAxis(m_axisY);
    m_series_HV_hi_limit->setPointsVisible(false);
    m_series_HV_hi_limit->setPen(dotted_pen);
    //// m_series_HV_hi_limit->setColor(Qt::red);

    //---------------------------------------------------------
    m_series_CPS_lo_limit = new QLineSeries();
    m_series_CPS_lo_limit->setName(tr("CPS Low Limit"));
    m_chart->addSeries(m_series_CPS_lo_limit);
    m_series_CPS_lo_limit->attachAxis(m_axisX);
    m_series_CPS_lo_limit->attachAxis(m_axisY);
    m_series_CPS_lo_limit->setPointsVisible(false);
    m_series_CPS_lo_limit->setPen(dotted_pen);
    //// m_series_CPS_lo_limit->setColor(Qt::red);

    m_series_CPS_hi_limit = new QLineSeries();
    m_series_CPS_hi_limit->setName(tr("CPS High Limit"));
    m_chart->addSeries(m_series_CPS_hi_limit);
    m_series_CPS_hi_limit->attachAxis(m_axisX);
    m_series_CPS_hi_limit->attachAxis(m_axisY);
    m_series_CPS_hi_limit->setPointsVisible(false);
    m_series_CPS_hi_limit->setPen(dotted_pen);
    //// m_series_CPS_hi_limit->setColor(Qt::red);

    //---------------------------------------------------------
    m_series_cold_CPS = new QLineSeries();
    m_chart->addSeries(m_series_cold_CPS);
    m_series_cold_CPS->setName(tr("COLD MODE"));
    m_series_cold_CPS->attachAxis(m_axisX);
    m_series_cold_CPS->attachAxis(m_axisY);
    m_series_cold_CPS->setPointsVisible(true);
    m_series_cold_CPS->setColor(Qt::blue);

    //---------------------------------------------------------
    m_series_hot_CPS = new QLineSeries();
    m_chart->addSeries(m_series_hot_CPS);
    m_series_hot_CPS->setName(tr("HOT MODE"));
    m_series_hot_CPS->attachAxis(m_axisX);
    m_series_hot_CPS->attachAxis(m_axisY);
    m_series_hot_CPS->setPointsVisible(true);
    m_series_hot_CPS->setColor(Qt::red);

    //---------------------------------------------------------
    QPen pen_hv_accepted(dotted_pen);
    pen_hv_accepted.setStyle(Qt::DotLine);
    pen_hv_accepted.setWidth(4);

    pen_hv_accepted.setColor(m_series_cold_CPS->color());
    m_series_cold_HV_lo_border = new QLineSeries();
    m_chart->addSeries(m_series_cold_HV_lo_border);
    m_series_cold_HV_lo_border->setName(tr("Lo HV (COLD)"));
    m_series_cold_HV_lo_border->attachAxis(m_axisX);
    m_series_cold_HV_lo_border->attachAxis(m_axisY);
    m_series_cold_HV_lo_border->setPointsVisible(false);
    m_series_cold_HV_lo_border->setPen(pen_hv_accepted);

    m_series_cold_HV_hi_border = new QLineSeries();
    m_chart->addSeries(m_series_cold_HV_hi_border);
    m_series_cold_HV_hi_border->setName(tr("Hi HV (COLD)"));
    m_series_cold_HV_hi_border->attachAxis(m_axisX);
    m_series_cold_HV_hi_border->attachAxis(m_axisY);
    m_series_cold_HV_hi_border->setPointsVisible(false);
    m_series_cold_HV_hi_border->setPen(pen_hv_accepted);

    //
    pen_hv_accepted.setColor(m_series_hot_CPS->color());
    m_series_hot_HV_lo_border = new QLineSeries();
    m_chart->addSeries(m_series_hot_HV_lo_border);
    m_series_hot_HV_lo_border->setName(tr("Lo HV (HOT)"));
    m_series_hot_HV_lo_border->attachAxis(m_axisX);
    m_series_hot_HV_lo_border->attachAxis(m_axisY);
    m_series_hot_HV_lo_border->setPointsVisible(false);
    m_series_hot_HV_lo_border->setPen(pen_hv_accepted);

    m_series_hot_HV_hi_border = new QLineSeries();
    m_chart->addSeries(m_series_hot_HV_hi_border);
    m_series_hot_HV_hi_border->setName(tr("Hi HV (HOT)"));
    m_series_hot_HV_hi_border->attachAxis(m_axisX);
    m_series_hot_HV_hi_border->attachAxis(m_axisY);
    m_series_hot_HV_hi_border->setPointsVisible(false);
    m_series_hot_HV_hi_border->setPen(pen_hv_accepted);

    //
    QPen pen_final_accepted(pen_hv_accepted);
    pen_final_accepted.setWidth(6);
    //// pen_accepted.setColor(Qt::green);
    pen_final_accepted.setColor(QColor(QRgb(0x33CC66)));

    m_series_HV_accepted = new QLineSeries();
    m_chart->addSeries(m_series_HV_accepted);
    m_series_HV_accepted->setName(tr("Calibration HV"));
    m_series_HV_accepted->attachAxis(m_axisX);
    m_series_HV_accepted->attachAxis(m_axisY);
    m_series_HV_accepted->setPointsVisible(false);
    m_series_HV_accepted->setPen(pen_final_accepted);

    //---------------------------------------------------------
    QPen pen;
    m_series_cold_temper = new QLineSeries();
    m_series_cold_temper->setName(tr("COLD MODE TEMP"));
    m_chart->addSeries(m_series_cold_temper);
    m_series_cold_temper->attachAxis(m_axisX);
    m_series_cold_temper->attachAxis(m_axisY2);
    // m_series_cold_temper->setPointsVisible(true);
    m_series_cold_temper->setColor(Qt::blue); // Qt::darkGreen  // QColor(0x00, 0x00, 0x90)
    pen = m_series_cold_temper->pen();
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(3);
    m_series_cold_temper->setPen(pen);

    //---------------------------------------------------------
    m_series_hot_temper = new QLineSeries();
    m_series_hot_temper->setName(tr("HOT MODE TEMP"));
    m_chart->addSeries(m_series_hot_temper);
    m_series_hot_temper->attachAxis(m_axisX);
    m_series_hot_temper->attachAxis(m_axisY2);
    // m_series_hot_temper->setPointsVisible(true);
    m_series_hot_temper->setColor(Qt::magenta); // QColor("orange") //Qt::darkRed // QColor(0xeb, 0x80, 0x00)
    pen = m_series_hot_temper->pen();
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(3);
    m_series_hot_temper->setPen(pen);

    //---------------------------------------------------------
    m_chart->legend()->hide();
#if 0
    m_chart->legend()->show();
    //m_chart->legend()->detachFromChart();
    //m_chart->legend()->setBackgroundVisible(true);
    m_chart->legend()->setBrush(QBrush(QColor(250, 250, 250, 192)));
    m_chart->legend()->setPen(QPen(QColor(255, 255, 255, 255)));
    m_chart->legend()->setBorderColor(QColor(192, 192, 192, 255));
    m_chart->legend()->setShowToolTips(true);
    m_chart->legend()->setAlignment(Qt::AlignRight);
#endif

#if 0
    QFont fnt = m_chart->font();
    QFontMetrics fm(fnt);
    int ww = fm.boundingRect(m_series_cnts1->name()).width();
    ww = qMax(ww, fm.boundingRect(m_series_temp1->name()).width());
    m_chart->legend()->setGeometry(fm.maxWidth()*3,
                                    fm.lineSpacing()*2,
                                    ww + 50,
                                    fm.lineSpacing()*6);
#endif
}

//---------------------------------------------------------
auto PLATO_GUI::axis_x()
{
    return m_axisX;
}

//---------------------------------------------------------
auto PLATO_GUI::axis_y()
{
    return m_axisY;
}

//---------------------------------------------------------
auto PLATO_GUI::axis_y2()
{
    return m_axisY2;
}

//---------------------------------------------------------
int PLATO_GUI::axis_x_min()
{
    return m_axisX->min();
}

//---------------------------------------------------------
int PLATO_GUI::axis_x_max()
{
    return m_axisX->max();
}

//---------------------------------------------------------
int PLATO_GUI::axis_x_ticks()
{
    return m_axisX->tickCount();
}

//---------------------------------------------------------
int PLATO_GUI::axis_y_min()
{
    return m_axisY->min();
}

//---------------------------------------------------------
int PLATO_GUI::axis_y_max()
{
    return m_axisY->max();
}

//---------------------------------------------------------
int PLATO_GUI::axis_y_ticks()
{
    return m_axisY->tickCount();
}

//---------------------------------------------------------
int PLATO_GUI::axis_y2_min()
{
    return m_axisY2->min();
}

//---------------------------------------------------------
int PLATO_GUI::axis_y2_max()
{
    return m_axisY2->max();
}

//---------------------------------------------------------
int PLATO_GUI::axis_y2_ticks()
{
    return m_axisY2->tickCount();
}

//---------------------------------------------------------
void PLATO_GUI::set_chart_x_range(int x_min, int x_max)
{
    ////m_axisY->setTickCount(x_tick);
    m_axisX->setRange(x_min, x_max);
    m_axisX->setTickType(QtCharts::QValueAxis::TicksDynamic);
    m_axisX->setTickAnchor(x_min);
    m_axisX->setTickInterval(50.0);
}

//---------------------------------------------------------
void PLATO_GUI::set_chart_y_range(int y_min, int y_max)
{
    //// m_graph_y_min = y_min;
    //// m_graph_y_max = y_max;
    //// m_axisY->setTickCount(y_tick);
    m_axisY->setTickType(QtCharts::QValueAxis::TicksDynamic);
    m_axisY->setTickAnchor(y_min);
    m_axisY->setTickInterval(50.0);
    m_axisY->setRange(y_min, y_max);
}

//---------------------------------------------------------
void PLATO_GUI::set_chart_y2_range(int y_min, int y_max)
{
    m_axisY2->setTickType(QtCharts::QValueAxis::TicksDynamic);
    m_axisY2->setTickAnchor(y_min);
    m_axisY2->setTickInterval(10.0);
    m_axisY2->setRange(y_min, y_max);
}

//---------------------------------------------------------
void PLATO_GUI::set_chart_y_min(int y_min)
{
    if (y_min < axis_y2_min()) {
        int max = static_cast<int>(m_axisY->max());
        max = std::max(max, m_plato->m_settings_CPS_HiLimit);
        max = std::ceil(max / 50) * 50;

        double min = std::floor(y_min / 50.0) * 50.0;

        m_axisY->setTickType(QtCharts::QValueAxis::TicksDynamic);
        m_axisY->setTickAnchor(min);
        m_axisY->setTickInterval(50.0);
        m_axisY->setRange(min, max);

        m_series_HV_lo_limit->clear();
        m_series_HV_lo_limit->append(m_plato->m_hv_lo_limit, m_axisY->min());
        m_series_HV_lo_limit->append(m_plato->m_hv_lo_limit, m_axisY->max());

        m_series_HV_hi_limit->clear();
        m_series_HV_hi_limit->append(m_plato->m_hv_hi_limit, m_axisY->min());
        m_series_HV_hi_limit->append(m_plato->m_hv_hi_limit, m_axisY->max());

    }
}
