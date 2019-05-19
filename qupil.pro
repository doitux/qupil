isEmpty( PREFIX ):PREFIX = /usr
TEMPLATE = app
CODECFORSRC = UTF-8
CONFIG += qt \
    thread \
    warn_on \
#     release

QT += svg sql uitools widgets printsupport
UI_DIR = uics
MOC_DIR = mocs
OBJECTS_DIR = obj
DEFINES += PREFIX=\"$${PREFIX}\"
TARGET = qupil
RESOURCES = src/gui/resources/qupil.qrc
TRANSLATIONS = ts/qupil_de.ts
INCLUDEPATH += . \
    src \
    src/core \
    src/core/config \
    src/core/db \
    src/core/sound \
    src/core/third_party/tinyxml \
    src/gui \
    src/gui/mainwindow \
    src/gui/mainwindow/models \
    src/gui/mainwindow/delegates \
    src/gui/settings \
    src/gui/concertmanager \
    src/gui/instrumentmanager \
    src/gui/aboutqupil \
    src/gui/csvimportfields \
    src/gui/docviewer \
    src/gui/metronom \
    src/gui/sheetmusiclibrary \
    src/gui/sheetmusiclibrary/models \
    src/gui/sheetmusiclibrary/delegates \
    src/gui/birthdays \
    src/gui/buildtimetabledoc \
    src/gui/reminder
DEPENDPATH += . \
    src \
    src/core \
    src/core/config \
    src/core/db \
    src/core/sound \
    src/core/third_party/tinyxml \
    src/gui \
    src/gui/mainwindow \
    src/gui/mainwindow/models \
    src/gui/mainwindow/delegates \
    src/gui/settings \
    src/gui/concertmanager \
    src/gui/instrumentmanager \
    src/gui/aboutqupil \
    src/gui/csvimportfields \
    src/gui/docviewer \
    src/gui/metronom \
    src/gui/sheetmusiclibrary \
    src/gui/sheetmusiclibrary/models \
    src/gui/sheetmusiclibrary/delegates \
    src/gui/birthdays \
    src/gui/buildtimetabledoc \
    src/gui/reminder
FORMS += src/gui/mainwindow/mainwindow.ui \
    src/gui/settings/settings.ui \
    src/gui/birthdays/birthdays.ui \
    src/gui/concertmanager/concertmanager.ui \
    src/gui/instrumentmanager/instrumentmanager.ui \
    src/gui/docviewer/docviewer.ui \
    src/gui/metronom/metronom.ui \
    src/gui/sheetmusiclibrary/sheetmusiclibrary.ui \
    src/gui/aboutqupil/aboutqupil.ui \
    src/gui/csvimportfields/csvimportfields.ui \
    src/gui/instrumentmanager/addrecitaldialog.ui \
    src/gui/concertmanager/recitaltabs.ui \
    src/gui/concertmanager/selectpiecesdialog.ui \
    src/gui/concertmanager/finishrecitaldialog.ui \
    src/gui/concertmanager/addexternalrecitalpiecedialog.ui \
    src/gui/pupilslastrecital/pupilslastrecitalviewdialog.ui \
    src/gui/reminder/reminderdialog.ui \
    src/gui/reminder/remindereditdialog.ui \
    src/gui/builddayviewdoc/builddayviewdialog.ui \
    src/gui/pupilsnotinensemble/pupilsnotinensembledialog.ui \
    src/gui/mymessagedialog/mymessagedialog.ui
