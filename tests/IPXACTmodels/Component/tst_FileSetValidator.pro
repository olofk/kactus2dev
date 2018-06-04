#-----------------------------------------------------------------------------
# FileSet: tst_FileSetValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 20.11.2015
#
# Description:
# Qt project FileSet template for running unit tests for Enumerated Value Validator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_FileSetValidator
QT += core xml testlib
QT -= gui

CONFIG += c++11 testcase console
DEFINES += IPXACTMODELS_LIB

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_FileSetValidator.pri)
