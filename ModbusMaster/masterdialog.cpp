#include "masterdialog.h"
#include "mainwindow.h"

#define MOTORCOILADD 0
#define STARTTIMESREGADD 40
#define Timer 500

MasterDialog::MasterDialog(QWidget *parent) : QWidget(parent)
{
    m_com = new ModbusSerialPort(((MainWindow*)parent)->m_logWindow);
    m_modbus = new ModbusProtocol();

    initWidget();
    connectSignalsAndSlots();
}

MasterDialog::~MasterDialog()
{
    delete m_com;
    delete m_modbus;
}

void MasterDialog::initWidget()
{
    m_timer = new QTimer;
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
    m_startBt = new QPushButton;
    m_startBt->setText("Start");
    m_startBt->setObjectName("startOrStop");
    m_stopBt = new QPushButton;
    m_stopBt->setText("Stop");
    m_stopBt->setObjectName("startOrStop");
    m_lamp = new QPushButton;
    m_lamp->setText("Stop");
    // m_lamp->setMaximumSize(100,100);
    m_lamp->setObjectName("lamp");
    m_lamp->setDisabled(true);
    m_gLayout = new QGridLayout;
    m_gLayout->addWidget(m_comLabel,0,0,1,1,Qt::AlignRight);
    m_gLayout->addWidget(m_comComboBox,0,1,1,1,Qt::AlignLeft);
    m_gLayout->addWidget(m_comRefresh,0,0,1,1,Qt::AlignLeft);
    m_gLayout->addWidget(m_comConnectBt,1,0,1,1,Qt::AlignRight);
    m_gLayout->addWidget(m_comDisconnectBt,1,1,1,1,Qt::AlignLeft);
    m_gLayout->addWidget(m_lamp,2,0,2,2,Qt::AlignCenter);
    m_gLayout->addWidget(m_startBt,4,0,1,1,Qt::AlignCenter);
    m_gLayout->addWidget(m_stopBt,4,1,1,1,Qt::AlignCenter);
    this->setLayout(m_gLayout);
}

void MasterDialog::connectSignalsAndSlots()
{
    connect(m_timer, SIGNAL(timeout()), this, SLOT(checkCoilStatus()));
    connect(m_com,SIGNAL(finishReadData()),this,SLOT(getData()));
    connect(m_comRefresh,SIGNAL(clicked()),this,SLOT(refreshCom()));
    connect(m_comConnectBt,SIGNAL(clicked()),this,SLOT(connectCom()));
    connect(m_comDisconnectBt,SIGNAL(clicked()),this,SLOT(disconnectCom()));
    connect(m_startBt,SIGNAL(clicked()),this,SLOT(start()));
    connect(m_stopBt,SIGNAL(clicked()),this,SLOT(stop()));
}

void MasterDialog::checkCoilStatus()
{
    if (m_connected)
    {
        // 定时读取启动次数
        if (!m_waitForResponse)
        {
            if (m_run)
            {
                readMultiReg(STARTTIMESREGADD,1);
            }
        }
        else
        {
            // 闪烁灯提示错误
            m_flash = !m_flash;
            if (m_flash)
            {
                m_lamp->setStyleSheet("background-color:red;");
            }
            else
            {
                m_lamp->setStyleSheet("background-color:yellow;");
            }
            m_lamp->setText("ERROR");
            m_com->showResponseTimeout();
            writeMultiCoil(MOTORCOILADD,1,m_run);
        }

    }
}

void MasterDialog::refreshCom()
{
    m_comComboBox->clear();
    m_com->getAvaliableComList(m_comComboBox);
}

