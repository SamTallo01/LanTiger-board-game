#include "game.h"
#include "lpc17xx.h"
#include "../button_EXINT/button.h"
#include "../GLCD/GLCD.h" 
#include "../timer/timer.h"
#include "../RIT/RIT.h"
#include <stdio.h>
			 
int muri_v[7][6]=				 {0, 0, 0, 0, 0, 0,
													0, 0, 0, 0, 0, 0,
													0, 0, 0, 0, 0, 0,
													0, 0, 0, 0, 0, 0,
													0, 0, 0, 0, 0, 0,
													0, 0, 0, 0, 0, 0,
													0, 0, 0, 0, 0, 0,};

int muri_o[6][7]=					 {0, 0, 0, 0, 0, 0, 0,
														0, 0, 0, 0, 0, 0, 0,
														0, 0, 0, 0, 0, 0, 0,
														0, 0, 0, 0, 0, 0, 0,
														0, 0, 0, 0, 0, 0, 0,
														0, 0, 0, 0, 0, 0, 0};
typedef struct{
	int r;
	int c;
	int rotation;
}Walls_coord;
Walls_coord muro;

typedef struct{
	int r;
	int c;
	int walls;
	int prev_r;
	int prev_c;
	uint16_t color;
}Player;

Player p[2];
extern int player;
int volatile conferma=0;

uint32_t mossaUnsignedInt;
extern volatile int movement;
extern volatile int rotation;

