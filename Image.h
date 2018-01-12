#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>

#include "Ppm.h"

//---------------- STRUCTURES 	--------------------

//Couleur
typedef struct
{
	float _red, _green, _blue;
} Color;

//Image
typedef struct
{
	int _width, _height;
	int _xzoom, _yzoom;
	int _xoffset, _yoffset;
	double _zoom;
	Color _current_color;
	Color **_buffer;
} Image;

//Point
typedef struct
{ int x; int y; } Point;


//Node de la liste chainé
typedef struct str_points{
  Point point;
  struct str_points* previous;
  struct str_points* next;
} Points;

//Liste doublement chainé pour stockage de point
typedef struct str_ListePoint{
  int length;
  struct str_points* head;
  struct str_points* end;
} ListePoints;

//Structure d'arrete
typedef struct str_arrete{
	Point Pmin;
	Point Pmax;
	int x_inters;
	int inc;
} arete;


//Création d'un point
Point P_new(int x, int y);

//Création couleur
Color C_new(float red, float green, float blue);

//Egalité de couleur
void C_check(Color c, char *message);

//Egalité de couleur perso
int equalColor(Color c1, Color c2);

//------------------ AFFICHE LIGNE BRISE ------------------
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


//------------------ DESSIN PAR SOURIS ------------------

//Dessine bresenham entre le dernier des points de la list et x,y
void DrawNewPoints(Image *img, ListePoints* list,int x, int y);

//Dessine toutes les bresenham de la liste
void DrawAllListPoints(Image *img, ListePoints* list);

//------------------ REMPLISSAGE ------------------

//trie le tableau dans l'ordre croissant
void trierTab(int tab[], int lenght);

//Trouve la boundingBox du polygone
void FindBoundingBox(ListePoints* list, Point* boundingBox);

//Regarde si le point est a l'interieur du polygone
int InterieurPloygone(Image *img,ListePoints* list, Point p, int xMax);

//Regarde si la ligne du sommet a une interesction superieur ou non
int IntersectSuperieur(ListePoints* list, Points* sommet);

//Renvoie si un sommet a le meme abscisse que le x donné
int isSameAbscisseAsSommet(ListePoints* list,int x);

//Remplie l'image avec la méthode de FAIN
void fill(Image *img,ListePoints* list);

//Remplie avec la méthode de M.Dischler en LMG
void fillByScanLine(Image *img, ListePoints* list);

//Regarde si un sommet avec x,y en coordonée existe
Points* isVertex(ListePoints* list, int x, int y);

//------------------ MANIPULATION LISTE DOUBLE CHAINE ------------------
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

//Free list
void freeList(ListePoints** list);

//------------------ SELECTION CLAVIER ------------------

//VERTEX
//Dessine le carré qui montre la selection
void selectSommet(Image* img,int x, int y, Color* save);

//Redessine comme avant la selection du sommet (applique le buffer de sauvgarde)
void deselectSommet(Image* img,int x, int y, Color* save);

//Bouge suivant la direction le x ou y du point actuel
void MooveSommet(Points* Actuel, int direction);

//Efface les lignes lié au point (question de mini opti)
void I_bresenhamDelete(Image *img, Points* Actuel);

//EDGE
//Dessine le carré qui montre la selection
void selectEdge(Image* img,Points** edgePoint);

//Regarde si le point donné appartient a la selection d'arrete
int isOnEdgeSelect(Points* cible, Points** edgePoint);

//------------------SELECTION SOURIS ------------------

//Choisi le point le plus pret
Points* closestVertex(ListePoints* list, Points* ActualPoint, int x, int y);

//Choisi l'arrete la plus proche
void closestEdge(ListePoints* list, Points** edgePoint, int x, int y);


//####################### MAGICS FONCTIONS DEJA EXISTANTE##############################

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
