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
Metodos de la clase "CEngine3d"
*/

// Contructor clase CEngine3d
CEngine3d::CEngine3d(void) {

	// Variables
	u8 n = 0;

	// Crea los objetos del tipo gema que usara esta clase
	for (n = 0; n < BG_GEM_NUM; n ++) {
		// Crea el objeto
		GemStone[n] = new CGemStone();
	}

}



// Destructor clase CEngine3d
CEngine3d::~CEngine3d(void) {

	// Variables
	u8 n = 0;

	// Al destruir este objeto, destruye todas los objetos del tipo gema usados
	for (n = 0; n < BG_GEM_NUM; n ++) {
		// Crea el objeto
		delete GemStone[n];
		GemStone[n] = NULL;
	}

}



// Metodos globlales de la clase CEngine3d
CEngine3d* Engine3d;



// Inicializa el Engine 3D
void CEngine3d::Init3D(void) {

	// Habilita el 3D en la pantalla principal
	videoSetMode(MODE_0_3D);


	// *** Inicializa OpenGL ***

	// Inicializa el OpenGL de Libnds
	glInit();

	// Define el tamaño de la ventana 3D (toda la pantalla)
	glViewport(0, 0, 255, 191);

	// Configura la matriz de proyeccion
	glMatrixMode(GL_PROJECTION);	// Selecciona la matriz
	glLoadIdentity();

	// Configura la visual
	gluPerspective(70, (256.0 / 192.0), 0.1, 40);		// Perspectiva

	// Posiciona la camara por defecto
	gluLookAtf32(	0, 0, 0,				// Posicion de la camara
					0, 0, 0,				// Punto hacia el que miras
					0, (128 << 4), 0		// Normal
					);

	// Configura la matriz de visualizacion de modelos
	glMatrixMode(GL_MODELVIEW);		// Selecciona la matriz
	glLoadIdentity();				// Y reseteala

	// Configura el fondo
	glClearColor(0, 0, 0, 0);		// Fondo transparente
	glClearDepth(0x7FFF);			// Define la distancia de vision

	// Configura la iluminacion global mediante el color del poligono
	glColor(RGB15(31, 31, 31));

	// Habilita la capa de dibujado
	REG_DISPCNT |= (DISPLAY_BG0_ACTIVE);

	// *** Fin de la inicializacion OpenGL ***



	// Inicializa los fondos 2D en el main engine
	NF_InitTiledBgSys(0);

	// Y habilita la capa de dibujado para el 3D en el main engine, poniendola al fondo
	Engine3d->Layer3d(2);
	NF_ShowBg(0, 0);

}



// Especifica en que capa se dibujara la escena 3d
void CEngine3d::Layer3d(u8 layer) {

	// Resetea los BITS de control de prioridad en todos los fondos
	REG_BG0CNT &= 0xfffc;		// Pon a 0 los bits 0 y 1 del registro
	REG_BG1CNT &= 0xfffc;
	REG_BG2CNT &= 0xfffc;
	REG_BG3CNT &= 0xfffc;

	// Reordena todas las capas segun la solicitud
	switch (layer) {
		case 0:		// 3D en la capa 0
			REG_BG0CNT += BG_PRIORITY_0;
			REG_BG1CNT += BG_PRIORITY_1;
			REG_BG2CNT += BG_PRIORITY_2;
			REG_BG3CNT += BG_PRIORITY_3;
			break;
		case 1:		// 3D en la capa 1
			REG_BG0CNT += BG_PRIORITY_1;
			REG_BG1CNT += BG_PRIORITY_0;
			REG_BG2CNT += BG_PRIORITY_2;
			REG_BG3CNT += BG_PRIORITY_3;
			break;
		case 2:		// 3D en la capa 2
			REG_BG0CNT += BG_PRIORITY_2;
			REG_BG1CNT += BG_PRIORITY_0;
			REG_BG2CNT += BG_PRIORITY_1;
			REG_BG3CNT += BG_PRIORITY_3;
			break;
		case 3:		// 3D en la capa 3
			REG_BG0CNT += BG_PRIORITY_3;
			REG_BG1CNT += BG_PRIORITY_0;
			REG_BG2CNT += BG_PRIORITY_1;
			REG_BG3CNT += BG_PRIORITY_2;
			break;
		default:	// 3D en la capa 0
			REG_BG0CNT += BG_PRIORITY_0;
			REG_BG1CNT += BG_PRIORITY_1;
			REG_BG2CNT += BG_PRIORITY_2;
			REG_BG3CNT += BG_PRIORITY_3;
			break;
	}

}



