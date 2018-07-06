#include "slavedialog.h"
#include "mainwindow.h"

#define MOTORCOILADD 0
#define STARTTIMESREGADD 40

SlaveDialog::SlaveDialog(QWidget *parent) : QWidget(parent)
{
    m_com = new ModbusSerialPort(((MainWindow*)parent)->m_logWindow);
    m_modbus = new ModbusProtocol();
    initWidget();
    connectSignalsAndSlots();
}

SlaveDialog::~SlaveDialog()
{
    delete m_com;
    delete m_modbus;
}

void SlaveDialog::initWidget()
{
    m_comLabel = new QLabel("Vaild COM");
    m_comComboBox = new QComboBox;
    m_com->getAvaliableComList(m_comComboBox);
    m_comComboBox->setView(new QListView());
    m_comComboBox->setStyleSheet("QComboBox QAbstractItemView::item {height:30px;}");
    m_comRefresh = new QPushButton;
    m_comRefresh->setText("Refresh");
    m_comRefresh->setObjectName("refresh");
    m_comConnectBt = new QPushButton;
    m_comConnectBt->setText("Connect");
    m_comConnectBt->setObjectName("connectOrDisconnect");
    m_comDisconnectBt = new QPushButton;
    m_comDisconnectBt->setText("Disconnect");
    m_comDisconnectBt->setObjectName("connectOrDisconnect");
    m_comDisconnectBt->setDisabled(true);
    m_lamp = new QPushButton;
    m_lamp->setText("Stop");
    m_lamp->setObjectName("lamp");
    m_lamp->setDisabled(true);
    m_gLayout = new QGridLayout;
    m_gLayout->addWidget(m_comLabel,0,0,1,1,Qt::AlignRight);
    m_gLayout->addWidget(m_comComboBox,0,1,1,1,Qt::AlignLeft);
    m_gLayout->addWidget(m_comRefresh,0,0,1,1,Qt::AlignLeft);
    m_gLayout->addWidget(m_comConnectBt,1,0,1,1,Qt::AlignRight);
    m_gLayout->addWidget(m_comDisconnectBt,1,1,1,1,Qt::AlignLeft);
    m_gLayout->addWidget(m_lamp,2,0,2,2,Qt::AlignCenter);
    this->setLayout(m_gLayout);
}

void SlaveDialog::connectSignalsAndSlots()
{
    connect(m_com,SIGNAL(finishReadData()),this,SLOT(getData()));
    connect(m_comRefresh,SIGNAL(clicked()),this,SLOT(refreshCom()));
    connect(m_comConnectBt,SIGNAL(clicked()),this,SLOT(connectCom()));
    connect(m_comDisconnectBt,SIGNAL(clicked()),this,SLOT(disconnectCom()));
}

void SlaveDialog::refreshCom()
{
    m_comComboBox->clear();
    m_com->getAvaliableComList(m_comComboBox);
}

void SlaveDialog::connectCom()
{
    if (m_com->connectCom(m_comComboBox->currentText()))
    {
        QMessageBox::information(NULL, "tips", "Connect succeeded", QMessageBox::Ok);
        m_connected = true;
        m_comConnectBt->setText("Connected");
        m_comConnectBt->setDisabled(true);
        m_comConnectBt->setStyleSheet("color:green;");
        m_comDisconnectBt->setDisabled(false);
        m_comDisconnectBt->setStyleSheet("color:red;");
        m_comRefresh->setDisabled(true);
    }
    else
    {
        QMessageBox::information(NULL, "tips", "Connect failed", QMessageBox::Ok);
    }
}

void SlaveDialog::disconnectCom()
{
    if (m_connected)
    {
        m_com->disconnectCom();
        QMessageBox::information(NULL, "tips", "Disconnect succeeded", QMessageBox::Ok);
        m_connected = false;
        m_comConnectBt->setText("Connect");
        m_comConnectBt->setDisabled(false);
        m_comConnectBt->setStyleSheet("color:black;");
        m_comDisconnectBt->setDisabled(true);
        m_comDisconnectBt->setStyleSheet("color:gray;");
        m_comRefresh->setDisabled(false);
    }
}

void SlaveDialog::setRunStatus()
{
    m_run = true;
    m_lamp->setText((QStringLiteral("Run")));
    m_lamp->setStyleSheet("background-color:yellow;");
}

void SlaveDialog::setStopStatus()
{
    m_run = false;
    m_lamp->setText((QStringLiteral("Stop")));
    m_lamp->setStyleSheet("background-color:red;");
}

void SlaveDialog::setStatus()
{
    if (m_run)
    {
        setRunStatus();
    }
    else
    {
        setStopStatus();
    }
}

void SlaveDialog::getData()
{
    qDebug() << "start";
    QByteArray buffer;
    m_com->getBuffer(buffer);
    if (buffer.size() > 0)
    {
        int respondFC = m_modbus->analyzeMasterRequest(buffer);
        qDebug() << buffer.toHex().toUpper();
        if (respondFC == 0x03)
        {
            // 返回启动次数
            readMultiRegRespond(buffer, &m_runTimes, 1);
        }
        else if (respondFC == 0x0F)
        {
            // 记录上一次状态
            m_runLastTime = m_run;
            // 判断PDU数据，设置线圈状态
            if (m_run = buffer.at(buffer.size()-1)&0x01)
            {
                if (!m_runLastTime)
                {
                    m_runTimes+=1;
                    qDebug() << m_runTimes;
                }
            }
            setStatus();
            // 返回消息
            QByteArray respondData;
            respondData = buffer.mid(0,4);
            writeMultiCoilRespond(buffer, respondData);
        }
    }
    qDebug() << "end";
}

void SlaveDialog::readMultiCoilRespond(QByteArray &buffer, uint8_t* var, uint coilNum)
{

    m_modbus->getReadMultiCoilRespondBuff(buffer, var, coilNum);
    m_com->sendData(buffer);
}

void SlaveDialog::readMultiRegRespond(QByteArray &buffer, uint16_t* var, uint regNum)
{

    m_modbus->getReadMultiRegRespondBuff(buffer, var, regNum);
    m_com->sendData(buffer);
}

void SlaveDialog::writeMultiCoilRespond(QByteArray &buffer, QByteArray respondData)
{
    if (m_modbus->getWriteMultiCoilRespondBuff(buffer, respondData))
    {
        m_com->sendData(buffer);
    }
}

void SlaveDialog::writeMultiRegRespond(QByteArray &buffer, QByteArray respondData)
{
    if (m_modbus->getwriteMultiRegRespondBuff(buffer, respondData))
    {
        m_com->sendData(buffer);
    }
}

