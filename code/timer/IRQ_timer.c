
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../Game/game.h"
#include "../button_EXINT/button.h"
#include "../RIT/RIT.h"
#include <stdio.h>

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
volatile int counter;
int volatile player=1;

extern volatile int conferma;
extern volatile int movement;
extern volatile int rotation;
extern int down1;
extern int down2;

void TIMER0_IRQHandler (void)
{
	char buffer[4];
	static int move=0;
	sprintf(buffer, "%02ds", counter);
	
	GUI_Text(110, 293, (uint8_t *)buffer, Black, White);
	counter--;
		
	
	if(getConferma()){
		// Il giocatore corrente ha effettuato una mossa
	
		// Passo il turno
		counter=0;
		movement=0;
		rotation=0;
		setConferma(0);
		move=1;
	}
	
	if(checkVittoria()>0){
		NVIC_DisableIRQ(TIMER0_IRQn);
		NVIC_DisableIRQ(RIT_IRQn);
		disable_timer(0);
		disable_RIT();
		NVIC_EnableIRQ(EINT0_IRQn);
		if(checkVittoria()==1){
			LCD_Clear(Red);
			GUI_Text(10, 10, (uint8_t *)"Complimenti player 1", Black, Red);
			GUI_Text(10, 30, (uint8_t *)"hai vinto!!", Black, Red);
			GUI_Text(10, 70, (uint8_t *)"Premere INT0 per il rematch", Black, Red);
		}else{
			LCD_Clear(Green);
			GUI_Text(10, 10, (uint8_t *)"Complimenti player 2", Black, Green);
			GUI_Text(10, 30, (uint8_t *)"hai vinto!!", Black, Green);
			GUI_Text(10, 70, (uint8_t *)"Premere INT0 per il rematch", Black, Green);
		}	
		return;
	}
		
	if(counter==0){
		//cambio il turno
		if(movement==1){
			eliminaMuro(0);
			movement=0;
			rotation=0;
		}
		if(player==1){
			//Turno del giocatore 2
			if(move!=1){
				removeError();
				DrawPossibleMoves(White); //Cancella le mosse del giocatore 1
			}
			NVIC_DisableIRQ(EINT2_IRQn);
			NVIC_EnableIRQ(RIT_IRQn);
			player=2;
			mossaNulla();
			LCD_DrawBorder(Green);
			DrawPossibleMoves(Yellow); // Vedi le mosse del player 2
			
		}else{
			//Turno del giocatore 1
			if(move!=1){
				removeError();
				DrawPossibleMoves(White); //Cancella le mosse del giocatore 1
			}
			NVIC_DisableIRQ(EINT2_IRQn);
			NVIC_EnableIRQ(RIT_IRQn);
			player=1;
			mossaNulla();
			LCD_DrawBorder(Red);
			DrawPossibleMoves(Yellow); // Vedi le mosse del player 1
		}
		move=0;
		counter=20;
		sprintf(buffer, "%02ds", counter);
	
		GUI_Text(110, 293, (uint8_t *)buffer, Black, White);
		counter--;
	}
	

	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}
