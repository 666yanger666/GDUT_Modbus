#ifndef MASTERDIALOG_H
#define MASTERDIALOG_H

#include "../ModbusProtocol/modbusprotocol.h"
#include "../ModbusSerialPort/modbusserialport.h"

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QListView>
#include <QMessageBox>
#include <QDebug>
#include <QByteArray>
#include <QTimer>

class MasterDialog : public QWidget
{
    Q_OBJECT
public:
    explicit MasterDialog(QWidget *parent = nullptr);
    ~MasterDialog();

private:
    ModbusSerialPort* m_com;
    ModbusProtocol* m_modbus;
    bool m_connected = false;
    bool m_start = false;
    bool m_run = false;
    bool m_stop = false;
    int m_runTimes = 0;
    QTimer* m_timer;
    bool m_waitForResponse = false;
    bool m_flash =false;

    // 控件
    QLabel* m_comLabel;
    QComboBox* m_comComboBox;
    QPushButton* m_comRefresh;
    QPushButton* m_comConnectBt;
    QPushButton* m_comDisconnectBt;
    QPushButton* m_startBt;
    QPushButton* m_stopBt;
    QPushButton* m_lamp;
    QGridLayout *m_gLayout;

    void initWidget();
    void connectSignalsAndSlots();
    void startOrStop();
    void setRunStatus();
    void setStopStatus();
    void setStatus();
    // 02：读多个输入线圈
    void readMultiCoil(unsigned short int RegAdd, unsigned short int RegCount);
    // 03：读多个保持寄存器
    void readMultiReg(unsigned short int RegAdd, unsigned short int RegCount);
    // 15：设置多个输出线圈（继电器）
    void writeMultiCoil(unsigned short int RegAdd, unsigned short int RegCount, unsigned int var);
    // 16：设置多个保存寄存器（16位寄存器）
    void writeMultiReg(unsigned short int RegAdd, unsigned short int RegCount, unsigned short* var, unsigned int varNum);



signals:


private slots:
    void refreshCom();
    void connectCom();
    void disconnectCom();
    void start();
    void stop();
    void getData();
    void checkCoilStatus();


};

#endif // MASTERDIALOG_H
