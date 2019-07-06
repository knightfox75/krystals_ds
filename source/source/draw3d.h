#ifndef __DRAW3D_H__
#define __DRAW3D_H__



/*
-------------------------------------------------

	Krystals DS

	Funciones de dibujado 3D

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





// Clase CEngine3d
class CEngine3d {

	// Segmento privado
	private:

		// Especifica en que capa se dibujara la escena 3D
		void Layer3d(u8 layer);



	// Segmento publico
	public:

		// Constructor
		CEngine3d(void);

		// Destructor
		~CEngine3d(void);

		// Inicializa el Engine 3D
		void Init3D(void);

		// Prepara el mundo 3D
		void Setup3dWorld(void);

		// Renderiza la escena
		void Render(void);

		// Dibuja un cuadrado solido
		void DrawSolidQuad	(s32 x1, s32 y1, s32 z1,		// Coordenadas Vertice 1 (arriba / izquierda)
							s32 x2, s32 y2, s32 z2,			// Coordenadas Vertice 2 (abajo / izquierda)
							s32 x3, s32 y3, s32 z3,			// Coordenadas Vertice 3 (abajo / derecha)
							s32 x4, s32 y4, s32 z4,			// Coordenadas Vertice 4 (arriba / derecha)
							float nx, float ny, float nz	// Normales de la cara
							);

};
// Metodos globlales de la clase CEngine3d
extern CEngine3d* Engine3d;



// Clase Vector3 para el uso de vectores tridimensionales
class Vector3 {

	// Segmento privado
	private:

	// Segmento publico
	public:

		// Constructor de la clase
		Vector3(void);

		// Destructor de la clase
		~Vector3(void);

		// Variables de la clase
		v16 x, y, z;	// Coordenadas

		// Asigna las coordenadas al Vector3
		void Set(s32 px, s32 py, s32 pz);

};



// Clase CGemStone (gestion de todas las gemas en pantalla)
class CGemStone {

	// Segmento privadado
	private:


	// Segmento publico
	public:

		// Constructor
		CGemStone(void);

		// Destructor
		~CGemStone(void);

		// Variables de la gema
		s32 px, py, pz;				// Posicion
		s32 width, height, deep;	// Tamaño
		s32 rx, ry, rz;				// Angulo de rotacion

		u32 light_color;			// Color de las luces que afectaran a la gema
		u32 speed;					// Velocidad

		// Dibuja una gema en las coordenadas indicadas (centro)
		void Draw(void);

		// Posicion la gema
		void Position(s32 x, s32 y, s32 z);

		// Angulo de rotacion
		void Rotation(s32 x, s32 y, s32 z);

		// Mueve la gema
		void Translate(s32 x, s32 y, s32 z);

		// Rota la gema
		void Rotate(s32 x, s32 y, s32 z);

};
extern CGemStone* GemStone[BG_GEM_NUM];





#endif