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
Metodos de la clase "CControl"
*/

// Contructor clase CControl
CControl::CControl(void) {

	/*
	Inicializa las variables privadas
	*/

	// Reseteo del control del automove
	delay.down = 0;
	delay.left = 0;
	delay.right = 0;

	/*
	Inicializa las variables publicas
	*/

	// Velocidad inicial de caida
	drop.delay = DROP_MIN_SPEED;
	// Reteteo del contador de autocaida
	drop.timer = 0;

}



// Destructor clase CControl
CControl::~CControl(void) {
}



// Metodos globlales de la clase CControl
CControl* Control;



// Control manual de la gema (jugador)
bool CControl::MoveGem(void) {

	// Variables
	u8 n = 0;
	bool result = true;
	bool auto_drop = false;
	bool move_right = true;
	bool move_left = true;


	/*
	Autodrop (caida automatica)
	*/

	// Actualiza el contador de tiempo
	drop.timer ++;
	if (drop.timer > drop.delay) {
		// Baja una posicion
		auto_drop = true;
		drop.timer = 0;
		result = CheckGemBottom();		// Verifica si puede bajar una posicion
		if (result) MoveGemBottom();	// Bajala si es posible
	}


	/*
	Control manual
	*/

	// Registra si hay alguna invalidacion de movimiento al pulsar teclas
	if (Keypress->left || Keyheld->left || Keypress->l || Keyheld->l || !result) move_right = false;
	if (Keypress->right || Keyheld->right || Keypress->r || Keyheld->r || !result) move_left = false;

	// Si esta en pantalla, permite el movimiento
	if ((Engine->gem_y > -3)) {

		// Movimiento a la derecha
		if (move_right) {
			// Muevela a la derecha (paso a paso)
			if (Keypress->right) {
				MoveGemRight();
			} else {
				if (Keyheld->right) {	// Muevela a la derecha (automove)
					delay.right ++;
					if (delay.right > 20) {
						delay.right = 10;
						MoveGemRight();
					}
				} else {
					delay.right = 0;
				}
			}
			// Muevela a la derecha (rapido)
			if (Keyheld->r && !Keypress->right && !Keyheld->right) {
				MoveGemRight();
			}
		}

		// Movimiento a la izquierda
		if (move_left) {
			// Muevela a la izquierda (paso a paso)
			if (Keypress->left) {
				MoveGemLeft();
			} else {
				if (Keyheld->left) {	// Muevela a la izquierda (automove)
					delay.left ++;
					if (delay.left > 20) {
						delay.left = 10;
						MoveGemLeft();
					}
				} else {
					delay.left = 0;
				}
			}
			// Muevela a la izquierda (rapido)
			if (Keyheld->l && !Keypress->left && !Keyheld->left) {
				MoveGemLeft();
			}
		}

		// Si no estas en autodrop...
		if (!auto_drop) {
			// Muevela abajo (paso a paso)
			if (Keypress->down) {
				result = CheckGemBottom();				// Si en esta posicion, no se puede descender mas, indicalo
				if (result) MoveGemBottom();			// Intenta bajar una posicion
			} else {
				if (Keyheld->down) {	// Muevela abajo (automove)
					delay.down ++;
					if (delay.down > 20) {
						delay.down = 10;
						result = CheckGemBottom();		// Si en esta posicion, no se puede descender mas, indicalo
						if (result) MoveGemBottom();	// Intenta bajar una posicion
					}
				} else {
					delay.down = 0;
				}
			}
			// Muevela abajo (rapido)
			if ((Keyheld->b || Keyheld->x) && !Keypress->down && !Keyheld->down) {
				// Si en esta posicion, no se puede descender mas, pero se presiona abajo, indicalo
				result = CheckGemBottom();
				// Intenta bajar la una posicion de todos modos
				MoveGemBottom();
			}
		}

		// Rota la gema
		if (Keypress->up || Keypress->a || Keypress->y) {
			n = Engine->gem[0];
			Engine->gem[0] = Engine->gem[1];
			Engine->gem[1] = Engine->gem[2];
			Engine->gem[2] = n;
			Draw->update.my_gem = true;
		}

	}

	// Lee la señal de abortar partida
	if (Keypress->select) {
		Engine->abort = true;
	}

	// Devuelve el resultado
	return result;

}



// Verifica si la gema puede ir a la derecha. De poder, muevela
void CControl::MoveGemRight(void) {

	// Variables
	s16 x = (Engine->gem_x + 1);
	s16 y;
	u8 n = 0;
	u8 collision = 0;


	// Si aun no estas al final del tablero (7)
	if (x < 8) {
		// Si hay espacio libre
		for (n = 0; n < 3; n ++) {
			y = (Engine->gem_y + n);
			if (y >= 0) {
				if (Board->board[y][x] != EMPTY) collision ++;
			}
		}
		// Si hay espacio, muevela
		if (collision == 0) {
			Engine->gem_x ++;
			Draw->update.gem_pos = true;
		}
	}
	

}



// Verifica si la gema puede ir a la izquierda. De poder, muevela
void CControl::MoveGemLeft(void) {

	// Variables
	s16 x = (Engine->gem_x - 1);
	s16 y;
	u8 n = 0;
	u8 collision = 0;


	// Si aun no estas al final del tablero (0)
	if (x > -1) {
		// Si hay espacio libre
		for (n = 0; n < 3; n ++) {
			y = (Engine->gem_y + n);
			if (y >= 0) {
				if (Board->board[y][x] != EMPTY) collision ++;
			}
		}
		// Si hay espacio, muevela
		if (collision == 0) {
			Engine->gem_x --;
			Draw->update.gem_pos = true;
		}
	}
	

}



// Verifica si una gema ya no puede bajar mas, de poder, bajala
void CControl::MoveGemBottom(void) {

	// Variables
	s16 y = (Engine->gem_y + 3);

	// Si ya esta al final del tablero
	if (y <= 19) {
		// Verifica si puede bajar mas
		if (Board->board[y][Engine->gem_x] == EMPTY) {
			// Baja una posicion
			Engine->gem_y ++;
			// Marca para actualizar la posicion
			Draw->update.gem_pos = true;
		}
	}

}



// Verifica si una gema ya no puede bajar mas y devuelve el resultado
bool CControl::CheckGemBottom(void) {

	// Variables
	bool result = true;
	s16 y = (Engine->gem_y + 3);

	// Si ya esta al final del tablero
	if (y > 19) {
		// Estas fuera del tablero
		result = false;
	} else if (Board->board[y][Engine->gem_x] != EMPTY) {	// Verifica si puede bajar mas
		// No puede bajar mas
		result = false;
	}

	// Devuelve si puedes o no moverte abajo
	return result;

}
