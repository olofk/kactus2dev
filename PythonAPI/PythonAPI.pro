# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------

TEMPLATE = lib
TARGET = PythonAPI
DESTDIR = .
CONFIG += c++11 release dll
DEFINES += PYTHONAPI_LIB
QT += core gui widgets xml
LIBS += -L"$(SolutionDir)x64/executable" \
    -L$$(PWD)/../executable -lIPXACTmodels \
    -L$$(PWD)/../executable -lKactus2
DEPENDPATH += ..
INCLUDEPATH += ..
MOC_DIR += ./GeneratedFiles
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles

QMAKE_CXXFLAGS += $$PYTHON_C_FLAGS

include(PythonAPI.pri)

# Run SWIG to generate PythonAPI_wrap.cxx.
swig.depends = ./PythonAPI.i
swig.target = PythonAPI_wrap.cxx
swig.commands = swig -python -c++ -O PythonAPI.i
swig.path = .

QMAKE_EXTRA_TARGETS += swig

# Rename shared library for SWIG wrapper.
unix:QMAKE_POST_LINK = ln -s -f libPythonAPI.so.1.0.0 _pythonAPI.so

# Install target
target.path = $$lib_path
target.files = _pythonAPI.so

INSTALLS += target