// Prepara el mundo 3D
void CEngine3d::Setup3dWorld(void) {

	// Variables
	u16 n = 0;
	u8 row = 0, column = 0;		// Control de la parrilla inicial

	// Activas las opciones OpenGL de esta escena
	// (Alpha blending, antialias, outline)
	glEnable(GL_BLEND | GL_ANTIALIAS | GL_OUTLINE | GL_FOG);
	//glEnable(GL_BLEND | GL_ANTIALIAS | GL_FOG);

	// Habilita el OUTLINE (Contornea los poligonos con el fondo 2D, dara efecto de reflejos en el borde)
	glClearPolyID(0);							// Los poligonos con el ID 0, no tienen outline, usar el ID 1 para habilitarlo
	glSetOutlineColor(0, RGB15(8, 8, 8));		// Color de la linea 

	// Crea las luces (Se usaran para dar profundidad y colorear las diferentes gemas)
	glLight(0, RGB15(31, 0, 0), floattov10(0.5), floattov10(-0.66), floattov10(-1.0));
	glLight(1, RGB15(0, 31, 0), floattov10(0.5), floattov10(-0.66), floattov10(-1.0));
	glLight(2, RGB15(0, 0, 31), floattov10(0.5), floattov10(-0.66), floattov10(-1.0));

	// *** Inicializa la niebla *** (Se usa esta configuracion para simular iluminacion global, a mas distancia, mas oscuridad)
	// Define el factor de profundidad de la niebla
	glFogShift(2);
	// Color de la niebla, sin ALPHA
	glFogColor(0, 0, 0, 31);
	// Crea la tabla de densidad de la niebla
	for (int f = 0; f < 31; f ++) {
		glFogDensity(f, (f << 2));
	}
	glFogDensity(31, 127);
	// Define a partir de que punto empieza la niebla
	glFogOffset(0x6000 + (64 << 4));

	// Crea todas las gemas
	for (n = 0; n < BG_GEM_NUM; n ++) {
		// Colocalas
		GemStone[n]->Position(((column << 6) + 24), (int)(rand() % 256), -((row << 6) + 24));
		GemStone[n]->Rotation((int)(rand() % 512), (int)(rand() % 512), (int)(rand() % 512));
		GemStone[n]->light_color = (n % 6);
		GemStone[n]->speed = (((int)(rand() % 4)) + 1);
		// Cambio de posicion
		column ++;
		if (column > 3) {
			column = 0;
			row ++;
			if (row > 5) {
				row = 0;
			}
		}
	}

}



// Renderiza la escena y mandala a la pantalla
void CEngine3d::Render(void) {

	// Variables
	u16 n = 0;
	s32 top, bottom;	// Limites
	s32 hide;			// Tamaño de ocultacion
	s32 current_z;		// Posicion Z actual

	/*
		Ejecuta aqui todos los renders necesarios
	*/
	for (n = 0; n <BG_GEM_NUM; n ++) {
		GemStone[n]->Rotate(GemStone[n]->speed, GemStone[n]->speed, 0);
		GemStone[n]->Translate(0, -GemStone[n]->speed, 0);
		// Calcula los limites de la gema actual
		current_z = abs(GemStone[n]->pz);
		hide = (current_z + 32);
		top = (192 + hide);
		bottom = -hide;
		// Si la gema sale del plano, recolocala
		if (GemStone[n]->py < bottom) {
			GemStone[n]->py = top;								// Nueva posicion
			GemStone[n]->light_color = (int)(rand() % 6);		// Nueva iluminacion
			GemStone[n]->speed = (((int)(rand() % 4)) + 1);		// Nueva velocidad
		}
		GemStone[n]->Draw();
	}

	// Manda la escena preparada para su renderizado al Engine 3D
	glFlush(0);

}



// Dibuja un cuadrado solido
void CEngine3d::DrawSolidQuad(	s32 x1, s32 y1, s32 z1,			// Coordenadas Vertice 1 (arriba / izquierda)
								s32 x2, s32 y2, s32 z2,			// Coordenadas Vertice 2 (abajo / izquierda)
								s32 x3, s32 y3, s32 z3,			// Coordenadas Vertice 3 (abajo / derecha)
								s32 x4, s32 y4, s32 z4,			// Coordenadas Vertice 4 (arriba / derecha)
								float nx, float ny, float nz	// Normales de la cara
								) {
	// Dibuja el cuadrado
	glBegin(GL_QUAD);
		// Normal de la cara
		glNormal(NORMAL_PACK(floattov10(nx), floattov10(ny), floattov10(nz)));
		// Vertice 1 - (superior izquierdo)
		glVertex3v16(x1, y1, z1);
		// Vertice 2 - (inferior izquierdo)
		glVertex3v16(x2, y2, z2);
		// Vertice 3 - (inferior derecho)
		glVertex3v16(x3, y3, z3);
		// Vertice 4 - (superior derecho)
		glVertex3v16(x4, y4, z4);
		
}



