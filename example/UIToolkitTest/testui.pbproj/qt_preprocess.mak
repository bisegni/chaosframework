#############################################################################
# Makefile for building: testui.app/Contents/MacOS/testui
# Generated by qmake (2.01a) (Qt 4.8.0) on: Wed Feb 22 14:18:25 2012
# Project:  UIToolkitTest.pro
# Template: app
# Command: /usr/bin/qmake-4.8 -o testui.pbproj/project.pbxproj UIToolkitTest.pro
#############################################################################

MOC       = /Developer/Tools/Qt/moc
UIC       = /Developer/Tools/Qt/uic
LEX       = flex
LEXFLAGS  = 
YACC      = yacc
YACCFLAGS = -d
DEFINES       = -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
INCPATH       = -I/usr/local/Qt4.8/mkspecs/macx-xcode -I. -I/Library/Frameworks/QtCore.framework/Versions/4/Headers -I/usr/include/QtCore -I/Library/Frameworks/QtGui.framework/Versions/4/Headers -I/usr/include/QtGui -I/usr/include -I/usr/local/qwt-6.0.2-svn/lib/qwt.framework/Versions/6/Headers -I/usr/local/qwt-6.0.2-svn/include -I. -I. -I/usr/local/include -I/System/Library/Frameworks/CarbonCore.framework/Headers -F/Library/Frameworks
DEL_FILE  = rm -f
MOVE      = mv -f

IMAGES = 
PARSERS =
preprocess: $(PARSERS) compilers
clean preprocess_clean: parser_clean compiler_clean

parser_clean:
check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compilers: ./moc_mainwindow.cpp ./moc_graphwidget.cpp ./moc_controldialog.cpp ./ui_mainwindow.h ./ui_controldialog.h
compiler_objective_c_make_all:
compiler_objective_c_clean:
compiler_moc_header_make_all: moc_mainwindow.cpp moc_graphwidget.cpp moc_controldialog.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_mainwindow.cpp moc_graphwidget.cpp moc_controldialog.cpp
moc_mainwindow.cpp: graphwidget.h \
		mainwindow.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ mainwindow.h -o moc_mainwindow.cpp

moc_graphwidget.cpp: graphwidget.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ graphwidget.h -o moc_graphwidget.cpp

moc_controldialog.cpp: controldialog.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ controldialog.h -o moc_controldialog.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_rez_source_make_all:
compiler_rez_source_clean:
compiler_uic_make_all: ui_mainwindow.h ui_controldialog.h
compiler_uic_clean:
	-$(DEL_FILE) ui_mainwindow.h ui_controldialog.h
ui_mainwindow.h: mainwindow.ui
	/Developer/Tools/Qt/uic mainwindow.ui -o ui_mainwindow.h

ui_controldialog.h: controldialog.ui
	/Developer/Tools/Qt/uic controldialog.ui -o ui_controldialog.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_uic_clean 

