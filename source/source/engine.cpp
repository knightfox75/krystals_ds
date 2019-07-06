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



/*
-------------------------------------------------
	Includes
-------------------------------------------------
*/

// Includes C
#include <stdio.h>
#include <time.h>
// Includes propietarios NDS
#include <nds.h>
// Includes librerias propias
#include <nf_lib.h>
// Includes del juego
#include "includes.h"



/*
Metodos de la clase "CEngine"
*/

// Contructor clase CEngine
CEngine::CEngine(void) {

	/*
	Variables privadas
	*/

	// Inicializa el Run ID
	run_id = RUNID_KERNEL;
	next_run_id = RUNID_KERNEL;

	// Control del thread
	thread = ENGINE_DEFAULT;
	next_thread = ENGINE_DEFAULT;
	delay = 0;

	// Control de pausa
	pause_x[0] = -64, pause_y[0] = -64;
	pause_x[1] = -64, pause_y[1] = -64;
	pause_sx[0] = 0, pause_sy[0] = 0;
	pause_sx[1] = 0, pause_sy[1] = 0;
	pause_up[0] = PAUSE_UP_TOP, pause_down[0] = PAUSE_DOWN_TOP, pause_left[0] = PAUSE_LEFT_TOP, pause_right[0] = PAUSE_RIGHT_TOP;
	pause_up[1] = PAUSE_UP_BOTTOM, pause_down[1] = PAUSE_DOWN_BOTTOM, pause_left[1] = PAUSE_LEFT_BOTTOM, pause_right[1] = PAUSE_RIGHT_BOTTOM;

	// Control de los banners de fin de juego
	banner_x[0] = 0, banner_x[1] = 0;


	/*
	Variables publicas
	*/

	// Inicializa las gem y next_gem
	memset(gem, 0, sizeof(gem));				// Borra el contenido previo
	memset(next_gem, 0, sizeof(next_gem));
	gem_type = GEM_NORMAL;						// Gemas normales por defecto
	next_gem_type = GEM_NORMAL;

	// Posicion inicial de la gema movil
	gem_x = 3, gem_y = -3;
	// Numero total de gemas
	total_gems = 0;

	// Flags
	pause = false;
	abort = false;

	// Escribe el cache en la ram
	DC_FlushRange(gem, sizeof(gem));
	DC_FlushRange(next_gem, sizeof(next_gem));

}



// Destructor clase CEngine
CEngine::~CEngine(void) {
}



// Metodos globlales de la clase CEngine
CEngine* Engine;



/*
Inicializaciones
*/

// Inicializa los datos de la partida
void CEngine::Init(void) {

	setBrightness(3, -16);		// Pantallas en negro
	swiWaitForVBlank();

	System->Reset();			// Borra todos los datos en memoria
	Sound->Reset();
	Sound->GameSfxInfo();		// Carga la info de los SFX del juego
	Load->GameMainFiles();		// Carga los archivos necesarios

	srand(time(NULL));			// Inicializa el random

	// Crea las gemas GEM y NEXT GEM
	MakeNewGem();
	do {
		MakeNewGem();
	} while ((gem_type != GEM_NORMAL) || (next_gem_type != GEM_NORMAL));

	Draw->MakeBgList();		// Genera una lista aleatoria de fondos
	Draw->DrawStage();		// Dibuja el decorado
	Draw->UpdateScreen();	// Actualiza la pantalla

	// Crea la lista de musicas aleatorias
	Sound->MakeBgmList();	// Genera una lista aleatoria de musicas
	Sound->bgm.num = 0;		// Primera cancion a reproducir
	Sound->BgmVolume(0);										// Volumen en silencio
	Sound->bgm.next_track = Sound->bgm.list[Sound->bgm.num];	// MOD a reproducir al inicio (primero de la lista)
	Sound->volume.main = 100;			// Volumen general
	Sound->volume.bgm = 25;				// Volumen de la musica
	Sound->volume.sfx = 100;			// Volumen de los efectos de sonido
	Sound->volume.voice = 100;			// Volumen de las voces

}



/*
Nucleo (Metodo publico)
*/

