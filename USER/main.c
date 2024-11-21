

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

u8 alarmFlag = 0;//�Ƿ񱨾��ı�־
u8 alarm_is_free = 10;//�������Ƿ��ֶ�������������ֶ�����������Ϊ0



u8 humidityH;	  //ʪ����������
u8 humidityL;	  //ʪ��С������
u8 temperatureH;   //�¶���������
u8 temperatureL;   //�¶�С������
extern char oledBuf[20];
float Light = 0; //���ն�
u8 Led_Status = 0;

char PUB_BUF[256];//�ϴ����ݵ�buf
const char *devSubTopic[] = {"/mysmarthome/sub"};
const char devPubTopic[] = "/mysmarthome/pub";
u8 ESP8266_INIT_OK = 0;//esp8266��ʼ����ɱ�־



 int main(void)
 {	
	unsigned short timeCount = 0;	//���ͼ������
	unsigned char *dataPtr = NULL;
	Usart1_Init(115200);//debug����
	delay_init();	    	 //��ʱ������ʼ��	
	delay_ms(500);// �ӳ�һ�µȴ�oledʶ��
	OLED_Init();
	OLED_ColorTurn(0);//0������ʾ��1 ��ɫ��ʾ
  OLED_DisplayTurn(0);//0������ʾ 1 ��Ļ��ת��ʾ
	OLED_Clear();
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
	//EXTIX_Init();		//�ⲿ�жϳ�ʼ��
	BEEP_Init();
	DHT11_Init();
  BH1750_Init();
	Usart2_Init(115200);//stm32-8266ͨѶ����	
	delay_ms(1000);	
	ESP8266_Init();					//��ʼ��ESP8266	
	while(OneNet_DevLink()){//����OneNET
		delay_ms(500);
	}		
	
	OLED_Clear();	
	
	TIM2_Int_Init(4999,7199);
	TIM3_Int_Init(2499,7199);
	
	BEEP = 0;//������ʾ����ɹ�
	delay_ms(250);
	BEEP = 1;
	
	OneNet_Subscribe(devSubTopic, 1);
	
	while(1)
	{
		if(timeCount % 40 == 0)//1000ms / 25 = 40 һ��ִ��һ��
		{
			/********** ��ʪ�ȴ�������ȡ����**************/
			DHT11_Read_Data(&humidityH,&humidityL,&temperatureH,&temperatureL);
			
			/********** ���նȴ�������ȡ����**************/
			if (!i2c_CheckDevice(BH1750_Addr))
			{
				Light = LIght_Intensity();
			}
			
			/********** ��ȡLED1��״̬ **************/
			Led_Status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);
			
			/********** �����߼� **************/
			if((humidityH < 80) && (temperatureH < 30) && (Light < 1000))alarmFlag = 0;
			else alarmFlag = 1;
			
			/********** ���������Ϣ **************/
			DEBUG_LOG(" | ʪ�ȣ�%d.%d C | �¶ȣ�%d.%d %% | ���նȣ�%.1f lx | ָʾ�ƣ�%s | ��������%s | ",humidityH,humidityL,temperatureH,temperatureL,Light,Led_Status?"�ر�":"��������",alarmFlag?"��������":"ֹͣ");
		}
		if(++timeCount >= 200)	// 5000ms / 25 = 200 ���ͼ��5000ms
		{
			Led_Status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);//��ȡLED0��״̬
			DEBUG_LOG("*************************************************************************************");
			DEBUG_LOG("�������� ----- OneNet_Publish");
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

