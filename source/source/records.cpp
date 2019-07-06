/*
-------------------------------------------------

	Krystals DS

	Archivo de gestion de los records

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
Metodos de la clase "CRecords"
*/

// Contructor clase CRecords
CRecords::CRecords(void) {

	// Inicializa las variables
	max_combo = 0;
	game_clear = false;
	new_record = false;
	record_id = 0;
	namepos = 0;
	blink = true;
	delay = 0;

	// Control del fade
	bright = 0;
	fade_in = true;
	fade_out = false;

	// Datos del report
	memset((void*)&score, 0, sizeof(score));
	DC_FlushRange((void*)&score, sizeof(score));

}



// Destructor clase CRecords
CRecords::~CRecords(void) {
}



// Metodos globales de la clase "CRecords"
CRecords* Records;



// Informe de la puntuacion
void CRecords::Report(void) {

	// Variables
	bool loop = true;		// Control de ejecucion

	// Inicializa el menu
	Init();


	// Bucle de ejecucion del resumen de puntuacion
	while (loop) {

		// Lee el teclado
		System->ReadKeys();

		// Efectos Fade IN/OUT
		if (fade_in) FadeIn();
		if (fade_out) loop = !FadeOut();

		// Gestion de la pantalla de records
		if (!fade_in && !fade_out && loop) {
			// Si hay nuevo record...
			if (new_record) {
				// Registra las iniciales
				new_record = !EnterName();
			} else {
				// Si no lo hay, espera a que se pulse una tecla para salir
				if (Keypress->a || Keypress->b || Keypress->start) {
					fade_out = true;
					Sound->bgm.stop = true;
				}
			}
		}

		// Gestion del sonido
		Sound->BgmController();

		swiWaitForVBlank();			// Espera al sincronismo

	}

	// Clean Up del sonido
	Sound->Reset();

}



// Inicializa el resumen de puntacion
void CRecords::Init(void) {

	// Borra todo el contenido de la memoria
	System->Reset();

	// Pantalla en negro
	setBrightness(3, -16);
	swiWaitForVBlank();

	// Carga los archivos necesarios
	Load->RecordsFiles();

	// Genera el report
	Generate();

	// Manda todos los elementos a la pantalla y dejalos listos para el Fade In
	swiWaitForVBlank();			// Espera al sincronismo

	// Prepara el Fade IN
	fade_in = true;

	// E indica la musica que sonara
	Sound->bgm.next_track = MOD_RECORDS;

	// Inicializa los niveles de volumen
	Sound->volume.main = 100;			// Volumen general
	Sound->volume.bgm = 100;			// Volumen de la musica
	Sound->volume.sfx = 100;			// Volumen de los efectos de sonido
	Sound->volume.voice = 100;			// Volumen de las voces

}



// Genera el report
void CRecords::Generate(void) {

	// Variables
	u32 n = 0;
	u32 id = 0;
	bool done = false;
	char text[32];

	struct {
		char name[5];		// Siglas del jugador
		u32 score;			// Puntuacion alcanzada
		u32 level;			// Nivel
		u32 combo;			// Combo
		bool cleared;		// Juego terminado
	} temp[3];
	memset((void*)&temp, 0, sizeof(temp));
	DC_FlushRange((void*)&temp, sizeof(temp));

	// Crea los fondos
	NF_CreateTiledBg(0, 3, "bgtop");
	NF_CreateTiledBg(1, 3, "bgbottom");

	// Calcula los datos del report
	score.base = Score->score;
	score.level = (int)((Score->level * score.base) / 100);
	score.combo = (int)(((max_combo >> 1) * score.base) / 100);
	if (game_clear) {
		score.clear = (int)((25 * score.base) / 100);
	} else {
		score.clear = 0;
	}
	score.total = (score.base + score.level + score.combo);

	// Verifica si hay nuevo record
	if (score.total > Savegame->data[2].score) {
		// Marca que hay nuevo record
		new_record = true;
		// Reorganiza los records
		id = 0;
		done = false;
		for (n = 0; n < 3; n ++) {
			// Si la puntuacion actual es superior a la existente
			if (!done && (score.total > Savegame->data[n].score)) {
				// Inserta en esta posicion el nuevo record
				temp[n].name[0] = 'A', temp[n].name[1] = 'A', temp[n].name[2] = 'A', temp[n].name[3] = '\0';
				temp[n].score = score.total;
				temp[n].level = Score->level;
				temp[n].combo = max_combo;
				temp[n].cleared = game_clear;
				record_id = n;
				done = true;
			} else {
				// Copia los datos actuales
				sprintf(temp[n].name, "%s", Savegame->data[id].name);
				temp[n].score = Savegame->data[id].score;
				temp[n].level = Savegame->data[id].level;
				temp[n].combo = Savegame->data[id].combo;
				temp[n].cleared = Savegame->data[id].cleared;
				id ++;
			}
		}
		// Copia de vuelta los records actualizados a la estructura del savegame
		for (n = 0; n < 3; n ++) {
			sprintf(Savegame->data[n].name, "%s", temp[n].name);
			Savegame->data[n].score = temp[n].score;
			Savegame->data[n].level = temp[n].level;
			Savegame->data[n].combo = temp[n].combo;
			Savegame->data[n].cleared = temp[n].cleared;
		}
	}

	/* Informe de puntuacion */
	// Crea la capa de texto...
	NF_CreateTextLayer(0, 0, 0, "report");
	// Escribe los textos a mostrar
	NF_WriteText16(0, 0, 8, 2, "GAMEPLAY  REPORT");
	sprintf(text, "BASE SCORE      %07d", (int)score.base);
	NF_WriteText16(0, 0, 4, 4, text);
	sprintf(text, "LEVEL BONUS     %07d", (int)score.level);
	NF_WriteText16(0, 0, 4, 5, text);
	sprintf(text, "COMBO BONUS     %07d", (int)score.combo);
	NF_WriteText16(0, 0, 4, 6, text);
	sprintf(text, "CLEAR BONUS     %07d", (int)score.clear);
	NF_WriteText16(0, 0, 4, 7, text);
	sprintf(text, "TOTAL SCORE     %07d", (int)score.total);
	NF_WriteText16(0, 0, 4, 9, text);

	/* Records */
	// Crea la capa de texto...
	NF_CreateTextLayer(1, 0, 0, "records");
	// Escribe los textos a mostrar
	NF_WriteText16(1, 0, 11, 2, "TOP SCORES");
	NF_WriteText16(1, 0, 5, 5, "       SCORE   LV  MAX");
	sprintf(text, "%s   %07d  %02d  %03d", Savegame->data[0].name, (int)Savegame->data[0].score, (int)Savegame->data[0].level, (int)Savegame->data[0].combo);
	NF_WriteText16(1, 0, 5, 7, text);
	sprintf(text, "%s   %07d  %02d  %03d", Savegame->data[1].name, (int)Savegame->data[1].score, (int)Savegame->data[1].level, (int)Savegame->data[1].combo);
	NF_WriteText16(1, 0, 5, 8, text);
	sprintf(text, "%s   %07d  %02d  %03d", Savegame->data[2].name, (int)Savegame->data[2].score, (int)Savegame->data[2].level, (int)Savegame->data[2].combo);
	NF_WriteText16(1, 0, 5, 9, text);
	NF_UpdateTextLayers();

}