// Nucleo de ejecucion del juego
void CEngine::Run(void) {

	// Variables
	bool loop = true;


	// Inicializaciones
	Init();


	// Habilita la ejecucion del efecto "modo 7" en el fondo
	irqSet(IRQ_HBLANK, Mode7AnimateBg);
	irqEnable(IRQ_HBLANK);


	/*
	Nucleo de ejecucion
	*/

	while (loop) {

		// Lee los botones
		System->ReadKeys();

		// Control del fade
		if (Draw->fade_in) Draw->FadeIn();
		if (Draw->fade_out) loop = !Draw->FadeOut();

		// Control de la partida
		switch (run_id) {

			// Kernel - Juego en marcha
			case RUNID_KERNEL:
				next_run_id = Kernel();		// Kernel del juego
				break;

			// Game Over
			case RUNID_GAMEOVER:
				next_run_id = GameOver();	// Animacion del Game Over
				break;

			// Juego Completado
			case RUNID_GAMEDONE:
				next_run_id = GameDone();	// Animacion del juego completado
				break;

			// Juego cancelado
			case RUNID_ABORT:
				next_run_id = Abort();		// Animacion del juego cancelado
				break;

			// Pausa
			case RUNID_PAUSE:
				next_run_id = GamePause();	// Gestion de la pausa del juego
				break;

			// Nada
			default:
				break;

		}

		// Verifica si hay que cambiar de Run ID
		if (run_id != next_run_id) run_id = next_run_id;

		// Si se ha lanzado orden de abortar la partida...
		if (abort) {
			thread = ABORT_INIT;
			next_thread = ABORT_INIT;
			run_id = RUNID_ABORT;
			abort = false;
		}

		// Gestion del sonido
		Sound->BgmController();
		Sound->SfxController();
		Sound->VoiceController();
		// Actualiza la pantalla
		Draw->UpdateScreen();

	}


	// Deshabilita la interrupcion
	irqClear(IRQ_HBLANK);
	irqDisable(IRQ_HBLANK);

	// Deten todos los sonidos antes de salir
	Sound->Reset();

}



/*
Metodos adicionales
*/

