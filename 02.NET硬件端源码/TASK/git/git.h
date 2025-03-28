#ifndef __GIT_H
#define __GIT_H
#include "stdio.h"

typedef unsigned char U8;
typedef signed char S8;
typedef unsigned short U16;
typedef signed short S16;
typedef unsigned int U32;
typedef signed int S32;
typedef float F32;

// 服务器信息
#define SSID "NET"				 // 路由器SSID名称
#define PASS "12345678"			 // 路由器密码
#define ServerIP "iot-06z00axdhgfk24n.mqtt.iothub.aliyuncs.com" // 服务器IP地址
#define ServerPort 1883			 // 服务器IP地址端口号
// 设备信息
#define PROID "smart_helmet_device&h9sjncSJ2s1"	  // 产品ID
#define DEVID "h9sjncSJ2s1.smart_helmet_device|securemode=2,signmethod=hmacsha256,timestamp=1703087515634|" // 设备ID
#define AUTH_INFO "adc39bf3a2d945b9e1b124ea696e6c3bbad43c2dc0c1f1ef17b464b912a021a3"	  // 鉴权信息
// MQTT主题
#define S_TOPIC_NAME "/broadcast/h9sjncSJ2s1/test1" // 需要订阅的主题
#define P_TOPIC_NAME "/broadcast/h9sjncSJ2s1/test2" // 需要发布的主题

//  设备使用外设定义
#define OLED  1// 是否使用OLED1

// 自定义执行结果类型
typedef enum
{
	MY_SUCCESSFUL = 0x01, // 成功
	MY_FAIL = 0x00		  // 失败

} mySta; // 成功标志位

typedef enum
{
	OPEN = 0x01, // 打开
	CLOSE = 0x00 // 关闭

} On_or_Off_TypeDef; // 成功标志位

typedef enum
{
	DERVICE_SEND = 0x00, // 设备->平台
	PLATFORM_SEND = 0x01 // 平台->设备

} Send_directino; // 发送方向

typedef struct
{
	U8 App;			 // 指令模式
	U8 Page; 		 // 页面
	U8 Page_time; 	// 切换
	U8 Flage;			 // 模式选择
	
	U16 people;		 //人数
	U16 light;			 // 光照
	U16 somg;			 // 烟雾浓度
	U16 voice;			//声音大小

} Data_TypeDef; // 数据参数结构体

typedef struct
{

	F32 threshold_temperatuer_max; // 温度
	F32 threshold_temperatuer_min; // 温度

} Threshold_Value_TypeDef; // 数据参数结构体

typedef struct
{
	
	U8 open; // 门
	U8 check_open; // 监测是否由物体
	
	U8 Heard_State;//佩戴情况
	U8 Fall_State;//跌倒情况
	U8 Warning_State;//手动警告
	
	U8 location_state;	// 定位
	U8 voice_time;	// 倒计时
	
} Device_Satte_Typedef; // 状态参数结构体

typedef struct
{
	U16 pwd;
	



	
} Pwd_Box_Typedef; // 密码

// 获取数据参数
mySta Read_Data(Data_TypeDef *Device_Data);
// 初始化
mySta Reset_Threshole_Value(Pwd_Box_Typedef *box_pwd, Device_Satte_Typedef *device_state);
// 更新OLED显示屏中内容
mySta Update_oled_massage(void);
// 更新设备状态
mySta Update_device_massage(void);
// 解析json数据
mySta massage_parse_json(char *message);
// 验证密码并开锁
void Check_Key_ON_OFF(void);
// 自动切换界面
void Automation_Close(void);

#endif
