#include "git.h"
#include "onenet.h"
#if OLED // OLED�ļ�����
#include "oled.h"
#endif

#include "adc.h"
#include "led.h"
#include "relay.h"  
#include "key.h"  
#include "delay.h"
#include "bus.h"
#include "usart2.h"
#include "flash.h"
//  C��
#include "cjson.h"
#include <string.h>
#include <stdio.h>

Data_TypeDef Data_init;						  // �豸���ݽṹ��
Threshold_Value_TypeDef threshold_value_init; // �豸��ֵ���ýṹ��
Device_Satte_Typedef device_state_init;		  // �豸״̬
Pwd_Box_Typedef box_pwd_init;				  // �豸����



void errorLog(U8 num)
{
	while (1)
	{
		printf("ERROR%d\r\n", num);
	}
}
void parseGpsBuffer()
{
	char *subString;
	char *subStringNext;
	char i = 0;
	char usefullBuffer[2];
	if (Save_Data.isGetData)
	{
		Save_Data.isGetData = false;
	//printf("**************\r\n");
		// printf(Save_Data.GPS_Buffer);

		for (i = 0; i <= 6; i++)
		{
			if (i == 0)
			{
				if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
					errorLog(1); // ��������
			}
			else
			{
				subString++;
				if ((subStringNext = strstr(subString, ",")) != NULL)
				{
					switch (i)
					{
					case 1:
						memcpy(Save_Data.UTCTime, subString, subStringNext - subString);
						break; // ��ȡUTCʱ��
					case 2:
						memcpy(usefullBuffer, subString, subStringNext - subString);
						break; // ��ȡUTCʱ��
					case 3:
						memcpy(Save_Data.latitude, subString, subStringNext - subString);
						break; // ��ȡγ����Ϣ
					case 4:
						memcpy(Save_Data.N_S, subString, subStringNext - subString);
						break; // ��ȡN/S
					case 5:
						memcpy(Save_Data.longitude, subString, subStringNext - subString);
						break; // ��ȡ������Ϣ
					case 6:
						memcpy(Save_Data.E_W, subString, subStringNext - subString);
						break; // ��ȡE/W

					default:
						break;
					}

					subString = subStringNext;
					Save_Data.isParseData = true;
					if (usefullBuffer[0] == 'A')
					{
						//��λ�ɹ�
						device_state_init.location_state = 1;
						Save_Data.isUsefull = true;
					}
					else if (usefullBuffer[0] == 'V')
						Save_Data.isUsefull = false;
				}
				else
				{
					errorLog(2); // ��������
				}
			}
		}
	}
}
F32 longitude_sum, latitude_sum;
U8 longitude_int, latitude_int;
void printGpsBuffer()
{
	// ת��Ϊ����
	longitude_sum = atof(Save_Data.longitude);
	latitude_sum = atof(Save_Data.latitude);
	// printf("ά�� = %.5f %.5f\r\n",longitude_sum,latitude_sum);
	// ����
	longitude_int = longitude_sum / 100;
	latitude_int = latitude_sum / 100;

	// ת��Ϊ��γ��
	longitude_sum = longitude_int + ((longitude_sum / 100 - longitude_int) * 100) / 60;
	latitude_sum = latitude_int + ((latitude_sum / 100 - latitude_int) * 100) / 60;

	if (Save_Data.isParseData)
	{
		Save_Data.isParseData = false;

		//printf("Save_Data.UTCTime = %s\r\n", Save_Data.UTCTime);
		if (Save_Data.isUsefull)
		{
			Save_Data.isUsefull = false;

			// printf("���� = %s\r\n",Save_Data.longitude);
			//printf("���� = %.5f\r\n", longitude_sum);
			// printf("ά�� = %s\r\n",Save_Data.latitude);
			//printf("γ�� = %.5f\r\n", latitude_sum);
		}
		else
		{
			//printf("GPS DATA is not usefull!\r\n");
		}
	}
}


// ��ȡ���ݲ���
mySta Read_Data(Data_TypeDef *Device_Data)
{
	Device_Data->light = Light_value();								  // ��ȡ���ǿ��
	Device_Data->somg = Smog_Trans_Concentration();	
	Device_Data->voice = Voice_value();			// ��ȡ
	obtain_bus();							//����
	
	// ��ȡ��γ��
	parseGpsBuffer();
	if (device_state_init.location_state == 1)
	{
		// ������γ��
		printGpsBuffer();
	}
	
	return MY_SUCCESSFUL;
}
// ��ʼ��
mySta Reset_Threshole_Value(Pwd_Box_Typedef *box_pwd, Device_Satte_Typedef *device_state)
{

	// ��ֵ����

	// ״̬����
	Data_init.Page_time=30;
	device_state_init.location_state=0;
	
//	longitude_sum = 111.434380;
//	latitude_sum = 27.197769;
//	// д
//	W_Test();
	// ��
	R_Test();
	return MY_SUCCESSFUL;
}
// ����OLED��ʾ��������
mySta Update_oled_massage()
{
#if OLED // �Ƿ��
	char str[50];
	u8 i;
	if(	Data_init.Page ==1){
		if (device_state_init.location_state == 1)
		{		
				sprintf(str, " ��λ״̬:ʵʱ ");
		}else{
				sprintf(str, " ��λ״̬:���� ");
		}
		OLED_ShowCH(0, 0, (unsigned char *)str);
		sprintf(str, "����: %.6f   ",longitude_sum);
		OLED_ShowCH(0, 2, (unsigned char *)str);
		sprintf(str, "γ��: %.6f   ",latitude_sum);
		OLED_ShowCH(0, 4, (unsigned char *)str);
//		sprintf(str, "              %d",KEY2);
//		OLED_ShowCH(0, 6, (unsigned char *)str);
		
	}else
	{
		sprintf(str, "��������: %d    ",Data_init.somg);
		OLED_ShowCH(0, 0, (unsigned char *)str);
		sprintf(str, "����:     %d    ",Data_init.light);
		OLED_ShowCH(0, 2, (unsigned char *)str);

	
		if(Data_init.voice <250){
			device_state_init.voice_time =10;
		}
		if(device_state_init.voice_time){
			sprintf(str, "����:  ��   %02d",device_state_init.voice_time);
		}else{
			
			sprintf(str, "����:  ��   %02d",device_state_init.voice_time);
		}
		OLED_ShowCH(0, 4, (unsigned char *)str);
		if(device_state_init.Fall_State ==1){
			sprintf(str, "�Ƿ����:  ��   ");

		}else{
			sprintf(str, "�Ƿ����:  ��   ");
		}
		OLED_ShowCH(0, 6, (unsigned char *)str);
	}


	
#endif

	return MY_SUCCESSFUL;
}