// Kernel del engine
u8 CEngine::Kernel(void) {

	// Variables
	u8 result = RUNID_KERNEL;

	// Si no estas en espera
	if (delay == 0) {

		// Hilos de ejecuccion
		switch (thread) {

			// Procesos por defecto (mover la gema)
			case ENGINE_DEFAULT:
				// Si el control de la gema, devuelve que no se puede mover hacia abajo..
				if (!Control->MoveGem()) {
					// Verifica si no se ha podido colocar en el tablero (GAME OVER)
					if (gem_y >= 0) {
						next_thread = ENGINE_GEMTOBOARD; 	// Manda la gema al tablero (Integrala)
					} else {
						next_thread = ENGINE_GAMEOVER;		// Game over
					}
				}
				break;


			// Integra la gema en el tablero
			case ENGINE_GEMTOBOARD:
				Board->PutGemOnBoard();				// Integra la gema en el tablero
				delay = 10;							// Retardo
				next_thread = ENGINE_CHECKDELETE;
				break;


			// Verifica si hay que borrar alguna gema
			case ENGINE_CHECKDELETE:
				// Verifica si hay que borrar alguna
				Board->CheckGemsToDelete();
				if (Board->delgems > 0) {
					// Si hay que borrar gemas
					Sound->SfxPlay(SFX_ID_EXPLOSION);	// Sonido
					next_thread = ENGINE_DELETEGEMS;	// Indica el cambio de thread
				} else {
					// Si no hay gemas que borrar
					next_thread = ENGINE_MAKENEWGEM;
				}
				break;


			// Crea una nueva gema
			case ENGINE_MAKENEWGEM:
				MakeNewGem();				// Crea una nueva gema
				Sound->voice.last = -1;		// Reinicia el control de voces
				next_thread = ENGINE_DEFAULT;
				break;


			// Animacion del borrado de las gemas
			case ENGINE_DELETEGEMS:
				// Cuando la animacion no termine...
				if (Draw->EraseAnimation()) {
					// Actualiza el marcador de puntuacion
					Score->CalculateCombo(Board->delgems);
					// Prepara el siguiente thread
					delay = 10;
					next_thread = ENGINE_REALLOCATE;
				}
				break;


			// Reorganiza el tablero despues del borrado
			case ENGINE_REALLOCATE:
				// Reorganiza el tablero
				Board->ReallocateGems();
				next_thread = ENGINE_CHECKEXTRADELETE;
				break;


			// Y verifica si hay que borrar mas fichas
			case ENGINE_CHECKEXTRADELETE:
				// Verifica si hay que borrar alguna
				Board->CheckGemsToDelete();
				if (Board->delgems > 0) {
					// Si hay que borrar gemas
					delay = 10;
					Sound->SfxPlay(SFX_ID_EXPLOSION);	// Sonido
					next_thread = ENGINE_DELETEGEMS;	// Indica el cambio de thread
				} else {
					// Si no hay gemas que borrar
					if (Score->combo > Records->max_combo) Records->max_combo = Score->combo;		// Actualiza el contador de maximo combo de ser necesario
					Score->AddToScore();				// Actualiza el Score
					Score->combo = 0;					// Borra el combo
					if ((Score->level >= SCORE_LAST_LEVEL) && (Score->left == 0)) {
						// Si has alcanzado el ultimo nivel del juego, sal
						Records->game_clear = true;
						next_thread = ENGINE_GAMEDONE;
					} else {
						// Si no, continua
						next_thread = ENGINE_MAKENEWGEM;
					}
				}
				break;


			// Preapara el Game Over
			case ENGINE_GAMEOVER:
				next_thread = GAMEOVER_INIT;	// Prepara el thread ID del Game Over
				result = RUNID_GAMEOVER;
				break;

			// Prepara el Game Done
			case ENGINE_GAMEDONE:
				next_thread = GAMEDONE_INIT;	// Prepara el thread ID del Game Done
				result = RUNID_GAMEDONE;
				break;


			// Por defecto
			default:
				break;

		}

	} else {

		// Contador de espera
		delay --;

	}

	// Actualizacion de thread si es necesario
	if ((thread != next_thread) && (delay == 0)) thread = next_thread;

	// Una vez procesado todo, verifica si tienes que entrar en pausa
	if (Keypress->start || Keypress->lid) {
		// Si no has recibido orden de GAME OVER o GAME COMPLETE, etc y no estas en FADE
		if (result != RUNID_GAMEOVER && !Draw->fade_in) {
			result = RUNID_PAUSE;
		}
	}

	// Devuelve el estado
	return result;

}



