#include "LPC17xx.h"

typedef enum{
	alto, destra, sinistra, giu, select, fermo
}Mossa;

typedef struct{
	int r;
	int c;
}Coord;

//Funzioni player
void playersInit(void);
void DrawPlayers(int x);

//Funzioni grafica e spostamento muri/player
void DrawPossibleMoves(uint16_t color);
void SpostoGiocatore(void);
int isMuroHere(int orientation);
int isMuroThere(Mossa mossa);
int isMuroDavanti(Mossa mossa, int pos);
void ScelgoMossaMuro(Mossa mossa, int orientation, int moving);

// Funzioni per cifrare le mosse
void ScelgoMossa(Mossa mossa, int orientation, int moving);
void cifraMossa(int x, int y, int orientation, int moving);
int isPlayerThere(Mossa mossa);
void mossaNulla(void);
uint8_t getNextX(void);
uint8_t getNextY(void);
uint8_t getOrientation(void);
uint8_t getMoving(void);
uint8_t getPlayr(void);

// Funzioni Muri
void changeOrientation(int rotation);
void nuovoMuro(int orientation);
void eliminaMuro(int orientation);
void WallsRimanenti(void);
void muriInnit(void);

//Funzione ricorsiva che controlla che ci sia sempre una possibilità di vittoria
int checkBlock(void);
int checkBlockRecursive(int giocatore, int tabr[7][7], int traguardo, int hit, Coord cordPlayer);
int muroRecursive(Mossa mov, Coord cordPlayer);

//Altre funzioni
int getConferma(void);
void setConferma(int x);
int checkVittoria(void);
uint16_t getColor(void);
