#ifndef SLAVEDIALOG_H
#define SLAVEDIALOG_H

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

class SlaveDialog : public QWidget
{
    Q_OBJECT
public:
    explicit SlaveDialog(QWidget *parent = nullptr);
    ~SlaveDialog();

private:
    ModbusSerialPort* m_com;
    ModbusProtocol* m_modbus;
    bool m_connected = false;
    bool m_runLastTime = false;
    bool m_run = false;
    uint16_t m_runTimes = 0;

    // 控件
    QLabel* m_comLabel;
    QComboBox* m_comComboBox;
    QPushButton* m_comRefresh;
    QPushButton* m_comConnectBt;
    QPushButton* m_comDisconnectBt;
    QPushButton* m_lamp;
    QGridLayout *m_gLayout;

    void initWidget();
    void connectSignalsAndSlots();
    void setRunStatus();
    void setStopStatus();
    void setStatus();
    // 02：读多个输入线圈
    void readMultiCoilRespond(QByteArray &buffer, uint8_t* var, uint coilNum);
    // 03：读多个保持寄存器
    void readMultiRegRespond(QByteArray &buffer, uint16_t* var, uint regNum);
    // 15：设置多个输出线圈（继电器）
    void writeMultiCoilRespond(QByteArray &buffer, QByteArray respondData);
    // 16：设置多个保存寄存器（16位寄存器）
    void writeMultiRegRespond(QByteArray &buffer, QByteArray respondData);



signals:


private slots:
    void refreshCom();
    void connectCom();
    void disconnectCom();
    void getData();

};

#endif // SLAVEDIALOG_H
