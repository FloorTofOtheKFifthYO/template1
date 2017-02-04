二值化的传感器一般使用外部中断的形式来接收信息

外部中断：使用io口作为中断，stm32f4xx_exti.c

STM32F407 的中断控制器支持 22 个外部
中断/事件请求。每个中断设有状态位，每个中断/事件都有独立的触发和屏蔽设置。STM32F407
的 22 个外部中断为：
EXTI 线 0~15：对应外部 IO 口的输入中断。
EXTI 线 16：连接到 PVD 输出。
EXTI 线 17：连接到 RTC 闹钟事件。
EXTI 线 18：连接到 USB OTG FS 唤醒事件。
EXTI 线 19：连接到以太网唤醒事件。
EXTI 线 20：连接到 USB OTG HS(在 FS 中配置)唤醒事件。
EXTI 线 21：连接到 RTC 入侵和时间戳事件。
EXTI 线 22：连接到 RTC 唤醒事件。

使用io口做中断时，使用EXIT线0-15，GPIO 的管脚 GPIOx.0~GPIOx.15(x=A,B,C,D,E，F,G,H,I)分别对应中断线 0~15
使用时需要重新配置，确定外部中断线与IO口的对应关系，并使能外部中断

SYSCFG
系统配置控制器主要用于管理对可执行代码的存储区域的地址重映射、选择以太网 PHY 接
口以及管理 GPIO 的外部中断线连接，使用该管理器进行对外部中断的配置
具体看代码 /EXIT

STM32F4 的 IO 口外部中断函数只有 7 个，
分别为：
> EXPORT EXTI0_IRQHandler
> EXPORT EXTI1_IRQHandler
> EXPORT EXTI2_IRQHandler
> EXPORT EXTI3_IRQHandler
> EXPORT EXTI4_IRQHandler
> EXPORT EXTI9_5_IRQHandler
> EXPORT EXTI15_10_IRQHandler

***

` 使用 IO 口外部中断的一般步骤：
1. 使能 IO 口时钟，初始化 IO 口为输入。
2. 使能 SYSCFG 时钟，设置 IO 口与中断线的映射关系。
3. 初始化线上中断，设置触发条件等。
4. 配置中断分组（NVIC），并使能中断。
5. 编写中断服务函数。 `
