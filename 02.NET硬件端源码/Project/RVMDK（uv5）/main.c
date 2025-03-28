// C库
#include <string.h>
// 单片机头文件
#include "sys.h"
// 网络协议层
#include "onenet.h"
// 网络设备
#include "esp8266.h"
// 任务栏
#include "task.h"
#include "timer.h"
// 硬件驱动
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
#if OLED // OLED文件存在
#include "oled.h"
#endif
// 联网状态

// 软件定时器设定
static Timer task1_id;
static Timer task2_id;
static Timer task3_id;

// 获取全局变量
extern Data_TypeDef Data_init;
extern Threshold_Value_TypeDef threshold_value_init; // 设备阈值设置结构体
extern Device_Satte_Typedef device_state_init;       // 设备状态
extern Pwd_Box_Typedef box_pwd_init;				  // 设备密码
const char *topics[] = {S_TOPIC_NAME};

// 硬件初始化
void Hardware_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
		HZ = GB16_NUM();                                // 字数
    delay_init(); // 延时函数初始化
    GENERAL_TIM_Init(TIM_4, 0, 2);
    Usart1_Init(115200); // 串口1初始化为115200
		Usart2_Init(9600);   // GPS
    Usart3_Init(115200); // 串口2，驱动ESP8266用
		System_PB34_setIO();
    LED_Init();

		Key_GPIO_Config();//key
		ADXL345_Init();
		ADXL345_AUTO_Adjust((char*)&x,(char*)&y,(char*)&z);//自动校准
	  ADCx_Init();	//ADC
#if OLED // OLED文件存在
	  OLED_Init();
    OLED_ColorTurn(0);   // 0正常显示，1 反色显示
    OLED_DisplayTurn(0); // 0正常显示 1 屏幕翻转显示
#endif
	 while (Reset_Threshole_Value(&box_pwd_init, &device_state_init) != MY_SUCCESSFUL)
			delay_ms(5);  // 初始化阈值
   
		LED3_OFF;
}
// 网络初始化
void Net_Init()
{

#if OLED // OLED文件存在
  char str[50];
    OLED_Clear();
    // 写OLED内容
    sprintf(str, "-请打开WIFI热点");
    OLED_ShowCH(0, 0, (unsigned char *)str);
    sprintf(str, "-名称:%s         ", SSID);
    OLED_ShowCH(0, 2, (unsigned char *)str);
    sprintf(str, "-密码:%s         ", PASS);
    OLED_ShowCH(0, 4, (unsigned char *)str);
    sprintf(str, "-频率: 2.4 GHz   ");
    OLED_ShowCH(0, 6, (unsigned char *)str);

#endif
    ESP8266_Init();          // 初始化ESP8266
    while (OneNet_DevLink()) // 接入OneNET
        delay_ms(300);
		while (OneNet_Subscribe(topics, 1))  // 订阅主题
            delay_ms(300);
    LED = ON; // 入网成功
    delay_ms(250);
    LED = OFF;
 
    Connect_Net = 60; // 入网成功
#if OLED // OLED文件存在		
		OLED_Clear();
#endif
}

// 任务1
void task1(void)
{
	if (device_state_init.location_state == 1)
	{	
		//保存数据
		W_Test();
	}
	// 计时器
	Automation_Close();
		
}
// 任务2
void task2(void)
{
    // 设备重连
    if (Connect_Net == 0) {
#if OLED // OLED文件存在
			char str[50];
			OLED_Clear();
			// 写OLED内容
			sprintf(str, "-请打开WIFI热点");
			OLED_ShowCH(0, 0, (unsigned char *)str);
			sprintf(str, "-名称:%s         ", SSID);
			OLED_ShowCH(0, 2, (unsigned char *)str);
			sprintf(str, "-密码:%s         ", PASS);
			OLED_ShowCH(0, 4, (unsigned char *)str);
			sprintf(str, "-频率: 2.4 GHz   ");
			OLED_ShowCH(0, 6, (unsigned char *)str);
#endif
       
    }

    Read_Data(&Data_init); // 更新传感器数据
    Update_oled_massage() ;// 更新OLED 
		Update_device_massage(); // 更新设备
		LED=~LED;
		//printf("Ok\n");

}
// 任务3
void task3(void)
{
    if (Connect_Net&&Data_init.App==0) {
				Data_init.App=1;
    }
  
}
// 软件初始化
void SoftWare_Init(void)
{
    // 定时器初始化
    timer_init(&task1_id, task1, 1000, 1);  
    timer_init(&task2_id, task2, 250, 1);  // 跟新数据包
    timer_init(&task3_id, task3, 3000, 1); // 每3秒发送一次数据到客户端

     
}
// 主函数
int main(void)
{

    unsigned char *dataPtr = NULL;
    SoftWare_Init(); // 软件初始化
    Hardware_Init(); // 硬件初始化
    Net_Init();      // 网络初始化
		TIM_Cmd(TIM4, ENABLE); // 使能计数器
    while (1) {

        // 线程
        timer_loop(); // 定时器执行
        // 串口接收判断
 
        if (dataPtr != NULL) {
            OneNet_RevPro(dataPtr); // 接收命令
        }
    }
}
