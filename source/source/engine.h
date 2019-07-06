#ifndef __ENGINE_H__
#define __ENGINE_H__



/*
-------------------------------------------------

	Krystals DS

	Archivo de motor del juego

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



// Clase CEngine
class CEngine {

	// Segmento privado
	private:

		// Variables

		// Control de Run();
		u8 run_id;
		u8 next_run_id;

		// Control del Kernel
		u8 thread;				// Hilo de ejecucion del nucleo
		u8 next_thread;			// Siguiente thread
		s16 delay;				// Control de espera entre threads

		// Control de la pausa
		s32 pause_x[2], pause_y[2];
		s32 pause_sx[2], pause_sy[2];
		s32 pause_up[2], pause_down[2], pause_left[2], pause_right[2];

		// Control de los banners de finde juego
		s32 banner_x[2];


		// Inicializa los datos de la partida
		void Init(void);

		// Kernel del engine
		u8 Kernel(void);

		// Genera una nueva gema
		void MakeNewGem(void);

		// GameOver - Rutinas del Game Over
		u8 GameOver(void);

		// GameDone - Rutinas del juego completado
		u8 GameDone(void);

		// Abort - Rutinas para abortar la partida
		u8 Abort(void);

		// Menu de pausa
		u8 GamePause(void);



	// Segmento publico
	public:

		// Variables
		u8 gem[3];				// Gema actual
		u8 gem_type;			// Tipo de gema

		u8 next_gem[3];			// Siguiente gema
		u8 next_gem_type;		// Tipo de siguiente gema

		s32 gem_x, gem_y;		// Posicion actual de la gema movil

		s32 total_gems;			// Numero de gemas en pantalla

		bool pause;				// Flag de pausa
		bool abort;				// Flag de abortar el juego


		// Constructor
		CEngine(void);

		// Destructor
		~CEngine(void);

		// Nucleo del juego
		void Run(void);

		// Cuenta la gemas eliminadas, para saber si hay que cambiar de nivel
		void CountDeletedGems(u16 gems);

};
// Metodos globlales de la clase CEngine
extern CEngine* Engine;

/*
Metodos principales del programa (Motor del juego)
*/



#endif
