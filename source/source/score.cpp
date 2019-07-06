/*
-------------------------------------------------

	Krystals DS

	Archivo de funciones de control del score

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
Metodos de la clase "CScore"
*/

// Contructor clase CScore
CScore::CScore(void) {

	// Inicializa el score, level, left, combo score, etc
	score = 0;
	level = 1;
	left = SCORE_LEFT_START;
	combo_score = SCORE_COMBO_START;

	// Reinicializa las demas variables
	combo = 0;
	points = 0;
	banner_combo = SCORE_BANNER;

	// Inicializa los flags
	update.score = true;
	update.level = true;
	update.left = true;

}



// Destructor clase CScore
CScore::~CScore(void) {
}



// Metodos globales de la clase CScore
CScore* Score;



// Actualiza el marcador de Score
void CScore::UpdateScore(void) {

	// Variables
	char text[8];

	// Genera el texto a imprimir
	memset(text, 0, sizeof(text));
	DC_FlushRange(text, sizeof(text));
	sprintf(text, "%07d", (int)score);

	// Imprimelo (LAYER_GUI)
	NF_WriteText(1, LAYER_GUI, 23, 4, text);
	Draw->update.text = true;

}



// Actualiza el marcador de Level
void CScore::UpdateLevel(void) {

	// Variables
	char text[3];

	// Genera el texto a imprimir
	memset(text, 0, sizeof(text));
	DC_FlushRange(text, sizeof(text));
	sprintf(text, "%02d", (int)level);

	// Imprimelo (LAYER_GUI)
	NF_WriteText(1, LAYER_GUI, 25, 13, text);
	Draw->update.text = true;

}



// Actualiza el marcador de Left
void CScore::UpdateLeft(void) {

	// Variables
	char text[4];

	// Genera el texto a imprimir
	memset(text, 0, sizeof(text));
	DC_FlushRange(text, sizeof(text));
	sprintf(text, "%03d", (int)left);

	// Imprimelo (LAYER_GUI)
	NF_WriteText(0, LAYER_GUI, 25, 18, text);
	Draw->update.text = true;

}



// Calcula el Score y demas marcadores y eventos en funcion de las fichas eliminadas
void CScore::CalculateCombo(u16 gems) {

	// Calcula el combo actual
	combo += gems;
	points = (combo * combo_score);

	// Habilita los banners
	MakeComboBanner(combo);
	MakeScoreBanner(points);
	// Registra que se ha activado
	banner_combo = SCORE_BANNER;

	// Descuenta las fichas eliminadas del total para avanzar de nivel
	Engine->CountDeletedGems(gems);

	// Voces del combo si son necesarias
	Sound->ComboVoice(combo);

}



// Añade los puntos de combo conseguidos al marcador total
void CScore::AddToScore(void) {

	// Añade los puntos del combo al Score total
	score += points;
	update.score = true;

}



// Crea el banner con el Combo
void CScore::MakeComboBanner(u16 combo) {

	// Variables locales
	u16 temp = combo;
	if (temp > 999) temp = 999;
	u16 d[3];
	s8 n = 0;
	u8 num = 0;
	u16 div = 100;
	s16 x = 0;
	u8 spr = 0;

	// Reinicia los sprites del banner
	for (n = 0; n < COMBO_NUM; n ++) {
		NF_MoveSprite(0, (NUMBERS_SPR + n), -32, -32);
		NF_ShowSprite(0, (NUMBERS_SPR + n), false);
		NF_SpriteFrame(0, (NUMBERS_SPR + n), 0);
	}

	// Desglosa el resultado en digitos (3)
	for (n = 2; n >= 0; n --) {
		d[n] = (int)(temp / div);
		temp -= (d[n] * div);
		div /= 10;
		if ((d[n] > 0) && (n > num)) num = n;
	}

	// Calcula la X
	x = ((95 - (((num + 1) << 4) >> 1)) - 8);

	// Coloca los sprites
	NF_ShowSprite(0, COMBO_SPR, true);
	NF_ShowSprite(0, (COMBO_SPR + 1), true);
	NF_MoveSprite(0, (NUMBERS_SPR + spr), x, 103);
	NF_ShowSprite(0, (NUMBERS_SPR + spr), true);
	NF_SpriteFrame(0, (NUMBERS_SPR + spr), 10);
	for (n = num; n >= 0; n --) {
		x += 16;
		spr ++;
		NF_MoveSprite(0, (NUMBERS_SPR + spr), x, 103);
		NF_ShowSprite(0, (NUMBERS_SPR + spr), true);
		NF_SpriteFrame(0, (NUMBERS_SPR + spr), d[n]);
	}

}



// Borra el banner con el Combo
void CScore::DeleteComboBanner(void) {

	// Variables
	s8 n = 0;

	// Reinicia los sprites del banner
	NF_ShowSprite(0, COMBO_SPR, false);
	NF_ShowSprite(0, (COMBO_SPR + 1), false);
	for (n = 0; n < COMBO_NUM; n ++) {
		NF_MoveSprite(0, (NUMBERS_SPR + n), -32, -32);
		NF_ShowSprite(0, (NUMBERS_SPR + n), false);
	}

}



// Crea el banner con el Score x Combo
void CScore::MakeScoreBanner(u32 combo) {

	// Variables locales
	u32 temp = combo;
	if (temp > 999999) temp = 999999;
	u16 d[6];
	s8 n = 0;
	u8 num = 0;
	u32 div = 100000;
	s16 x = 0;
	u8 spr = 0;

	// Reinicia los sprites del banner
	for (n = 0; n < SCORE_NUM; n ++) {
		NF_MoveSprite(1, (NUMBERS_SPR + n), -32, -32);
		NF_ShowSprite(1, (NUMBERS_SPR + n), false);
		NF_SpriteFrame(1, (NUMBERS_SPR + n), 0);
	}

	// Desglosa el resultado en digitos (6)
	for (n = 5; n >= 0; n --) {
		d[n] = (int)(temp / div);
		temp -= (d[n] * div);
		div /= 10;
		if ((d[n] > 0) && (n > num)) num = n;
	}

	// Calcula la X
	x = ((95 - ((num << 4) >> 1)) - 8);

	// Coloca los sprites
	NF_ShowSprite(1, SCORE_SPR, true);
	NF_ShowSprite(1, (SCORE_SPR + 1), true);
	for (n = num; n >= 0; n --) {
		NF_MoveSprite(1, (NUMBERS_SPR + spr), x, 87);
		NF_ShowSprite(1, (NUMBERS_SPR + spr), true);
		NF_SpriteFrame(1, (NUMBERS_SPR + spr), d[n]);
		x += 16;
		spr ++;
	}

}



// Borra el banner con el Score x Combo
void CScore::DeleteScoreBanner(void) {

	// Variables
	s8 n = 0;

	// Reinicia los sprites del banner
	NF_ShowSprite(1, SCORE_SPR, false);
	NF_ShowSprite(1, (SCORE_SPR + 1), false);
	for (n = 0; n < SCORE_NUM; n ++) {
		NF_MoveSprite(1, (NUMBERS_SPR + n), -32, -32);
		NF_ShowSprite(1, (NUMBERS_SPR + n), false);
	}

}
