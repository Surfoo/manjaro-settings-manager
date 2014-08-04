#-------------------------------------------------
#
# Project created by QtCreator 2013-03-13T01:28:06
#
#-------------------------------------------------

QT       += core gui webkitwidgets dbus

LIBS += -lmhwd
TARGET = manjaro-settings-manager-gui
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++0x

SOURCES  += main.cpp \
            mainwindow.cpp \
            delegates/kernel_list_view_delegate.cpp \
            dialogs/accounttypedialog.cpp \
            dialogs/adduserdialog.cpp \
            dialogs/applydialog.cpp \
            dialogs/changepassworddialog.cpp \
            dialogs/kernel_info_dialog.cpp \
            dialogs/previewfiledialog.cpp \
            dialogs/TimeZoneDialog.cpp \
            models/KernelModel.cpp \
            models/Kernel.cpp \
            models/TimeDate.cpp \
            pages/page_keyboard.cpp \
            pages/page_language.cpp \
            pages/page_languagepackages.cpp \
            pages/page_mhwd.cpp \
            pages/page_notifications.cpp \
            pages/page_users.cpp \
            pages/PageKernel.cpp \
            pages/PageTimeDate.cpp \
            widgets/flatbutton.cpp \
            widgets/listwidget.cpp \
            widgets/LocaleGlobal.cpp \
            widgets/keyboardpreview.cpp \
            widgets/pagewidget.cpp \
            widgets/selectlocalesdialog.cpp \
            widgets/TimeZoneWidget.cpp \
            widgets/toolbarstylewidget.cpp




HEADERS  += mainwindow.h \
            delegates/kernel_list_view_delegate.h \
            dialogs/accounttypedialog.h \
            dialogs/adduserdialog.h \
            dialogs/applydialog.h \
            dialogs/changepassworddialog.h \
            dialogs/kernel_info_dialog.h \
            dialogs/previewfiledialog.h \
            dialogs/TimeZoneDialog.h \
            models/KernelModel.h \
            models/Kernel.h \
            models/TimeDate.h \
            pages/page_keyboard.h \
            pages/page_language.h \
            pages/page_languagepackages.h \
            pages/page_mhwd.h \
            pages/page_notifications.h \
            pages/page_users.h \
            pages/PageKernel.h \
            pages/PageTimeDate.h \
            widgets/flatbutton.h \
            widgets/listwidget.h \
            widgets/LocaleGlobal.h \
            widgets/keyboardpreview.h \
            widgets/pagewidget.h \
            widgets/selectlocalesdialog.h \
            widgets/TimeZoneWidget.h \
            widgets/toolbarstylewidget.h




FORMS    += mainwindow.ui \
            dialogs/adduserdialog.ui \
            dialogs/accounttypedialog.ui \
            dialogs/applydialog.ui \
            dialogs/changepassworddialog.ui \
            dialogs/kernel_info_dialog.ui \
            dialogs/TimeZoneDialog.ui \
            pages/page_keyboard.ui \
            pages/page_language.ui \
            pages/page_languagepackages.ui \
            pages/page_mhwd.ui \
            pages/page_notifications.ui \
            pages/page_users.ui \
            pages/PageKernel.ui \
            pages/PageTimeDate.ui \
            widgets/selectlocalesdialog.ui




TRANSLATIONS += translations/msm_ast.ts \
                translations/msm_bg_BG.ts \
                translations/msm_ca.ts \
                translations/msm_cs_CZ.ts \
                translations/msm_da.ts \
                translations/msm_de.ts \
                translations/msm_en.ts \
                translations/msm_el.ts \
                translations/msm_es.ts \
                translations/msm_fa.ts \
                translations/msm_fa_IR.ts \
                translations/msm_fo.ts \
                translations/msm_fr.ts \
                translations/msm_he.ts \
                translations/msm_hi.ts \
                translations/msm_hr.ts \
                translations/msm_hu.ts \
                translations/msm_id_ID.ts \
                translations/msm_it.ts \
                translations/msm_ja.ts \
                translations/msm_nb.ts \
                translations/msm_nl.ts \
                translations/msm_pl.ts \
                translations/msm_pt_BR.ts \
                translations/msm_pt_PT.ts \
                translations/msm_ro.ts \
                translations/msm_ru.ts \
                translations/msm_sk_SK.ts \
                translations/msm_sl.ts \
                translations/msm_sr@latin.ts \
                translations/msm_sv.ts \
                translations/msm_tr.ts \
                translations/msm_uk.ts \
                translations/msm_vi.ts \
                translations/msm_zh_CN.ts \
                translations/msm_zh_TW.ts

RESOURCES += \
    resources.qrc

unix:!macx: LIBS += -L$$OUT_PWD/../global/ -lglobal

INCLUDEPATH += $$PWD/../global
DEPENDPATH += $$PWD/../global

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../global/libglobal.a



target.path = /usr/bin
INSTALLS += target
