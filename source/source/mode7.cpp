/*
-------------------------------------------------

	Krystals DS

	Archivo de funciones del MODO 7

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




// Rutina de fondo animado 2D/Affine
void Mode7AnimateBg(void) {

	// Deshabilita la interrupcion
	irqDisable(IRQ_HBLANK);

	// Variables
	s32 wave = 0;			// Efecto WAVE
	s32 fsin = 0;			// Calculo del seno con fixed point
	s32 angle = 0;			// Angulo para el calculo
	s32 zoom;				// Nivel de Zoom
	u32 line = VLINE;		// Linia actual de dibujado

	// Sombreado total para esconder defectos del VLINE
	REG_BLDY = 0x10;
	REG_BLDY_SUB = 0x10;

	// Si la linea de dibujado esta en pantalla
	if (line < 192) {

		// Calcula el effecto wave
		angle = (Draw->mode7_angle + line);
		if (angle > 512) angle -= 512;
		fsin = (sinLerp((angle << 6)) >> 5);
		if (fsin >= 0) {
			wave = ((fsin * 24) >> 7);
		}
		if (fsin < 0) {
			fsin = -fsin;
			wave = ((fsin * 24) >> 7);
			wave = -wave;
		}

		///////////////////////
		// Pantalla Superior //
		///////////////////////

		// Sombreado parte superior
		REG_BLDY = Draw->shading[line];

		// Calcula el valor del ZOOM
		zoom = ((line + 64) << 1);

		// Aplicalo al fondo affine
		NF_AffineBgMove(0, LAYER_BG, ((Draw->mode7_scroll >> 8) + wave), ((Draw->mode7_scroll >> 8) + wave), 0);
		NF_AffineBgTransform(0, LAYER_BG, zoom, zoom, 0, 0);



		///////////////////////
		// Pantalla Inferior //
		///////////////////////


		// Sombreado parte inferior
		REG_BLDY_SUB = Draw->shading[(191 - line)];

		// Calcula el valor del ZOOM
		zoom = (512 - (line << 1));

		// Aplicalo al fondo affine
		NF_AffineBgMove(1, LAYER_BG, ((Draw->mode7_scroll >> 8) + wave), ((Draw->mode7_scroll >> 8) + wave), 0);
		NF_AffineBgTransform(1, LAYER_BG, zoom, zoom, 0, 0);

	}

	// Vuelve a habilitar el HBLANK
	irqEnable(IRQ_HBLANK);

}



// Rutina de fondo animado 2D/Affine
void Mode7Credits(void) {

	// Deshabilita la interrupcion
	irqDisable(IRQ_HBLANK);

	u32 line = VLINE;		// Linia actual de dibujado

		// Si la linea de dibujado esta en pantalla
	if (line < 192) {
		REG_BLDALPHA_SUB = Credits->blending[line] | (0x10 << 8);
	}

	// Vuelve a habilitar el HBLANK
	irqEnable(IRQ_HBLANK);

}