// Serve per muovere il muro e cifrare la mossa fatta
// Devo separare la parte che cifra la mossa da quella che la fa
void ScelgoMossaMuro(Mossa mossa, int orientation, int moving){
	int muroHere=isMuroHere(orientation);
	int r=muro.r, c=muro.c;
	
	// Quadrato davanti
		if((muro.r!=0) && (mossa == alto)){
			LCD_DrawWall(muro.rotation, muro.r, muro.c, White);
			muro.r=muro.r-1;
			cifraMossa(muro.c, muro.r, orientation, moving);
			LCD_DrawWall(muro.rotation, muro.r, muro.c, Black);
			if(muroHere==1){// Caso 1 uguale per entrambi
				LCD_DrawWall(muro.rotation, muro.r+1, muro.c, Black);
			}else if(muroHere==2){ //Caso 2 
				if(orientation==0){ // 0
					LCD_DrawWall(muro.rotation, muro.r+2, muro.c, Black);
				}else{ // 1
					LCD_DrawWall(muro.rotation, muro.r+1, muro.c-1, Black);
					LCD_DrawWall(muro.rotation, muro.r+1, muro.c+1, Black);
				}
			}else if(muroHere==4){ //Caso 4
				if(orientation==0){ // 0
					LCD_DrawWall(muro.rotation, muro.r+2, muro.c, Black);
				}else{ // 1
					LCD_DrawWall(muro.rotation, muro.r+1, muro.c+1, Black);
				}
			}else if(muroHere==3){ //Caso 3
				if(orientation==1){// solo orizzontale
					LCD_DrawWall(muro.rotation, muro.r+1, muro.c-1, Black);
				}
			}else if(muroHere==5){
				LCD_DrawSeparatore(muro.r+1, muro.c, Black);
			}
		}else
		// Quadrato dietro
		if((muro.r!=5) && (mossa == giu)){
			LCD_DrawWall(muro.rotation, muro.r, muro.c, White);
			muro.r=muro.r+1;
			cifraMossa(muro.c, muro.r, orientation, moving);
			LCD_DrawWall(muro.rotation, muro.r, muro.c, Black);
			if(muroHere==1){
				LCD_DrawWall(muro.rotation, muro.r-1, muro.c, Black);
			}else if(muroHere==2){
				if(orientation==0){
					LCD_DrawWall(muro.rotation, muro.r-2, muro.c, Black);
				}else{
					LCD_DrawWall(muro.rotation, muro.r-1, muro.c+1, Black);
					LCD_DrawWall(muro.rotation, muro.r-1, muro.c-1, Black);
				}
			}else if(muroHere==3){
				if(orientation==0){
					LCD_DrawWall(muro.rotation, muro.r-2, muro.c, Black);
				}else{
					LCD_DrawWall(muro.rotation, muro.r-1, muro.c-1, Black);
				}
			}else if(muroHere==4){
				if(orientation==1){
					LCD_DrawWall(muro.rotation, muro.r-1, muro.c+1, Black);
				}
			}else if(muroHere==5){
				LCD_DrawSeparatore(muro.r-1, muro.c, Black);
			}
		}else
		// Quadrato sinistra
		if((muro.c!=0) && (mossa == sinistra)){
				LCD_DrawWall(muro.rotation, muro.r, muro.c, White);
				muro.c=muro.c-1;
				cifraMossa(muro.c, muro.r, orientation, moving);
				LCD_DrawWall(muro.rotation, muro.r, muro.c, Black);
			if(muroHere==1){
				LCD_DrawWall(muro.rotation, muro.r, muro.c+1, Black);
			}else if(muroHere==2){
				if(orientation==0){
					LCD_DrawWall(muro.rotation, muro.r-1, muro.c+1, Black); // muro sopra
					LCD_DrawWall(muro.rotation, muro.r+1, muro.c+1, Black); // muro sotto
				}else{
					LCD_DrawWall(muro.rotation, muro.r, muro.c+2, Black);
				}
			}else if(muroHere==3){
				if(orientation==0){
					LCD_DrawWall(muro.rotation, muro.r-1, muro.c+1, Black);
				}
			}else if(muroHere==4){
				if(orientation==0){
					LCD_DrawWall(muro.rotation, muro.r+1, muro.c+1, Black);
				}else{
					LCD_DrawWall(muro.rotation, muro.r, muro.c+2, Black);
				}
			}else if(muroHere==5){
				LCD_DrawSeparatore(muro.r, muro.c+1, Black);
			}
		}else
		// Quadrato destra
		if((muro.c!=5) && (mossa == destra)){
				LCD_DrawWall(muro.rotation, muro.r, muro.c, White);
				muro.c=muro.c+1;
				cifraMossa(muro.c, muro.r, orientation, moving);
				LCD_DrawWall(muro.rotation, muro.r, muro.c, Black);
			if(muroHere==1){
				LCD_DrawWall(muro.rotation, muro.r, muro.c-1, Black);
			}else if(muroHere==2){
				if(orientation==0){
					LCD_DrawWall(muro.rotation, muro.r-1, muro.c-1, Black); // muro sopra
					LCD_DrawWall(muro.rotation, muro.r+1, muro.c-1, Black); // muro sotto
				}else{
					LCD_DrawWall(muro.rotation, muro.r, muro.c-2, Black);
				}
			}else if(muroHere==3){
				if(orientation==0){
					LCD_DrawWall(muro.rotation, muro.r-1, muro.c-1, Black); // muro sopra
				}else{
					LCD_DrawWall(muro.rotation, muro.r, muro.c-2, Black);
				}
			}else if(muroHere==4){
				if(orientation==0){
					LCD_DrawWall(muro.rotation, muro.r+1, muro.c-1, Black);
				}
			}else if(muroHere==5){
				LCD_DrawSeparatore(muro.r, muro.c-1, Black);
			}
		}else
		if(mossa==select){
			//Controllo di non essere su nessun muro e di non bloccare il passaggio
			// Se è così allora conferma = 1
			if(isMuroThere(select)==0){ // Devo controllare che ci sia sempre un passaggio disponibile
				//Provo ad inserire il muro, guardo se non blocca la via
				//Devo toglierlo dopo
				if(muro.rotation==0){
						//Lo salvo nella tabella dei muri verticali
						muri_v[muro.r][muro.c]=2;
						muri_v[muro.r+1][muro.c]=1;
					}else{
						muri_o[muro.r][muro.c]=2;
						muri_o[muro.r][muro.c+1]=1;
				}
				if(checkBlock()==1){ // C'è una via libera
					conferma=1;
					SpostoGiocatore();
				}else{
					conferma=0;
					errorMessage("Player bloccato");
					// Back track, inserisco il muro nella funzione apposta se può starci
					// Se non ci sta, lo tolgo
					if(muro.rotation==0){
						//Lo salvo nella tabella dei muri verticali
						muri_v[muro.r][muro.c]=0;
						muri_v[muro.r+1][muro.c]=0;
					}else{
						muri_o[muro.r][muro.c]=0;
						muri_o[muro.r][muro.c+1]=0;
					}
				}
				
			}else{
				conferma=0;
				errorMessage("Si sovrappongono i muri");
			}
		}
		NVIC_EnableIRQ(RIT_IRQn);
		if(muri_o[r][c]==2 || muri_v[r][c]==2){
			LCD_DrawSeparatore(r, c, Black);
		}
}



