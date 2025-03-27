/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "led/led.h"
#include "button_EXINT/button.h"

#define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	
  LCD_Initialization();

  BUTTON_init();												/* BUTTON Initialization              */
	
	LCD_Clear(White);
	GUI_Text(10, 10, (uint8_t *)"Waiting phase....", Black, White);
	GUI_Text(10, 30, (uint8_t *)"Press INT0 to start", Black, White);
	GUI_Text(10, 70, (uint8_t *)"Il colore del bordo", Black, White);
	GUI_Text(10, 90, (uint8_t *)"corrisponde al colore del", Black, White);
	GUI_Text(10, 110, (uint8_t *)"player che deve giocare:", Black, White);
	GUI_Text(10, 130, (uint8_t *)"Player 1 rosso", Black, Red);
	GUI_Text(10, 150, (uint8_t *)"Player 2 verde", Black, Green);
	GUI_Text(10, 190, (uint8_t *)"Chi arriva prima dalla", Black, White);
	GUI_Text(10, 210, (uint8_t *)"parte opposta vince!", Black, White);
	GUI_Text(30, 250, (uint8_t *)"Che vinca il migliore!", Black, White);
	NVIC_DisableIRQ(EINT1_IRQn);
	NVIC_DisableIRQ(EINT2_IRQn);

	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
