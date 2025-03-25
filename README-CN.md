# 西柏科技期末课堂作业

## 项目简介 

该项目是基于STM32微控制器开发的一个嵌入式应用程序，主要功能包括：

1. 通过UART接收命令并解析，实现串口通信控制。

2. 控制LED灯模式的切换（如闪烁模式、状态显示）。

3. 与EEPROM通信，完成数据的读写操作。

4. 使用事件回调机制简化定时器和外部中断的处理。

## 系统架构 

程序采用模块化设计，核心模块包括：
 
- **main.cpp** ：主程序逻辑。
 
- **stm32f0xx_it.cpp** ：中断服务函数。
 
- **rom.h** ：EEPROM (AT24C08C) 数据读写实现。
 
- **settings.h** ：串口重定向实现。
 
- **gpioPlus.h** ：GPIO封装类，简化了GPIO操作。
 
- **event.h** ：模板事件类，支持灵活的回调注册与触发。


---


## 功能说明 

### 1. 串口通信与命令解析 

程序通过UART接收命令，支持以下功能：

- 切换LED模式。

- EEPROM数据读写。
**核心代码解析：** 

```c++
void ProcessReceivedData(uint8_t data) {
    static uint8_t tempSerialData[RX_BUFFER_SIZE];
    static uint16_t tempSerialDataIndex = 0;

    tempSerialData[tempSerialDataIndex] = data;
    tempSerialDataIndex++;
    if (data == 0x21) { // '!' 表示命令结束符
        tempSerialData[tempSerialDataIndex - 1] = 0;
        tempSerialDataIndex = 0;
        printf("输入命令: %s\n", tempSerialData);

        char *p;
        if ((p = strstr((char *)tempSerialData, "mode")) != NULL) {
            mode = p[4] - 48; // 提取模式
        } else if ((p = strstr((char *)tempSerialData, "w 0xb")) != NULL) {
            // EEPROM写操作
        } else if ((p = strstr((char *)tempSerialData, "r 0xb")) != NULL) {
            // EEPROM读操作
        } else {
            printf("未知命令\n");
        }
    }

    if (tempSerialDataIndex >= RX_BUFFER_SIZE) {
        tempSerialDataIndex = 0;
    }
}
```

此段代码通过解析命令字符串，实现动态功能调用。


---


### 2. LED模式切换 
支持多种LED模式（如闪烁、全灭）。通过按键触发或者串口命令改变 `mode`。**代码示例：** 

```c++
void ledModeSwitch(uint8_t mode) {
    ledclear(); // 关闭所有LED
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
        printf("模式不存在\n");
        return;
    }
    printf("Mode%d activated\n", mode);
}
```
此函数通过事件回调 `milliseconds_100` 实现LED灯的闪烁逻辑。

---


### 3. EEPROM数据操作 
通过 `rom.h` 提供的封装，简化了AT24C08C EEPROM的读写操作。**代码示例：** 

```c++
// 写单字节数据
if (eeprom.writeByte(addr, data) != HAL_ERROR) {
    printf("OK\n");
} else {
    printf("FAIL\n");
}

// 读单字节数据
if (eeprom.readByte(addr, data) != HAL_ERROR) {
    printf("读取数据: 地址0x%x 数据0x%x\n", addr, data);
}
```
`rom.h` 使用了I2C协议对EEPROM进行封装，支持单字节和多字节读写。

---


### 4. GPIO封装 
`gpioPlus.h` 提供了 `Gpio` 类，简化GPIO的常见操作（如开、关、反转等），并支持状态读取。**核心代码示例：** 

```c++
Gpio led5(GPIOA, GPIO_PIN_6), led6(GPIOA, GPIO_PIN_7);

void ledclear() {
    led5.off();
    led6.off();
}

led5.toggle(); // 切换状态
led5.on();     // 打开GPIO
led5.off();    // 关闭GPIO
```

通过封装，开发者无需直接操作寄存器，大大提高了代码可读性。


---


## 类库介绍 
1. `rom.h`: 
封装了EEPROM AT24C08C的I2C读写操作，支持以下功能：
 
- `writeByte`：写入单字节数据。
 
- `readByte`：读取单字节数据。
 
- `writeBytes`：写入多字节数据，带分页支持。
 
- `readBytes`：读取多字节数据。
2. `settings.h`: 实现了UART的重定向，将 `printf` 输出重定向到串口，使调试更加方便。3. `gpioPlus.h`** 
封装了GPIO操作，提供了更简洁的接口，支持状态读取、切换、反转等功能。
4. `event.h`: 
模板事件类，支持注册和触发回调函数，广泛用于定时器和中断处理逻辑。
**示例：** 

```c++
Event<> milliseconds_100;
milliseconds_100.registerCallback([]() {
    led5.toggle();
    led6.toggle();
});
milliseconds_100.trigger();
```


---


## 如何运行 
 
1. 使用STM32CubeMX生成基础工程，确保外设配置如下：
  - 使用C++环境编译

  - GPIO：PA6, PA7用于LED控制 PA8，PB15用于按钮中断。

  - UART：用于串口通信。

  - I2C：用于与EEPROM通信。
 
2. 通过串口工具发送命令控制程序： 
  - ! 用于分隔数据
    
  - 切换LED模式：`modeX!`，例如 `mode1!` 切换到模式1。
 
  - 写EEPROM：`w 0xbAADD!`，例如 `w 0xb0110!` 将数据 `0x10` 写入地址 `0x01`。
 
  - 读EEPROM：`r 0xbAA!`，例如 `r 0xb01!` 读取地址 `0x01` 的数据。

## 展示
![1735287828367 jpg_compressed](https://github.com/user-attachments/assets/5404a76d-f7e4-46c4-93ac-b1af0b3878bd)

https://github.com/user-attachments/assets/90de9c9f-3e9d-4e44-8e27-8f8f550da950