// Ritorna diversi valori per poter ricolorare i muri già piazzati
int isMuroHere(int orientation){
	
	if(orientation==0){
		if(muri_v[muro.r][muro.c]==1){
			if(muri_v[muro.r+1][muro.c]==2){ // C'è un muro completo sopra ed uno completo sotto
				return 2; // Sono nel caso 2
			}else{
				return 3; // Sono nel caso 3
			}
		}else{
			if(muri_v[muro.r][muro.c]==2){
				return 1; // Sono nel caso 1
			}else if(muri_v[muro.r+1][muro.c]==2){
				return 4;
			}
		}
		if(muri_o[muro.r][muro.c]==2){
			return 5;
		}
	}else{
		if(muri_o[muro.r][muro.c]==1){
			if(muri_o[muro.r][muro.c+1]==2){ // C'è un muro completo sopra ed uno completo sotto
				return 2; // Sono nel caso 2
			}else{
				return 3; // Sono nel caso 3
			}
		}else{
			if(muri_o[muro.r][muro.c]==2){
				return 1; // Sono nel caso 1
			}else if(muri_o[muro.r][muro.c+1]==2){
				return 4;
			}
		}
		if(muri_v[muro.r][muro.c]==2){
			return 5;
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------
// Funzioni per i players
//
//Inizializza i players
void playersInit(){
	p[0].walls=8;
	p[1].walls=8;
	
	p[0].r=6;
	p[1].r=0;

	p[0].c=3;
	p[1].c=3;
	
	p[0].color=Red;
	p[1].color=Green;
}

//Disegna il player
void DrawPlayers(int x){
	
	LCD_DrawFullSquare(p[x-1].r, p[x-1].c, p[x-1].color);
	WallsRimanenti();
}


//
//----------------------------------------------------------------------
// Funzioni per i muri
//
//scrive sul display il numero di muri rimanenti per ogni player
void WallsRimanenti(){
	char buffer[3];
	
	sprintf(buffer, "%01d", p[0].walls);
	GUI_Text(38, 293, (uint8_t *)buffer, Black, White);
	sprintf(buffer, "%01d", p[1].walls);
	GUI_Text(192, 293, (uint8_t *)buffer, Black, White);
}

// "Crea" un nuovo "oggetto" muro nella posizione 3x3
void nuovoMuro(int orientation){
	// Muro verticale in mezzo alla scacchiera
	if(p[player-1].walls>0){
		cifraMossa(3, 3, 0, 1);
		muro.c=3;
		muro.r=3;
		muro.rotation=0;
		LCD_DrawWall(muro.rotation, muro.r, muro.c, Blue2);
	}else{
		errorMessage("Sono finiti i muri");
		DrawPossibleMoves(Yellow);
		movement=0;
		rotation=0;
		NVIC_EnableIRQ(RIT_IRQn);
	}
}

// Serve a cambiare l'orientazione del muro colora di bianco la vecchia posizione e colora il muro in quella nuova
void changeOrientation(int rotation){
	LCD_DrawWall(muro.rotation, muro.r, muro.c, White); // Il vecchio muro diventa bianco
	if(muro.rotation==0){ // se era verticale
		if(muri_v[muro.r][muro.c]==2){
			LCD_DrawWall(muro.rotation, muro.r, muro.c, Black);
		}else if(muri_v[muro.r][muro.c]==1){
			LCD_DrawWall(muro.rotation, muro.r-1, muro.c, Black);
		}
		if(muri_v[muro.r+1][muro.c]==2){
			LCD_DrawWall(muro.rotation, muro.r+1, muro.c, Black);
		}
	}else{
		if(muri_o[muro.r][muro.c]==2){
			LCD_DrawWall(muro.rotation, muro.r, muro.c, Black);
		}else if(muri_o[muro.r][muro.c]==1){
			LCD_DrawWall(muro.rotation, muro.r, muro.c-1, Black);
		}
		if(muri_o[muro.r][muro.c+1]==2){
			LCD_DrawWall(muro.rotation, muro.r, muro.c+1, Black);
		}	
	}
	muro.rotation=rotation;
	cifraMossa(muro.c, muro.r, muro.rotation, 1);
	LCD_DrawWall(muro.rotation, muro.r, muro.c, Black);
}
// Colora di bianco il muro corrente che non è stato piazzato e cifra la mossa come mossa nulla
void eliminaMuro(int orientation){
	int muroHere=isMuroHere(muro.rotation);
	int r=muro.r, c=muro.c;
	
	LCD_DrawWall(muro.rotation, muro.r, muro.c, White);
	if(muroHere==1){
				LCD_DrawWall(muro.rotation, muro.r, muro.c, Black);
	}else if(muroHere==2){
				if(muro.rotation==0){
					LCD_DrawWall(muro.rotation, muro.r-1, muro.c, Black); // muro sopra
					LCD_DrawWall(muro.rotation, muro.r+1, muro.c, Black); // muro sotto
				}else{
					LCD_DrawWall(muro.rotation, muro.r, muro.c-1, Black);
					LCD_DrawWall(muro.rotation, muro.r, muro.c+1, Black);
				}
	}else if(muroHere==3){
				if(muro.rotation==0){
					LCD_DrawWall(muro.rotation, muro.r-1, muro.c, Black); // muro sopra
				}else{
					LCD_DrawWall(muro.rotation, muro.r, muro.c-1, Black);
				}
	}else if(muroHere==4){
				if(muro.rotation==0){
					LCD_DrawWall(muro.rotation, muro.r+1, muro.c, Black);
				}else{
					LCD_DrawWall(muro.rotation, muro.r, muro.c+1, Black);
				}			
	}else if(muroHere==5){
		LCD_DrawSeparatore(muro.r, muro.c, Black);
	}
	if(muri_o[r][c]==2 || muri_v[r][c]==2){
			LCD_DrawSeparatore(r, c, Black);
	}
	mossaNulla();
}
//
//-------------------------------------------------------------------------------------------------------------------------
// Funzioni per disegnare e per muovere player e muri
//
// Grafica: Disegna i quadrati gialli attorno al player per indicare dove si può muovere
void DrawPossibleMoves(uint16_t color){
	int x=player;
	
	// Quadrato davanti
	if((p[x-1].r!=0)){ // Devo controllare che non ci sia l'altro player nella cella e che non ci sia un muro davanti
		if(isPlayerThere(alto)==1){
			if(p[x-1].r-1!=0 && (isMuroDavanti(alto, 2)==0) && (isMuroDavanti(alto, 1)==0)){
				LCD_DrawFullSquare(p[x-1].r-2, p[x-1].c, color);
			}
		}else{
			if(isMuroDavanti(alto, 1)==0){
				LCD_DrawFullSquare(p[x-1].r-1, p[x-1].c, color);
			}
		}		
	}
	// Quadrato dietro
	if((p[x-1].r!=6)){
		if(isPlayerThere(giu)==1){
			if(p[x-1].r+1!=6 && (isMuroDavanti(giu, 1)==0) && (isMuroDavanti(giu, 0)==0)){
				LCD_DrawFullSquare(p[x-1].r+2, p[x-1].c, color);
			}
		}else{
			if(isMuroDavanti(giu, 0)==0){
				LCD_DrawFullSquare(p[x-1].r+1, p[x-1].c, color);
			}
		}	
	}
	// Quadrato sinistra
	if((p[x-1].c!=0)){
		if(isPlayerThere(sinistra)==1){
			if(p[x-1].c-1!=0 && (isMuroDavanti(sinistra, 2)==0) && (isMuroDavanti(sinistra, 1)==0)){
				LCD_DrawFullSquare(p[x-1].r, p[x-1].c-2, color);
			}
		}else{
			if(isMuroDavanti(sinistra, 1)==0){
				LCD_DrawFullSquare(p[x-1].r, p[x-1].c-1, color);
			}
		}	
	}
	// Quadrato destra
	if((p[x-1].c!=6)){
		if(isPlayerThere(destra)==1){
			if(p[x-1].c+1!=6 && (isMuroDavanti(destra, 1)==0) && (isMuroDavanti(destra, 0)==0)){
				LCD_DrawFullSquare(p[x-1].r, p[x-1].c+2, color);
			}
		}else{
			if(isMuroDavanti(destra, 0)==0){
				LCD_DrawFullSquare(p[x-1].r, p[x-1].c+1, color);
			}
		}	
	}
}

// Questa funzione usa solo la mossa cifrata per muovere il giocatore
// Per i muri invece sono già stati mossi prima e quindi li salva nelle tabelle di memoria
void SpostoGiocatore(){
		
		removeError();
		if(!(getOrientation()==1 && getMoving()==0)){
			if(getMoving()!=1){
				//Muovo il player
				DrawPossibleMoves(White); // Cancello le indicazioni delle mosse
				LCD_DrawFullSquare(p[getPlayr()].r, p[getPlayr()].c, White);
				p[getPlayr()].r = getNextY();
				p[getPlayr()].c = getNextX();
				LCD_DrawFullSquare(p[getPlayr()].r, p[getPlayr()].c, p[getPlayr()].color); //Inserisco la nuova posizione
			}	else if(getMoving()==1){
					//Salvo il muretto in memoria	
					p[getPlayr()].walls=p[getPlayr()].walls-1;
					WallsRimanenti();
					if(getOrientation()==0){
						//Lo salvo nella tabella dei muri verticali
						if((muro.r==muro.c)&&(muro.c==3)){
							LCD_DrawWall(muro.rotation, muro.c, muro.r, Black);
						}
						muri_v[getNextY()][getNextX()]=2;
						muri_v[getNextY()+1][getNextX()]=1;
					}else{
						muri_o[getNextY()][getNextX()]=2;
						muri_o[getNextY()][getNextX()+1]=1;
				}
			}
		}	else{
			DrawPossibleMoves(White);
		}
}
// Se ritorna 0 allora non sono su un'altro muro e posso inserire il muro nelle tabelle
int isMuroThere(Mossa mossa){
	if(getOrientation()==0){
			if((muri_v[muro.r][muro.c]>0) || muri_v[muro.r+1][muro.c]>0 || (muri_o[muro.r][muro.c]==2)){
				return 1;
			}
	}else{
		if((muri_o[muro.r][muro.c]>0) || (muri_o[muro.r][muro.c+1]>0) || (muri_v[muro.r][muro.c]==2)){
				return 1;
		}
	}
	return 0;
}
// Ritorna 1 se c'è un muro davanti alla mossa che vuole fare il player
// Pos indica di quante caselle il player si vuole spostare
int isMuroDavanti(Mossa mossa, int pos){
	int x=player;
	
	if(mossa == alto){
		if(muri_o[p[x-1].r-pos][p[x-1].c]>0){
			return 1;
		}
	}else if(mossa == giu){
		if(muri_o[p[x-1].r+pos][p[x-1].c]>0){ //pos max = 1
			return 1;
		}
	}else if(mossa == destra){
		if(muri_v[p[x-1].r][p[x-1].c+pos]>0){ // pos max = 1
			return 1;
		}
	}else if (mossa == sinistra)
		if(muri_v[p[x-1].r][p[x-1].c-pos]>0){
			return 1;
		}
	return 0;
}
//
void muriInnit(){
	int i, j;
	
	for(i=0; i<6; i++){
		for(j=0; j<7; j++){
			muri_o[i][j]=0;
		}
	}
	for(i=0; i<7; i++){
		for(j=0; j<6; j++){
			muri_v[i][j]=0;
		}
	}
}
//
//
//-------------------------------------------------------------------------------------------------------------------------
// Funzioni per cifrare la mossa
//
// In base al tasto che ho premuto cifro la mossa
// Se si sta muovendo il player allora devo vedere se può essere effettuato un salto
// Se su muove un muro allora invoco la funzione per muovere il muro
void ScelgoMossa(Mossa mossa, int orientation, int moving){
	int x=player;
	
	//Sposto il player
	if((orientation==0 && moving==0)){
		// Quadrato davanti
		if((p[x-1].r!=0) && (mossa == alto)){
			if((isPlayerThere(alto)==1)){
				DrawPossibleMoves(Yellow);
				if(p[x-1].r-1!=0 && (isMuroDavanti(alto, 2)==0) && (isMuroDavanti(alto, 1)==0)){
					LCD_DrawFullSquare(p[x-1].r-2, p[x-1].c, p[x-1].color);
					cifraMossa(p[x-1].c, p[x-1].r-2, orientation, moving);
				}
			}else{
				if((isMuroDavanti(alto, 1)==0)){
					DrawPossibleMoves(Yellow);
					LCD_DrawFullSquare(p[x-1].r-1, p[x-1].c, p[x-1].color);
					cifraMossa(p[x-1].c, p[x-1].r-1, orientation, moving);
				}
			}
		}else
		// Quadrato dietro
		if((p[x-1].r!=6) && (mossa == giu)){
			if((isPlayerThere(giu)==1)){
				DrawPossibleMoves(Yellow);
				if(p[x-1].r+1!=6 && (isMuroDavanti(giu, 1)==0) && (isMuroDavanti(giu, 0)==0)){
					LCD_DrawFullSquare(p[x-1].r+2, p[x-1].c, p[x-1].color);
					cifraMossa(p[x-1].c, p[x-1].r+2, orientation, moving);
				}
			}else{
				if(isMuroDavanti(giu, 0)==0){
					DrawPossibleMoves(Yellow);
					LCD_DrawFullSquare(p[x-1].r+1, p[x-1].c, p[x-1].color);
					cifraMossa(p[x-1].c, p[x-1].r+1, orientation, moving);
				}
			}
		}else
		// Quadrato sinistra
		if((p[x-1].c!=0) && (mossa == sinistra) ){
			if((isPlayerThere(sinistra)==1)){
				DrawPossibleMoves(Yellow);
				if(p[x-1].c-1!=0 && (isMuroDavanti(sinistra, 2)==0) && (isMuroDavanti(sinistra, 1)==0)){
					LCD_DrawFullSquare(p[x-1].r, p[x-1].c-2, p[x-1].color);
					cifraMossa(p[x-1].c-2, p[x-1].r, orientation, moving);
				}
			}else{
				if(isMuroDavanti(sinistra, 1)==0){
					DrawPossibleMoves(Yellow);
					LCD_DrawFullSquare(p[x-1].r, p[x-1].c-1, p[x-1].color);
					cifraMossa(p[x-1].c-1, p[x-1].r, orientation, moving);
				}
			}
		}else
		// Quadrato destra
		if((p[x-1].c!=6) && (mossa == destra)){
			if((isPlayerThere(destra)==1)){
				DrawPossibleMoves(Yellow);
				if(p[x-1].c+1!=6 && (isMuroDavanti(destra, 1)==0) && (isMuroDavanti(destra, 0)==0)){
					LCD_DrawFullSquare(p[x-1].r, p[x-1].c+2, p[x-1].color);
					cifraMossa(p[x-1].c+2, p[x-1].r, orientation, moving);
				}
			}else{
				if(isMuroDavanti(destra, 0)==0){
					DrawPossibleMoves(Yellow);
					LCD_DrawFullSquare(p[x-1].r, p[x-1].c+1, p[x-1].color);
					cifraMossa(p[x-1].c+1, p[x-1].r, orientation, moving);
				}
			}
		}else
		if(mossa==select){
			conferma=1;
			SpostoGiocatore();
		}
	}else if (moving==1){ // Sposto i muretti
		ScelgoMossaMuro(mossa, orientation, moving);
	}
}
// Controllo se nella casella in cui mi voglio muovere c'è l'altro player
int isPlayerThere(Mossa mossa){
	int p1=player-1;
	int p2;
	
	if(p1==0){
		p2=1;
	}else{
	 p2=0;	
	}
	// Quadrato davanti
	if(mossa == alto){
		if((p[p1].c == p[p2].c) && (p[p1].r-1 == p[p2].r)){
			return 1;
		}
	}
	// Quadrato dietro
	if(mossa == giu){
		if((p[p1].c == p[p2].c) && (p[p1].r+1 == p[p2].r)){
			return 1;
		}
	}
	// Quadrato destra
	if(mossa == destra){
		if((p[p1].c+1 == p[p2].c) && (p[p1].r == p[p2].r)){
			return 1;
		}
	}
	// Quadrato sinistra
	if(mossa == sinistra){
		if((p[p1].c-1 == p[p2].c) && (p[p1].r == p[p2].r)){
			return 1;
		}		
	}
	return 0;
}

// Serve per cifrare la mossa in una variabile intera senza segno a 32bit
void cifraMossa(int x, int y, int orientation, int moving){ 
	
	uint8_t player8bit = player-1;
  uint8_t moving8bit = moving;
	uint8_t orientation8bit = orientation;
	uint8_t x8bit = x;
	uint8_t y8bit = y;
	
	mossaUnsignedInt=0; // 32-bit unsigned int variable
	removeError();
  // X = 0 colonne
  mossaUnsignedInt |= ((uint32_t)x8bit) << 0;
  // Y = 8 righe
  mossaUnsignedInt |= ((uint32_t)y8bit) << 8;
  // V/H = 16 
  mossaUnsignedInt |= ((uint32_t)orientation8bit) << 16;
  // PM/WP = 20
  mossaUnsignedInt |= ((uint32_t)moving8bit) << 20;
  //Player ID = 24
  mossaUnsignedInt |= ((uint32_t)player8bit) << 24;
}

// Metodi getters per ottenere informazioni sulla mossa da fare
uint8_t getNextX(){
	
	// Leggi i bit da 0 a 7
  uint8_t x8bit = (uint8_t)(mossaUnsignedInt & 0xFF);
	return x8bit;
}

uint8_t getNextY(){
	// Leggi i bit da 8 a 15
  uint8_t y8bit = (uint8_t)((mossaUnsignedInt >> 8) & 0xFF);
	return y8bit;
}

uint8_t getOrientation(){
	// Leggi i bit da 16 a 19
  uint8_t orientation8bit = (uint8_t)((mossaUnsignedInt >> 16) & 0x0F);
	return orientation8bit;
}

uint8_t getMoving(){
	// Leggi i bit da 20 a 13
  uint8_t moving8bit = (uint8_t)((mossaUnsignedInt >> 20) & 0x0F);
	return moving8bit;
}

uint8_t getPlayr(){
  // Leggi i bit da 24 a 31
  uint8_t player8bit = (uint8_t)((mossaUnsignedInt >> 24) & 0xFF);
	return player8bit;
}

//Cifra come mossa quella nulla
void mossaNulla(){
	cifraMossa(0,0,1,0);
}



//
//
//-------------------------------------------------------------------------------------------------------------------------
// Funzioni per controllare che ci sia ancora un percorso disponibile
// Funzione wrapper inizializza la tabella di prova a tutti 0 tranne il player e chiama la funzione ricorsiva
int checkBlock(){
	//Devo controllare sia che il player 1 che il player 2 possano raggiungere il fondo
	int j, i, tabr[7][7], r0=0, r1=0;
	Coord cordPlayer;
	
	cordPlayer.c = p[0].c;
	cordPlayer.r = p[0].r;
	
	for(i=0;i<7;i++){
		for(j=0;j<7;j++){
			tabr[i][j]=0;
		}
	}
	tabr[cordPlayer.r][cordPlayer.c]=1;
	r0=checkBlockRecursive(0, tabr, 0, 0, cordPlayer); //Controllo che il player 0 non sia bloccato
	
	for(i=0;i<7;i++){
		for(j=0;j<7;j++){
			tabr[i][j]=0;
		}
	}
	cordPlayer.c = p[1].c;
	cordPlayer.r = p[1].r;
	tabr[cordPlayer.r][cordPlayer.c]=1;
  r1=checkBlockRecursive(1, tabr, 6, 0, cordPlayer); //Controllo che il player 1 non sia bloccato
	
	if((r0==1) && (r1==1)){
		return 1;
	}
	return 0;
}

// Funzione ricorsiva, cerca se esiste ancora un passaggio se esiste ritorna 1 se no 0
int checkBlockRecursive(int giocatore, int tabr[7][7], int traguardo, int hit, Coord cordPlayer){
	
	if(cordPlayer.r==traguardo){
		return 1;
	}
		
	// Mi muovo in alto
	if((hit==0) && (tabr[cordPlayer.r-1][cordPlayer.c]==0) && ((cordPlayer.r!=0) && (muroRecursive(alto, cordPlayer)==0))){ // Se non c'è un muro davanti
		cordPlayer.r = cordPlayer.r-1;
		tabr[cordPlayer.r][cordPlayer.c]=1;
		hit=checkBlockRecursive(giocatore, tabr, traguardo, hit, cordPlayer);
		//Backtrack
		tabr[cordPlayer.r][cordPlayer.c]=0;
		cordPlayer.r = cordPlayer.r+1;
		
	}
	if((hit==0) &&(tabr[cordPlayer.r+1][cordPlayer.c]==0) && ((cordPlayer.r!=6) && (muroRecursive(giu, cordPlayer)==0))){ // Se non c'è un muro davanti
		cordPlayer.r = cordPlayer.r+1;
		tabr[cordPlayer.r][cordPlayer.c]=1;
		hit=checkBlockRecursive(giocatore, tabr, traguardo, hit, cordPlayer);
		tabr[cordPlayer.r][cordPlayer.c]=0;	
		cordPlayer.r = cordPlayer.r-1;
		
	}
	if((hit==0) &&(tabr[cordPlayer.r][cordPlayer.c-1]==0) && ((cordPlayer.c!=0) && (muroRecursive(sinistra, cordPlayer)==0))){ // Se non c'è un muro davanti
		cordPlayer.c = cordPlayer.c-1;
		tabr[cordPlayer.r][cordPlayer.c]=1;
		hit=checkBlockRecursive(giocatore, tabr, traguardo, hit, cordPlayer);
		tabr[cordPlayer.r][cordPlayer.c]=0;	
		cordPlayer.c = cordPlayer.c+1;
		
	}
	if((hit==0) &&(tabr[cordPlayer.r][cordPlayer.c+1]==0) && ((cordPlayer.c!=6) && (muroRecursive(destra, cordPlayer)==0))){ // Se non c'è un muro davanti
		cordPlayer.c = cordPlayer.c+1;
		tabr[cordPlayer.r][cordPlayer.c]=1;
		hit=checkBlockRecursive(giocatore, tabr, traguardo, hit, cordPlayer);
		tabr[cordPlayer.r][cordPlayer.c]=0;	
		cordPlayer.c = cordPlayer.c-1;
		
	}
	return hit;
}

// Controlla che davanti al player ci sia un muro o no se c'è ritorna 1 altrimenti 0
int muroRecursive(Mossa mov, Coord cordPlayer){
	
	if(mov == alto){
		if(muri_o[cordPlayer.r-1][cordPlayer.c]>0){
			return 1;
		}
	}else if(mov == giu){
		if(muri_o[cordPlayer.r][cordPlayer.c]>0){ //pos max = 1
			return 1;
		}
	}else if(mov == destra){
		if(muri_v[cordPlayer.r][cordPlayer.c]>0){ // pos max = 1
			return 1;
		}
	}else if (mov == sinistra)
		if(muri_v[cordPlayer.r][cordPlayer.c-1]>0){
			return 1;
		}
	return 0;
}
//
//
//-------------------------------------------------------------------------------------------------------------------------
// Funzioni per variabili esterne
//
// Controllo se uno dei due giocatori ha vinto
int checkVittoria(){
	if(p[0].r==0){
		//Il player 1 ha vinto, ritorno 1
		return 1;
	}else if(p[1].r==6) {
		return 2;
	}
	return 0;
}

// Ritorna la variabile conferma
int getConferma(){
	return conferma;
}
// Ritorna il colore del player che sta giocando
uint16_t getColor(void){
	int x=player-1;
	
	if(x==0){
		x=1;
	}else{
	 x=0;	
	}
	return p[x].color;
}
// Setta la variabile conferma
void setConferma(int x){
	conferma=x;
}











// fine
