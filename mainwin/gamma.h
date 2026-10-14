/*!
  \file   gamma.h
  \author Aleksey Lekhtin
  \date   20.10.2016
  \brief  Константы и граничные значения для тестирования гамма-модуля
*/

#ifndef __GAMMA_H_
#define __GAMMA_H_

namespace GAMMA
{
    static constexpr const char *PN_00                  = "0000000000";
    static constexpr const char *SN_00                  = "000000000000";
    static constexpr const char *ESKD_00                = "EC.00.00.00.00.000";
    static constexpr int        SN_LEN                  = 12;

    // тест линейности
    static constexpr int    HV_Min                      = 800;
    static constexpr int    HV_Max                      = 1800;

    static constexpr int    HV_Lo_Limit                 = 1000;
    static constexpr int    HV_Hi_Limit                 = 1700;
    static constexpr int    HV_Spread_Limit             = 150;

    static constexpr int    CPS_Lo_Limit                = 250;
    static constexpr int    CPS_Hi_Limit                = 600;
    static constexpr int    CPS_Spread_Limit            = 50;

    static constexpr int    HV_Chart_Lo                 = 800;
    static constexpr int    HV_Chart_Hi                 = 1800;
    static constexpr int    HV_Chart_Tick               = 21;

    static constexpr int    Chart_CPS_Min               = 200; // 0
    static constexpr int    Chart_CPS_Max               = 700;
    static constexpr int    Chart_CPS_Ticks             = 8;

    static constexpr int    Chart_Temper_Min            = 20;
    static constexpr int    Chart_Temper_Max            = 100;
    static constexpr int    Chart_Temper_Tick           = 17;

    static constexpr int    HV_Test_Lo                  = 850;
    static constexpr int    HV_Test_Hi                  = 1750;
    static constexpr int    HV_Test_Step                = 25;

    static constexpr double HV_Proportionality          = 0.65;
    static constexpr double CPS_SlopeMin                = 0.2;
    static constexpr double CPS_SlopeMax                = 0.55;
    static constexpr double CPS_SlopeCutOff             = 2.3;
    static constexpr int    Slope_Calc_Points           = 5;
    static constexpr int    HOT_Mode                    = 80;    // пороговая температура
    static constexpr int    WarmUp                      = 60;    // время "прогрева" устаовленного напряжения

    // API тест
    static constexpr int    API_Points                  = 10;
    static constexpr double API_Calibr_Koef_Min         = 0.68;
    static constexpr double API_Calibr_Koef_Max         = 1.6;
    static constexpr int    API_Calibr_Koef_Old         = 200;
    static constexpr int    API_Calibr_Koef_New         = 202; // 229 => предыдущ. рачсет до 10.02.2026
    static constexpr double API_Calibr_Koef_Change_Min  = 0.0;
    static constexpr double API_Calibr_Koef_Change_Max  = 8.0;
    static constexpr double API_Tol_Deviat0_Min         = 0.0;
    static constexpr double API_Tol_Deviat0_Max         = 10.0;
    static constexpr double API_Tol_Deviat200_Min       = 0.0;
    static constexpr double API_Tol_Deviat200_Max       = 25.0;
    static constexpr double ODID_Beg                    = 0.375;
    static constexpr double ODID_End                    = 7.313;
    static constexpr double ODID_Step                   = 0.0625;
}

#endif // __GAMMA_H_
