#include "modbusprotocol.h"
#include <QByteArray>
#include <QtMath>
#include <QDebug>

#define IDBUFFSIZE 1
#define FCBUFFSIZE 1
#define DATASIZEBUFFSIZE 1
#define ADDBUFFSIZE 2
#define COUNTBUFFSIZE 2
#define CRCBUFFSIZE 2
#define ERROR -1

static const uint8_t table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

static const uint8_t table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

ModbusProtocol::ModbusProtocol()
{

}

// crc 16循环校验
uint16_t ModbusProtocol::calcCrc16Fast(uint8_t *buffer, uint16_t bufferLength)
{
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    while (bufferLength--) {
        i = crc_hi ^ *buffer++; /* calculate the CRC  */
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }

    return (crc_hi << 8 | crc_lo);
}
void ModbusProtocol::addCrc16(QByteArray &buffer)
{
    uint16_t crc16;
    crc16= calcCrc16Fast((uint8_t*)buffer.data(), buffer.size());
    buffer.append((uint8_t)(crc16 >> 8)); // CRC高位
    buffer.append((uint8_t)crc16); // CRC低位
}

// 主站
void ModbusProtocol::getReadBuff(QByteArray &buffer, uint16_t FC, uint16_t RegAdd, uint16_t RegCount)
{
    buffer.resize(6);
    buffer[0]= 0x01; // 从站地址
    buffer[1]= (uint8_t)FC; // 功能码
    buffer[2]= (uint8_t)(RegAdd >> 8); // 寄存器高位地址
    buffer[3]= (uint8_t)RegAdd; // 寄存器低位地址
    buffer[4]= (uint8_t)(RegCount >> 8); // 寄存器数量高位
    buffer[5]= (uint8_t)RegCount; // 寄存器数量低位
    addCrc16(buffer);
}
// 0x02：读多个输入线圈
void ModbusProtocol::getReadMultiCoilBuff(QByteArray &buffer, uint16_t RegAdd, uint16_t RegCount)
{
    getReadBuff(buffer, 0x02, RegAdd, RegCount);
}
// 0x03：读多个保持寄存器
void ModbusProtocol::getReadMultiRegBuff(QByteArray &buffer, uint16_t RegAdd, uint16_t RegCount)
{
    getReadBuff(buffer, 0x03, RegAdd, RegCount);
}

// 0x0F：写多个输出线圈
bool ModbusProtocol::getWriteMultiCoilBuff(QByteArray &buffer, uint16_t RegAdd, uint16_t RegCount,uint var)
{
    int PDUIndex = IDBUFFSIZE + FCBUFFSIZE + ADDBUFFSIZE + COUNTBUFFSIZE + 1;
    int PDUSize = (RegCount/8) + (RegCount%8==0?0:1); // 发送的数据字节个数
    if (var > qPow(2,PDUSize*8)-1)
    {
        return false;
    }
    int bufferSize = PDUIndex + PDUSize;
    buffer.resize(bufferSize);
    buffer[0]= 0x01;
    buffer[1]= 0x0F; // 功能码
    buffer[2]= (uint8_t)(RegAdd >> 8);
    buffer[3]= (uint8_t)RegAdd;
    buffer[4]= (uint8_t)(RegCount >> 8);
    buffer[5]= (uint8_t)RegCount;
    buffer[6]= (uint8_t)PDUSize; // 发送数据字节数
    // 数据
    for (int i = PDUIndex; i < (RegCount/8); i++)
    {
        buffer[i]= (unsigned char)(var>>8*(i-PDUIndex));
    }
    for (int i=0; i<(bufferSize-(RegCount/8)); i++)
    {
        //剩余位补0
    }
    addCrc16(buffer);
    return true;
}
// 0x10:写多个保持寄存器（16位）
bool ModbusProtocol::getwriteMultiRegBuff(QByteArray &buffer, uint16_t RegAdd, uint16_t RegCount, uint16_t* var, uint varNum)
{
    int PDUIndex = IDBUFFSIZE + FCBUFFSIZE + ADDBUFFSIZE + COUNTBUFFSIZE + 1;
    int PDUSize = varNum*2; // 发送的数据字节个数
    if (varNum != RegCount)
    {
        return false;
    }
    int bufferSize = PDUIndex + PDUSize;
    buffer.resize(bufferSize);
    buffer[0]= 0x01;
    buffer[1]= 0x10; // 功能码
    buffer[2]= (unsigned char)(RegAdd >> 8);
    buffer[3]= (unsigned char)RegAdd;
    buffer[4]= (unsigned char)(RegCount >> 8);
    buffer[5]= (unsigned char)RegCount;
    buffer[6]= (unsigned char)PDUSize; // 发送数据字节数
    // 数据
    for (int i = PDUIndex; i < PDUIndex+PDUSize; i+=2)
    {
        buffer[i]= (unsigned char)(*var>>8);
        buffer[i+1]= (unsigned char)*var;
    }
    addCrc16(buffer);
    return true;
}