SOURCES += src/qupil.cpp \
    src/core/config/configfile.cpp \
    src/core/third_party/tinyxml/tinystr.cpp \
    src/core/third_party/tinyxml/tinyxml.cpp \
    src/core/third_party/tinyxml/tinyxmlerror.cpp \
    src/core/third_party/tinyxml/tinyxmlparser.cpp \
    src/core/db/mydbhandler.cpp \
    src/core/db/obsoletepersonaldataconverter.cpp \
    src/gui/mainwindow/mainwindowimpl.cpp \
    src/gui/mainwindow/lessontabwidget.cpp \
    src/gui/mainwindow/timetabletreewidget.cpp \
    src/gui/mainwindow/models/palnotesmodel.cpp \
    src/gui/mainwindow/models/palpiecesmodel.cpp \
    src/gui/mainwindow/delegates/palpiecesdelegate.cpp \
    src/gui/mainwindow/pupiltabwidget.cpp \
    src/gui/settings/settingsdialogimpl.cpp \
    src/gui/birthdays/birthdaysdialogimpl.cpp \
    src/gui/mainwindow/pupillisttreewidget.cpp \
    src/core/sound/sdlplayer.cpp \
    src/gui/mainwindow/pupilarchivetextbrowser.cpp \
    src/gui/mainwindow/pupilarchivelisttreewidget.cpp \
    src/gui/mainwindow/delegates/palnotesdelegate.cpp \
    src/gui/concertmanager/concertmanagerdialogimpl.cpp \
    src/gui/instrumentmanager/instrumentmanagerdialogimpl.cpp \
    src/gui/docviewer/docviewerdialogimpl.cpp \
    src/gui/mainwindow/models/pupilcontactivitymodel.cpp \
    src/gui/mainwindow/models/pupilsingularactivitymodel.cpp \
    src/gui/mainwindow/delegates/pupilcontactivitydelegate.cpp \
    src/gui/mainwindow/delegates/pupilsingularactivitydelegate.cpp \
    src/gui/metronom/metronomdialogimpl.cpp \
    src/core/sound/metronomplayer.cpp \
    src/gui/sheetmusiclibrary/sheetmusiclibrarydialogimpl.cpp \
    src/gui/sheetmusiclibrary/delegates/smlalldelegate.cpp \
    src/gui/sheetmusiclibrary/models/smlallmodel.cpp \
    src/gui/mainwindow/models/qxtcsvmodel.cpp \
    src/gui/mainwindow/models/qxtglobal.cpp \
    src/gui/aboutqupil/aboutqupildialogimpl.cpp \
    src/gui/csvimportfields/csvimportfieldsdialogimpl.cpp \
    src/gui/buildtimetabledoc/buildtimetabledoc.cpp \
    src/core/db/dbupdater.cpp \
    src/gui/concertmanager/addrecitaldialogimpl.cpp \
    src/gui/concertmanager/recitaltabs.cpp \
    src/gui/concertmanager/selectpiecesdialog.cpp \
    src/gui/concertmanager/finishrecitaldialog.cpp \
    src/gui/concertmanager/addexternalrecitalpiecedialog.cpp \
    src/gui/pupilslastrecital/pupilslastrecitalviewdialog.cpp \
    src/gui/mainwindow/delegates/delegatetextedit.cpp \
    src/gui/reminder/reminderdialog.cpp \
    src/gui/reminder/remindereditdialog.cpp \
    src/gui/builddayviewdoc/builddayviewdialog.cpp \
    src/gui/pupilsnotinensemble/pupilsnotinensembledialog.cpp \
    src/gui/mymessagedialog/mymessagedialogimpl.cpp
HEADERS += src/core/third_party/tinyxml/tinyxml.h \
    src/core/third_party/tinyxml/tinystr.h \
    src/core/config/configfile.h \
    src/core/db/mydbhandler.h \
    src/core/db/obsoletepersonaldataconverter.cpp \
    src/gui/mainwindow/mainwindowimpl.h \
    src/gui/mainwindow/lessontabwidget.h \
    src/gui/mainwindow/timetabletreewidget.h \
    src/gui/mainwindow/models/palnotesmodel.h \
    src/gui/mainwindow/models/palpiecesmodel.h \
    src/gui/mainwindow/delegates/palpiecesdelegate.h \
    src/gui/mainwindow/pupiltabwidget.h \
    src/gui/settings/settingsdialogimpl.h \
    src/gui/birthdays/birthdaysdialogimpl.h \
    src/gui/mainwindow/pupillisttreewidget.h \
    src/core/sound/sdlplayer.h \
    src/gui/mainwindow/pupilarchivetextbrowser.h \
    src/gui/mainwindow/pupilarchivelisttreewidget.h \
    src/gui/mainwindow/delegates/palnotesdelegate.h \
    src/gui/concertmanager/concertmanagerdialogimpl.h \
    src/gui/instrumentmanager/instrumentmanagerdialogimpl.h \
    src/gui/docviewer/docviewerdialogimpl.h \
    src/gui/mainwindow/models/pupilcontactivitymodel.h \
    src/gui/mainwindow/models/pupilsingularactivitymodel.h \
    src/gui/mainwindow/delegates/pupilcontactivitydelegate.h \
    src/gui/mainwindow/delegates/pupilsingularactivitydelegate.h \
    src/gui/metronom/metronomdialogimpl.h \
    src/core/sound/metronomplayer.h \
    src/gui/sheetmusiclibrary/sheetmusiclibrarydialogimpl.h \
    src/gui/sheetmusiclibrary/delegates/smlalldelegate.h \
    src/gui/sheetmusiclibrary/models/smlallmodel.h \
    src/gui/mainwindow/models/qxtcsvmodel.h \
    src/gui/mainwindow/models/qxtglobal.h \
    src/gui/mainwindow/models/qxtpimpl.h \
    src/gui/aboutqupil/aboutqupildialogimpl.h \
    src/gui/csvimportfields/csvimportfieldsdialogimpl.h \
    src/gui/buildtimetabledoc/buildtimetabledoc.h \
    src/core/db/dbupdater.h \
    src/gui/concertmanager/addrecitaldialogimpl.h \
    src/gui/concertmanager/recitaltabs.h \
    src/gui/concertmanager/selectpiecesdialog.h \
    src/gui/concertmanager/finishrecitaldialog.h \
    src/gui/concertmanager/addexternalrecitalpiecedialog.h \
    src/gui/pupilslastrecital/pupilslastrecitalviewdialog.h \
    src/gui/mainwindow/delegates/delegatetextedit.h \
    src/gui/reminder/reminderdialog.h \
    src/qupil_defs.h \
    src/gui/reminder/remindereditdialog.h \
    src/gui/builddayviewdoc/builddayviewdialog.h \
    src/gui/pupilsnotinensemble/pupilsnotinensembledialog.h \
    src/gui/mymessagedialog/mymessagedialogimpl.h \
    src/core/third_party/boost/timers.hpp
