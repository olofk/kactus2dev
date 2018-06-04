#-----------------------------------------------------------------------------
# File: tst_FieldValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 17.11.2015
#
# Description:
# Qt project file template for running unit tests for FieldValidator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_FieldValidator

QT += core gui xml testlib

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
include(tst_FieldValidator.pri)