// 0x02：响应读多个输入线圈
void ModbusProtocol::getReadMultiCoilRespondBuff(QByteArray &buffer, uint8_t* var, uint coilNum)
{  
    uint16_t respondDataSize = (coilNum/8) + (coilNum%8==0?0:1);
    buffer.resize(IDBUFFSIZE+FCBUFFSIZE+DATASIZEBUFFSIZE+respondDataSize);
    buffer[0]= 0x01;
    buffer[1]= 0x02; // 功能码
    buffer[3] = respondDataSize;
    for (int i=0; i<(coilNum/8); i++)
    {
        uint8_t respondDataByte = 0x00;
        for (int j=0; j<8; j++)
        {
            respondDataByte |= (uint8_t)((*(var+i+j))<<(7-j));
        }
        buffer[IDBUFFSIZE+FCBUFFSIZE+DATASIZEBUFFSIZE+i] = respondDataByte;
    }
    for (int i=0; i<(respondDataSize-(coilNum/8)); i++)
    {
        //剩余位补0
    }
    addCrc16(buffer);
}
// 0x03：响应读多个保持寄存器
void ModbusProtocol::getReadMultiRegRespondBuff(QByteArray &buffer, uint16_t* var, uint regNum)
{
    uint16_t respondDataSize = regNum*2;
    buffer.resize(IDBUFFSIZE+FCBUFFSIZE+DATASIZEBUFFSIZE+respondDataSize);
    buffer[0]= 0x01;
    buffer[1]= 0x03; // 功能码
    buffer[2] = respondDataSize;
    for (int i=0; i<respondDataSize; i+=2)
    {
        buffer[IDBUFFSIZE+FCBUFFSIZE+DATASIZEBUFFSIZE+i] = (unsigned char)((*(var+i)) >> 8);
        buffer[IDBUFFSIZE+FCBUFFSIZE+DATASIZEBUFFSIZE+i+1] = (unsigned char)(*(var+i));
    }
    addCrc16(buffer);
}
// 0x0F：响应写多个输出线圈
bool ModbusProtocol::getWriteMultiCoilRespondBuff(QByteArray &buffer, QByteArray RespondData)
{
    return getWriteRespondBuff(buffer, 0x0F, RespondData);
}
// 0x10:响应写多个保持寄存器（16位）
bool ModbusProtocol::getwriteMultiRegRespondBuff(QByteArray &buffer, QByteArray RespondData)
{
    return getWriteRespondBuff(buffer, 0x10, RespondData);
}

bool ModbusProtocol::getWriteRespondBuff(QByteArray &buffer, uint16_t FC, QByteArray RespondData)
{
    if (RespondData.size() != 4)
    {
        return false;
    }
    buffer.resize(2);
    buffer[0]= 0x01;
    buffer[1]= (uint8_t)FC; // 功能码
    buffer.append(RespondData);
    addCrc16(buffer);
    return true;
}

uint16_t ModbusProtocol::getDataFC(QByteArray buffer)
{
    if (buffer.size()>1)
    {
        return buffer.at(1);
    }
    return ERROR;
}

bool ModbusProtocol::analyzeBufferSize(QByteArray buffer, int PDUIndex, int PDUSize)
{
    int bufferSize = PDUIndex+PDUSize+CRCBUFFSIZE;
    // 数据长度校验
    if (buffer.size() != bufferSize)
    {
        qDebug() << "wrong data length";
        return false;
    }
    return true;
}

