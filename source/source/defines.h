#ifndef __DEFINES_H__
#define __DEFINES_H__



/*
-------------------------------------------------

	Krystals DS

	Archivo de definiciones

	Requiere DevkitARM
	Requiere NightFox's Lib

	Codigo por Cesar Rincon "NightFox"
	http://www.nightfoxandco.com
	Inicio 08 de Septiembre del 2012

	(c) 2012 - 2013 NightFox & Co.

-------------------------------------------------
*/



/*
Datos del Savegame
*/
#define SAVEGAMEVERSION 20130823		// ID de version del juego
#define MAGIC_STRING "KRYSTALSDS"		// Id del Juego
#define USE_SRAM true					// FLAG de uso de SRAM


/*
Definiciones para los graficos
*/

#define GEM_GFX 0			// Datos para los sprites de las gemas
#define GEM_PAL 0
#define GEM_SPR 47			// Primer sprite del tablero

#define GEM_NEXT_SPR 44		// Primer sprite de la "next gem"
#define GEM_NEXT_X 208		// Coordenadas de la "next gem"
#define GEM_NEXT_Y 32

#define GEM_MOV_SPR 41		// Primer sprite de la gema en movimiento

#define GEM_BLUE 0			// Primer frame de la gema azul
#define GEM_RED 17			// roja
#define GEM_GREEN 34		// verde
#define GEM_YELLOW 51		// amarilla
#define GEM_LILAC 68		// lila
#define GEM_WHITE 85		// blanca
#define GEM_GRAY 96			// gris

#define EMPTY 0				// Define los colores internos del tablero
#define BLUE 1
#define RED 2
#define GREEN 3
#define YELLOW 4
#define LILAC 5
#define WHITE 6
#define GRAY 7

#define GEM_NORMAL 0		// Tipos de gemas
#define GEM_MORPH 1
#define GEM_ERASE 2

#define DELETE_ANIMATION_SPEED 3	// Velociadad en la animacion de los sprites

#define LAYER_GUI 0			// Capa de la GUI (Texto, marcadores)
#define LAYER_FRAME 1		// Capa para el marco del decorado
#define LAYER_BOARD 2		// Capa en la que dibujar el tablero
#define LAYER_BG 3			// Capa del fondo


#define VLINE *(vu16*)0x04000006	// Obtiene via registro la linea actual del dibujado

#define BG_NUMBER 12			// Numero de fondos
#define BGCHANGE_START 0		// Prepara el cambio de fondo
#define BGCHANGE_FADEOUT 1		// Fade out
#define BGCHANGE_CHANGE 2		// Cambia al siguiente fondo
#define BGCHANGE_FADEIN 3		// Fade in
#define BGCHANGE_EXIT 4			// Sal del cambio de fondo


/*
Definiciones de las opciones del menu
*/

#define MMOPT_RUNGAME 0			// Ejecuta el juego
#define MMOPT_RECORDS 1			// Records
#define MMOPT_CREDITS 2			// Creditos

#define MMOPT_FIRST 0			// Primera opcion del menu
#define MMOPT_LAST 2			// Ultima opcion del menu

#define BG_GEM_NUM 24			// Numero de gemas en el fondo del menu

#define MMST_MENU 0				// Menu, estado "Menu principal"
#define MMST_RECORDS 1			// Menu, estado "Records"
#define MMST_CREDITS 2			// Menu, estado "Credits"


/*
Definiciones para el engine
*/

#define RUNID_KERNEL 0			// Ejecucion del Kernel del Juego
#define RUNID_GAMEOVER 1		// Game over
#define RUNID_GAMEDONE 2		// Juego terminado
#define RUNID_ABORT 3			// Partida abortada
#define RUNID_PAUSE 4			// Gestion de la pausa del juego
#define RUNID_WAIT 10			// Nada, mantente a la espera

#define ENGINE_DEFAULT 0			// Ejecucion normal
#define ENGINE_GEMTOBOARD 1			// Integra la gema mobil al tablero
#define ENGINE_CHECKDELETE 2		// Analiza que gemas deben borrarse
#define ENGINE_MAKENEWGEM 3			// Crea una nueva gema
#define ENGINE_DELETEGEMS 4			// Animacion del borrado de las gemas
#define ENGINE_REALLOCATE 5			// Reorganiza el tablero despues del borrado
#define ENGINE_CHECKEXTRADELETE 6	// Verifica si hay que borrar mas gemas
#define ENGINE_GAMEOVER 20			// Game Over
#define ENGINE_GAMEDONE 30			// Game Done

