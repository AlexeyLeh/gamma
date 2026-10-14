/*!
  \file   api_data.h
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Данные для отчета измерений
*/

#ifndef _API_DATA_H_
#define _API_DATA_H_

#include <QDateTime>
#include <QString>
#include <QtMath>

class API_DATA
{
public:
    API_DATA()
    {
    }

    API_DATA(const QDateTime &dt_1, int hv_cold, qreal cps_cold, qreal temp_cold,
             const QDateTime &dt_2, int hv_hot,  qreal cps_hot,  qreal temp_hot)
    {
        dt      = dt_1;
        hv1     = hv_cold;
        cps1    = cps_cold;
        temper1 = temp_cold;
        dt2     = dt_2;
        hv2     = hv_hot;
        cps2    = cps_hot;
        temper2 = temp_hot;
    }

    ~API_DATA()
    {
    }

    //-------------------------------------------------
    // Оператор "меньше" по температуре "холодного" теста
    // \param el1[in] - элемент данных
    // \param el2[in] - элемент данных
    // \return
    // - true - если меньше
    // - иначе - false
    static bool temp1_less_than(API_DATA el1, API_DATA el2 )
    {
        return ( (el1.temper1 < el2.temper1) ? true : false );
    }

    //-------------------------------------------------
    //! Оператор "меньше" по температуре "горячего" тест
    // \param el1[in] - элемент данных
    // \param el2[in] - элемент данных
    // \return
    // - true - если меньше
    // - иначе - false
    static bool temp2_less_than( API_DATA el1, API_DATA el2 )
    {
        return ( (el1.temper2 < el2.temper2) ? true : false );
    }

public:
    QDateTime   dt{};
    int         hv1{};
    double      cps1{};
    double      temper1{};
    ushort      status1{}; // не используется
    QDateTime   dt2{};
    int         hv2{};
    double      cps2{};
    double      temper2{};
    ushort      status2{}; // не используется
};

class ODID
{
public:
    ODID() :
        m_odid(0),
        m_factor(0)
    {
    }

    ODID(int type, double odid, double factor)
    {
        m_type   = type;    // 0 = old, 1 - new
        m_odid   = odid;
        m_factor = factor;
    }

    ~ODID()
    {
    }

    void clear()
    {
        m_odid   = 0.0;
        m_factor = 0.0;
    }

    static std::string api_factor_version() {
        return "v2.0";
    }

    static double calculate_api_old_method(double od_id) {
        double k = 0.300872941764994 + 0.335001052605634 * od_id;
        auto value = qExp(k);
        return value;
    }

    static double calculate_api_new_method(double Xi) {
        // один из вариантов расчета - отложен 10.02.2026
        // double v5 = -0.000120652963759 * (Xi * Xi * Xi * Xi * Xi); // x^5
        // double v4 =  0.001299639395787 * (Xi * Xi * Xi * Xi); // x^4
        // double v3 =  0.007535400530002 * (Xi * Xi * Xi); // x^3
        // double v2 =  0.103126821063576 * (Xi * Xi); // x^2
        // double v1 =  0.471861807512432 * (Xi);
        // double v0 =  1.00086711220352;
        // auto value = v5 + v4 + v3 + v2 + v1 + v0;
        // return value;

        // 10.02.2026
        // По новому GammaCal:
        // CalFactor_new = 202/( CPS200 - CPS0)
        // Yi = CalFactor_new * (
        //     0.00198217825017366*Xi^5 -
        //     0.0273686875261614*Xi^4 +
        //     0.117526184999962*Xi^3 +
        //     0.0258875881786809*Xi*Xi +
        //     0.379204696558935*Xi +
        //     0.875992903785928)
        double v5 = 0.00198217825017366 * (Xi * Xi * Xi * Xi * Xi); // x^5
        double v4 = 0.0273686875261614  * (Xi * Xi * Xi * Xi); // x^4
        double v3 = 0.117526184999962   * (Xi * Xi * Xi); // x^3
        double v2 = 0.0258875881786809  * (Xi * Xi); // x^2
        double v1 = 0.379204696558935   * (Xi);
        double v0 = 0.875992903785928;
        auto value = v5 - v4 + v3 + v2 + v1 + v0;
        return value;
    }

public:
    int         m_type;     // 0 = old, 1 - new
    double      m_odid;
    double      m_factor;
};

#endif // _API_DATA_H_