win32 {
        DEFINES += _WIN32_WINNT=0x0501
        DEPENDPATH += src/net/win32/ \
                src/core/win32
        INCLUDEPATH += ../boost/ \
                ../SDL/include \
                ../SDL/include/SDL \
                ../SDL_mixer/include \
                ../zlib
        LIBPATH += ../boost/stage/lib \
                ../SDL/lib \
                ../SDL_mixer/lib \
                ../mysql/lib \
                ../zlib
        LIBS += -lSDL_mixer \
                -lSDL \
                -lSDLmain \
                -lidn
        debug:LIBPATH += debug/lib
        release:LIBPATH += release/lib
        win32-g++-cross {
            LIBS += -lntlm -lmikmod -lddraw -ldxguid -lsmpeg -lvorbisfile -lvorbis -logg -lmodplug
        }
        win32-g++ {

        }
        LIBS += \
                -lgdi32 \
                -lcomdlg32 \
                -loleaut32 \
                -limm32 \
                -lwinmm \
                -lwinspool \
                -lole32 \
                -luuid \
                -luser32 \
                -lmsimg32 \
                -lshell32 \
                -lkernel32 \
                -lmswsock \
                -lws2_32 \
                -ladvapi32 \
                -lwldap32
    RC_FILE = qupil.rc
}
unix:!mac { 
    # #### My release static build options
    QMAKE_CXXFLAGS += -ffunction-sections \
        -fdata-sections
    QMAKE_LFLAGS += -Wl,--gc-sections
    LIB_DIRS = $${PREFIX}/lib \
        $${PREFIX}/lib64
    if($$system(sdl-config --version)):error("sdl-config not found in PATH - libSDL_mixer, libSDL are required!")
    LIBS += -lSDL_mixer -lSDL
    
    # #### My release static libs
    # LIBS += -lSDL_mixer_static -lSDL -lmikmod
    # ### INSTALL ####
    binary.path += $${PREFIX}/bin/
    binary.files += qupil
    data.path += $${PREFIX}/share/qupil/data/
    data.files += data/*
    INSTALLS += binary \
        data
}
mac { 
    # make it universal
    CONFIG += x86_64
    CONFIG -= x86
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6

    # for universal-compilation on PPC-Mac uncomment the following line
    # on Intel-Mac you have to comment this line out or build will fail.
    # QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk/
    # Qt static (path is standard for self-compiling qt)
    # LIBS += /usr/local/Trolltech/Qt-4.2.3/lib/libQtCore.a
    # LIBS += /usr/local/Trolltech/Qt-4.2.3/lib/libQtGui.a
    # QT dynamic linked framework (see also mac_post_make.sh)
    LIBS += -framework \
        QtCore
    LIBS += -framework \
        QtGui
    
    # SDL and SDL_mixer come as frameworks
    LIBS += -framework \
        SDL
    LIBS += -framework \
        SDL_mixer
    
    # standard path for darwinports
    # make sure you have a universal version of boost
    # libraries installed on every mac
    LIBS += -framework \
        Carbon
    
    # set the application icon
    RC_FILE = qupil.icns
    LIBPATH += /Developer/SDKs/MacOSX10.6.sdk/usr/lib
    INCLUDEPATH += /Developer/SDKs/MacOSX10.6.sdk/usr/include/
    INCLUDEPATH += /Library/Frameworks/SDL.framework/Headers
    INCLUDEPATH += /Library/Frameworks/SDL_mixer.framework/Headers
}