void MasterDialog::connectCom()
{
    if (!m_connected && m_com->connectCom(m_comComboBox->currentText()))
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

void MasterDialog::disconnectCom()
{
    if (m_connected)
    {
        if (m_run)
        {
            stop();
        }
        QMessageBox::information(NULL, "tips", "Disconnect succeeded", QMessageBox::Ok);
        m_connected = false;
        m_comConnectBt->setText("Connect");
        m_comConnectBt->setDisabled(false);
        m_comConnectBt->setStyleSheet("color:black;");
        m_comDisconnectBt->setDisabled(true);
        m_comDisconnectBt->setStyleSheet("color:gray;");
        m_comRefresh->setDisabled(false);

        m_com->disconnectCom();
    }
}

void MasterDialog::setRunStatus()
{
    m_run = true;
    m_lamp->setText((QStringLiteral("")));
    m_lamp->setStyleSheet("background-color:yellow;");
}

void MasterDialog::setStopStatus()
{
    m_run = false;
    m_lamp->setText((QStringLiteral("Stop")));
    m_lamp->setStyleSheet("background-color:red;");
}

void MasterDialog::setStatus()
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

void MasterDialog::startOrStop()
{
    if (m_start)
    {
        m_run = true;
        m_start = false;
    }
    else if (m_stop)
    {
        m_run = false;
        m_stop = false;
    }
}

void MasterDialog::start()
{
    if (m_connected)
    {
        m_start = true;
        startOrStop();
        writeMultiCoil(MOTORCOILADD,1,m_run);
        m_timer->start(Timer);
    }
    else
    {
        QMessageBox::information(NULL, "tips", "Please connect first.", QMessageBox::Ok);
    }
}

void MasterDialog::stop()
{
    if (m_connected)
    {
        m_timer->stop();
        m_stop = true;
        startOrStop();
        writeMultiCoil(MOTORCOILADD,1,m_run);
    }
    else
    {
        QMessageBox::information(NULL, "tips", "Please connect first.", QMessageBox::Ok);
    }
}

void MasterDialog::getData()
{
    m_waitForResponse = false;
    QByteArray buffer;
    m_com->getBuffer(buffer);
    uint8_t respondFC = m_modbus->analyzeSlaveRespond(buffer);
    if (respondFC == 0x03 && m_run)
    {
        // 设置启动次数（寄存器的第一个值）
        m_runTimes = (unsigned char)buffer.at(0)<<8 | (unsigned char)buffer.at(1);
        m_lamp->setText(QString::number(m_runTimes));
    }
    else if (respondFC == 0x0f)
    {
        setStatus();
    }
    // 符号位为1出错（0x80 | FC）
    else if (respondFC>>(sizeof(uint8_t)*8-1))
    {
        m_timer->stop();
        // 启动、停止失败（写线圈失败）
        if (respondFC == 0x8f)
        {
            m_run = !m_run;
            QMessageBox::information(NULL, "tips", "Write coil failed.", QMessageBox::Ok);
        }
        else
        {
            QMessageBox::information(NULL, "tips", "Response data error.", QMessageBox::Ok);
        }
    }
}

void MasterDialog::readMultiCoil(unsigned short int RegAdd, unsigned short int RegCount)
{
    QByteArray buffer;
    m_modbus->getReadMultiCoilBuff(buffer, RegAdd, RegCount);
    m_waitForResponse = true;
    m_com->sendData(buffer);
}

void MasterDialog::readMultiReg(unsigned short int RegAdd, unsigned short int RegCount)
{
    QByteArray buffer;
    m_modbus->getReadMultiRegBuff(buffer, RegAdd, RegCount);
    m_waitForResponse = true;
    m_com->sendData(buffer);
}

void MasterDialog::writeMultiCoil(unsigned short int RegAdd, unsigned short int RegCount, unsigned int var)
{
    QByteArray buffer;
    if (!m_modbus->getWriteMultiCoilBuff(buffer, RegAdd, RegCount, var))
    {
        QMessageBox::information(NULL, "tips", "The data ready to send are wrong.", QMessageBox::Ok);
    }
    m_waitForResponse = true;
    m_com->sendData(buffer);
}

void MasterDialog::writeMultiReg(unsigned short int RegAdd, unsigned short int RegCount, unsigned short* var, unsigned int varNum)
{
    QByteArray buffer;
    if (!m_modbus->getwriteMultiRegBuff(buffer, RegAdd, RegCount, var, varNum))
    {
        QMessageBox::information(NULL, "tips", "The data ready to send are wrong.", QMessageBox::Ok);
    }
    m_waitForResponse = true;
    m_com->sendData(buffer);
}



