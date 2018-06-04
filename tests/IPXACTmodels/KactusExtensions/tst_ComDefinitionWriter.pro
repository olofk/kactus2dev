#-----------------------------------------------------------------------------
# File: tst_ComDefinitionWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 11.12.2015
#
# Description:
# Qt project file template for running unit tests for ComDefinition writer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ComDefinitionWriter

QT += core xml testlib
QT -= gui

CONFIG += c++11 testcase console
DEFINES += IPXACTMODELS_LIB

DESTDIR = ./release

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_ComDefinitionWriter.pri)
