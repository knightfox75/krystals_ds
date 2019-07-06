/*
-------------------------------------------------

	Krystals DS

	Archivo de gestion del tablero del juego

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
Metodos de la clase "CBoard"
*/

// Contructor clase CBoard
CBoard::CBoard(void) {

	/*
	Inicializa las variables privadas
	*/

	delete_gems = false;


	/*
	Inicializa las variables publicas
	*/

	// Borra el tablero
	memset(board, EMPTY, sizeof(board));
	memset(erase, EMPTY, sizeof(erase));
	delgems = 0;

	// Borra los arrays de control
	memset((void*)&delete_gem, 0, sizeof(delete_gem));
	row = 0;
	delay = 0;

	// Manda todo el cache a la ram
	DC_FlushRange(board, sizeof(board));
	DC_FlushRange(erase, sizeof(erase));
	DC_FlushRange((void*)&delete_gem, sizeof(delete_gem));

}



// Destructor clase CBoard
CBoard::~CBoard(void) {
}



// Metodos globlales de la clase CBoard
CBoard* Board;



// Integra la gema en el tablero
void CBoard::PutGemOnBoard(void) {

	// Variables
	u8 n = 0;
	bool done = true;
	s8 y;

	// Integra la gemma en el tablero
	for (n = 0; n < 3; n ++) {
		y = (Engine->gem_y + n);
		if (y >= 0) {
			board[y][Engine->gem_x] = Engine->gem[n];
		} else {
			done = false;
		}
	}


	// Si has podido integrar todas las gemas en el tablero (no es game over)
	if (done) {

		// Si es alguna ficha especial...
		switch (Engine->gem_type) {

			// Morph
			case GEM_MORPH:
				if (Engine->gem_y < 17) {
					// Morph de la todas las gemas del color tocado
					MorphBoardGems(board[(Engine->gem_y + 3)][Engine->gem_x], Engine->gem[2], false);
				} else {
					// Ha tocado el suelo, morph random de todas las gemas
					MorphBoardGems(0, 0, true);
				}
				break;

			// Erase
			case GEM_ERASE:
				if (Engine->gem_y < 17) {
					// Borra todas las gemas del color tocado
					EraseBoardGems(board[(Engine->gem_y + 3)][Engine->gem_x], false);
				} else {
					// Ha tocado el suelo, borra todas las gemas
					EraseBoardGems(0, true);
				}
				break;

			default:
				// Nada
				break;

		}

	}


	// Reinicia la posicion de la gema
	Engine->gem_x = 3, Engine->gem_y = -3;

	// Añade 3 gemas al contador de gemas totales
	Engine->total_gems += 3;

	// Marca los flags
	Draw->update.board = true;
	Draw->update.gem_pos = true;

}



// Morph de las gemas del tablero
void CBoard::MorphBoardGems(u8 src, u8 dst, bool all) {

	// Variables
	s8 x, y;

	// Cambia todas las gemas por el color indicado
	for (y = 0; y < 20; y ++) {
		for (x = 0; x < 8; x ++) {
			if (all) {
				// Todas en random
				if (board[y][x] != EMPTY) board[y][x] = (((int)(rand() % 5)) + 1);
			} else {
				// Solo las indicadas
				if (board[y][x] == src) board[y][x] = dst;
			}
		}
	}

}



// Borra las fichas del tablero
void CBoard::EraseBoardGems(u8 dst, bool all) {

	// Variables
	s8 x, y;

	// Cambia todas las gemas del color indicado a blanco (borrado especial)
	for (y = 0; y < 20; y ++) {
		for (x = 0; x < 8; x ++) {
			if (all) {
				// Todas a blanco
				if (board[y][x] != EMPTY) board[y][x] = WHITE;
			} else {
				// Solo las indicadas
				if (board[y][x] == dst) board[y][x] = WHITE;
			}
		}
	}

}



// Busca si hay gemas a eliminar
void CBoard::CheckGemsToDelete(void) {

	// Inicializa el flag
	delete_gems = false;

	// Limpia el array de borrado
	memset(erase, EMPTY, sizeof(erase));
	DC_FlushRange(erase, sizeof(erase));

	// Busca si hay gemas a eliminar y marcalas
	CheckRows();
	CheckColumns();
	CheckDiagonalRight();
	CheckDiagonalLeft();
	CheckWhiteGems();

	// Crea la lista de sprites a animar al borrarlas
	if (delete_gems) {
		delgems = CreateDeleteList();
	} else {
		delgems = 0;
	}

}



