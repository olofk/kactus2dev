#-----------------------------------------------------------------------------
# File: tst_DesignConfigurationValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 11.01.2016
#
# Description:
# Qt project file for running unit tests for DesignConfigurationValidator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_DesignConfigurationValidator

QT += core gui xml testlib

CONFIG += c++11 testcase console
DEFINES += IPXACTMODELS_LIB

DESTDIR = ./release

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_DesignConfigurationValidator.pri)
