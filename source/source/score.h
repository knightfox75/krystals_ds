#ifndef __SCORE_H__
#define __SCORE_H__



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



// Includes
#include <nds.h>



// Clase CScore
class CScore {

	// Segmento privado
	private:


	// Segmento publico
	public:

		// Variables
		s32 score;				// Puntuacion actual
		s32 level;				// Nivel actual
		s32 left;				// Fichas que quedan para pasar de nivel
		s32 combo;				// Combo actual
		s32 points;				// Puntos por el combo actual
		s32 combo_score;		// Puntuacion por combo
		s16 banner_combo;		// Flags de los banners

		struct {				// Flags de actualizacion
			bool score;
			bool level;
			bool left;
		} update;

		// Constructor
		CScore(void);

		// Destructor
		~CScore(void);

		// Actualiza el marcador de Score
		void UpdateScore(void);

		// Actualiza el marcador de Level
		void UpdateLevel(void);

		// Actualiza el marcador de Left
		void UpdateLeft(void);

		// Calcula el Combo-Score y fichas eliminadas
		void CalculateCombo(u16 gems);

		// Añade los puntos de combo conseguidos al marcador total
		void AddToScore(void);

		// Crea el banner con el Combo
		void MakeComboBanner(u16 score);

		// Borra el banner con el Combo
		void DeleteComboBanner(void);

		// Crea el banner con el Score x Combo
		void MakeScoreBanner(u32 score);

		// Borra el banner con el Score x Combo
		void DeleteScoreBanner(void);

};
// Metodos globlales de la clase CScore
extern CScore* Score;




#endif