// GameOver - Rutinas del Game Over
u8 CEngine::GameOver(void) {

	// Variables
	u8 result = RUNID_GAMEOVER;
	s32 speed = 0;
	s32 temp_x = 0;

	// Si no estas en espera
	if (delay == 0) {

		// Hilos de ejecuccion
		switch (thread) {

			// Inicializa el Game Over
			case GAMEOVER_INIT:
				// Coloca la gema en el tablero
				Board->PutGemOnBoard();
				// Inicializa la animacion de game over
				Board->row = 19;
				Board->delay = 0;
				// Incia el fade out de la musica
				Sound->bgm.stop = true;
				// Siguiente thread
				next_thread = GAMEOVER_ANIMATION;
				break;

			// Animacion del Game Over
			case GAMEOVER_ANIMATION:
				if (Board->FillBoardWithGray()) {
					delay = 15;
					next_thread = GAMEOVER_MAKEBANNER;
				}
				break;

			// Prepara los Sprites de los banners
			case GAMEOVER_MAKEBANNER:
				// Transfiere los graficos y paletas
				NF_VramSpriteGfx(0, ENDGAME_GFX, ENDGAME_SPR, true);
				NF_VramSpriteGfx(0, (ENDGAME_GFX + 1), (ENDGAME_SPR + 1), true);
				NF_VramSpriteGfx(1, ENDGAME_GFX, ENDGAME_SPR, true);
				NF_VramSpriteGfx(1, (ENDGAME_GFX + 1), (ENDGAME_SPR + 1), true);
				NF_VramSpritePal(0, ENDGAME_PAL, ENDGAME_PAL);
				NF_VramSpritePal(1, ENDGAME_PAL, ENDGAME_PAL);
				// Crea los Sprites de los banners
				NF_CreateSprite(0, ENDGAME_SPR, ENDGAME_SPR, ENDGAME_PAL, -64, 95);
				NF_CreateSprite(0, (ENDGAME_SPR + 1), (ENDGAME_SPR + 1), ENDGAME_PAL, -64, 95);
				NF_SpriteLayer(0, ENDGAME_SPR, LAYER_BOARD);
				NF_SpriteLayer(0, (ENDGAME_SPR + 1), LAYER_BOARD);
				NF_SpriteFrame(0, ENDGAME_SPR, 0);
				NF_SpriteFrame(0, (ENDGAME_SPR + 1), 0);
				NF_CreateSprite(1, ENDGAME_SPR, ENDGAME_SPR, ENDGAME_PAL, 256, 63);
				NF_CreateSprite(1, (ENDGAME_SPR + 1), (ENDGAME_SPR + 1), ENDGAME_PAL, 256, 63);
				NF_SpriteLayer(1, ENDGAME_SPR, LAYER_BOARD);
				NF_SpriteLayer(1, (ENDGAME_SPR + 1), LAYER_BOARD);
				NF_SpriteFrame(1, ENDGAME_SPR, 1);
				NF_SpriteFrame(1, (ENDGAME_SPR + 1), 1);
				// Posicion inicial de los banners
				banner_x[0] = (112 << 8);
				banner_x[1] = (112 << 8);
				// Siguiente thread
				next_thread = GAMEOVER_SHOWBANNER;
				break;

			// Animacion de los banners
			case GAMEOVER_SHOWBANNER:
				if (banner_x[0] > 0) {
					// Banner superior
					speed = (banner_x[0] >> 3);
					if (speed < 256) speed = 256;
					banner_x[0] -= speed;
					temp_x = (31 - (banner_x[0] >> 8));
					if (temp_x < -64) temp_x = -64;
					NF_MoveSprite(0, ENDGAME_SPR, temp_x, 95);
					temp_x = (95 - (banner_x[0] >> 8));
					if (temp_x < -64) temp_x = -64;
					NF_MoveSprite(0, (ENDGAME_SPR + 1), temp_x, 95);
				} else if (banner_x[1] > 0) {
					// Banner inferior
					speed = (banner_x[1] >> 3);
					if (speed < 256) speed = 256;
					banner_x[1] -= speed;
					temp_x = (31 + (banner_x[1] >> 8));
					if (temp_x < -64) temp_x = -64;
					NF_MoveSprite(1, ENDGAME_SPR, temp_x, 63);
					temp_x = (95 + (banner_x[1] >> 8));
					if (temp_x < -64) temp_x = -64;
					NF_MoveSprite(1, (ENDGAME_SPR + 1), temp_x, 63);
				} else {
					// Salida
					next_thread = GAMEOVER_WAIT;
				}
				break;

			// Espera a que se pulse una tecla
			case GAMEOVER_WAIT:
				if (Keypress->a || Keypress->b || Keypress->start) next_thread = GAMEOVER_EXIT;
				break;

			// Prepara la salida del Game Over
			case GAMEOVER_EXIT:
				// Prepara el Fade Out
				Draw->fade_out = true;
				// Y deja el engine en espera
				result = RUNID_WAIT;
				break;

			// Por defecto
			default:
				break;

		}

	} else {

		// Contador de espera
		delay --;

	}

	// Actualizacion de thread si es necesario
	if ((thread != next_thread) && (delay == 0)) thread = next_thread;

	// Devuelve el estado
	return result;

}


