/*!
  \file   api_simulate.cpp
  \author Aleksey Lekhtin
  \date   02.10.2026
  \brief  API-симуляция гамма-модуля
*/

#include "api.h"
#include "api_gui.h"

//---------------------------------------------------------
bool API::simulation()
{
    return m_app->simulation();
}

//---------------------------------------------------------
void API::do_simulation()
{
    if (!simulation()) // for safe
    {
        QApplication::restoreOverrideCursor();
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        m_test_stage = {API::CANCEL, 0}; // отменено
        m_timer_delay = 0;
        return;
    }

    gui()->set_run_label();

    if (m_test_stage.step == 0)
    {
        init_simulation_data(m_sim_data);

        m_data.data.clear();
        m_data.toolpn = m_sim_data.toolpn;
        m_data.toolsn = m_sim_data.toolsn;
        m_data.oper = m_sim_data.oper;
        gui()->setup_titles(m_data);

        m_api_mode = API::api_mode_t::API_0;
        m_test_stage.step = 1;
    }
    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(Qt::ArrowCursor);

    if (m_test_stage.step == 1)
    {
        m_current_step = 0;
        gui()->update_field_steps__done(m_current_step, m_current_step, m_total_steps);

        auto ok = gui()->message_about_selected_api_source(m_api_mode);
        if (!ok)
        {
            m_test_stage = {API::CANCEL, 0}; // отменено
            m_timer_delay = 0;
            return;
        }

        for (const auto& sim : m_sim_data.data)
        {
            if (m_api_mode == API::api_mode_t::API_0)
            {
                nodeE4()->m_operating_hv = sim.hv1;
                nodeE4()->m_temperature_value = sim.temper1;
                nodeE4()->m_gamma_value = sim.cps1;
                nodeE4()->m_gamma_acq_status = 0;
            }
            else // API::api_mode_t::API_200
            {
                nodeE4()->m_operating_hv = sim.hv2;
                nodeE4()->m_temperature_value = sim.temper2;
                nodeE4()->m_gamma_value = sim.cps2;
                nodeE4()->m_gamma_acq_status = 0;
            }
            gui()->update_fields(0x01); // gamma
            gui()->update_fields(0x05); // temperature
            gui()->update_fields(0x43); // hv

            remember_CPS();
            update_table();
            calculate_cps_average();

            ++m_current_step;
            gui()->update_field_steps__done(m_current_step, m_current_step, m_total_steps);
        }

        if (m_api_mode == API::api_mode_t::API_0)
        {
            // закончили c 0 API - переходим к 200 API
            m_api_mode = API::api_mode_t::API_200;
            m_test_stage = {API::SIMULATION, 1};
            m_timer_delay = 0;
            return;
        }

        m_test_stage = {API::CALCULATIONS, 0};
        m_timer_delay = 0;
        return;
    }
    m_test_stage = {API::CANCEL, 0}; // отменено
    m_timer_delay = 0;
}

//---------------------------------------------------------
void API::init_simulation_data(api_data_t &d)
{
    d.clear();
    if (!simulation()) return;

    auto dt = QDateTime::currentDateTime();
    int hv = nodeE4()->m_operating_hv;
    double temp = nodeE4()->m_temperature_value;

    d.date = dt;
    d.toolpn = nodeE4()->m_partnumber;
    d.toolsn = "00001A5EBACA";
    d.oper = app()->Operator();
    d.operating_hv = hv;
    d.settings_calibr_min        = m_settings_calibr_min;
    d.settings_calibr_max        = m_settings_calibr_max;
    d.settings_calibr_old        = m_settings_calibr_old;
    d.settings_calibr_new        = m_settings_calibr_new;
    d.settings_tol_deviat0_min   = m_settings_tol_deviat0_min;
    d.settings_tol_deviat0_max   = m_settings_tol_deviat0_max;
    d.settings_tol_deviat200_min = m_settings_tol_deviat200_min;
    d.settings_tol_deviat200_max = m_settings_tol_deviat200_max;

    d.data = {
        {dt, hv, 56.65, temp, dt, hv, 304.68, temp},
        {dt, hv, 56.92, temp, dt, hv, 307.58, temp},
        {dt, hv, 57.43, temp, dt, hv, 307.93, temp},
        {dt, hv, 56.67, temp, dt, hv, 312.07, temp},
        {dt, hv, 56.27, temp, dt, hv, 308.50, temp},
        {dt, hv, 57.15, temp, dt, hv, 310.73, temp},
        {dt, hv, 57.47, temp, dt, hv, 310.88, temp},
        {dt, hv, 57.92, temp, dt, hv, 310.28, temp},
        {dt, hv, 56.53, temp, dt, hv, 307.62, temp},
        {dt, hv, 57.20, temp, dt, hv, 311.50, temp},
    };
}
