# gamma.pri

INCLUDEPATH +=  \
    .           \
    adam/       \
    bto/        \
    comm/       \
    crc/        \
    dbg/        \
    help/       \
    gamma-api/  \
    gamma-diagn/\
    gamma-plato/\
    gamma-fw/   \
    main/       \
    mainwin/    \
    serialport/ \
    settings/   \
    tools/      \
    table/      \
    terminal    \

HEADERS += \
    $$PWD/adam/adam.h \
    $$PWD/adam/unmarshal_float.h \
    $$PWD/adam/frame.h \
    $$PWD/adam/response.h \
    $$PWD/adam/node.h \
    $$PWD/adam/NodeBA.h \
    $$PWD/adam/NodeE4.h \
    $$PWD/adam/NodeE4_ctable.h \
    $$PWD/adam/NodeE4_ktable.h \
    \
    $$PWD/bto/bto.h \
    $$PWD/bto/bto_gui.h \
    \
    $$PWD/dbg/dbg.h \
    \
    $$PWD/comm/conn.h \
    $$PWD/comm/conn_comport.h \
    $$PWD/comm/conn_eth.h \
    $$PWD/comm/comm.h \
    \
    $$PWD/crc/crc.h \
    $$PWD/crc/lib_crc/lib_crc.h \
    \
    $$PWD/gamma-api/api.h \
    $$PWD/gamma-api/api_gui.h \
    $$PWD/gamma-api/api_data.h \
    \
    $$PWD/gamma-diagn/diagn.h \
    $$PWD/gamma-diagn/diagn_gui.h \
    \
    $$PWD/gamma-fw/fw.h \
    $$PWD/gamma-fw/fw_gui.h \
    $$PWD/gamma-fw/ktable.h \
    \
    $$PWD/gamma-plato/plato.h \
    $$PWD/gamma-plato/plato_gui.h \
    $$PWD/gamma-plato/plato_data.h \
    \
    $$PWD/main/main.h \
    $$PWD/main/translator.h \
    $$PWD/main/ver.h \
    $$PWD/main/utils.h \
    \
    $$PWD/mainwin/gamma.h \
    $$PWD/mainwin/lineedit.h \
    $$PWD/mainwin/mainwindow.h \
    $$PWD/mainwin/progress.h \
    $$PWD/mainwin/wmsg.h \
    \
    $$PWD/serialport/serial_conf.h \
    $$PWD/serialport/serial_port.h \
    \
    $$PWD/table/table.h \
    $$PWD/terminal/terminal.h \
    \

SOURCES += \
    $$PWD/adam/unmarshal_float.cpp \
    $$PWD/adam/response.cpp \
    $$PWD/adam/frame.cpp \
    $$PWD/adam/node.cpp \
    $$PWD/adam/NodeBA.cpp \
    $$PWD/adam/NodeE4.cpp \
    $$PWD/adam/NodeE4_ctable.cpp \
    $$PWD/adam/NodeE4_ktable.cpp \
    $$PWD/gamma-api/api_simulate.cpp \
    $$PWD/gamma-fw/fw_gui_create_page.cpp \
    $$PWD/gamma-fw/fw_gui_slots.cpp \
    $$PWD/gamma-plato/plato_create_chart.cpp \
    $$PWD/gamma-plato/plato_simulation.cpp \
    $$PWD/main/utils.cpp \
    $$PWD/mainwin/dialog.cpp \
    \
    $$PWD/bto/bto.cpp \
    $$PWD/bto/bto_settings.cpp \
    $$PWD/bto/bto_gui.cpp \
    $$PWD/bto/bto_gui_create_page.cpp \
    $$PWD/bto/bto_update_BA_fields.cpp \
    $$PWD/bto/bto_update_E4_fields.cpp \
    \
    $$PWD/comm/comm.cpp \
    $$PWD/comm/conn_comport.cpp \
    $$PWD/comm/conn_eth.cpp \
    $$PWD/comm/comm_settings.cpp \
    \
    $$PWD/crc/lib_crc/lib_crc.c\
    $$PWD/crc/crc.cpp \
    \
    $$PWD/dbg/dbg.cpp \
    \
    $$PWD/gamma-api/api.cpp \
    $$PWD/gamma-api/api_gui.cpp \
    $$PWD/gamma-api/api_gui_create_page.cpp \
    $$PWD/gamma-api/api_update_fields.cpp \
    $$PWD/gamma-api/api_log.cpp \
    $$PWD/gamma-api/api_log_template.cpp \
    $$PWD/gamma-api/api_settings.cpp \
    \
    $$PWD/gamma-diagn/diagn.cpp \
    $$PWD/gamma-diagn/diagn_gui.cpp \
    $$PWD/gamma-diagn/diagn_gui_create_page.cpp \
    $$PWD/gamma-diagn/diagn_gui_edit.cpp \
    $$PWD/gamma-diagn/diagn_gui_log.cpp \
    $$PWD/gamma-diagn/diagn_settings.cpp \
    $$PWD/gamma-diagn/diagn_update_fields.cpp \
    \
    $$PWD/gamma-fw/fw.cpp \
    $$PWD/gamma-fw/fw_gui.cpp \
    $$PWD/gamma-fw/fw_settings.cpp \
    $$PWD/gamma-fw/fw_update_fields.cpp \
    $$PWD/gamma-fw/ktable.cpp \
    \
    $$PWD/gamma-plato/plato.cpp \
    $$PWD/gamma-plato/plato_gui.cpp \
    $$PWD/gamma-plato/plato_create_table.cpp \
    $$PWD/gamma-plato/plato_gui_create_page.cpp \
    $$PWD/gamma-plato/plato_log.cpp \
    $$PWD/gamma-plato/plato_settings.cpp \
    $$PWD/gamma-plato/plato_log_template.cpp \
    $$PWD/gamma-plato/plato_update_fields.cpp \
    \
    $$PWD/main/help.cpp \
    $$PWD/main/ini.cpp \
    $$PWD/main/main.cpp \
    $$PWD/main/translator.cpp \
    $$PWD/main/ver.cpp \
    \
    $$PWD/mainwin/create_action.cpp\
    $$PWD/mainwin/create_menu.cpp\
    $$PWD/mainwin/create_pages.cpp \
    $$PWD/mainwin/create_toolbar.cpp\
    $$PWD/mainwin/lineedit.cpp\
    $$PWD/mainwin/mainwindow.cpp\
    $$PWD/mainwin/progress.cpp\
    $$PWD/mainwin/slot_fonts.cpp\
    $$PWD/mainwin/wmsg.cpp \
    \
    $$PWD/serialport/serial_conf.cpp \
    $$PWD/serialport/serial_port.cpp \
    \
    $$PWD/table/table.cpp \
    $$PWD/terminal/terminal.cpp \
    \

FORMS +=
