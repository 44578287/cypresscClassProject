#ifndef __SETTINS_H__
#define __SETTINS_H__

#include "usart.h"

// 重定向输出
#ifdef REDIRECT_OUTPUT
#ifdef __cplusplus
extern "C"
{
    int _write(int file, char *ptr, int len)
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
        return len;
    }
}
#else
int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
}
#endif // __cplusplus
#endif // REDIRECT_OUTPUT

#endif // __SETTINS_H__