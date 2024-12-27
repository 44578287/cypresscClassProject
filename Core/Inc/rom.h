#ifndef __ROM_H__
#define __ROM_H__
#include "i2c.h"
#include <string.h>

class AT24C08C
{
private:
    I2C_HandleTypeDef *_i2cHandle; // I2C句柄
    uint8_t _deviceAddress;        // 设备地址
    
public:
    // 构造函数
    AT24C08C(I2C_HandleTypeDef *i2cHandle, uint8_t deviceAddress)
        : _i2cHandle(i2cHandle), _deviceAddress(deviceAddress << 1) {}

    // 写入单字节数据到指定地址
    HAL_StatusTypeDef writeByte(uint16_t memAddress, uint8_t data)
    {
        uint8_t buffer[2];
        buffer[0] = memAddress & 0xFF; // 低地址字节
        buffer[1] = data;

        return HAL_I2C_Master_Transmit(_i2cHandle, _deviceAddress, buffer, 2, HAL_MAX_DELAY);
    }

    // 从指定地址读取单字节数据
    HAL_StatusTypeDef readByte(uint16_t memAddress, uint8_t &data)
    {
        uint8_t memAddr = memAddress & 0xFF;

        // 发送内存地址
        if (HAL_I2C_Master_Transmit(_i2cHandle, _deviceAddress, &memAddr, 1, HAL_MAX_DELAY) != HAL_OK)
        {
            return HAL_ERROR;
        }

        // 读取数据
        return HAL_I2C_Master_Receive(_i2cHandle, _deviceAddress, &data, 1, HAL_MAX_DELAY);
    }

    // 写入多字节数据
    HAL_StatusTypeDef writeBytes(uint16_t memAddress, const uint8_t *data, uint16_t size)
    {
        uint8_t buffer[17]; // 每次最多写16字节（AT24C08C每页16字节）
        uint16_t offset = 0;

        while (size > 0)
        {
            uint8_t chunkSize = (size > 16) ? 16 : size; // 每次最多16字节

            buffer[0] = memAddress & 0xFF; // 低地址字节
            memcpy(&buffer[1], data + offset, chunkSize);

            // 写入数据块
            if (HAL_I2C_Master_Transmit(_i2cHandle, _deviceAddress, buffer, chunkSize + 1, HAL_MAX_DELAY) != HAL_OK)
            {
                return HAL_ERROR;
            }

            // 等待EEPROM写入完成（使用轮询方式）
            HAL_Delay(5);

            // 更新状态
            memAddress += chunkSize;
            offset += chunkSize;
            size -= chunkSize;
        }

        return HAL_OK;
    }

    // 读取多字节数据
    HAL_StatusTypeDef readBytes(uint16_t memAddress, uint8_t *data, uint16_t size)
    {
        uint8_t memAddr = memAddress & 0xFF;

        // 发送内存地址
        if (HAL_I2C_Master_Transmit(_i2cHandle, _deviceAddress, &memAddr, 1, HAL_MAX_DELAY) != HAL_OK)
        {
            return HAL_ERROR;
        }

        // 读取数据块
        return HAL_I2C_Master_Receive(_i2cHandle, _deviceAddress, data, size, HAL_MAX_DELAY);
    }
};

#endif // __ROM_H__