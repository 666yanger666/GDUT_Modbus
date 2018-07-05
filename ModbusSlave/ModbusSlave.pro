#-------------------------------------------------
#
# Project created by QtCreator 2018-06-08T09:42:33
#
#-------------------------------------------------

QT       += \
         core gui \
         serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ModbusSlave
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        slavedialog.cpp

HEADERS += \
        mainwindow.h \
        slavedialog.h

RESOURCES += \
    images.qrc

RC_ICONS = slave.ico

greaterThan(QT_MAJOR_VERSION, 4) {
    TARGET_ARCH=$${QT_ARCH}
} else {
    TARGET_ARCH=$${QMAKE_HOST.arch}
}

contains(TARGET_ARCH, x86_64) {
    ARCHITECTURE = x64
    # ModbusSerialPort
    win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-Modbus-Desktop_Qt_5_9_3_MSVC2015_64bit-Release/ModbusSerialPort/release/ -lModbusSerialPort
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-Modbus-Desktop_Qt_5_9_3_MSVC2015_64bit-Debug/ModbusSerialPort/debug/ -lModbusSerialPort
    # ModbusProtocol
    win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-Modbus-Desktop_Qt_5_9_3_MSVC2015_64bit-Release/ModbusProtocol/release/ -lModbusProtocol
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-Modbus-Desktop_Qt_5_9_3_MSVC2015_64bit-Debug/ModbusProtocol/debug/ -lModbusProtocol
} else {
    ARCHITECTURE = x86
    # ModbusSerialPort
    win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-Modbus-Desktop_Qt_5_9_3_MSVC2015_32bit-Release/ModbusSerialPort/release/ -lModbusSerialPort
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-Modbus-Desktop_Qt_5_9_3_MSVC2015_32bit-Debug/ModbusSerialPort/debug/ -lModbusSerialPort
    # ModbusProtocol
    win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build-Modbus-Desktop_Qt_5_9_3_MSVC2015_32bit-Release/ModbusProtocol/release/ -lModbusProtocol
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build-Modbus-Desktop_Qt_5_9_3_MSVC2015_32bit-Debug/ModbusProtocol/debug/ -lModbusProtocol
}
