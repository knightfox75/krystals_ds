#ifndef __KERNEL_H__
#define __KERNEL_H__



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



// Includes
#include <nds.h>



// Clase CKernel
class CKernel {

	// Segmento publico
	public:

		// Constructor
		CKernel(void);

		// Destructor
		~CKernel(void);

		// Metodo Principal (Nucleo de ejecucion del programa)
		void Run(void);

		// Metodo del Menu principal
		u8 MainMenu(void);

		// Metodo del Juego
		void RunGame(void);

};
// Metodos globlales de la clase CGame
extern CKernel* Kernel;

/*
Metodos principales del programa (CKernel)
*/





#endif
