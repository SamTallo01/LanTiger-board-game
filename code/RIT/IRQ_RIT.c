
#include "lpc17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../GLCD/GLCD.h" 
#include "../timer/timer.h"
#include "../button_EXINT/button.h"
#include "../Game/game.h"
/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
volatile int down1=0;
volatile int down2=0;
volatile int key1=0;

static int J_select=0;
static int J_down = 0;
static int J_up=0;
static int J_right = 0;
static int J_left=0;
volatile int movement=0;	
volatile int rotation=0;
	
void RIT_IRQHandler (void)
{					
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	// SELECT
		J_select++;
		switch(J_select){
			case 1:
				NVIC_DisableIRQ(RIT_IRQn);
				ScelgoMossa(select, rotation, movement);
				break;
			default:
				break;
		}
	}else{
		J_select=0;
	}

	
	if(((LPC_GPIO1->FIOPIN & (1<<26)) == 0)){	// DOWN
		J_down++;
		switch(J_down){
			case 1:
				ScelgoMossa(giu, rotation, movement);
				break;
			default:
				break;
		}
	}else{
		J_down=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	// RIGHT
		J_right++;
		switch(J_right){
			case 1:
				ScelgoMossa(destra, rotation, movement);
				break;
			default:
				break;
		}
	}else{
		J_right=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	// LEFT
		J_left++;
		switch(J_left){
			case 1:
				ScelgoMossa(sinistra, rotation, movement);
				break;
			default:
				break;
		}
	}else{
		J_left=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	// UP
		J_up++;
		switch(J_up){
			case 1:
				ScelgoMossa(alto, rotation, movement);
				break;
			default:
				break;
		}
	}else{
		J_up=0;
	}
	
	/*BUTTON CONTROLLER*/

	if(down1!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed */
			down1++;				
			switch(down1){
				case 2:				/* pay attention here: please see slides to understand value 2 */
				if( movement == 0){
					// wall placement mode
					// Disegno il muro in metà alla griglia
					DrawPossibleMoves(White); // Vado in modalità muro
					movement = 1;
					rotation=0;
					nuovoMuro(rotation);
				}
				else{
					// player movement mode
					DrawPossibleMoves(Yellow); // Vado in modalità player
					eliminaMuro(rotation);
					movement=0;
					rotation=0;
				}
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down1=0;			
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	
	if(down2!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){	/* KEY2 pressed */
			down2++;				
			switch(down2){
				case 2:				/* pay attention here: please see slides to understand value 2 */
				if( rotation == 0){
					// Metto il muro in orizzontale
					rotation = 1;
					changeOrientation(rotation);
				}
				else{
					// Metto il muro in verticale
					rotation=0;
					changeOrientation(rotation);
				}
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down2=0;			
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	}

	
	
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
// Quando muovo il joystick devo ricolorare le altre scelte, altrimenti se lo muovo tante volte
// Coloro tutto attorno al player dello stesso colore