// �����豸״̬
mySta Update_device_massage()
{
	

	// ���в���ִֻ��һ�Σ���Լϵͳ��Դ,
	if (device_state_init.Heard_State && KEY2)
	{
		// δ��ͷ��
		device_state_init.Heard_State = 0;

	}
	else if (!device_state_init.Heard_State && !KEY2)
	{
		// ��ͷ��
		device_state_init.Heard_State = 1;

	}
	if (Data_init.App)
	{
		switch (Data_init.App)
		{
		case 1:
			OneNet_SendMqtt(1); // �������ݵ�APP
			break;

		}
		Data_init.App = 0;
	}

	return MY_SUCCESSFUL;
}

// �Զ��л�����
void Automation_Close(void)
{

	if(device_state_init.voice_time>0)
	{
		device_state_init.voice_time--;	
	}
	
	if(Data_init.Page_time>0)
	{
		Data_init.Page_time--;	
	
	}else{
	 if(Data_init.Page ==1){
			Data_init.Page = 0;
		}else{
			Data_init.Page = 1;
		}
		OLED_Clear();
		Data_init.Page_time=30;
	}
	
}
// ��ⰴ���Ƿ���
static U8 num_on = 0;
static U8 key_old = 0;
void Check_Key_ON_OFF()
{
	U8 key;
	key = KEY_Scan(1);
	// ����һ�εļ�ֵ�Ƚ� �������ȣ������м�ֵ�ı仯����ʼ��ʱ
	if (key != 0 && num_on == 0)
	{
		key_old = key;
		num_on = 1;
	}
	if (key != 0 && num_on >= 1 && num_on <= Key_Scan_Time) // 25*10ms
	{
		num_on++; // ʱ���¼��
	}
	if (key == 0 && num_on > 0 && num_on < Key_Scan_Time) // �̰�
	{
		switch (key_old)
		{
		case KEY1_PRES:
			printf("Key1_Short\n");
			if(	device_state_init.Fall_State ==1){
				device_state_init.Fall_State = 0;
			}

			break;
		case KEY2_PRES:
			printf("Key2_Short\n");
		

			break;
		case KEY3_PRES:
			printf("Key3_Short\n");
		
			break;

		default:
			break;
		}
		num_on = 0;
	}
	else if (key == 0 && num_on >= Key_Scan_Time) // ����
	{
		switch (key_old)
		{
		case KEY1_PRES:
			printf("Key1_Long\n");
			if(	device_state_init.Warning_State ==1){
				device_state_init.Warning_State = 0;
			}else{
				device_state_init.Warning_State = 1;
			}

			break;
		case KEY2_PRES:
			printf("Key2_Long\n");
		
			break;
		case KEY3_PRES:
			printf("Key3_Long\n");

			break;
		default:
			break;
		}
		num_on = 0;
	}
}
// ����json����
mySta massage_parse_json(char *message)
{

	cJSON *cjson_test = NULL; // ���json��ʽ
	cJSON *cjson_data = NULL; // ����
	const char *massage;
	// ������������
	u8 cjson_cmd; // ָ��,����

	/* ��������JSO���� */
	cjson_test = cJSON_Parse(message);
	if (cjson_test == NULL)
	{
		// ����ʧ��
		printf("parse fail.\n");
		return MY_FAIL;
	}

	/* ���θ���������ȡJSON���ݣ���ֵ�ԣ� */
	cjson_cmd = cJSON_GetObjectItem(cjson_test, "cmd")->valueint;
	/* ����Ƕ��json���� */
	cjson_data = cJSON_GetObjectItem(cjson_test, "data");

	switch (cjson_cmd)
	{
	case 0x01: // ��Ϣ��
	
		device_state_init.open = cJSON_GetObjectItem(cjson_data, "Door_open")->valueint;
		Data_init.App=1;
		break;
	case 0x02: // ��Ϣ��
		Data_init.App=2;

		break;
	case 0x03: // ���ݰ�
		Data_init.App=3;

		break;
	case 0x04: // ���ݰ�
		Data_init.App=4;
		break;
	default:
		break;
	}

	/* ���JSON����(��������)���������� */
	cJSON_Delete(cjson_test);

	return MY_SUCCESSFUL;
}
