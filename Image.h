/*====================================================*\
  Vendredi 8 novembre 2013
  Arash HABIBI
  Image.h
\*====================================================*/

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>

#include "Ppm.h"

//-----------------------------------------------------

typedef struct
{
	float _red, _green, _blue;
} Color;

Color C_new(float red, float green, float blue);
void C_check(Color c, char *message);

//-----------------------------------------------------

typedef struct
{
	int _width, _height;
	int _xzoom, _yzoom;
	int _xoffset, _yoffset;
	double _zoom;
	Color _current_color;
	Color **_buffer;
} Image;


typedef struct
{ int x, y; } Point;

Point P_new(int x, int y);


//####################### MAGICS FONCTIONS ##############################

Image* I_new(int _width, int _height);
Image* I_read(char *ppmfilename);

void I_fill       (Image *img, Color c);
void I_checker    (Image *img, Color c1, Color c2, int step);

void I_changeColor(Image *img, Color c);
void I_plot       (Image *img, int x, int y);
void I_plotColor  (Image *img, int x, int y, Color c);

void I_focusPoint (Image *img, int x, int y);
void I_zoomInit   (Image *img);
void I_zoom       (Image *img, double zoom_coef);
void I_move       (Image *img, int x, int y);

void I_draw       (Image *img);


//##################### FONCTION BRESENHAM ###############################

//Dessine dans le 1er octant, x et y supposé y appartenant
void I_bresenhamOrigin(Image *img, int x, int y);

//Ramene une droite venant du Nieme octant dans le 1er
void I_bresenhamZ2_to_1Oct(int xA, int yA, int xB, int yB, int *O1xA, int *O1yA, int *O1xB, int *O1yB);

//Ramene la droite du 1er octant a sa place d'origine
void I_bresenham1Oct_to_Z2(int xA, int yA, int xB, int yB, int O1x, int O1y, int *x, int *y);

//Dessine la droite de bresenham
void I_bresenham(Image *img, int xA, int yA, int xB, int yB);

//Dessine les droites de bresenham suivant un tableau de point donnés
void DrawAllPoints(Image *img, int points[], int pointsSize);

#endif