// Marca las gemas a borrar al completar la ronda
void CBoard::CheckRows(void) {

	// Variables
	u8 x, y, n;
	u8 idx;
	u8 start;
	u8 count;

	// Busca horizontales (3 o mas)
	for (y = 0; y < 20; y ++) {
		// Reinicia variables
		start = 0, count = 1;
		for (x = 0; x < 8; x ++) {
			// Calculo de indices a analizar
			idx = (x + 1);
			// Si estas dentro del tablero
			if (idx < 8) { 
				// Si hay dos gemas contiguas iguales
				if (
					(board[y][x] == board[y][idx])
					&&
					(board[y][x] != EMPTY)
					&&
					(board[y][idx] != EMPTY)
					) {
					// Marca el inicio
					if (count == 1) start = x;
					// Suma 1 acierto
					count ++;
				} else {
					// Si no habia 3 de consecutivas
					if (count < 3) {
						// Reinicia el contador
						count = 1, start = 0;
					} else {
						// Marca las gemas a eliminar
						delete_gems = true;
						for (n = 0; n < count; n ++) erase[y][(start + n)] = 1;
						// Reinicia el proceso de busqueda
						x = (start + count);
						count = 1, start = 0;
					}
				}
			}
		}	// Final del for (x...
		// Si al salir, habia 3 o mas aciertos
		if (count >= 3) {
			// Marca las gemas a eliminar
			delete_gems = true;
			for (n = 0; n < count; n ++) erase[y][(start + n)] = 1;
		}
	}	// Final del for (y...

}



// Marca las gemas a borrar al completar la ronda (columnas)
void CBoard::CheckColumns(void) {

	// Variables
	u8 x, y, n;
	u8 idy;
	u8 start;
	u8 count;

	// Busca verticales (3 o mas)
	for (x = 0; x < 8; x ++) {
		// Reinicia variables
		start = 0, count = 1;
		for (y = 0; y < 20; y ++) {
			// Calculo de indices a analizar
			idy = (y + 1);
			// Si estas dentro del tablero
			if (idy < 20) { 
				// Si hay dos gemas contiguas iguales
				if (
					(board[y][x] == board[idy][x])
					&&
					(board[y][x] != EMPTY)
					&&
					(board[idy][x] != EMPTY)
					) {
					// Marca el inicio
					if (count == 1) start = y;
					// Suma 1 acierto
					count ++;
				} else {
					// Si no habia 3 de consecutivas
					if (count < 3) {
						// Reinicia el contador
						count = 1, start = 0;
					} else {
						// Marca las gemas a eliminar
						delete_gems = true;
						for (n = 0; n < count; n ++) erase[(start + n)][x] = 1;
						// Reinicia el proceso de busqueda
						y = (start + count);
						count = 1, start = 0;
					}
				}
			}
		}	// Final del for (y... 
		// Si al salir, habia 3 o mas aciertos
		if (count >= 3) {
			// Marca las gemas a eliminar
			delete_gems = true;
			for (n = 0; n < count; n ++) erase[(start + n)][x] = 1;
		}
	}	// Final del for (x...

}



// Marca las gemas a borrar (diagonal a derecha)
void CBoard::CheckDiagonalRight(void) {

	// Variables
	u8 x, y, n;
	u8 px, py;
	u8 idx, idy;
	u8 start_x, start_y;
	u8 count;

	// Busca diagonales (3 o mas)
	for (y = 0; y < 20; y ++) {
		for (x = 0; x < 8; x ++) {
			// Punto inicial de examen
			px = x, py = y;
			// Reinicia variables
			count = 1;
			start_x = 0, start_y = 0;
			// Calculo inicial de los indices a analizar
			idx = (px + 1), idy = (py + 1);
			// Si estas dentro del tablero
			while ((idx < 8) && (idy < 20)) { 
				// Si hay dos gemas contiguas iguales
				if (
					(board[py][px] == board[idy][idx])
					&&
					(board[py][px] != EMPTY)
					&&
					(board[idy][idx] != EMPTY)
					) {
					// Marca el inicio
					if (count == 1) {
						start_x = px, start_y = py;
					}
					// Suma 1 acierto
					count ++;
				} else {
					// Si no habia 3 de consecutivas
					if (count < 3) {
						// Reinicia el contador
						count = 1;
						start_x = 0, start_y = 0;
					} else {
						// Marca las gemas a eliminar
						delete_gems = true;
						for (n = 0; n < count; n ++) erase[(start_y + n)][(start_x + n)] = 1;
						// Reinicia el proceso de busqueda
						px = (start_x + count);
						py = (start_y + count);
						start_x = 0, start_y = 0;
						count = 1;
					}
				}
				// Siguiente posicion a examinar en el array
				px ++, py ++;
				idx = (px + 1), idy = (py + 1);
			} // Final del While
			// Si al salir, habia 3 o mas aciertos
			if (count >= 3) {
				// Marca las gemas a eliminar
				delete_gems = true;
				for (n = 0; n < count; n ++) erase[(start_y + n)][(start_x + n)] = 1;
			}
		}	// Final del for (x...
	}	// Final del for (y...

}



