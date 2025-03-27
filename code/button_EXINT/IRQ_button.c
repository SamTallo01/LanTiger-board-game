#include "button.h"
#include "lpc17xx.h"
#include "../GLCD/GLCD.h" 
#include "../timer/timer.h"
#include "../Game/game.h"
#include "../RIT/RIT.h"
#include "../joystick/joystick.h"

extern volatile int player;
extern int down1;
extern int down2;

void EINT0_IRQHandler (void)	  	/* INT0														 */
{		
	extern volatile int counter;
	
	NVIC_DisableIRQ(EINT0_IRQn);
	
	LCD_Clear(White);
	
	player=1;
	counter=20;
	// Inizializzo la tabella
	LCD_DrawTabella();
	LCD_StoreVerticalWalls(); //Coordinate muri verticali
	LCD_StoreHorizontalWalls(); //Coordinate muri orizzontali
	
	// Inizializzo i player
	playersInit();
	muriInnit();
	DrawPlayers(1); // Coloro il player 1
	DrawPlayers(2); // Coloro il player 2
	
	DrawPossibleMoves(Yellow); // Coloro le caselle attorno al player 1
	
	// Inizia il timer e si scambiano i turni da adesso
	init_timer(0, 25000000);
	reset_timer(0);
	enable_timer(0);
	
	// Faccio iniziare il RIT ed abilito il joystick
	joystick_init();											/* Joystick Initialization            */
	init_RIT(5000000);								/* RIT Initialization 50 msec       	*/
	
	enable_RIT();													/* RIT enabled												*/
	
	cifraMossa(0, 0, 1, 0); // Inizializzo nessuna mossa
	
	NVIC_EnableIRQ(EINT1_IRQn);
	
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	NVIC_EnableIRQ(EINT2_IRQn);
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */
	down1=1;
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	NVIC_DisableIRQ(EINT2_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO pin selection */
	down2=1;
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
}


