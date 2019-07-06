#ifndef __DRAW_H__
#define __DRAW_H__



/*
-------------------------------------------------

	Krystals DS

	Archivo de funciones de dibujado del juego

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
#include <vector>



// Clase CDraw
class CDraw {

	// Segmento privado
	private:

		struct {
			bool active;		// Animacion del destello activa
			u16 timer;			// Control del tiempo entre animaciones
			s8 frame[8];		// Control de los frames de la animacion
			u8 delay;			// Control de la velocidad de la animacion
			u8 thread;			// Control de ejecucion de la rutina
		} sparkle;

		// Actualizacion de las gemas del tablero
		void UpdateBoard(void);

		// Actualizacion de los elementos de la gema "NEXT" en la pantalla
		void UpdateNextGem(void);

		// Actualizion de los elementos de la gema en movimiento
		void UpdateMyGem(void);

		// Posiciona la gema en pantalla
		void UpdateGemPos(void);

		// Actualiza el movimiento del fondo affine
		void Mode7Animation(void);

		// Crea la tabla de shading
		void MakeShadeTable(void);

		// Cambio activo del fondo
		void ChangeBg(void);



	// Segmento publico
	public:

		// Variables
		bool fade_in;				// Control del fade de la pantalla
		bool fade_out;
		s32 bright;

		u8 erase_frame;				// Control del frame de borrado
		u8 erase_delay;				// Retardo en el borrado

		s32 mode7_scroll;			// Movimiento del modo 7
		s32 mode7_angle;			// Efecto "onda" del modo 7

		s8 shading_table[192];		// Guarda los valores para el efecto de shading
		s8 shading[192];

		struct {
			bool update;			// Flag de cambio de fondo
			u8 thread;				// Control del cambio de fondo
			u8 next_thread;
			s8 bright;				// Nivell de brillo del fondo
			u8 num;					// Numero de fondo
			std::vector<u8> cue;	// Vector para el random de fondos en la cola (1 - BG_NUMBER)
			u8 list[BG_NUMBER];		// Cola con todos los fondos a usar (bg 1 = id 0)
		} background;

		struct {
			bool board;			// Flags de actualizacion de las gemas
			bool next_gem;
			bool my_gem;
			bool gem_pos;
			bool text;			// Flag para actualizar las capas de texto
		} update;

		// Constructor
		CDraw(void);

		// Destructor
		~CDraw(void);

		// Efecto Fade-in
		bool FadeIn(void);

		// Efecto FadeOut
		bool FadeOut(void);

		// Actualiza el contenido de la pantalla
		void UpdateScreen(void);

		// Dibujado inicial del decorado
		void DrawStage(void);

		// Opten el primer frame de la gema segun color
		u8 GetGemColorFrame(u8 color);

		// Animacion de borrado
		bool EraseAnimation(void);

		// Animacion de destellos
		void SparkleAnimation(void);

		// Crea una lista random de los fondos disponibles
		void MakeBgList(void);

};
// Metodos globlales de la clase CDraw
extern CDraw* Draw;

/*
Metodos de dibujado del programa (Juego)
*/





#endif
