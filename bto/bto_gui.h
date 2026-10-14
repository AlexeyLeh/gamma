/*!
  \file   bto_gui.h
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Отображение информации с БТО
*/

#ifndef __MAINTENANCE_UNIT_GUI_H_
#define __MAINTENANCE_UNIT_GUI_H_

#include <QObject>
#include <QIcon>
#include <QLabel>
#include <QFrame>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QStackedWidget>
#include <QTableWidgetItem>

#include "bto.h"
#include "comm.h"
#include "lineedit.h"
#include "mainwindow.h"
#include "NodeBA.h"

class BTO;
class COMM;
class APP;

class BTO_GUI : public QObject {
    Q_OBJECT;

public:
    static constexpr const char *amperage_text_limit       = QT_TRANSLATE_NOOP("BTO_GUI", "Amperage limit");
    static constexpr const char *amperage_text_consumption = QT_TRANSLATE_NOOP("BTO_GUI", "Amperage consumption");
    static constexpr const char *gamma_power_button_text   = QT_TRANSLATE_NOOP("BTO_GUI", "GAMMA SONDE\nPOWER");
    static constexpr const char *module_not_powered        = QT_TRANSLATE_NOOP("BTO_GUI", "Not powered\n");
    static constexpr const char *module_not_found          = QT_TRANSLATE_NOOP("BTO_GUI", "Not found\n");
    static constexpr const char *module_search             = QT_TRANSLATE_NOOP("BTO_GUI", "Searching for\nGamma module");

public:
    BTO_GUI(APP *app);
    ~BTO_GUI();

    inline APP     *app()  const { return m_app; }
    inline MW      *mw()   const { return m_app->mw(); }
    inline COMM    *comm() const { return m_app->comm(); }
    inline QWidget *page() const { return m_gui_page; }

    bool eventFilter(QObject *obj, QEvent *event);

    void            set_backgr(BTO *bto);
    void            set_content_font_size(int sz);
    void            run();
    void            set_requests_list(int mode);
    COMM::CONNECT_TYPE connection_type() const;

    void            showstatus(const QString &msg = {}, int sec = 5) const;

    void            connected_event(const QString &msg);
    void            disconnected_event(int err = 0, const QString &msg = "");
    void            requests_completed(int nodecode);

    void            show_bto_settings();
    static int      power_output_switch_to_index(NodeBA::PowerOutputType sw);
    NodeBA::PowerOutputType get_power_output_switch_from_settings() const;
    void            show_amperage_limit_setup();
    void            hide_amperage_limit_setup();

    void            set_led_icon_inactive();
    void            set_led_icon_active();
    void            set_led_icon_light_active();

    void            clear_NodeBA_fields();
    void            clear_nodeE4_fields();
    void            clear_NodeE4_PWR_fields();
    void            update_NodeBA_fields(byte cmd, byte ext);
    void            update_nodeE4_fields(byte cmd, byte ext);
    void            update_gamma_status(const BTO::GAMMA_STATUS status);
    void            set_gamma_power_button_status(bool status);
    void            set_enabled_gamma_power_button(bool status);
    void            update_poweron_button_state();

    void            progress_bar_connection_show();
    void            progress_bar_connection_set_max();
    void            progress_bar_connection_close();
    void            progress_bar_connection_settext_only(const QString &msg);

    void            progress_bar_message_show(const QString &msg);
    void            progress_bar_message_close();
    void            progress_bar_message_settext(const QString &msg);

    void            progress_bar_apply_setting_show();
    void            progress_bar_apply_setting_close();

private:
    void            update_BA_required_voltage();
    void            update_BA_output_voltage();
    void            update_BA_amperage();
    void            update_BA_power_output_sw();
    void            update_BA_status();
    void            update_BA_partnumber();

    void            update_E4_field_0x01();
    void            update_E4_field_0x05();
    void            update_E4_field_0x43();
    void            update_E4_field_0x1e(byte ext);
    void            update_E4_field_0xfc();

private:
    QWidget         *create_bto_page();
    QWidget         *create_led_icon_box();
    QWidget         *create_gamma_page();
    QWidget         *create_poweron_buttons_box();
    QGroupBox       *create_current_values_box();
    QGroupBox       *create_settings_box();

    void            disable_all_controls();
    void            enable_all_controls();

    void            set_poweron_button_text();
    void            update_bto_settings_button();

signals:

private slots:
    void            slot_connect_button();
    void            slot_gamma_poweron_button();
    void            slot_bto_set_power_output_sw(int index);
    void            slot_bto_set_amperage_limit(double value);
    void            slot_bto_set_voltage(double value);
    void            slot_bto_set_settings();
    void            slot_progress_bar_connection_cancel();
    void            slot_progress_bar_connection_action();

public slots:

private:
    APP            *m_app;                // главный объект
    BTO             *m_bto;                 // объект работы с 'железом' БТО

    QWidget         *m_gui_page;            // раскладка UI элементов
    QPixmap         m_bto_pixmap_red;       // "красный индикатор"
    QPixmap         m_bto_pixmap_green;     // "зеленый индикатор"
    QPixmap         m_bto_pixmap_light_green;// яркий "зеленый индикатор"
    QPixmap         m_bto_pixmap_yellow;    // "желтый индикатор"
    QIcon           m_gamma_icon;           // картинка гамма-модуля
    QLabel         *m_bto_led;              // индикатор соединения с БТО

public:
    QProgressDialog *m_bar_connection;      // окно ожидания подключения к БТО
    QProgressDialog *m_bar_message;         // окно ожидания с сообщением
    QProgressDialog *m_bar_gamma_poweron;   // окно ожидания питания модуля
    QProgressDialog *m_bar_apply_settings;  // окно ожидания применения настроек

public:
    int             m_connection_bar_value; //
    QTimer          progress_bar_timer;     //
    QLabel          *m_gamma_found_icon;    // индикатор найденного гамма-модуля
    QLabel          *m_gamma_status;        // уведомление о найденном гамма-модуля

private:
    std::mutex      m_lock_bar1;
    std::mutex      m_lock_bar2;

    QPushButton     *m_button_bto_connect;
    QPushButton     *m_button_gamma_poweron;

    // текущие значения БТО (Node)
    QLineEdit       *m_field_required_voltage;
    QLineEdit       *m_field_output_voltage;
    QLabel          *m_field_amperage_limit_label;
    QLineEdit       *m_field_amperage_limit;
    QLabel          *m_field_amperage_consumption_label;
    QLineEdit       *m_field_amperage_consumption;
    QLineEdit       *m_field_power_output_sw;

    // настройки для БТО (уставки)
    QComboBox       *m_field_set_power_output_sw;
    QDoubleSpinBox  *m_field_requested_voltage;
    QLabel          *m_field_requested_amperage_limit_label;
    QDoubleSpinBox  *m_field_requested_amperage_limit;
    QPushButton     *m_button_set_settings;

    // данные с SRIG (nodeE4)
    QLineEdit       *m_field_gamma_15VDC;
    QLineEdit       *m_field_gamma_5VDC;
    QLineEdit       *m_field_gamma_DV485;
    QLineEdit       *m_field_gamma_HV_raw;
    QLineEdit       *m_field_gamma_TempV;
    QLineEdit       *m_field_gamma_TempVCal;
    QLineEdit       *m_field_gamma_25Ref;

    QLineEdit       *m_field_part_number;
    QLineEdit       *m_field_gamma_voltage;
    QLineEdit       *m_field_gamma_gamma;
    QLineEdit       *m_field_gamma_temp;
};

#endif // __MAINTENANCE_UNIT_GUI_H_