bool ModbusProtocol::analyzeCRC16(QByteArray buffer, int PDUIndex, int PDUSize)
{
    unsigned short int crc16;
    crc16 = calcCrc16Fast((unsigned char*)buffer.data(), PDUIndex+PDUSize);
    // CRC校验
    if ((unsigned char)buffer.at(PDUIndex+PDUSize) != (unsigned char)(crc16 >> 8) ||
        (unsigned char)buffer.at(PDUIndex+PDUSize+1) != (unsigned char)crc16)
    {
        qDebug() << "wrong crc16";
        return false;
    }
    return true;
}

void ModbusProtocol::getPDUData(QByteArray &buffer, int PDUIndex, int PDUSize)
{
    // 取数据部分
    buffer = buffer.mid(PDUIndex,PDUSize);
}

int ModbusProtocol::analyzeData (QByteArray &buffer, bool isMasterRequest)
{
    qDebug() << buffer.toHex().toUpper();
    int respondFC = getDataFC(buffer);
    if (respondFC == ERROR)
    {
        return ERROR;
    }
    int PDUIndex=0, PDUSize=0;
    if (!isMasterRequest)
    {
        PDUIndex = getSlaveRespondPDUIndex(respondFC);
        PDUSize =  getSlaveRespondPDUSize(buffer,respondFC);
    }
    else
    {
        PDUIndex = getMasterRequestPDUIndex();
        PDUSize =  getMasterRequestPDUSize(buffer,respondFC);
    }
    if ( analyzeBufferSize(buffer,PDUIndex,PDUSize)
         && analyzeCRC16(buffer,PDUIndex,PDUSize) )
    {
        getPDUData(buffer,PDUIndex,PDUSize);
        return respondFC;
    }
    return ERROR;
}

// 获取从站响应PDU数据首地址
int ModbusProtocol::getSlaveRespondPDUIndex(uint16_t FC)
{
    // 判断返回FC类型（屏蔽错误响应FC+0x80）
    // 0x02 0x03
    if ((FC&0x0f) == 0x02 || (FC&0x0f) == 0x03)
    {
        if (FC == 0x02 || FC == 0x03)
        {
            return IDBUFFSIZE+FCBUFFSIZE+1;
        }
        else    // 错误响应
        {
            return 2;
        }
    }
    // 0x0f 0x10
    else if ((FC&0x0f) == 0x0f || ((FC-1)&0x0f) == 0x0f)
    {
        if (FC == 0x0f || FC == 0x10)
        {
            return IDBUFFSIZE+FCBUFFSIZE;
        }
        else    // 错误响应
        {
            return 2;
        }
    }
    return -1;
}
// 获取从站响应PDU数据长度
int ModbusProtocol::getSlaveRespondPDUSize(QByteArray buffer, int FC)
{
    // 判断返回FC类型（屏蔽错误响应FC+0x80）
    // 0x02 0x03
    if ((FC&0x0f) == 0x02 || (FC&0x0f) == 0x03)
    {
        if (FC == 0x02 || FC == 0x03)
        {
            return (int)buffer.at(2);
        }
        else    // 错误响应
        {
            return 1;
        }
    }
    // 0x0f 0x10
    else if ((FC&0x0f) == 0x0f || ((FC-1)&0x0f) == 0x0f)
    {
        if (FC == 0x0f || FC == 0x10)
        {
            return ADDBUFFSIZE+COUNTBUFFSIZE;
        }
        else    // 错误响应
        {
            return 1;
        }
    }
    return -1;
}
// 分析从站响应PDU数据
int ModbusProtocol::analyzeSlaveRespond (QByteArray &buffer)
{
    return analyzeData(buffer,false);
}

// 获取主站请求PDU数据首地址
int ModbusProtocol::getMasterRequestPDUIndex()
{
    return IDBUFFSIZE+FCBUFFSIZE;
}
// 获取主站请求PDU数据长度
int ModbusProtocol::getMasterRequestPDUSize(QByteArray buffer, int FC)
{
    // 判断请求FC类型
    // 0x02 0x03
    if (FC == 0x02 || FC == 0x03)
    {
        return ADDBUFFSIZE+COUNTBUFFSIZE;
    }
    // 0x0f 0x10
    else if (FC == 0x0f || FC == 0x10)
    {
        return ADDBUFFSIZE+COUNTBUFFSIZE+1+(int)buffer.at(6);
    }
    return -1;
}
// 分析主站请求PDU数据
int ModbusProtocol::analyzeMasterRequest (QByteArray &buffer)
{
    return analyzeData(buffer,true);
}

