// C��
#include <string.h>
// ��Ƭ��ͷ�ļ�
#include "sys.h"
// ����Э���
#include "onenet.h"
// �����豸
#include "esp8266.h"
// ������
#include "task.h"
#include "timer.h"
// Ӳ������
#include "delay.h"
#include "usart.h"
#include "git.h"
#include "led.h"
#include "adc.h"
#include "relay.h"  
#include "key.h" 
#include "timer.h"
#include "adxl345.h"
#include "usart2.h"
#include "flash.h"
#if OLED // OLED�ļ�����
#include "oled.h"
#endif
// ����״̬

// �����ʱ���趨
static Timer task1_id;
static Timer task2_id;
static Timer task3_id;

// ��ȡȫ�ֱ���
extern Data_TypeDef Data_init;
extern Threshold_Value_TypeDef threshold_value_init; // �豸��ֵ���ýṹ��
extern Device_Satte_Typedef device_state_init;       // �豸״̬
extern Pwd_Box_Typedef box_pwd_init;				  // �豸����
const char *topics[] = {S_TOPIC_NAME};

// Ӳ����ʼ��
void Hardware_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // �����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
		HZ = GB16_NUM();                                // ����
    delay_init(); // ��ʱ������ʼ��
    GENERAL_TIM_Init(TIM_4, 0, 2);
    Usart1_Init(115200); // ����1��ʼ��Ϊ115200
		Usart2_Init(9600);   // GPS
    Usart3_Init(115200); // ����2������ESP8266��
		System_PB34_setIO();
    LED_Init();

		Key_GPIO_Config();//key
		ADXL345_Init();
		ADXL345_AUTO_Adjust((char*)&x,(char*)&y,(char*)&z);//�Զ�У׼
	  ADCx_Init();	//ADC
#if OLED // OLED�ļ�����
	  OLED_Init();
    OLED_ColorTurn(0);   // 0������ʾ��1 ��ɫ��ʾ
    OLED_DisplayTurn(0); // 0������ʾ 1 ��Ļ��ת��ʾ
#endif
	 while (Reset_Threshole_Value(&box_pwd_init, &device_state_init) != MY_SUCCESSFUL)
			delay_ms(5);  // ��ʼ����ֵ
   
		LED3_OFF;
}
// �����ʼ��
void Net_Init()
{

#if OLED // OLED�ļ�����
  char str[50];
    OLED_Clear();
    // дOLED����
    sprintf(str, "-���WIFI�ȵ�");
    OLED_ShowCH(0, 0, (unsigned char *)str);
    sprintf(str, "-����:%s         ", SSID);
    OLED_ShowCH(0, 2, (unsigned char *)str);
    sprintf(str, "-����:%s         ", PASS);
    OLED_ShowCH(0, 4, (unsigned char *)str);
    sprintf(str, "-Ƶ��: 2.4 GHz   ");
    OLED_ShowCH(0, 6, (unsigned char *)str);

#endif
    ESP8266_Init();          // ��ʼ��ESP8266
    while (OneNet_DevLink()) // ����OneNET
        delay_ms(300);
		while (OneNet_Subscribe(topics, 1))  // ��������
            delay_ms(300);
    LED = ON; // �����ɹ�
    delay_ms(250);
    LED = OFF;
 
    Connect_Net = 60; // �����ɹ�
#if OLED // OLED�ļ�����		
		OLED_Clear();
#endif
}

// ����1
void task1(void)
{
	if (device_state_init.location_state == 1)
	{	
		//��������
		W_Test();
	}
	// ��ʱ��
	Automation_Close();
		
}
// ����2
void task2(void)
{
    // �豸����
    if (Connect_Net == 0) {
#if OLED // OLED�ļ�����
			char str[50];
			OLED_Clear();
			// дOLED����
			sprintf(str, "-���WIFI�ȵ�");
			OLED_ShowCH(0, 0, (unsigned char *)str);
			sprintf(str, "-����:%s         ", SSID);
			OLED_ShowCH(0, 2, (unsigned char *)str);
			sprintf(str, "-����:%s         ", PASS);
			OLED_ShowCH(0, 4, (unsigned char *)str);
			sprintf(str, "-Ƶ��: 2.4 GHz   ");
			OLED_ShowCH(0, 6, (unsigned char *)str);
#endif
       
    }

    Read_Data(&Data_init); // ���´���������
    Update_oled_massage() ;// ����OLED 
		Update_device_massage(); // �����豸
		LED=~LED;
		//printf("Ok\n");

}
// ����3
void task3(void)
{
    if (Connect_Net&&Data_init.App==0) {
				Data_init.App=1;
    }
  
}
// �����ʼ��
void SoftWare_Init(void)
{
    // ��ʱ����ʼ��
    timer_init(&task1_id, task1, 1000, 1);  
    timer_init(&task2_id, task2, 250, 1);  // �������ݰ�
    timer_init(&task3_id, task3, 3000, 1); // ÿ3�뷢��һ�����ݵ��ͻ���

     
}
// ������
int main(void)
{

    unsigned char *dataPtr = NULL;
    SoftWare_Init(); // �����ʼ��
    Hardware_Init(); // Ӳ����ʼ��
    Net_Init();      // �����ʼ��
		TIM_Cmd(TIM4, ENABLE); // ʹ�ܼ�����
    while (1) {

        // �߳�
        timer_loop(); // ��ʱ��ִ��
        // ���ڽ����ж�
 
        if (dataPtr != NULL) {
            OneNet_RevPro(dataPtr); // ��������
        }
    }
}