/*
Metodos de la clase "Vector3"
*/

// Contructor
Vector3::Vector3(void) {

	// Inicializa los componentes del Vector3 a 0
	x = 0;
	y = 0;
	z = 0;

}

// Destructor
Vector3::~Vector3(void) {
}


// Asigna las coordenadas al Vector3 (Ya convertidas en coordenadas de pantalla)
void Vector3::Set(s32 px, s32 py, s32 pz) {

	x = (px << 4);
	y = (py << 4);
	z = (pz << 4);

}



/*
Metodos de la clase "CGemStone"
*/

// Constructor
CGemStone::CGemStone(void) {

	// Inicializa todas las variables a 0
	px = 0;
	py = 0;
	pz = 0;
	width = 48;
	height = 48;
	deep = 16;
	rx = 0;
	ry = 0;
	rz = 0;
	light_color = 0;
	speed = 0;

}

// Destructor
CGemStone::~CGemStone(void) {
}

// Metodos globales (punteros)
CGemStone* GemStone[BG_GEM_NUM];



// Dibuja una Gema en la posicion y rotaciones indicadas
void CGemStone::Draw(void) {

	// Variables
	u8 n = 0;
	u16 gl_color;	// Luces a usar en OpenGL

	// Crea el array de vertices con objetos Vector3
	Vector3* vertex[16];
	for (n = 0; n < 16; n ++) {
		vertex[n] = new Vector3();
	}

	// Calcula el valor de cada vertice de la parte frontal del cuerpo de la gema
	vertex[0]->Set((px - (width >> 1)), (py + (height >> 1)), (pz + (deep >> 1)));
	vertex[1]->Set((px - (width >> 1)), (py - (height >> 1)), (pz + (deep >> 1)));
	vertex[2]->Set((px + (width >> 1)), (py - (height >> 1)), (pz + (deep >> 1)));
	vertex[3]->Set((px + (width >> 1)), (py + (height >> 1)), (pz + (deep >> 1)));

	// Calcula el valor de cada vertice de la parte anterior del cuerpo de la gema
	vertex[4]->Set((px - (width >> 1)), (py + (height >> 1)), (pz - (deep >> 1)));
	vertex[5]->Set((px - (width >> 1)), (py - (height >> 1)), (pz - (deep >> 1)));
	vertex[6]->Set((px + (width >> 1)), (py - (height >> 1)), (pz - (deep >> 1)));
	vertex[7]->Set((px + (width >> 1)), (py + (height >> 1)), (pz - (deep >> 1)));

	// Calcula el valor de cada vertice de la cara frontal de la gema
	vertex[8]->Set((px - (width >> 1) + (width >> 3)), (py + (height >> 1) - (height >> 3)), (pz + (deep >> 1) + (deep >> 1)));
	vertex[9]->Set((px - (width >> 1) + (width >> 3)), (py - (height >> 1) + (height >> 3)), (pz + (deep >> 1) + (deep >> 1)));
	vertex[10]->Set((px + (width >> 1) - (width >> 3)), (py - (height >> 1) + (height >> 3)), (pz + (deep >> 1) + (deep >> 1)));
	vertex[11]->Set((px + (width >> 1) - (width >> 3)), (py + (height >> 1) - (height >> 3)), (pz + (deep >> 1) + (deep >> 1)));

	// Calcula el valor de cada vertice de la cara anterior de la gema
	vertex[12]->Set((px - (width >> 1) + (width >> 3)), (py + (height >> 1) - (height >> 3)), (pz - (deep >> 1) - (deep >> 1)));
	vertex[13]->Set((px - (width >> 1) + (width >> 3)), (py - (height >> 1) + (height >> 3)), (pz - (deep >> 1) - (deep >> 1)));
	vertex[14]->Set((px + (width >> 1) - (width >> 3)), (py - (height >> 1) + (height >> 3)), (pz - (deep >> 1) - (deep >> 1)));
	vertex[15]->Set((px + (width >> 1) - (width >> 3)), (py + (height >> 1) - (height >> 3)), (pz - (deep >> 1) - (deep >> 1)));

	// Calcula que luces usara esta gema
	switch (light_color) {
		case 0:		// Rojo
			gl_color = POLY_FORMAT_LIGHT0;
			break;
		case 1:		// Verde
			gl_color = POLY_FORMAT_LIGHT1;
			break;
		case 2:		// Azul
			gl_color = POLY_FORMAT_LIGHT2;
			break;
		case 3:		// Amarillo
			gl_color = (POLY_FORMAT_LIGHT0 | POLY_FORMAT_LIGHT1);
			break;
		case 4:		// Fucsia
			gl_color = (POLY_FORMAT_LIGHT0 | POLY_FORMAT_LIGHT2);
			break;
		case 5:		// Cian
			gl_color = (POLY_FORMAT_LIGHT1 | POLY_FORMAT_LIGHT2);
			break;
		default:
			gl_color = (POLY_FORMAT_LIGHT0 | POLY_FORMAT_LIGHT1 | POLY_FORMAT_LIGHT2);
			break;
	}



	// Primero de todo, guarda la matriz de transformacion
	glPushMatrix();

	// Posiciona la camara
	gluLookAtf32(	(128 << 4), (96 << 4), (128 << 4),		// Posicion
					(128 << 4), (96 << 4), 0,				// Look At
					0 , (128 << 4), 0);						// Normal

	// Define el material a usar en el render
	glMaterialf(GL_AMBIENT, RGB15(8, 8, 8));
	glMaterialf(GL_DIFFUSE, RGB15(16, 16, 16));
	glMaterialf(GL_SPECULAR, RGB15(24, 24, 24));
	glMaterialf(GL_EMISSION, RGB15(0, 0, 0));

	// Define el formato de los poligonos a dibujar en esta fase
	glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | gl_color | POLY_ID(1) | POLY_FOG);
	//glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | gl_color | POLY_ID(1));

	// Aplica si es necesario la transformacion de la matriz
	// Muevete al centro
	glTranslatef32((px << 4), (py << 4), (pz << 4));
	// Aplica la rotacion
	glRotateXi((rx << 6));
	glRotateYi((ry << 6));
	glRotateZi((rz << 6));
	// Vuelve a la posicion original
	glTranslatef32(-(px << 4), -(py << 4), -(pz << 4));


	// *** Dibuja el objeto, cuerpo

	// Cara frontal
	Engine3d->DrawSolidQuad(	vertex[8]->x, vertex[8]->y, vertex[8]->z,
								vertex[9]->x, vertex[9]->y, vertex[9]->z,
								vertex[10]->x, vertex[10]->y, vertex[10]->z,
								vertex[11]->x, vertex[11]->y, vertex[11]->z,
								0, 0, 1.0);

	// Cara derecha
	Engine3d->DrawSolidQuad(	vertex[3]->x, vertex[3]->y, vertex[3]->z,
								vertex[2]->x, vertex[2]->y, vertex[2]->z,
								vertex[6]->x, vertex[6]->y, vertex[6]->z,
								vertex[7]->x, vertex[7]->y, vertex[7]->z,
								1.0, 0, 0);

	// Cara posterior
	Engine3d->DrawSolidQuad(	vertex[15]->x, vertex[15]->y, vertex[15]->z,
								vertex[14]->x, vertex[14]->y, vertex[14]->z,
								vertex[13]->x, vertex[13]->y, vertex[13]->z,
								vertex[12]->x, vertex[12]->y, vertex[12]->z,
								0, 0, -1.0);

	// Cara izquierda
	Engine3d->DrawSolidQuad(	vertex[4]->x, vertex[4]->y, vertex[4]->z,
								vertex[5]->x, vertex[5]->y, vertex[5]->z,
								vertex[1]->x, vertex[1]->y, vertex[1]->z,
								vertex[0]->x, vertex[0]->y, vertex[0]->z,
								-1.0, 0, 0);

	// Cara Superior
	Engine3d->DrawSolidQuad(	vertex[4]->x, vertex[4]->y, vertex[4]->z,
								vertex[0]->x, vertex[0]->y, vertex[0]->z,
								vertex[3]->x, vertex[3]->y, vertex[3]->z,
								vertex[7]->x, vertex[7]->y, vertex[7]->z,
								0, 1.0, 0);

	// Cara Inferior
	Engine3d->DrawSolidQuad(	vertex[1]->x, vertex[1]->y, vertex[1]->z,
								vertex[5]->x, vertex[5]->y, vertex[5]->z,
								vertex[6]->x, vertex[6]->y, vertex[6]->z,
								vertex[2]->x, vertex[2]->y, vertex[2]->z,
								0, -1.0, 0);


	// *** Dibuja el objeto, cortes frontales

	// Corte frontal superior
	Engine3d->DrawSolidQuad(	vertex[11]->x, vertex[11]->y, vertex[11]->z,
								vertex[3]->x, vertex[3]->y, vertex[3]->z,
								vertex[0]->x, vertex[0]->y, vertex[0]->z,
								vertex[8]->x, vertex[8]->y, vertex[8]->z,
								0, 0.66, 0.66);

	// Corte frontal izquierdo
	Engine3d->DrawSolidQuad(	vertex[8]->x, vertex[8]->y, vertex[8]->z,
								vertex[0]->x, vertex[0]->y, vertex[0]->z,
								vertex[1]->x, vertex[1]->y, vertex[1]->z,
								vertex[9]->x, vertex[9]->y, vertex[9]->z,
								-0.66, 0, 0.66);

	// Corte frontal inferior
	Engine3d->DrawSolidQuad(	vertex[9]->x, vertex[9]->y, vertex[9]->z,
								vertex[1]->x, vertex[1]->y, vertex[1]->z,
								vertex[2]->x, vertex[2]->y, vertex[2]->z,
								vertex[10]->x, vertex[10]->y, vertex[10]->z,
								0, -0.66, 0.66);

	// Corte frontal derecho
	Engine3d->DrawSolidQuad(	vertex[10]->x, vertex[10]->y, vertex[10]->z,
								vertex[2]->x, vertex[2]->y, vertex[2]->z,
								vertex[3]->x, vertex[3]->y, vertex[3]->z,
								vertex[11]->x, vertex[11]->y, vertex[11]->z,
								0.66, 0, 0.66);


	// *** Dibuja el objeto, cortes anteriores

	// Corte anterior superior
	Engine3d->DrawSolidQuad(	vertex[12]->x, vertex[12]->y, vertex[12]->z,
								vertex[4]->x, vertex[4]->y, vertex[4]->z,
								vertex[7]->x, vertex[7]->y, vertex[7]->z,
								vertex[15]->x, vertex[15]->y, vertex[15]->z,
								0, 0.66, -0.66);

	// Corte anterior izquierdo
	Engine3d->DrawSolidQuad(	vertex[13]->x, vertex[13]->y, vertex[13]->z,
								vertex[5]->x, vertex[5]->y, vertex[5]->z,
								vertex[4]->x, vertex[4]->y, vertex[4]->z,
								vertex[12]->x, vertex[12]->y, vertex[12]->z,
								-0.66, 0, -0.66);

	// Corte anterior inferior
	Engine3d->DrawSolidQuad(	vertex[14]->x, vertex[14]->y, vertex[14]->z,
								vertex[6]->x, vertex[6]->y, vertex[6]->z,
								vertex[5]->x, vertex[5]->y, vertex[5]->z,
								vertex[13]->x, vertex[13]->y, vertex[13]->z,
								0, -0.66, -0.66);

	// Corte anterior derecho
	Engine3d->DrawSolidQuad(	vertex[15]->x, vertex[15]->y, vertex[15]->z,
								vertex[7]->x, vertex[7]->y, vertex[7]->z,
								vertex[6]->x, vertex[6]->y, vertex[6]->z,
								vertex[14]->x, vertex[14]->y, vertex[14]->z,
								0.66, 0, -0.66);


	// Recupera la matriz de transformacion original
	glPopMatrix(1);


	// Elimina el array de vertices
	for (n = 0; n < 16; n ++) {
		delete vertex[n];
		vertex[n] = NULL;
	}

}


// Posicion la gema
void CGemStone::Position(s32 x, s32 y, s32 z) {

	// Asigna las coordenadas
	px = x;
	py = y;
	pz = z;

}


// Rotacion de la gema
void CGemStone::Rotation(s32 x, s32 y, s32 z) {

	// Asigna los angulos de rotacion
	rx = x;
	ry = y;
	rz = z;

}


// Mueve a la gema
void CGemStone::Translate(s32 x, s32 y, s32 z) {

	// Desplaza la gema
	px += x;
	py += y;
	pz += z;

}


// Rota la gema
void CGemStone::Rotate(s32 x, s32 y, s32 z) {

	// Rota el eje X
	rx += x;
	if (rx > 512) rx -= 512;
	if (rx < -512) rx += 512;

	// Rota el eje Y
	ry += y;
	if (ry > 512) ry -= 512;
	if (ry < -512) ry += 512;

	// Rota el eje Z
	rz += z;
	if (rz > 512) rz -= 512;
	if (rz < -512) rz += 512;

}
