#include "modbusserialport.h"

ModbusSerialPort::ModbusSerialPort(QPlainTextEdit* logWindow)
{
    MW_logWindow = logWindow;
    m_com = new QSerialPort;
    connect(m_com,SIGNAL(readyRead()),this,SLOT(readData()));

}

void ModbusSerialPort::getAvaliableComList(QComboBox* comboBox)
{
    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        m_com->setPort(info);
        if(m_com->open(QIODevice::ReadWrite))
        {
            comboBox->addItem(m_com->portName());
            m_com->close();
        }
    }
    if (!comboBox->count())
    {
        QMessageBox::information(NULL, "tips", "There is not any avaliable serial ports now.", QMessageBox::Ok);
    }
}

bool ModbusSerialPort::connectCom(QString portName)
{
    m_com->setPortName(portName); // 设置串口名
    if(m_com->open(QIODevice::ReadWrite)) // 打开串口
    {
        m_com->setBaudRate(QSerialPort::Baud9600); // 设置波特率
        m_com->setDataBits(QSerialPort::Data8); // 设置数据位数8
        m_com->setParity(QSerialPort::NoParity); // 设置校验位
        m_com->setStopBits(QSerialPort::OneStop); // 停止位设置为1
        m_com->setFlowControl(QSerialPort::HardwareControl); // 设置为无流控制
        return true;
    }
    else
    {
        if (m_com->error() == QSerialPort::DeviceNotFoundError)
        {
            QMessageBox::information(NULL, "tips", "An error occurred while attempting to open an non-existing device.", QMessageBox::Ok);
        }
        else if (m_com->error() == QSerialPort::PermissionError)
        {
            QMessageBox::information(NULL, "tips", "This serial port is already used now.", QMessageBox::Ok);
        }
        m_com->clearError();
        return false;
    }
}

void ModbusSerialPort::disconnectCom()
{
    m_com->clear();
    m_com->close();
}

void ModbusSerialPort::readData()
{
    m_com->waitForReadyRead(10);
    m_buffer = m_com->readAll();
    if(!m_buffer.isEmpty())
    {
      feedbackInfo = "RX: " + QString(m_buffer.toHex().toUpper()) + "\n";
      showFeedBackInfo();
      emit finishReadData();
    }
}

void ModbusSerialPort::getBuffer(QByteArray &buffer)
{
    buffer = m_buffer;
    for (int i = 0; i < m_buffer.size(); i++)
    {
        buffer[i] = m_buffer.at(i);
    }
}

void ModbusSerialPort::setBuffer(QByteArray buffer)
{
    m_buffer.resize(buffer.size());
    for (int i = 0; i < buffer.size(); i++)
    {
        m_buffer[i] = buffer.at(i);
    }
}

bool ModbusSerialPort::sendData(QByteArray buffer)
{
    setBuffer(buffer);
    feedbackInfo = "TX: " + QString(m_buffer.toHex().toUpper()) + "\n";
    showFeedBackInfo();
    qint64 sendDataNum = m_com->write(m_buffer);
    if (sendDataNum != m_buffer.size())
    {
        return false;
    }
    return true;
}

void ModbusSerialPort::showFeedBackInfo()
{
    MW_logWindow->appendPlainText(feedbackInfo);
    MW_logWindow->update();
}

void ModbusSerialPort::showResponseTimeout()
{
    feedbackInfo = "RX: Timeout\n";
    showFeedBackInfo();
}
