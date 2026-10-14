/*!
  \file   mainwindow.h
  \author Aleksey Lekhtin
  \date   created 17.11.2014
  \brief  Главный класс данных
*/

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <mutex>

#include <QtWidgets>
#include <QPrinter>

#include "main.h"
#include "comm.h"

class API;
class BTO;
class DIAGN;
class FIRMWARE;
class PLATO;

//-----------------------------------------------------------
class MW : public QMainWindow
{
    Q_OBJECT;

public:

    enum class PAGES
    {
        UNKNOWN,
        INFO_PAGE,
        BTO_PAGE,
        FIRMWARE_PAGE,
        DIAGN_PAGE,
        PLATO_PAGE,
        API_PAGE,
    };

    enum class STATUS_BAR_LABEL
    {
        NORMAL = 0,
        DARK = 1,
        GREEN = 2
    };

public:
    explicit MW(APP *m);
    ~MW();

private:
    APP                *m_app;

public:
    void                run();
    bool                shuttingdown() const;
    void                create_environment();
    void                read_settings(const QSettings *st);
    void                save_settings(QSettings *st);
    PAGES               current_tab() const;
    COMM::CONNECT_TYPE  connection_type() const;

    inline APP          *app()  const { return m_app; }
    inline MW           *mw()   { return this; }
    inline COMM         *comm() const { return m_app->comm(); }

    API                 *api() const;
    BTO                 *bto() const;
    DIAGN               *diagn() const;
    FIRMWARE            *fw() const;
    PLATO               *plato() const;

    QToolBar            *toolBar();
    QLabel              *status_bar_label();
    void                get_fonts_size();
    int                 content_font_pointSize() const;
    void                clearstatus();
    void                showstatus_serialport();
    void                select_bto_page(int mode = 0);
    void                select_diagn_page();
    void                select_firmware_page();
    void                lock_pages();
    void                unlock_pages();
    void                update_statusbar_label_text();
    void                set_status_bar_label_color(STATUS_BAR_LABEL mode);

    void                delay(int ms);
    static void         set_style_sheet(QWidget *wgt, const QString &style);
    static void         set_style_sheet(QWidget &wgt, const QString &style);
    static QList<QWidget *>make_dialog_two_button(QDialog *dlg, const QString &button1, const QString &button2);

    //// void                WMsg(QMessageBox::Icon icon, const QString &msg);
    //// void                WMsg(QMessageBox::Icon icon, const QString &title, const QString &msg, const QString &filename = {});

    inline void showstatus(const QString &msg, int sec = 0)
    {
        if (msg.isEmpty()) {
            statusBar()->clearMessage();
        } else {
            statusBar()->showMessage(msg, sec * 1000);
        }
    }

protected:
    void                closeEvent(QCloseEvent *e);

public slots:
     void               slot_select_bto_page();
     void               slot_select_firmware_page();
     void               slot_select_diagn_page();
     void               slot_select_plato_page();
     void               slot_select_api_page();

private slots:
    void                slot_font_settings();
    void                slot_comm__comport_disconnected();

    void                slot_set_icons_size(int sz);
    void                slot_set_menu_font_size(int sz);
    void                slot_set_content_font_size(int sz);
    void                slot_set_graph_font_size(int sz);
    void                slot_set_msg_font_size(int sz);

    void                slot_setup_comport();
    void                slot_setup_serialno();
    void                slot_setup_operator();
    void                slot_setup_bto();

signals:
     void               signal_save_settings();
     void               signal_showstatus(const QString &msg, int sec = 0);
     void               signal_message(QMessageBox::Icon icon, const QString &msg);
     void               signal_message(QMessageBox::Icon icon, const QString &title, const QString &msg, const QString &filename = {});

private:
    void                create_action();
    void                create_menu();
    void                create_toolbar();
    void                create_pages();
    QString             make_tip(QAction *act);

