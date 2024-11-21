

#include "led.h"
#include "beep.h"
#include "delay.h"
#include "sys.h"
#include "timer.h"
#include "usart.h"
#include "dht11.h"
#include "bh1750.h"
#include "oled.h"
#include "stdio.h"
#include "esp8266.h"
#include "onenet.h"

u8 alarmFlag = 0;//是否报警的标志
u8 alarm_is_free = 10;//报警器是否被手动操作，如果被手动操作即设置为0



u8 humidityH;	  //湿度整数部分
u8 humidityL;	  //湿度小数部分
u8 temperatureH;   //温度整数部分
u8 temperatureL;   //温度小数部分
extern char oledBuf[20];
float Light = 0; //光照度
u8 Led_Status = 0;

char PUB_BUF[256];//上传数据的buf
const char *devSubTopic[] = {"/mysmarthome/sub"};
const char devPubTopic[] = "/mysmarthome/pub";
u8 ESP8266_INIT_OK = 0;//esp8266初始化完成标志



 int main(void)
 {	
	unsigned short timeCount = 0;	//发送间隔变量
	unsigned char *dataPtr = NULL;
	Usart1_Init(115200);//debug串口
	delay_init();	    	 //延时函数初始化	
	delay_ms(500);// 延迟一下等待oled识别
	OLED_Init();
	OLED_ColorTurn(0);//0正常显示，1 反色显示
  OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	OLED_Clear();
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	LED_Init();		  	//初始化与LED连接的硬件接口
	//EXTIX_Init();		//外部中断初始化
	BEEP_Init();
	DHT11_Init();
  BH1750_Init();
	Usart2_Init(115200);//stm32-8266通讯串口	
	delay_ms(1000);	
	ESP8266_Init();					//初始化ESP8266	
	while(OneNet_DevLink()){//接入OneNET
		delay_ms(500);
	}		
	
	OLED_Clear();	
	
	TIM2_Int_Init(4999,7199);
	TIM3_Int_Init(2499,7199);
	
	BEEP = 0;//鸣叫提示接入成功
	delay_ms(250);
	BEEP = 1;
	
	OneNet_Subscribe(devSubTopic, 1);
	
	while(1)
	{
		if(timeCount % 40 == 0)//1000ms / 25 = 40 一秒执行一次
		{
			/********** 温湿度传感器获取数据**************/
			DHT11_Read_Data(&humidityH,&humidityL,&temperatureH,&temperatureL);
			
			/********** 光照度传感器获取数据**************/
			if (!i2c_CheckDevice(BH1750_Addr))
			{
				Light = LIght_Intensity();
			}
			
			/********** 读取LED1的状态 **************/
			Led_Status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);
			
			/********** 报警逻辑 **************/
			if((humidityH < 80) && (temperatureH < 30) && (Light < 1000))alarmFlag = 0;
			else alarmFlag = 1;
			
			/********** 输出调试信息 **************/
			DEBUG_LOG(" | 湿度：%d.%d C | 温度：%d.%d %% | 光照度：%.1f lx | 指示灯：%s | 报警器：%s | ",humidityH,humidityL,temperatureH,temperatureL,Light,Led_Status?"关闭":"【启动】",alarmFlag?"【启动】":"停止");
		}
		if(++timeCount >= 200)	// 5000ms / 25 = 200 发送间隔5000ms
		{
			Led_Status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);//读取LED0的状态
			DEBUG_LOG("*************************************************************************************");
			DEBUG_LOG("发布数据 ----- OneNet_Publish");
			sprintf(PUB_BUF,"{\"Hum\":%d.%d,\"Temp\":%d.%d,\"Light\":%.1f,\"Led\":%d,\"Beep\":%d}",
				humidityH,humidityL,temperatureH,temperatureL,Light,Led_Status?0:1,alarmFlag);
			OneNet_Publish(devPubTopic, PUB_BUF);
			DEBUG_LOG("*************************************************************************************");
			timeCount = 0;
			ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(3);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		delay_ms(10);
	}
}

