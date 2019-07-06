#ifndef __GAME_H__
#define __GAME_H__



/*
-------------------------------------------------

	Krystals DS

	Archivo de gestion del juego

	Requiere DevkitARM
	Requiere NightFox's Lib

	Codigo por Cesar Rincon "NightFox"
	http://www.nightfoxandco.com
	Inicio 08 de Septiembre del 2012

	(c) 2012 - 2013 NightFox & Co.

-------------------------------------------------
*/



// Includes
#include <nds.h>



// Clase CGame
class CGame {

	// Segmento publico
	public:

		// Constructor
		CGame(void);

		// Destructor
		~CGame(void);

		// Nucleo de ejecucion
		void Run(void);

};
// Metodos globlales de la clase CGame
extern CGame* Game;

/*
Metodos principales del Juego
*/





#endif
