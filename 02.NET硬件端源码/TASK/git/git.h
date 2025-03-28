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

// ��������Ϣ
#define SSID "NET"				 // ·����SSID����
#define PASS "12345678"			 // ·��������
#define ServerIP "iot-06z00axdhgfk24n.mqtt.iothub.aliyuncs.com" // ������IP��ַ
#define ServerPort 1883			 // ������IP��ַ�˿ں�
// �豸��Ϣ
#define PROID "smart_helmet_device&h9sjncSJ2s1"	  // ��ƷID
#define DEVID "h9sjncSJ2s1.smart_helmet_device|securemode=2,signmethod=hmacsha256,timestamp=1703087515634|" // �豸ID
#define AUTH_INFO "adc39bf3a2d945b9e1b124ea696e6c3bbad43c2dc0c1f1ef17b464b912a021a3"	  // ��Ȩ��Ϣ
// MQTT����
#define S_TOPIC_NAME "/broadcast/h9sjncSJ2s1/test1" // ��Ҫ���ĵ�����
#define P_TOPIC_NAME "/broadcast/h9sjncSJ2s1/test2" // ��Ҫ����������

//  �豸ʹ�����趨��
#define OLED  1// �Ƿ�ʹ��OLED1

// �Զ���ִ�н������
typedef enum
{
	MY_SUCCESSFUL = 0x01, // �ɹ�
	MY_FAIL = 0x00		  // ʧ��

} mySta; // �ɹ���־λ

typedef enum
{
	OPEN = 0x01, // ��
	CLOSE = 0x00 // �ر�

} On_or_Off_TypeDef; // �ɹ���־λ

typedef enum
{
	DERVICE_SEND = 0x00, // �豸->ƽ̨
	PLATFORM_SEND = 0x01 // ƽ̨->�豸

} Send_directino; // ���ͷ���

typedef struct
{
	U8 App;			 // ָ��ģʽ
	U8 Page; 		 // ҳ��
	U8 Page_time; 	// �л�
	U8 Flage;			 // ģʽѡ��
	
	U16 people;		 //����
	U16 light;			 // ����
	U16 somg;			 // ����Ũ��
	U16 voice;			//������С

} Data_TypeDef; // ���ݲ����ṹ��

typedef struct
{

	F32 threshold_temperatuer_max; // �¶�
	F32 threshold_temperatuer_min; // �¶�

} Threshold_Value_TypeDef; // ���ݲ����ṹ��

typedef struct
{
	
	U8 open; // ��
	U8 check_open; // ����Ƿ�������
	
	U8 Heard_State;//������
	U8 Fall_State;//�������
	U8 Warning_State;//�ֶ�����
	
	U8 location_state;	// ��λ
	U8 voice_time;	// ����ʱ
	
} Device_Satte_Typedef; // ״̬�����ṹ��

typedef struct
{
	U16 pwd;
	



	
} Pwd_Box_Typedef; // ����

// ��ȡ���ݲ���
mySta Read_Data(Data_TypeDef *Device_Data);
// ��ʼ��
mySta Reset_Threshole_Value(Pwd_Box_Typedef *box_pwd, Device_Satte_Typedef *device_state);
// ����OLED��ʾ��������
mySta Update_oled_massage(void);
// �����豸״̬
mySta Update_device_massage(void);
// ����json����
mySta massage_parse_json(char *message);
// ��֤���벢����
void Check_Key_ON_OFF(void);
// �Զ��л�����
void Automation_Close(void);

#endif
