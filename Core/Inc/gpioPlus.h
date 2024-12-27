#ifndef __GPIO_PLUS_H__
#define __GPIO_PLUS_H__
#include "gpio.h"

#ifdef __cplusplus
/**
 * @class 自定Gpio
 * @brief 用來简化控制GPIO的操作
 */
class Gpio
{
private:
    GPIO_TypeDef *port;
    uint16_t pin;
    // 是否反转
    uint8_t reverse;

public:
    /**
     * @brief 创建新的Gpio对象
     * @param port GPIO端口
     * @param pin GPIO引脚
     * @param reverse 是否反转
     */
    Gpio(GPIO_TypeDef *port, uint16_t pin, uint8_t reverse = 0)
        : port(port), pin(pin), reverse(reverse) {}

    /**
     * @brief 创建新的Gpio对象
     * @param port GPIO端口
     * @param pin GPIO引脚
     * @param state GPIO状态
     * @param reverse 是否反转
     */
    Gpio(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state, uint8_t reverse = 0)
        : port(port), pin(pin), reverse(reverse)
    {
        HAL_GPIO_WritePin(port, pin, state);
    }

    /**
     * @brief 切换GPIO状态
     */
    void toggle()
    {
        HAL_GPIO_TogglePin(port, pin);
    }

    /**
     * @brief 打开GPIO
     */
    inline void on()
    {
        GPIO_PinState pinState = reverse ? GPIO_PIN_RESET : GPIO_PIN_SET;
        HAL_GPIO_WritePin(port, pin, pinState);
    }

    /**
     * @brief 关闭GPIO
     */
    inline void off()
    {
        GPIO_PinState pinState = reverse ? GPIO_PIN_SET : GPIO_PIN_RESET;
        HAL_GPIO_WritePin(port, pin, pinState);
    }

    /**
     * @brief 设置GPIO状态
     * @param state GPIO状态
     */
    inline void set(GPIO_PinState state)
    {
        GPIO_PinState pinState = reverse ? state : (GPIO_PinState)!state;
        HAL_GPIO_WritePin(port, pin, pinState);
    }

    /**
     * @brief 读取GPIO状态
     * @return GPIO状态
     */
    GPIO_PinState read()
    {
        return HAL_GPIO_ReadPin(port, pin);
    }
};
#else
/**
 * @brief 自定Gpio
 */
typedef struct Gpio
{
    GPIO_TypeDef *port;
    uint16_t pin;
};
#endif // __cplusplus

#endif // __GPIO_PLUS_H__