// GameOver - Rutinas del juego terminado
u8 CEngine::GameDone(void) {

	// Variables
	u8 result = RUNID_GAMEDONE;
	s32 speed = 0;
	s32 temp_x = 0;

	// Si no estas en espera
	if (delay == 0) {

		// Hilos de ejecuccion
		switch (thread) {

			// Inicializa el Game Done
			case GAMEDONE_INIT:
				// Coloca la gema en el tablero
				Board->PutGemOnBoard();
				// Incia el fade out de la musica
				Sound->bgm.stop = true;
				// Tiempo de espera a mostrar el banner
				delay = 60;
				// Siguiente thread
				next_thread = GAMEDONE_MAKEBANNER;
				break;

			// Prepara los Sprites de los banners
			case GAMEDONE_MAKEBANNER:
				// Transfiere los graficos y paletas
				NF_VramSpriteGfx(0, ENDGAME_GFX, ENDGAME_SPR, true);
				NF_VramSpriteGfx(0, (ENDGAME_GFX + 1), (ENDGAME_SPR + 1), true);
				NF_VramSpriteGfx(1, ENDGAME_GFX, ENDGAME_SPR, true);
				NF_VramSpriteGfx(1, (ENDGAME_GFX + 1), (ENDGAME_SPR + 1), true);
				NF_VramSpritePal(0, ENDGAME_PAL, ENDGAME_PAL);
				NF_VramSpritePal(1, ENDGAME_PAL, ENDGAME_PAL);
				// Crea los Sprites de los banners
				NF_CreateSprite(0, ENDGAME_SPR, ENDGAME_SPR, ENDGAME_PAL, -64, 95);
				NF_CreateSprite(0, (ENDGAME_SPR + 1), (ENDGAME_SPR + 1), ENDGAME_PAL, -64, 95);
				NF_SpriteLayer(0, ENDGAME_SPR, LAYER_BOARD);
				NF_SpriteLayer(0, (ENDGAME_SPR + 1), LAYER_BOARD);
				NF_SpriteFrame(0, ENDGAME_SPR, 0);
				NF_SpriteFrame(0, (ENDGAME_SPR + 1), 0);
				NF_CreateSprite(1, ENDGAME_SPR, ENDGAME_SPR, ENDGAME_PAL, 256, 63);
				NF_CreateSprite(1, (ENDGAME_SPR + 1), (ENDGAME_SPR + 1), ENDGAME_PAL, 256, 63);
				NF_SpriteLayer(1, ENDGAME_SPR, LAYER_BOARD);
				NF_SpriteLayer(1, (ENDGAME_SPR + 1), LAYER_BOARD);
				NF_SpriteFrame(1, ENDGAME_SPR, 2);
				NF_SpriteFrame(1, (ENDGAME_SPR + 1), 2);
				// Posicion inicial de los banners
				banner_x[0] = (112 << 8);
				banner_x[1] = (112 << 8);
				// Siguiente thread
				next_thread = GAMEDONE_SHOWBANNER;
				break;

			// Animacion de los banners
			case GAMEDONE_SHOWBANNER:
				if (banner_x[0] > 0) {
					// Banner superior
					speed = (banner_x[0] >> 3);
					if (speed < 256) speed = 256;
					banner_x[0] -= speed;
					temp_x = (31 - (banner_x[0] >> 8));
					if (temp_x < -64) temp_x = -64;
					NF_MoveSprite(0, ENDGAME_SPR, temp_x, 95);
					temp_x = (95 - (banner_x[0] >> 8));
					if (temp_x < -64) temp_x = -64;
					NF_MoveSprite(0, (ENDGAME_SPR + 1), temp_x, 95);
				} else if (banner_x[1] > 0) {
					// Banner inferior
					speed = (banner_x[1] >> 3);
					if (speed < 256) speed = 256;
					banner_x[1] -= speed;
					temp_x = (31 + (banner_x[1] >> 8));
					if (temp_x < -64) temp_x = -64;
					NF_MoveSprite(1, ENDGAME_SPR, temp_x, 63);
					temp_x = (95 + (banner_x[1] >> 8));
					if (temp_x < -64) temp_x = -64;
					NF_MoveSprite(1, (ENDGAME_SPR + 1), temp_x, 63);
				} else {
					// Salida
					next_thread = GAMEDONE_WAIT;
				}
				break;

			// Espera a que se pulse una tecla
			case GAMEDONE_WAIT:
				if (Keypress->a || Keypress->b || Keypress->start) next_thread = GAMEDONE_EXIT;
				break;

			// Prepara la salida del Game Over
			case GAMEDONE_EXIT:
				// Prepara el Fade Out
				Draw->fade_out = true;
				// Y deja el engine en espera
				result = RUNID_WAIT;
				break;

			// Por defecto
			default:
				break;

		}

	} else {

		// Contador de espera
		delay --;

	}

	// Actualizacion de thread si es necesario
	if ((thread != next_thread) && (delay == 0)) thread = next_thread;

	// Devuelve el estado
	return result;

}



