#ifndef __CONTROL_H__
#define __CONTROL_H__



/*
-------------------------------------------------

	Krystals DS

	Archivo de metodos de control del juego (manejo)

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



// Clase CControl
class CControl {

	// Segmento privado
	private:

		// Variables
		struct {				// Retardo en el automove de la gema
			s16 right;
			s16 left;
			s16 down;
		} delay;

		// Verifica si la gema puede ir a la derecha. De poder, muevela
		void MoveGemRight(void);

		// Verifica si la gema puede ir a la izquierda. De poder, muevela
		void MoveGemLeft(void);

		// Verifica si la gema ya no puede bajar mas. De poder, bajala
		void MoveGemBottom(void);

		// Verifica si puede moverse la ficha abajo y devuelve el resultado
		bool CheckGemBottom(void);



	// Segmento publico
	public:

		// Constructor
		CControl(void);

		// Destructor
		~CControl(void);

		// Variables
		struct {
			s16 delay;			// Tiempo a esperar entre caidas de la ficha (velocidad de caida)
			s16 timer;			// Timer caida de la gema
		} drop;

		// Control manual de la gema (jugador)
		bool MoveGem(void);

};
// Metodos globlales de la clase CBoard
extern CControl* Control;

/*
Metodos de gestion del tablero
*/





#endif
