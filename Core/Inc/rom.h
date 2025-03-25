#ifndef __ROM_H__
#define __ROM_H__
#include "i2c.h"
#include <string.h>

class AT24C08C
{
private:
    I2C_HandleTypeDef *_i2cHandle; // I2C handle
    uint8_t _deviceAddress;        // Device address
    
public:
    // Constructor
    AT24C08C(I2C_HandleTypeDef *i2cHandle, uint8_t deviceAddress)
        : _i2cHandle(i2cHandle), _deviceAddress(deviceAddress << 1) {}

    // Write a single byte of data to a specified address
    HAL_StatusTypeDef writeByte(uint16_t memAddress, uint8_t data)
    {
        uint8_t buffer[2];
        buffer[0] = memAddress & 0xFF; // Low-order byte
        buffer[1] = data;

        return HAL_I2C_Master_Transmit(_i2cHandle, _deviceAddress, buffer, 2, HAL_MAX_DELAY);
    }

    // Read a single byte of data from the specified address
    HAL_StatusTypeDef readByte(uint16_t memAddress, uint8_t &data)
    {
        uint8_t memAddr = memAddress & 0xFF;

        // Send memory address
        if (HAL_I2C_Master_Transmit(_i2cHandle, _deviceAddress, &memAddr, 1, HAL_MAX_DELAY) != HAL_OK)
        {
            return HAL_ERROR;
        }

        // Reading data
        return HAL_I2C_Master_Receive(_i2cHandle, _deviceAddress, &data, 1, HAL_MAX_DELAY);
    }

    // Write multi-byte data.
    HAL_StatusTypeDef writeBytes(uint16_t memAddress, const uint8_t *data, uint16_t size)
    {
        uint8_t buffer[17]; // Write a maximum of 16 bytes each time (16 bytes per page on the AT24C08C)
        uint16_t offset = 0;

        while (size > 0)
        {
            uint8_t chunkSize = (size > 16) ? 16 : size; // Maximum 16 bytes per time.

            buffer[0] = memAddress & 0xFF; // Low-order byte
            memcpy(&buffer[1], data + offset, chunkSize);

            // Write data block
            if (HAL_I2C_Master_Transmit(_i2cHandle, _deviceAddress, buffer, chunkSize + 1, HAL_MAX_DELAY) != HAL_OK)
            {
                return HAL_ERROR;
            }

            // Waiting for EEPROM write completion (using polling).
            HAL_Delay(5);

            // Update Status
            memAddress += chunkSize;
            offset += chunkSize;
            size -= chunkSize;
        }

        return HAL_OK;
    }

    // Reading multi-byte data
    HAL_StatusTypeDef readBytes(uint16_t memAddress, uint8_t *data, uint16_t size)
    {
        uint8_t memAddr = memAddress & 0xFF;

        // Send memory address
        if (HAL_I2C_Master_Transmit(_i2cHandle, _deviceAddress, &memAddr, 1, HAL_MAX_DELAY) != HAL_OK)
        {
            return HAL_ERROR;
        }

        // Read data block
        return HAL_I2C_Master_Receive(_i2cHandle, _deviceAddress, data, size, HAL_MAX_DELAY);
    }
};

#endif // __ROM_H__