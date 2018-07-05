#ifndef MODBUSPROTOCOL_H
#define MODBUSPROTOCOL_H

#include "modbusprotocol_global.h"

extern const unsigned char table_crc_hi[];
extern const unsigned char table_crc_lo[];

class MODBUSPROTOCOLSHARED_EXPORT ModbusProtocol
{
public:
    ModbusProtocol();
    // 主站
    // FC: 02
    void getReadMultiCoilBuff(QByteArray &buffer, uint16_t RegAdd, uint16_t RegCount);
    // FC: 03
    void getReadMultiRegBuff(QByteArray &buffer, uint16_t RegAdd, uint16_t RegCount);
    // FC: 0F
    bool getWriteMultiCoilBuff(QByteArray &buffer, uint16_t RegAdd, uint16_t RegCount, uint var);
    // FC: 10
    bool getwriteMultiRegBuff(QByteArray &buffer, uint16_t RegAdd, uint16_t RegCount, uint16_t* var, uint varNum);
    // 分析从站响应
    int analyzeSlaveRespond (QByteArray &buffer);
    // 从站
    // 分析主站请求
    int analyzeMasterRequest (QByteArray &buffer);
    // FC: 02
    void getReadMultiCoilRespondBuff(QByteArray &buffer, QByteArray RespondData);
    // FC: 03
    void getReadMultiRegRespondBuff(QByteArray &buffer, QByteArray RespondData);
    // FC: 0F
    bool getWriteMultiCoilRespondBuff(QByteArray &buffer, QByteArray RespondData);
    // FC: 10
    bool getwriteMultiRegRespondBuff(QByteArray &buffer, QByteArray RespondData);

private:

    // 计算CRC16校验码
    static uint16_t calcCrc16Fast(uint8_t *buffer, uint16_t bufferLength);
    void addCrc16(QByteArray &buffer);
    // 分析请求响应相应数据
    int analyzeData (QByteArray &buffer, bool isMasterRequest);
    int getMasterRequestPDUIndex();
    int getMasterRequestPDUSize(QByteArray buffer, int FC);
    int getSlaveRespondPDUIndex(uint16_t FC);
    int getSlaveRespondPDUSize(QByteArray buffer, int FC);
    // 获取功能码
    uint16_t getDataFC(QByteArray buffer);
    // 校验数据长度
    bool analyzeBufferSize(QByteArray buffer, int PDUIndex, int PDUSize);
    // 校验CRC
    bool analyzeCRC16(QByteArray buffer, int PDUIndex, int PDUSize);
    // 提取PDU数据
    void getPDUData(QByteArray &buffer, int PDUIndex, int PDUSize);

    void getReadBuff(QByteArray &buffer, uint16_t FC, uint16_t RegAdd, uint16_t RegCount);
    bool getWriteRespondBuff(QByteArray &buffer, uint16_t FC, QByteArray RespondData);
};

#endif // MODBUSPROTOCOL_H
