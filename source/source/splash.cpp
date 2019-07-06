/*
-------------------------------------------------

	Krystals DS

	Archivo de gestion de las pantallas de presentacion

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
Metodos de la clase "CSplash"
*/

// Contructor clase CSplash
CSplash::CSplash(void) {
}

// Destructor clase CSplash
CSplash::~CSplash(void) {
}

// Metodos globales de la clase CSplash
CSplash* Splash;



// Muestra las pantallas de presentacion
void CSplash::SplashScreens(void) {

	// Inicializa
	Init();

	// Muestra las pantallas
	ShowTime();

}



// Inicializa la presentacion
void CSplash::Init(void) {

	// Borra todo el contenido de la memoria
	System->Reset();

	// Pantalla en negro
	setBrightness(3, -16);
	swiWaitForVBlank();

	// Carga los archivos necesarios
	Load->SplashScreensFiles();

}



// Presentacion
void CSplash::ShowTime(void) {

	// Variables
	bool loop = true;					// Control del loop
	s32 bright = (16 << 8);				// Brillo
	u8 state = 0, next_state = 0;		// Control de estados
	s32 timer = 0;						// Temporizador
	u8 bg = 0;							// Set de fondos a cargar

	// Bucle 
	while (loop) {

		// Lee el teclado
		System->ReadKeys();

		// Estados
		switch (state) {

			// Carga del fondo en pantalla
			case 0:
				// Segun que fondo toque cargar
				switch (bg) {
					// Fondos Powered by...
					case 0:
						NF_CreateTiledBg(0, 3, "nfl");
						NF_CreateTiledBg(1, 3, "devkitpro");
						break;
					case 1:
						NF_CreateTiledBg(0, 3, "nfco");
						NF_CreateTiledBg(1, 3, "neoflash");
						break;
					// Error trap
					default:
						break;
				}
				// Indica el cambio de estado
				next_state = 1;
				break;

			// Realiza el Fade In
			case 1:
				// Nuevo nivel de brillo
				bright -= 64;
				if (bright <= 0) {
					bright = 0;
					// Indica el cambio de estado
					next_state = 2;
				}
				// Override con la pulsacion de tecla
				if (Keypress->a || Keypress->b || Keypress->start) {
					next_state = 3;
				}
				// Ajusta el nivel de brillo
				setBrightness(3, -(bright >> 8));
				break;

			// Espera 90 frames (1.5 segundos)
			case 2:
				timer ++;
				if (timer >= 90) {
					timer = 0;
					next_state = 3;
				}
				// Override con la pulsacion de tecla
				if (Keypress->a || Keypress->b || Keypress->start) {
					timer = 0;
					next_state = 3;
				}
				break;

			// Realiza el Fade Out
			case 3:
				bright += 64;
				if (bright >= (16 << 8)) {
					bright = (16 << 8);
					// Indica el cambio de estado que corresponda
					if (bg >= 1) {
						// Todas las pantallas mostradas, sal
						loop = false;
					} else {
						// Carga la siguiente pantalla
						bg ++;
						next_state = 0;
					}
				}
				// Ajusta el nivel de brillo
				setBrightness(3, -(bright >> 8));
				break;

			// Error trap
			default:
				break;

		}

		// Calcula el siguiente estado si es necesario
		if (state != next_state) state = next_state;

		// Espera al sincronismo vertical
		swiWaitForVBlank();

	}

}