// Abort - Rutinas para abortar la partida
u8 CEngine::Abort(void) {

	// Variables
	u8 result = RUNID_ABORT;
	s32 speed = 0;
	s32 temp_x = 0;

	// Si no estas en espera
	if (delay == 0) {

		// Hilos de ejecuccion
		switch (thread) {

			// Inicializa la cancelacion
			case ABORT_INIT:
				// Coloca la gema en el tablero
				Board->PutGemOnBoard();
				// Inicializa la animacion de cancelacion
				Board->row = 19;
				Board->delay = 0;
				// Incia el fade out de la musica
				Sound->bgm.stop = true;
				// Siguiente thread
				next_thread = ABORT_ANIMATION;
				break;

			// Animacion de la cancelacion
			case ABORT_ANIMATION:
				if (Board->FillBoardWithGray()) {
					delay = 15;
					next_thread = ABORT_MAKEBANNER;
				}
				break;

			// Prepara los Sprites de los banners
			case ABORT_MAKEBANNER:
				// Transfiere los graficos y paletas
				NF_VramSpriteGfx(0, ENDGAME_GFX, ENDGAME_SPR, true);
				NF_VramSpriteGfx(0, (ENDGAME_GFX + 1), (ENDGAME_SPR + 1), true);
				NF_VramSpriteGfx(1, ENDGAME_GFX, ENDGAME_SPR, true);
				NF_VramSpriteGfx(1, (ENDGAME_GFX + 1), (ENDGAME_SPR + 1), true);
				NF_VramSpritePal(0, ENDGAME_PAL, ENDGAME_PAL);
				NF_VramSpritePal(1, ENDGAME_PAL, ENDGAME_PAL);
				// Crea los Sprites de los banners
				NF_CreateSprite(0, ENDGAME_SPR, ENDGAME_SPR, ENDGAME_PAL, -64, 95);
				NF_CreateSprite(0, (ENDGAME_SPR + 1), (ENDGAME_SPR + 1), ENDGAME_PAL, -64, 95);
				NF_SpriteLayer(0, ENDGAME_SPR, LAYER_BOARD);
				NF_SpriteLayer(0, (ENDGAME_SPR + 1), LAYER_BOARD);
				NF_SpriteFrame(0, ENDGAME_SPR, 3);
				NF_SpriteFrame(0, (ENDGAME_SPR + 1), 3);
				NF_CreateSprite(1, ENDGAME_SPR, ENDGAME_SPR, ENDGAME_PAL, 256, 63);
				NF_CreateSprite(1, (ENDGAME_SPR + 1), (ENDGAME_SPR + 1), ENDGAME_PAL, 256, 63);
				NF_SpriteLayer(1, ENDGAME_SPR, LAYER_BOARD);
				NF_SpriteLayer(1, (ENDGAME_SPR + 1), LAYER_BOARD);
				NF_SpriteFrame(1, ENDGAME_SPR, 4);
				NF_SpriteFrame(1, (ENDGAME_SPR + 1), 4);
				// Posicion inicial de los banners
				banner_x[0] = (112 << 8);
				banner_x[1] = (112 << 8);
				// Siguiente thread
				next_thread = ABORT_SHOWBANNER;
				break;

			// Animacion de los banners
			case ABORT_SHOWBANNER:
				if (banner_x[0] > 0) {
					// Banner superior
					speed = (banner_x[0] >> 3);
					if (speed < 256) speed = 256;
					banner_x[0] -= speed;
					temp_x = (31 - (banner_x[0] >> 8));
					if (temp_x < -64) temp_x = -64;
					NF_MoveSprite(0, ENDGAME_SPR, temp_x, 95);
					temp_x = (95 - (banner_x[0] >> 8));
					if (temp_x < -64) temp_x = -64;
					NF_MoveSprite(0, (ENDGAME_SPR + 1), temp_x, 95);
				} else if (banner_x[1] > 0) {
					// Banner inferior
					speed = (banner_x[1] >> 3);
					if (speed < 256) speed = 256;
					banner_x[1] -= speed;
					temp_x = (31 + (banner_x[1] >> 8));
					if (temp_x < -64) temp_x = -64;
					NF_MoveSprite(1, ENDGAME_SPR, temp_x, 63);
					temp_x = (95 + (banner_x[1] >> 8));
					if (temp_x < -64) temp_x = -64;
					NF_MoveSprite(1, (ENDGAME_SPR + 1), temp_x, 63);
				} else {
					// Salida
					next_thread = ABORT_WAIT;
				}
				break;

			// Espera a que se pulse una tecla
			case ABORT_WAIT:
				if (Keypress->a || Keypress->b || Keypress->start) next_thread = ABORT_EXIT;
				break;

			// Prepara la salida la cancelacion
			case ABORT_EXIT:
				// Prepara el Fade Out
				Draw->fade_out = true;
				// Y deja el engine en espera
				result = RUNID_WAIT;
				break;

			// Por defecto
			default:
				break;

		}

	} else {

		// Contador de espera
		delay --;

	}

	// Actualizacion de thread si es necesario
	if ((thread != next_thread) && (delay == 0)) thread = next_thread;

	// Devuelve el estado
	return result;

}



