#ifndef __MENU_H__
#define __MENU_H__



/*
-------------------------------------------------

	Krystals DS

	Archivo de gestion del menu

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



// Clase CMenu
class CMenu {

	// Segmento privado
	private:

		// Variables
		s8 option;			// Opcion seleccionada del menu

		bool fade_in;		// Control del fade de la pantalla
		bool fade_out;
		s32 bright;

		u8 thread, next_thread;		// Control de estado del menu
		u8 state, next_state;		// Control de la submaquina de estados

		s32 menu_px;				// Referencia de posicion X de los elementos del menu
		s32 bg_x[2], bg_y[2];		// Referencias de posicion para las capas de texto o sombra del menu

		// Inicializaciones del menu
		void Init(void);

		// Crea los elementos 2D del menu
		void CreateMainMenu(void);

		// Efecto Fade-in
		bool FadeIn(void);

		// Efecto FadeOut
		bool FadeOut(void);

		// Rutina cuando esta en modo Menu Principal
		void StateMainMenu(void);

		// Rutina cuando esta seleccionada la opcion "RECORDS"
		void StateRecords(void);

		// Rutina cuando esta seleccionada la opcion "CREDITS"
		void StateCredits(void);

		// Renderiza la escena 2D
		void Render2d(void);

		// Cambia de opcion del menu
		void ChangeOption(s8 change);

		

	// Segmento publico
	public:

		// Constructor
		CMenu(void);

		// Destructor
		~CMenu(void);

		// Menu principal del juego
		u8 MainMenu(void);

};
// Metodos globlales de la clase CMenu
extern CMenu* Menu;



/*
Metodos principales del Menu
*/



#endif
