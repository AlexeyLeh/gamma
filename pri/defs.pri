# defs.pri

isEmpty( _NL_ ) : _NL_ = $$escape_expand( \\n\\t )
isEmpty( QTBIN ): QTBIN = $$dirname(QMAKE_QMAKE)

isEmpty( _SL_ ) {
   unix : _SL_ = /
   win32: _SL_ = "\\"
}

#DEST_LOCALE = ..$${_SL_}..$${_SL_}mess

#------------------------------------------------
unix {
   DESTDIR  = .
} else:mingw {
   #DESTDIR = release
   #DESTDIR = debug
   #CONFIG(debug, debug|release) {
   CONFIG(debug, debug|release) {
      DESTDIR  = debug
   } else {
      DESTDIR  = release
   }
} else:win32 {
   CONFIG(debug, debug|release) {
      DESTDIR  = debug
   } else {
      DESTDIR  = release
   }
}

#------------------------------------------------
mingw|unix {
   QMAKE_CXXFLAGS += -Wno-write-strings
} 

#-------------------------------------------------
win32 {
   Q_OS_WIN7 = 0.0
   # Execute 'ver' command, look for "Version 6.1" (which is Win7)
   win7_check = $$system(ver)
   contains(win7_check, ".*6\\.1.*") {
       #message("Windows 7 Detected")
       Q_OS_WIN7 = 6.1
   }

   # command utils:
   _OPTSL_ = "/"
   MV    = move /y
   CP    = copy /y
   CD    = xcopy /s /q /y /i
   RM    = del $${_OPTSL_}s $${_OPTSL_}q
   MD    = md

   #QMAKE_DEL_FILE = del $${_OPTSL_}s $${_OPTSL_}q $${_OPTSL_}f
   #QMAKE_DEL_DIR = rmdir $${_OPTSL_}s $${_OPTSL_}q
   DEL_DIR = rmdir $${_OPTSL_}s $${_OPTSL_}q

   _SEP_ = "&& "
   _1NUL_ = " 1>NUL"
   _2NUL_ = " 2>NUL"
   _EXE_  =".exe"
}

#-------------------------------------------------
unix {
   # command utils:
   MV = mv -f
   CP = cp -f
   CD = cp -fr
   RM = rm -f
   DEL_DIR = rm -fr
   MD = mkdir -p -m 770

   _SEP_ = "&& "
   _2NUL_ = " 2>/dev/null"
}

#-------------------------------------------------
Debug|Release|unix {

   DISTDIR = dist
   mydistclean.commands = -rmdir /s /q $${DISTDIR} $${_2NUL_}
   QMAKE_EXTRA_TARGETS += clean mydistclean
   clean.depends += mydistclean

   delexefile.commands = $${RM} release\\$${TARGET}$${_EXE_} debug\\$${TARGET}$${_EXE_}
   QMAKE_EXTRA_TARGETS += delexefile
   clean.depends += delexefile

   QMAKE_CLEAN += release\\$${TARGET}$${_EXE_} debug\\$${TARGET}$${_EXE_}
   QMAKE_CLEAN += *~ *.suo *.pdb

   #QMAKE_CLEAN += $${DISTDIR}\* 
   #QMAKE_CLEAN += $${DISTDIR}\locale\ru_RU\*  $${DISTDIR}\locale\en_US\*
   #QMAKE_CLEAN += $${DISTDIR}\bearer\* $${DISTDIR}\iconengines\* $${DISTDIR}\imageformats*
   #QMAKE_CLEAN += $${DISTDIR}\platforms\* $${DISTDIR}\styles\* $${DISTDIR}\translations\*

   #QMAKE_CLEAN += *.$${TARGET}.Debug *.$${TARGET}.Release
   #QMAKE_CLEAN += $${TARGET}$${_EXE_}

   ##QMAKE_CLEAN += ipch\\*.*
   #QMAKE_DISTCLEAN += release\\*.*
   #QMAKE_DISTCLEAN += debug\\*.*
   #QMAKE_DISTCLEAN += ipch\\*.*
   #QMAKE_DISTCLEAN += *.pdb
   #QMAKE_DISTCLEAN += *.sdf
   #QMAKE_DISTCLEAN += 2> nul
   #QMAKE_DISTCLEAN += & $${DEL_DIR} debug release obj ipch dist
   #QMAKE_DISTCLEAN += 2> nul

   #QMAKE_DISTCLEAN += debug\\ release\\ obj\\ ipch\\ dist\\

   #distclean.depends += ext_clean
   #ext_clean.commands = $${QMAKE_DEL_DIR} debug release obj ipch
   #QMAKE_EXTRA_TARGETS += ext_clean distclean
}
