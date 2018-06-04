#-----------------------------------------------------------------------------
# File: tst_AddressSpaceValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 30.11.2015
#
# Description:
# Qt project file template for running unit tests for AddressSpaceValidator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_AddressSpaceValidator

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
include(tst_AddressSpaceValidator.pri)
