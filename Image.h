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
{ int x; int y; } Point;


typedef struct str_points{
  Point point;
  struct str_points* previous;
  struct str_points* next;
} Points;

typedef struct str_ListePoint{
  int length;
  struct str_points* head;
  struct str_points* end;
} ListePoints;

//Création d'un point
Point P_new(int x, int y);


//------------------ Affiche ligne brisée ------------------
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


//------------------ Dessin par souris ------------------

//Dessine bresenham entre le dernier des points de la list et x,y
void DrawNewPoints(Image *img, ListePoints* list,int x, int y);

//Dessine toutes les bresenham de la liste
void DrawAllListPoints(Image *img, ListePoints* list);

//------------------ Remplissage scan-line ------------------
int equalColor(Color c1, Color c2);

//Trouve la boundingBox du polygone
void FindBoundingBox(ListePoints* list, Point* boundingBox);

void fillByScanLine(Image *img, ListePoints* list);

int isVertex(ListePoints* list, int x, int y);

//------------------ Insert et Suppr Sommets ------------------
// Utilisation d'une liste doublement chainé

//Init la liste chainée
ListePoints* initListPoints();

//Ajoute un sommet a la fin
ListePoints* push_Back_Point(ListePoints* list, int x, int y);

//Supprime un sommet a la fin
ListePoints* remove_Back_Point(ListePoints* list);

//Supprime le sommet trouvé
ListePoints* remove_Point(ListePoints* list, int x, int y);

//Insert le sommet
ListePoints* insert_Point(ListePoints* list, Points** edgePoint);


//------------------ Séléction par clavier ------------------

//VERTEX
//Dessine le carré qui montre la selection
void selectSommet(Image* img,int x, int y, Color* save);

//Redessine comme avant la selection du sommet
void deselectSommet(Image* img,int x, int y, Color* save);

//Bouge suivant la direction le x ou y du point actuel
void MooveSommet(Points* Actuel, int direction);

//Efface les lignes lié au point
void I_bresenhamDelete(Image *img, Points* Actuel);

//EDGE
//Dessine le carré qui montre la selection
void selectEdge(Image* img,Points** edgePoint);




//------------------ Séléction par souris ------------------

//Choisi le point le plus pret
Points* closestVertex(ListePoints* list, Points* ActualPoint, int x, int y);


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


#endif
