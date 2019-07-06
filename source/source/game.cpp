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



/*
-------------------------------------------------
	Includes
-------------------------------------------------
*/

// Includes C
#include <stdio.h>
// Includes propietarios NDS
#include <nds.h>
// Includes librerias propias
#include <nf_lib.h>
// Includes del juego
#include "includes.h"



/*
Metodos de la clase "CGame"
*/

// Contructor clase CGame
CGame::CGame(void) {

	// Crea los objetos necesarios
	Engine = new CEngine();			// Crea el objetos "Engine", que se encarga de la ejecucion y control del juego
	Draw = new CDraw();				// Objetos de dibujado de elementos en pantalla
	Board = new CBoard();			// Objetos de gestion del tablero
	Control = new CControl();		// Objetos de gestion del juego
	Score = new CScore();			// Objetos del control del Score
	Records = new CRecords();		// Objetos de la gestion de records

}



// Destructor clase CGame
CGame::~CGame(void) {

	// Destruye los objetos
	delete Records;
	Records = NULL;
	delete Score;
	Score = NULL;
	delete Control;
	Control = NULL;
	delete Board;
	Board = NULL;
	delete Draw;
	Draw = NULL;
	delete Engine;
	Engine = NULL;

}


// Metodos globales de la clase CSystem
CGame* Game;



// Nucleo de ejecucion
void CGame::Run(void) {

	Engine->Run();				// Ejecuta el Juego
	Records->Report();			// Resumen de la puntuacion de la partida

}