// Gestion de la pausa del juego
u8 CEngine::GamePause(void) {

	// Variables
	u8 result = RUNID_PAUSE;		// Devuelve estado pausa por defecto
	u8 n = 0;

	// Si no estas en pausa...
	if (!pause) {
		// Codigo para mostrar la GUI de pausa
		pause_x[0] = PAUSE_PX_TOP, pause_y[0] = PAUSE_PY_TOP;		// Reinicializa el indicador de pausa
		pause_x[1] = PAUSE_PX_BOTTOM, pause_y[1] = PAUSE_PY_BOTTOM;
		if (((int)(rand() % 100)) > 50) {							// Velocidades iniciales
			pause_sx[0] = PAUSE_SPEED;
		} else {
			pause_sx[0] = -PAUSE_SPEED;
		}
		if (((int)(rand() % 100)) > 50) {
			pause_sy[0] = PAUSE_SPEED;
		} else {
			pause_sy[0] = -PAUSE_SPEED;
		}
		pause_sx[1] = -pause_sx[0];
		pause_sy[1] = -pause_sy[0];
		for (n = 0; n < 2; n ++) {
			NF_MoveSprite(n, PAUSE_SPR, pause_x[n], pause_y[n]);
			NF_ShowSprite(n, PAUSE_SPR, true);
		}
		// Pausa la musica
		Sound->BgmPause();
		// Flag de pausa arriba
		pause = true;
	}

	// Si se pulsa "START"
	if (Keypress->start || Keyup->lid) {
		// Codigo para ocultar la GUI de pausa
		for (n = 0; n < 2; n ++) {
			pause_x[n] = -64, pause_y[n] = -64;
			NF_MoveSprite(n, PAUSE_SPR, pause_x[n], pause_y[n]);
			NF_ShowSprite(n, PAUSE_SPR, false);
		}
		// Reanuda la musica
		Sound->BgmResume();
		// Flag de pausa abajo
		pause = false;
		// Indica que la ejecucion del juego debe continuar con normalidad
		result = RUNID_KERNEL;
	}

	// Si esta en pausa, mueve los indicadores
	if (pause) {
		for (n = 0; n < 2; n ++) {
			// Calculo X
			pause_x[n] += pause_sx[n];
			if (pause_x[n] > pause_right[n]) {
				pause_x[n] = pause_right[n];
				pause_sx[n] = -pause_sx[n];
			}
			if (pause_x[n] < pause_left[n]) {
				pause_x[n] = pause_left[n];
				pause_sx[n] = -pause_sx[n];
			}
			// Calculo Y
			pause_y[n] += pause_sy[n];
			if (pause_y[n] > pause_down[n]) {
				pause_y[n] = pause_down[n];
				pause_sy[n] = -pause_sy[n];
			}
			if (pause_y[n] < pause_up[n]) {
				pause_y[n] = pause_up[n];
				pause_sy[n] = -pause_sy[n];
			}
			// Mueve el Sprite
			NF_MoveSprite(n, PAUSE_SPR, pause_x[n], pause_y[n]);
		}
	}

	// Devuelve el resultado
	return result;

}