// Marca las gemas a borrar (diagonal a izquierda)
void CBoard::CheckDiagonalLeft(void) {

	// Variables
	u8 x, y, n;
	s8 px;
	u8 py;
	s8 idx;
	u8 idy;
	s8 start_x;
	u8 start_y;
	u8 count;

	// Busca diagonales (3 o mas)
	for (y = 0; y < 20; y ++) {
		for (x = 0; x < 8; x ++) {
			// Punto inicial de examen
			px = x, py = y;
			// Reinicia variables
			count = 1;
			start_x = 0, start_y = 0;
			// Calculo inicial de los indices a analizar
			idx = (px - 1), idy = (py + 1);
			// Si estas dentro del tablero
			while ((idx > -1) && (idy < 20)) { 
				// Si hay dos gemas contiguas iguales
				if (
					(board[py][px] == board[idy][idx])
					&&
					(board[py][px] != EMPTY)
					&&
					(board[idy][idx] != EMPTY)
					) {
					// Marca el inicio
					if (count == 1) {
						start_x = px, start_y = py;
					}
					// Suma 1 acierto
					count ++;
				} else {
					// Si no habia 3 de consecutivas
					if (count < 3) {
						// Reinicia el contador
						count = 1;
						start_x = 0, start_y = 0;
					} else {
						// Marca las gemas a eliminar
						delete_gems = true;
						for (n = 0; n < count; n ++) erase[(start_y + n)][(start_x - n)] = 1;
						// Reinicia el proceso de busqueda
						px = (start_x - count);
						py = (start_y + count);
						start_x = 0, start_y = 0;
						count = 1;
					}
				}
				// Siguiente posicion a examinar en el array
				px --, py ++;
				idx = (px - 1), idy = (py + 1);
			} // Final del While
			// Si al salir, habia 3 o mas aciertos
			if (count >= 3) {
				// Marca las gemas a eliminar
				delete_gems = true;
				for (n = 0; n < count; n ++) erase[(start_y + n)][(start_x - n)] = 1;
			}
		}	// Final del for (x...
	}	// Final del for (y...

}



// Marca las gemas blancas para su borrado
void CBoard::CheckWhiteGems(void) {

	// Variables
	s8 x, y;

	// Marca las gemas
	for (y = 0; y < 20; y ++) {
		for (x = 0; x < 8; x ++) {
			if (board[y][x] == WHITE) {
				erase[y][x] = 1;
				delete_gems = true;
			}
		}
	}

}



// Crea la lista de Sprites a animar antes del borrado
u16 CBoard::CreateDeleteList(void) {

	// Variables
	u8 scr, board_x, board_y, spr;
	u32 idy;
	u16 gems = 0;

	// Borra el array de control
	memset((void*)&delete_gem, 0, sizeof(delete_gem));
	DC_FlushRange((void*)&delete_gem, sizeof(delete_gem));

	// Analiza ambas pantallas
	for (scr = 0; scr < 2; scr ++) {
		// Actualiza los frames de los sprites
		spr = GEM_SPR;
		delete_gem[scr].num = 0;
		for (board_y = 0; board_y < 10; board_y ++) {
			for (board_x = 0; board_x < 8; board_x ++) {
				// Calcula los indices
				idy = (board_y + (scr * 10));
				// Si esta marcada para borrar...
				if (erase[idy][board_x] == 1) {
					delete_gem[scr].spr[delete_gem[scr].num] = spr;
					delete_gem[scr].frame[delete_gem[scr].num] = Draw->GetGemColorFrame(board[idy][board_x]);
					delete_gem[scr].num ++;
					gems ++;
				}
				// Siguiente sprite
				spr ++;
			}
		}
	}

	// Inicializa la animacion de borrado
	Draw->erase_frame = 0;
	Draw->erase_delay = 0;

	// Devuelve el numero de gemas borradas
	return gems;

}



// Recoloca las gemas despues de la eliminacion
void CBoard::ReallocateGems(void) {

	// Variables
	s8 x, y;
	s8 n;
	u8 gems[20];

	// Busca espacios vacios columna a columna
	for (x = 0; x < 8; x ++) {
		// Inicializa la busqueda
		memset(gems, EMPTY, sizeof(gems));
		DC_FlushRange(gems, sizeof(gems));
		n = 19;
		// Busca las gemas validas y guardalas
		for (y = 19; y >= 0; y --) {
			if (board[y][x] != EMPTY) {		// Si hay una ficha, registralo en el buffer
				gems[n] = board[y][x];
				n --;
			}
		}
		// Recoloca las gemas validas en el tablero
		for (y = 0; y < 20; y ++) {
			board[y][x] = gems[y];
		}
	}

	// Marca para actualizar el tablero
	Draw->update.board = true;

}



// Empieza el rellenado del tablero con piezas grises (Animacion Game Over)
bool CBoard::FillBoardWithGray(void) {

	// Variables
	u8 x;
	bool done = false;

	// Control de espera
	delay ++;

	if (delay > 3) {

		// Reseteo
		delay = 0;

		// Rellena la fila con fichas de color gris
		for (x = 0; x < 8; x ++) {
			board[row][x] = GRAY;
		}

		// Marca para actualizar el tablero
		Draw->update.board = true;

		// Siguiente fila
		row --;

		// Si se ha rellenado todo el tablero, indicalo
		if (row < 0) done = true;

	}

	// Devuelve si has terminado
	return done;

}
