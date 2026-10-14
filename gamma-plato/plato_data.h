/*!
  \file   plato_data.h
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Данные для отчета измерений
*/

#ifndef _PLATO_DATA_H_
#define _PLATO_DATA_H_

#include <QString>
#include <QDateTime>

class PLATO_DATA
{
public:
    PLATO_DATA() :
        m_dt(),
        m_hv(0),
        m_cps(0),
        m_temper(0),
        m_slope(0)
    {
    }

    PLATO_DATA(const QDateTime& dt, int hv, qreal cps, qreal temp, qreal slope)
    {
        m_dt    = dt;
        m_hv    = hv;
        m_cps   = cps;
        m_temper= temp;
        m_slope = slope;
    }

    ~PLATO_DATA()
    {
    }

    //-------------------------------------------------
    // Оператор "меньше" по температуре "холодного" теста
    // \param el1[in] - элемент данных
    // \param el2[in] - элемент данных
    // \return
    // - true - если меньше
    // - иначе - false
    static bool temp_less_than(PLATO_DATA el1, PLATO_DATA el2)
    {
        return ( (el1.m_temper < el2.m_temper) ? true : false );
    }

    static bool temp_ptr_less_than(PLATO_DATA *el1, PLATO_DATA *el2)
    {
        return ( (el1->m_temper < el2->m_temper) ? true : false );
    }

public:
    QDateTime   m_dt;
    int         m_hv;
    float       m_cps;
    float       m_temper;
    float       m_slope;
};

#endif // _PLATO_DATA_H_