// Efecto fade-in
bool CRecords::FadeIn(void) {

	bool done = false;

	bright += 64;
	if (bright >= (16 << 8)) bright = (16 << 8);
	setBrightness(3, ((bright >> 8) - 16));
	if (bright == (16 << 8)) {
		fade_in = false;
		bright = 0;
		done = true;
	}

	return done;

}



// Efecto fade-out
bool CRecords::FadeOut(void) {

	bool done = false;

	bright += 64;
	if (bright >= (16 << 8)) bright = (16 << 8);
	setBrightness(3, -(bright >> 8));
	if (bright == (16 << 8)) {
		fade_out = false;
		bright = 0;
		done = true;
	}

	return done;

}



// Registra un nuevo record
bool CRecords::EnterName(void) {

	// Variables
	char text[5];					// Almacena el texto a mostrar
	u8 pos_y = (7 + record_id);		// Posicion de dibujado del texto
	bool result = false;			// Alamcena si se ha completado la entrada de texto

	// Copia el nombre del record al buffer de muestreo
	sprintf(text, "%s", Savegame->data[record_id].name);

	// Selecciona el caracter a cambiar
	if (Keypress->right && !Keypress->left && !Keypress->a && !Keypress->b) {
		namepos ++;
		delay = 0;
		blink = true;
		if (namepos > 2) namepos = 2;
	}
	if (Keypress->left && !Keypress->right && !Keypress->a && !Keypress->b) {
		namepos --;
		delay = 0;
		blink = true;
		if (namepos < 0) namepos = 0;
	}

	// Avanza al siguiente caracter si pulsas A
	if (Keypress->a && !Keypress->right && !Keypress->left && !Keypress->b) {
		namepos ++;
		delay = 0;
		blink = true;
		// Si era el ultimo caracter, termina la entrada
		if (namepos > 2) {
			namepos = 2;
			result = true;
		}
	}

	// Retrocede un caracter si pulsas B
	if (Keypress->b && !Keypress->left && !Keypress->right && !Keypress->a) {
		namepos --;
		delay = 0;
		blink = true;
		if (namepos < 0) namepos = 0;
	}

	// Si no se esta cambiando de seleccion, cambia la letra seleccionada
	if (!Keypress->right && !Keypress->left && !Keypress->a && !Keypress->b) {
		// Si pulsas arriba...
		if (Keypress->up && !Keypress->down) {
			Savegame->data[record_id].name[namepos] --;
			if (Savegame->data[record_id].name[namepos] < 0x41) Savegame->data[record_id].name[namepos] = 0x5A;
			delay = 0;
			blink = true;
		}
		// Si pulsas abajo...
		if (Keypress->down && !Keypress->up) {
			Savegame->data[record_id].name[namepos] ++;
			if (Savegame->data[record_id].name[namepos] > 0x5A) Savegame->data[record_id].name[namepos] = 0x41;
			delay = 0;
			blink = true;
		}
	}

	// Efecto "Blink" del caracter seleccionado
	delay ++;
	if (delay > 5) {
		delay = 0;
		blink = !blink;
	}

	// Si hay parpadeo
	if (!blink) {
		text[namepos] = ' ';
	}

	// Actualiza el texto en pantalla
	NF_WriteText16(1, 0, 5, pos_y, text);
	NF_UpdateTextLayers();

	// Devuele si ya esta completada la entrada de las iniciales
	if (result) {
		// Guarda el archivo de datos
		Savegame->WriteSavegame();
		// Devuelve el resultado
		return result;
	} else {
		// Devuelve el resultado
		return result;
	}

}