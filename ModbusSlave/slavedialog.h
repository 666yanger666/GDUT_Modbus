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
    int m_runTimes = 0;

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

    // 03：读多个保持寄存器

    // 15：设置多个输出线圈（继电器）

    // 16：设置多个保存寄存器（16位寄存器）




signals:


private slots:
    void refreshCom();
    void connectCom();
    void disconnectCom();
    void getData();

};

#endif // SLAVEDIALOG_H
