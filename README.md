# 项目名称

本项目基于 STM32F10x 系列开发，包含多种硬件驱动和网络功能模块，适用于嵌入式开发和物联网场景。

## 项目结构

```
ROOT
├── CORE/                   # ARM Cortex-M3 核心代码及启动文件
│   ├── core_cm3.c
│   ├── core_cm3.h
│   ├── startup_stm32f10x_hd.s
│   └── startup_stm32f10x_md.s
├── HARDWARE/               # 硬件驱动模块
│   ├── BEEP/               # 蜂鸣器驱动
│   │   ├── beep.c
│   │   └── beep.h
│   ├── BH1750/             # BH1750 传感器驱动
│   │   ├── bh1750.c
│   │   └── bh1750.h
│   ├── DHT11/              # DHT11 温湿度传感器驱动
│   │   ├── dht11.c
│   │   └── dht11.h
│   ├── LED/                # LED 驱动
│   │   ├── led.c
│   │   └── led.h
│   └── OLED/               # OLED 显示驱动
│       ├── oled.c
│       ├── oled.h
│       └── oledfont.h
├── NET/                    # 网络模块
│   ├── cJSON/              # JSON 解析模块
│   │   ├── cJSON.c
│   │   └── cJSON.h
│   ├── device/             # ESP8266 驱动模块
│   │   ├── esp8266.c
│   │   └── esp8266.h
│   ├── MQTT/               # MQTT 协议模块
│   │   ├── Common.h
│   │   ├── MqttKit.c
│   │   ├── MqttKit.h
│   │   └── sample.c
│   └── onenet/             # OneNet 平台接口
│       ├── onenet.c
│       └── onenet.h
├── STM32F10x_FWLib/        # STM32 官方固件库
│   ├── inc/                # 头文件
│   └── src/                # 源代码
├── SYSTEM/                 # 系统功能模块
│   ├── delay/              # 延时功能
│   │   ├── delay.c
│   │   └── delay.h
│   ├── EXTI/               # 外部中断功能
│   │   ├── exti.c
│   │   └── exti.h
│   ├── sys/                # 系统功能封装
│   │   ├── sys.c
│   │   └── sys.h
│   ├── TIMER/              # 定时器功能
│   │   ├── timer.c
│   │   └── timer.h
│   └── usart/              # 串口通信功能
│       ├── usart.c
│       └── usart.h
├── USER/                   # 用户代码和配置文件
│   ├── main.c              # 主程序入口
│   ├── stm32f10x.h         # STM32 头文件
│   ├── stm32f10x_it.c      # 中断处理函数
│   ├── stm32f10x_it.h
│   ├── system_stm32f10x.c  # 系统初始化
│   └── system_stm32f10x.h
└── OBJ/                    # 编译生成的中间文件
    ├── *.o
    ├── *.d
    ├── *.hex
    └── *.axf
```

## 使用说明

请根据项目需求，加载代码至 Keil 或其他支持的 IDE 环境，编译并下载至目标硬件。

## 注意事项

- 项目依赖 STM32F10x 官方固件库。
- 使用前请确保硬件连接正确，模块配置无误。