// Genera una nueva gema
void CEngine::MakeNewGem(void) {

	// Variables
	u8 n = 0, i = 0;			// Control de bucles
	u16 random = 0;				// Almacena el random
	bool loop = false;			// Control de repeticion
	u8 special = GEM_NORMAL;	// Tipo de gema

	// Copia la gema NEXT a la gema que se usara ahora (siguiente drop)
	memcpy(gem, next_gem, sizeof(next_gem));
	DC_FlushRange(gem, sizeof(gem));
	gem_type = next_gem_type;

	// Calcula si la gema NEXT sera especial (si la anterior ha sido normal)
	if (gem_type == GEM_NORMAL) {
		random = (int)(rand() % 1000);
		if (random > 900) special = GEM_MORPH;		// Gema morph 10%
		if (random < 20) special = GEM_ERASE;		// Gema blanca 2%
	}

	// Genera una nueva gema NEXT
	switch (special) {
		
		// Si es una ficha borradora..
		case GEM_ERASE:
			for (n = 0; n < 3; n ++) next_gem[n] = WHITE;
			break;

		// Si es una ficha morph
		case GEM_MORPH:
			for (n = 0; n < 3; n ++) {
				loop = true;
				// Repite mientras las gemas sean iguales entre ellas
				while (loop) {
					// Genera una nueva gema
					next_gem[n] = (((int)(rand() % 5)) + 1);
					loop = false;
					// Verifica que no hay ninguna repetida
					for (i = 0; i < n; i ++) {
						// Si la hay, repite la creacion de la ultima gema
						if (next_gem[i] == next_gem[n]) loop = true;
					}
				}
			}
			break;

		// Si es una ficha normal
		case GEM_NORMAL:
			for (n = 0; n < 3; n ++) {
				loop = true;
				// Repite mientras las gemas sean iguales entre ellas
				while (loop) {
					// Genera una nueva gema
					next_gem[n] = (((int)(rand() % 5)) + 1);
					loop = false;
					// Verifica que no hay ninguna repetida
					for (i = 0; i < n; i ++) {
						// Si la hay, repite la creacion de la ultima gema
						if (next_gem[i] == next_gem[n]) loop = true;
					}
				}
			}
			break;

		// Error trap
		default:
			break;

	}

	// Guarda que tipo de gema es la "next"
	next_gem_type = special;

	// Marca para actualizar
	Draw->update.my_gem = true;
	Draw->update.next_gem = true;

}



// Cuenta la gemas eliminadas, para saber si hay que cambiar de nivel
void CEngine::CountDeletedGems(u16 gems) {

	// Descuenta las fichas eliminadas del total para avanzar de nivel y indica que el marcador debe actualizarse
	Score->left -= gems;
	Score->update.left = true;

	// Actualiza el contador de gemas en pantalla
	total_gems -= gems;
	if (total_gems < 0) total_gems = 0;

	// Si has alcanzado las fichas necesarias, realiza el cambio de nivel
	if (Score->left <= 0) {
		// Si no has alcanzado el nivel SCORE_LAST_LEVEL...
		if (Score->level < SCORE_LAST_LEVEL) {
			// Avance de nivel
			Score->level ++;
			Score->update.level = true;
			// Nuevo valor de fichas a eliminar
			Score->left = SCORE_LEFT_START + (SCORE_LEFT_ADD * (Score->level - 1));
			// Nuevo valor de puntos x combo
			Score->combo_score = (SCORE_COMBO_START + ((Score->level - 1) << 1));
			// Nueva velocidad de caida
			if (Score->level < 6) {
				Control->drop.delay -= 2;
			} else {
				Control->drop.delay --;
			}
			if (Control->drop.delay < DROP_MAX_SPEED) Control->drop.delay = DROP_MAX_SPEED;
			// Prepara el cambio de fondo
			Draw->background.update = true;
		} else {
			// Indica que se han teminado las fichas, juego completado
			Score->left = 0;
		}
	}

}
