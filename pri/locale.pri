# text.pri

## include(defs.pri)

TRANSLATIONS += locale/$${TARGET}.ts

for(LOC, LOCALE) {
TRANSLATIONS += locale/$${LOC}/$${TARGET}.ts
}

#-------------------------------------------------
# command to create translation file
#text.depends = makedir text_mk text_fmt # text_copy
text.depends = text_mk text_fmt # text_copy
text_mk.commands = lupdate -no-obsolete $${TARGET}.pro
text_fmt.commands = lrelease $${TARGET}.pro

#QMAKE_EXTRA_TARGETS += makedir text text_mk text_fmt text_copy
QMAKE_EXTRA_TARGETS += text text_mk text_fmt

#--------------------------------------------------
# create necessary dirs

#for(LOC, LOCALE) {
#makedir.commands +=\
#-$${MD} locale$${_SL_}$${LOC}$${_SL_} $${_2NUL_} $${_NL_}
#}

#text_copy.commands = $${RM} locale$${_SL_}$${TARGET}.qm $${_NL_}

#for(LOC, LOCALE) {
#text_copy.commands += \
#   $${MV} locale$${_SL_}$${LOC}$${_SL_}$${TARGET}.qm locale$${_SL_}$${LOC}$${_SL_}LC_MESSAGES$${_SL_}$${TARGET}.qm $${_NL_}
#}
