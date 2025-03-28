#ifndef __RELAY_H
#define	__RELAY_H


#include "stm32f10x.h"
#include "sys.h"
#include "git.h"

#define BEEP_GPIO_PORT    	GPIOB
#define BEEP_GPIO_CLK 	    RCC_APB2Periph_GPIOB		
#define BEEP_GPIO_PIN		    GPIO_Pin_8	


#define RELAY4_GPIO_PORT    	GPIOA			              
#define RELAY4_GPIO_CLK 	    RCC_APB2Periph_GPIOA		
#define RELAY4_GPIO_PIN		    GPIO_Pin_12	



#define RELAY4_TOGGLE		 digitalToggle(RELAY4_GPIO_PORT,RELAY4_GPIO_PIN)
#define RELAY4_OFF		   digitalLo(RELAY4_GPIO_PORT,RELAY4_GPIO_PIN)
#define RELAY4_ON			   digitalHi(RELAY4_GPIO_PORT,RELAY4_GPIO_PIN)

#define BEEP_TOGGLE		 digitalToggle(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#define BEEP_OFF		   digitalLo(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#define BEEP_ON			   digitalHi(BEEP_GPIO_PORT,BEEP_GPIO_PIN)

void RELAY_GPIO_Config(void);
void Mqtt_Relay(U8 Cmd);
#endif /* __RELAY_H */