    bool                ask_for_SN();
    void                detect_all_SN(QComboBox *fld_SN);
    void                view_logX(int num);

private:
    QAction             *m_action__exit;            // завершить приложение
    QAction             *m_action__about;           // о приложении
    QAction             *m_action__fonts_size;      // настройка шрифтов приложения
    QAction             *m_action__setup_com;       // настройки com-порта
    QAction             *m_action__setup_SN;        // ввод серийного номера проверяемого прибора
    QAction             *m_action__setup_oper;      // ввод оператора проверки
    QAction             *m_action__setup_bto_host;        // ввод IP-адреса БТО
    QActionGroup        *m_action__select_group;    //
    QAction             *m_action__select_com;      // выбор com-порта для работы
    QAction             *m_action__select_eth;      // выбор ethernet-а для работы

    QAction             *m_action_sep_first;        // первый разделитель на toolBar
    QAction             *m_action_sep_second;       // второй разделитель на toolBar
    QAction             *m_action__sp;              // показать кнпку состояния подключения com-порта
    QAction             *m_action__bto;             // показать окно управления БТО
    QAction             *m_action__diagn;           // окно терминала
    QAction             *m_action__firmware;        // окно прошивки гамма-модуля
    QAction             *m_action__plato;           // окно теста линейности
    QAction             *m_action__api{};           // окно теста API
    QAction             *m_action__save_report{};   // сохранить отчет по текущему тестированию
    QAction             *m_action__load_report{};   //
    QAction             *m_action__load_last{};     // загрузить последний отчет
    QAction             *m_action__save_sim{};      // сохранить текущее тестирование как симуляцию
    QAction             *m_action__load_sim{};      // загрузить файл симуляции

private:
    QMenu               *m_fileMenu;                // пункт "Файл"
    QMenu               *m_settingsMenu;            // пункт "Настройки"
    QMenu               *m_helpMenu;                // пункт "Справка"
    QToolBar            *m_toolBar;                // панель с кнопками
    QToolBar            *m_toolBar0;                // панель с кнопкой 'exit'

    //-----------------------------------------------------------
private:
    QStackedWidget      *m_stacked;                 // окна с информацией

    //-----------------------------------------------------------
    QPrinter            *m_printer;                 // принтер

private:
    int                 m_sys_menu_font_size;       // системный размер фонта меню
    int                 m_sys_icon_size;            // сист. размер инструмент. кнопок
    int                 m_sys_graph_font_size;      // сист. размер фонта графиков
    int                 m_sys_content_font_size;    // сист. размер фонта
    QFont               m_sys_content_font;         // системный фонт
    int                 m_sys_msg_font_size;        // сист. размер фонта сообщений

private:
    QWidget             *create_info_page();

private:
    //--- элементы настроек ------------------------------------
    bool                m_shuttingdown{false};
    QString             m_root_path;
    QString             m_protocol_path;
    QString             m_log_path;

    QTabWidget          *m_firmware_tabwigdet;  // системные настройки
    QWidget             *m_tab_firmware_tool;   // вкладка системных настроек №2
    QWidget             *m_tab_system_dir;      // вкладка системных настроек №3
    QLabel              *m_status_bar_label;    // status bar permanent label

    //--- элементы отображения информации и управления ---------
private:
public:
    QWidget             *m_page_bto;            // окно БТО
    QWidget             *m_page_fw;             // окно настроек модуля gamma
    QWidget             *m_page_diagn;          // окно диагностики
    QWidget             *m_page_plato;          // окно тестирования линейности
    QWidget             *m_page_api;            // окно тестирования API

    // settings from .ini
private:
    int                 m_last_tab;
    int                 m_fullscreen;
    QByteArray          m_geometry;
    int                 m_icon_size;            // размер инструментальных кнопок
    int                 m_menu_font_size;       // размер фонта меню
    int                 m_graph_font_size;      // размер фонта графиков
    int                 m_content_font_size;    // размер фонта терминала

};

#endif // _MAINWINDOW_H_
