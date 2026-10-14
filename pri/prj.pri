# app.pri

unix|mingw {
     #CONFIG  += release
     #CONFIG  += debug
     #CONFIG  += debug_and_release
     #CONFIG  += build_all
} else:win32 {
     #CONFIG  += release
     #CONFIG  += debug
     #CONFIG  += debug_and_release
     #CONFIG  += build_all
}

#---------------------------------------------------
unix {
   DESTDIR = .

   #QMAKE_CXXFLAGS += -Wno-strict-aliasing
   #QMAKE_CXXFLAGS += -Wconversion
   #QMAKE_CXXFLAGS += -gstabs+

   OBJECTS_DIR  = ./obj
   MOC_DIR     += ./obj
   RCC_DIR     += ./obj
   OBJECTS_DIR  = ./obj
}

#---------------------------------------------------
mingw {
   #
} else:win32 {
   DEFINES += _CRT_SECURE_NO_WARNINGS
   DEFINES += _CRT_SECURE_NO_DEPRECATE
   DEFINES += _USE_MATH_DEFINES

   #QMAKE_CXXFLAGS += -MP
   #QMAKE_LFLAGS += /DEBUG
   QMAKE_LFLAGS_DEBUG = ""
   QMAKE_LFLAGS += /INCREMENTAL:NO
   #QMAKE_LFLAGS  += /STACK:4194304
}

#---------------------------------------------------
win32 {
   CONFIG(debug, debug|release) {
      # debug:
      DESTDIR  = debug
      QMAKE_CXXFLAGS += -Fd$${DESTDIR}\\$${TARGET}.pdb

   } else {
      # release:
      DESTDIR  = release
      #DEFINES += QT_NO_DEBUG_OUTPUT
      #DEFINES += QT_NO_WARNING_OUTPUT
   }
}
