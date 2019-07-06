/*
-------------------------------------------------

	Krystals DS

	Archivo de gestion del programa

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
Metodos de la clase "CKernel"
*/

// Contructor clase CKernel
CKernel::CKernel(void) {

	// Crea los objetos de las clases principales del programa
	System = new CSystem();			// Objetos del sistema
	Savegame = new CSavegame();		// Objetos para los datos de guardado
	Sound = new CSound();			// Objetos de la gestion de sonido
	Load = new CLoad();				// Objetos de la carga de archivos

}



// Destructor clase CKernel
CKernel::~CKernel(void) {

	// Elimina los objetos al destruir la clase
	delete Load;
	Load = NULL;
	delete Sound;
	Sound = NULL;
	delete Savegame;
	Savegame = NULL;
	delete System;
	System = NULL;

}



// Metodos globales de la clase CKernel
CKernel* Kernel;



// Nucleo de ejecucion del programa
void CKernel::Run(void) {

	// Variables
	bool loop = true;

	// Pantallas de presentacion
	Splash = new CSplash();		// Crea los objetos
	Splash->SplashScreens();	// Muestra la presentacion
	delete Splash;				// Elimina los objetos
	Splash = NULL;			

	// Bucle principal de ejecucion
	while (loop) {

		// Maquina de estados principal
		switch (MainMenu()) {
			// Ejecucion del juego
			case MMOPT_RUNGAME:
				RunGame();
				break;
			// Por defecto, nada
			default:
				break;
		}

	}

}



// Menu principal
u8 CKernel::MainMenu(void) {

	// Variables
	u8 option = 0;
	// Crea los objetos de la clase
	Menu = new CMenu();
	// Ejecuta el menu del juego
	option = Menu->MainMenu();
	// Elimina el objeto
	delete Menu;
	Menu = NULL;
	// Devuelve el resultado
	return option;

}



// Ejecucion del juego
void CKernel::RunGame(void) {

	// Crea los objetos de la clase
	Game = new CGame();
	// Ejecuta el Runtime del juego
	Game->Run();
	// Elimina el objeto
	delete Game;
	Game = NULL;

}