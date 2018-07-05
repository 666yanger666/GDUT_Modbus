#ifndef MODBUSSERIALPORT_H
#define MODBUSSERIALPORT_H

#include "modbusserialport_global.h"

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <QByteArray>
#include <QPlainTextEdit>

class MODBUSSERIALPORTSHARED_EXPORT ModbusSerialPort : public QObject
{  
    Q_OBJECT
public:
    ModbusSerialPort(QPlainTextEdit* logWindow);
    void getAvaliableComList(QComboBox* comboBox);
    bool connectCom(QString portName);
    void disconnectCom();
    void getBuffer(QByteArray &buffer);
    bool sendData(QByteArray buffer);
    void showResponseTimeout();


private:
    QPlainTextEdit* MW_logWindow;
    QSerialPort* m_com;
    QByteArray m_buffer;
    QString feedbackInfo;

    void setBuffer(QByteArray buffer);
    void showFeedBackInfo();

signals:
    void finishReadData();

public slots:
    void readData();

};

#endif // MODBUSSERIALPORT_H