#define GAMEOVER_INIT 0			// Inicializa el game over
#define GAMEOVER_ANIMATION 1	// Rellena de fichas grises el tablero
#define GAMEOVER_MAKEBANNER 2	// Crea el banner de game over
#define GAMEOVER_SHOWBANNER 3	// Muestra el banner de game over
#define GAMEOVER_WAIT 4			// Espera a que se pulse una tecla
#define GAMEOVER_EXIT 5			// Sal del game over

#define GAMEDONE_INIT 0			// Inicializa juego terminado
#define GAMEDONE_MAKEBANNER 1	// Crea el banner de game done
#define GAMEDONE_SHOWBANNER 2	// Muestra el banner de game done
#define GAMEDONE_WAIT 3			// Espera a que se pulse una tecla
#define GAMEDONE_EXIT 4			// Sal del juego terminado

#define ABORT_INIT 0			// Inicializa la cancelacion
#define ABORT_ANIMATION 1		// Rellena de fichas grises el tablero
#define ABORT_MAKEBANNER 2		// Crea el banner de rendicion
#define ABORT_SHOWBANNER 3		// Muestra el banner de rendicion
#define ABORT_WAIT 4			// Espera a que se pulse una tecla
#define ABORT_EXIT 5			// Sal la cancelacion


/*
Definiciones para el control del Score
*/

#define SCORE_COMBO_START 5		// Puntuacion inicial de combo
#define SCORE_LEFT_START 25		// Fichas iniciales a eliminar
#define SCORE_LEFT_ADD 5		// Incremento de fichas a eliminar por nivel
#define SCORE_LAST_LEVEL 25		// Ultimo nivel del juego

#define SCORE_GFX 1				// Sprite RAM SLOT para el GFX de "SCORE"
#define SCORE_SPR 8				// ID de Sprite (2)
#define COMBO_GFX 2				// Sprite RAM SLOT para el GFX de "COMBO"
#define COMBO_SPR 8				// ID de Sprite (2)
#define NUMBERS_GFX 3			// Sprite RAM SLOT para el GFX de "NUMBERS"
#define NUMBERS_SPR 10			// ID de Sprite (8)
#define SCORE_PAL 1				// Paleta para estos sprites
#define SCORE_NUM 6				// Digitos del Score (6)
#define COMBO_NUM 4				// Digitos del Combo (x + digitos)
#define SCORE_BANNER 90			// Tiempo de vida del BANNER


/*
Definiciones para el indicador de PAUSA
*/

#define PAUSE_GFX 4				// Sprite RAM SLOT para el GFX de "SCORE"
#define PAUSE_SPR 7				// ID de Sprite (1)
#define PAUSE_PAL 2				// Paleta
#define PAUSE_PX_TOP 64			// Posiciones TOP
#define PAUSE_PY_TOP 80
#define PAUSE_UP_TOP 32			// Limites TOP
#define PAUSE_LEFT_TOP 32
#define PAUSE_DOWN_TOP 128
#define PAUSE_RIGHT_TOP 96
#define PAUSE_PX_BOTTOM 64		// Posiciones BOTTOM
#define PAUSE_PY_BOTTOM 48
#define PAUSE_UP_BOTTOM 0		// Limites BOTTOM
#define PAUSE_LEFT_BOTTOM 32
#define PAUSE_DOWN_BOTTOM 96
#define PAUSE_RIGHT_BOTTOM 96
#define PAUSE_SPEED 1			// Velocidad inicial


/*
Definiciones para los banners de fin de juego
*/
#define ENDGAME_GFX 5			// Sprite RAM SLOT para el GFX de "SCORE"
#define ENDGAME_SPR 5			// ID de Sprite (2)
#define ENDGAME_PAL 3			// Paleta



/*
Definiciones para el control del juego
*/

#define DROP_MIN_SPEED 30		// Velocidad inicial de caida automatica de la ficha
#define DROP_MAX_SPEED 2		// Velocidad maxima de caida de la ficha


/*
Definiciones para la gestion de sonido
*/

#define BGM_NUMBER 10			// Nº de musicas totales en el SoundBank para el juego
#define SFX_NUMBER 1			// Nº de efectos de sonido en el SoundBank	para el juego
#define VOICE_NUMBER 5			// Nº de voces en el SoundBank para el juego

#define SFX_ID_EXPLOSION 0		// Sonido de la explosion de las fichas

#define VOICE_ID_NICE 0			// Voces durante la partida
#define VOICE_ID_GREAT 1
#define VOICE_ID_FANTASTIC 2
#define VOICE_ID_WONDREFULL 3
#define VOICE_ID_GAMEOVER 4





#endif
