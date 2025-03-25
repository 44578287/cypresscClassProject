
# Cypress Technology End-of-Term Classroom Assignment

## Project Introduction

This project is an embedded application developed based on the STM32 microcontroller. The main functions include:

1. Receive and parse commands through UART to achieve serial communication control.
2. Control the switching of LED light modes (such as blinking mode, status display).
3. Communicate with EEPROM to perform read and write operations.
4. Use an event callback mechanism to simplify the handling of timers and external interrupts.

## System Architecture

The program adopts a modular design, with core modules including:

- **main.cpp**: Main program logic.
- **stm32f0xx_it.cpp**: Interrupt service functions.
- **rom.h**: EEPROM (AT24C08C) data read and write implementation.
- **settings.h**: Serial port redirection implementation.
- **gpioPlus.h**: GPIO wrapper class, simplifying GPIO operations.
- **event.h**: Template event class, supporting flexible callback registration and triggering.

---

## Function Description

### 1. Serial Communication and Command Parsing

The program receives commands via UART, supporting the following functions:

- Switch LED modes.
- EEPROM data read and write.

**Core Code Analysis:**

```c++
void ProcessReceivedData(uint8_t data) {
    static uint8_t tempSerialData[RX_BUFFER_SIZE];
    static uint16_t tempSerialDataIndex = 0;

    tempSerialData[tempSerialDataIndex] = data;
    tempSerialDataIndex++;
    if (data == 0x21) { // '!' indicates end of command
        tempSerialData[tempSerialDataIndex - 1] = 0;
        tempSerialDataIndex = 0;
        printf("Input command: %s", tempSerialData);

        char *p;
        if ((p = strstr((char *)tempSerialData, "mode")) != NULL) {
            mode = p[4] - 48; // Extract mode
        } else if ((p = strstr((char *)tempSerialData, "w 0xb")) != NULL) {
            // EEPROM write operation
        } else if ((p = strstr((char *)tempSerialData, "r 0xb")) != NULL) {
            // EEPROM read operation
        } else {
            printf("Unknown command");
        }
    }

    if (tempSerialDataIndex >= RX_BUFFER_SIZE) {
        tempSerialDataIndex = 0;
    }
}
```

This code parses the command string and dynamically calls functions.

---

### 2. LED Mode Switching

Supports multiple LED modes (such as blinking, full off). The mode can be changed by pressing a button or using serial commands to modify the `mode`.

**Code Example:**

```c++
void ledModeSwitch(uint8_t mode) {
    ledclear(); // Turn off all LEDs
    switch (mode) {
    case 1:
        led5.on();
        milliseconds_100.registerCallback([]() {
            led5.toggle();
            led6.toggle();
        });
        break;
    case 2:
        milliseconds_100.registerCallback([]() {
            led5.toggle();
            led6.toggle();
        });
        break;
    default:
        printf("Mode does not exist");
        return;
    }
    printf("Mode%d activated", mode);
}
```

This function uses the event callback `milliseconds_100` to implement the blinking logic for the LEDs.

---

### 3. EEPROM Data Operations

The `rom.h` file simplifies the read and write operations for the AT24C08C EEPROM.

**Code Example:**

```c++
// Write a single byte of data
if (eeprom.writeByte(addr, data) != HAL_ERROR) {
    printf("OK");
} else {
    printf("FAIL");
}

// Read a single byte of data
if (eeprom.readByte(addr, data) != HAL_ERROR) {
    printf("Read data: Address 0x%x Data 0x%x", addr, data);
}
```

`rom.h` uses the I2C protocol to wrap EEPROM operations, supporting both single-byte and multi-byte reads and writes.

---

### 4. GPIO Wrapping

The `gpioPlus.h` file provides the `Gpio` class, simplifying common GPIO operations (such as turning on, off, toggling, etc.) and supports state reading.

**Core Code Example:**

```c++
Gpio led5(GPIOA, GPIO_PIN_6), led6(GPIOA, GPIO_PIN_7);

void ledclear() {
    led5.off();
    led6.off();
}

led5.toggle(); // Toggle state
led5.on();     // Turn on GPIO
led5.off();    // Turn off GPIO
```

With this wrapping, developers no longer need to directly manipulate registers, greatly improving code readability.

---

## Library Introduction

1. `rom.h`: 
   Encapsulates the I2C read and write operations for the AT24C08C EEPROM, supporting the following functions:
   - `writeByte`: Writes a single byte of data.
   - `readByte`: Reads a single byte of data.
   - `writeBytes`: Writes multiple bytes of data with paging support.
   - `readBytes`: Reads multiple bytes of data.

2. `settings.h`: Implements UART redirection, allowing `printf` output to be redirected to the serial port for easier debugging.
3. `gpioPlus.h`: 
   Simplifies GPIO operations, providing a more concise interface that supports status reading, toggling, and inversion.
4. `event.h`: 
   A template event class that supports registering and triggering callback functions, widely used for timer and interrupt handling logic.

**Example:**

```c++
Event<> milliseconds_100;
milliseconds_100.registerCallback([]() {
    led5.toggle();
    led6.toggle();
});
milliseconds_100.trigger();
```

---

## How to Run

1. Use STM32CubeMX to generate the basic project, ensuring the peripheral configuration is as follows:
   - Compile in a C++ environment.
   - GPIO: PA6, PA7 for LED control, PA8, PB15 for button interrupts.
   - UART: For serial communication.
   - I2C: For communication with EEPROM.

2. Use a serial tool to send commands to control the program:
   - `!` is used to separate data.
   - Switch LED mode: `modeX!`, for example, `mode1!` switches to mode 1.
   - Write to EEPROM: `w 0xbAADD!`, for example, `w 0xb0110!` writes data `0x10` to address `0x01`.
   - Read from EEPROM: `r 0xbAA!`, for example, `r 0xb01!` reads data from address `0x01`.

## Display
![1735287828367 jpg_compressed](https://github.com/user-attachments/assets/5404a76d-f7e4-46c4-93ac-b1af0b3878bd)

https://github.com/user-attachments/assets/90de9c9f-3e9d-4e44-8e27-8f8f550da950

--- 
