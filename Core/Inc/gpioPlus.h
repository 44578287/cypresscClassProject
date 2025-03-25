#ifndef __GPIO_PLUS_H__
#define __GPIO_PLUS_H__
#include "gpio.h"

#ifdef __cplusplus
/**
 * @class Custom GPIO
 * @brief Simplifying GPIO control operations
 */
class Gpio
{
private:
    GPIO_TypeDef *port;
    uint16_t pin;
    // Reverse
    uint8_t reverse;

public:
    /**
     * @brief Create a new GPIO object
     * @param port GPIO port
     * @param pin GPIO pins
     * @param reverse Reverse
     */
    Gpio(GPIO_TypeDef *port, uint16_t pin, uint8_t reverse = 0)
        : port(port), pin(pin), reverse(reverse) {}

    /**
     * @brief Create a new GPIO object
     * @param port GPIO port
     * @param pin GPIO pins
     * @param state GPIO Status
     * @param reverse Reverse
     */
    Gpio(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state, uint8_t reverse = 0)
        : port(port), pin(pin), reverse(reverse)
    {
        HAL_GPIO_WritePin(port, pin, state);
    }

    /**
     * @brief Toggle GPIO state
     */
    void toggle()
    {
        HAL_GPIO_TogglePin(port, pin);
    }

    /**
     * @brief Enable GPIO
     */
    inline void on()
    {
        GPIO_PinState pinState = reverse ? GPIO_PIN_RESET : GPIO_PIN_SET;
        HAL_GPIO_WritePin(port, pin, pinState);
    }

    /**
     * @brief Turn off the GPIO
     */
    inline void off()
    {
        GPIO_PinState pinState = reverse ? GPIO_PIN_SET : GPIO_PIN_RESET;
        HAL_GPIO_WritePin(port, pin, pinState);
    }

    /**
     * @brief Setting GPIO Status
     * @param state GPIO Status
     */
    inline void set(GPIO_PinState state)
    {
        GPIO_PinState pinState = reverse ? state : (GPIO_PinState)!state;
        HAL_GPIO_WritePin(port, pin, pinState);
    }

    /**
     * @brief Read GPIO status
     * @return GPIO Status
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