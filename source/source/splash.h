#ifndef __SPLASH_H__
#define __SPLASH_H__



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



// Includes
#include <nds.h>



// Clase CSplash
class CSplash {

	// Segmento privado
	private:

		// Inicializa la presentacion
		void Init(void);

		// Presentacion
		void ShowTime(void);



	// Segmento publico
	public:

		// Constructor
		CSplash(void);

		// Destructor
		~CSplash(void);

		// Muestra las pantallas de presentacion
		void SplashScreens(void);

};
// Metodos globales de la clase "CSplash"
extern CSplash* Splash;



/*
Metodos principales de la clase "CSplash"
*/





#endif
