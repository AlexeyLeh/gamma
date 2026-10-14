# install.pri

DIST = .$${_SL_}dist

LOCALE = en_US ru_RU

#--------------------------------------------------
# register targets
QMAKE_EXTRA_TARGETS += dist install makeinstdir strip binn distr system_translations

#--------------------------------------------------
# create install dirs
makeinstdir.commands += @echo "--- make dir $${DIST}$${_SL_}$${TARGET}$$" $${_NL_} \
-$${MD} $${DIST}$${_SL_}$${TARGET}$$ $${_NL_}

for(LOC, LOCALE) { \
makeinstdir.commands += \
-$${MD} $${DIST}$${_SL_}$${TARGET}$${_SL_}locale$${_SL_}$${LOC} $${_NL_} \
}

#--------------------------------------------------
# strip
unix: strip.commands = strip $(TARGET)
win32: strip.commands = @echo "--- strip: under Windows: no need strip"

#--- install --------------------------------------
dist.depends = install

Debug|Release|unix {
install.depends = makeinstdir binn distr system_translations
}

#--------------------------------------------------
binn.depends = strip
binn.commands = \
   @echo "--- copy binaries..."  $${_NL_} \
   -$${CP} $${DESTDIR}$${_SL_}$${TARGET}$${_EXE_} $${DIST}$${_SL_}$${TARGET} $${_1NUL_} $${_NL_} \
   -$${CP} $${TARGET}.changes $${DIST}$${_SL_}$${TARGET} $${_1NUL_} $${_NL_} \
   -$${CD} simulations $${DIST}$${_SL_}$${TARGET}$${_SL_}simulations $${_1NUL_} $${_NL_}

for(LOC, LOCALE) { \
binn.commands += \
@echo "--- copy $${LOC} translations"  $${_NL_} \
-$${CP} locale$${_SL_}$${LOC}$${_SL_}$${TARGET}.qm $${DIST}$${_SL_}$${TARGET}$${_SL_}$${TARGET}.$${LOC}.qm $${_1NUL_} $${_NL_} \
-$${CP} locale$${_SL_}$${LOC}$${_SL_}$${TARGET}.qm $${DIST}$${_SL_}$${TARGET}$${_SL_}locale$${_SL_}$${LOC} $${_1NUL_} $${_NL_} \
}

#--------------------------------------------------
# install sustem translation files is currently broken in Qt 6.5.0 :((
#distr.commands += windeployqt -dir $${DIST}$${_SL_}$${TARGET}$$ $${DESTDIR}$${_SL_}$${TARGET}$${_EXE_}
distr.commands += windeployqt --no-translations -dir $${DIST}$${_SL_}$${TARGET}$$ $${DESTDIR}$${_SL_}$${TARGET}$${_EXE_}

system_translations.commands = -$${CD} qt_translations $${DIST}$${_SL_}$${TARGET}$${_SL_}translations $${_1NUL_} $${_NL_}
