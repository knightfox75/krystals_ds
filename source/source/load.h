#ifndef __LOAD_H__
#define __LOAD_H__



/*
-------------------------------------------------

	Krystals DS

	Archivo de carga de archivos

	Requiere DevkitARM
	Requiere NightFox's Lib

	Codigo por Cesar Rincon "NightFox"
	http://www.nightfoxandco.com
	Inicio 08 de Septiembre del 2012

	(c) 2012 - 2013 NightFox & Co.

-------------------------------------------------
*/



// Clase CLoad
class CLoad {

	// Segmento privado
	private:



	// Segmento publico
	public:

		// Constructor
		CLoad(void);

		// Destructor
		~CLoad(void);

		// Carga de archivos para el juego (comunes)
		void GameMainFiles(void);

		// Carga los archivos para el menu principal
		void MenuMainFiles(void);

		// Carga los archivos para los splash screens
		void SplashScreensFiles(void);

		// Carga los archivos para el resumen de puntuacion
		void RecordsFiles(void);

};
// Metodos globlales de la clase CGame
extern CLoad* Load;

/*
Clase que controla la carga de datos del juego desde NitroFS
*/



#endif