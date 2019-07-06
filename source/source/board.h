#ifndef __BOARD_H__
#define __BOARD_H__



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



// Includes
#include <nds.h>



// Clase CBoard
class CBoard {

	// Segmento privado
	private:

		// Variables
		bool delete_gems;	// Marca si debe borrarse alguna gema


		// Morph de las gemas del tablero
		void MorphBoardGems(u8 src, u8 dst, bool all);

		// Pon las fichas "erase" en el tablero
		void EraseBoardGems(u8 dst, bool all);

		// Marca las gemas a borrar (filas)
		void CheckRows(void);

		// Marca las gemas a borrar (columnas)
		void CheckColumns(void);

		// Marca las gemas a borrar (diagonal a derecha)
		void CheckDiagonalRight(void);

		// Marca las gemas a borrar (diagonal a izquierda)
		void CheckDiagonalLeft(void);

		// Marca las gemas blancas para su borrado
		void CheckWhiteGems(void);

		// Crea la lista de Sprites a animar antes del borrado
		u16 CreateDeleteList(void);



	// Segmento publico
	public:

		// Variables
		u8 board[20][8];		// Control del tablero
		u8 erase[20][8];		// Gemas a borrar
		u16 delgems;			// Gemas borradas en la ronda

		// Lista de gemas a eliminar
		struct {
			u8 num;				// Numero de elementos a animar
			u8 spr[80];			// nº de Sprite
			u8 frame[80];		// nº de Frame inicial
		} delete_gem[2];

		s8 row;				// Control de la animacion del game over
		u8 delay;


		// Constructor
		CBoard(void);

		// Destructor
		~CBoard(void);

		// Integra la gema en el tablero
		void PutGemOnBoard(void);

		// Busca si hay gemas a eliminar
		void CheckGemsToDelete(void);

		// Recoloca las gemas despues de la eliminacion
		void ReallocateGems(void);

		// Empieza el rellenado del tablero con piezas grises (Animacion Game Over)
		bool FillBoardWithGray(void);

};
// Metodos globlales de la clase CBoard
extern CBoard* Board;

/*
Metodos de gestion del tablero
*/





#endif
