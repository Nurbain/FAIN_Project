
/*===============================================================*\

    Vendredi 25 septembre 2013

	Arash Habibi

	main.c

	Un programme equivalent à 02_glut.c et qui ne prend en compte
	que trois événements pour quitter le programme.

\*===============================================================*/

#include <stdio.h>
#include <GL/glut.h>
#include <GL/gl.h>

#include "Image.h"

#define APPEND 0
#define VERTEX 1
#define EDGE 2

//Mode actif
int _state = 0;

// Deux images de memes dimensions
Image *img , *img2;

// Bool : premier ou second clic
int _isFirstClic = 1;

// Bool : polygone fermé ou ouvert
int _isClosed = 0;


// Donné de point rentré au début (Q1)
int _AllPointSize = 18;
int _AllPoints[18] =
{
  118,95,
  388,96,
  365,314,
  136,320,
  118,95,
  365,314,
  275,464,
  136,320,
  388,96
};

// Structure utilisé
ListePoints* PointsPolygone;

// Point de selection VERTEX
Points* ActualPoint;

//Buffer de save avant selelction
#define Size_Select 3
Color SaveSelection[6*6];


// Points de selection EDGE
Points* EdgeSelect[2] = {NULL,NULL};



//------------------------------------------------------------------
//	C'est le display callback. A chaque fois qu'il faut
//	redessiner l'image, c'est cette fonction qui est
//	appelee. Tous les dessins doivent etre faits a partir
//	de cette fonction.
//------------------------------------------------------------------

void display_CB()
{
    glClear(GL_COLOR_BUFFER_BIT);

	  I_draw(img);

    glutSwapBuffers();
}

//------------------------------------------------------------------
// Cette fonction permet de réagir en fonction de la position de
// la souris (x,y), en fonction du bouton de la souris qui a été
// pressé ou relaché.
//------------------------------------------------------------------

void mouse_CB(int button, int state, int x, int y)
{
	if((button==GLUT_LEFT_BUTTON)&&(state==GLUT_DOWN)) {
    //Si fermé alors on se remet sur une image propre
    if(_isClosed){
      _isClosed = !_isClosed;
      DrawAllListPoints(img,PointsPolygone);
    }

		I_focusPoint(img,x,img->_height-y);
		int tmp_x = x, tmp_y = img->_height-y;

    //DIFFERENT MODE
    switch (_state) {
      case APPEND :
        if(_isFirstClic) {
          PointsPolygone =  initListPoints();
          push_Back_Point(PointsPolygone,tmp_x,tmp_y);
        }else{
          DrawNewPoints(img,PointsPolygone,tmp_x,tmp_y);
          push_Back_Point(PointsPolygone,tmp_x,tmp_y);
        }
        _isFirstClic = 0;
      break;

      case VERTEX :
        deselectSommet(img,ActualPoint->point.x,ActualPoint->point.y,SaveSelection);
        ActualPoint = closestVertex(PointsPolygone,ActualPoint,tmp_x,tmp_y);
        selectSommet(img,ActualPoint->point.x,ActualPoint->point.y,SaveSelection);
      break;

      case EDGE :
        I_bresenham(img,EdgeSelect[0]->point.x,EdgeSelect[0]->point.y,EdgeSelect[1]->point.x,EdgeSelect[1]->point.y);
        printf("clost \n");
        closestEdge(PointsPolygone,EdgeSelect,tmp_x,tmp_y);
        selectEdge(img,EdgeSelect);
      break;
    }

	}

  if((button==GLUT_MIDDLE_BUTTON)&&(state==GLUT_DOWN)) {
    if(_state == EDGE){
      insert_Point(PointsPolygone,EdgeSelect);
      DrawAllListPoints(img,PointsPolygone);
      selectEdge(img,EdgeSelect);
    }
  }

	glutPostRedisplay();
}

//------------------------------------------------------------------
// Cette fonction permet de réagir au fait que l'utilisateur
// presse une touche (non-spéciale) du clavier.
//------------------------------------------------------------------

void keyboard_CB(unsigned char key, int x, int y)
{
  //TODO supression de selection tout le temps
	//fprintf(stderr,"key=%d\n",key);
	switch(key)
	{
	case 27 : exit(1); break;
	case 'z' : I_zoom(img,2.0); break;
	case 'Z' : I_zoom(img,0.5); break;
	case 'i' : I_zoomInit(img); break;
  //Dessine la ligne brisé donné par une succesion de point
  case 'w' : DrawAllPoints(img,_AllPoints,_AllPointSize); break;

  //Close le polygone
  case 'c' :
    if(PointsPolygone==NULL)
      break;

    if(_isClosed){
      DrawAllListPoints(img,PointsPolygone);
    }else{
      DrawAllListPoints(img,PointsPolygone);
      DrawNewPoints(img,PointsPolygone,PointsPolygone->head->point.x,PointsPolygone->head->point.y);
    }
    _isClosed =! _isClosed ;
  break;

  case 'f' :
    if(_isClosed)
      fill(img,PointsPolygone);
    else
      printf("Fermé le polygone avant de le remplir !\n" );

  //Passe en mode "append"
  case 'a' :
    _state = APPEND;

  break;

  //Passe en mode "vertex"
  case 'v' :
    if(!_isClosed)
      DrawAllListPoints(img,PointsPolygone);
    else{
      DrawAllListPoints(img,PointsPolygone);
      DrawNewPoints(img,PointsPolygone,PointsPolygone->head->point.x,PointsPolygone->head->point.y);
    }

    if(PointsPolygone != NULL){
      _state = VERTEX;
    }
    if(ActualPoint == NULL)
      ActualPoint = PointsPolygone->head;

    deselectSommet(img,ActualPoint->point.x,ActualPoint->point.y,SaveSelection);
    selectSommet(img,ActualPoint->point.x,ActualPoint->point.y,SaveSelection);
  break;

  //Passe en mode "edge"
  case 'e' :
    //TODO SUppresion de point du coup faire gaffe faire une fonction find point
    if(ActualPoint != NULL){
      deselectSommet(img,ActualPoint->point.x,ActualPoint->point.y,SaveSelection);
    }

    if(_isClosed){
      DrawAllListPoints(img,PointsPolygone);
      _isClosed = !_isClosed;
    }

    _state = EDGE;
    if(EdgeSelect[0] != NULL && EdgeSelect[1] != NULL){
      selectEdge(img,EdgeSelect);
    }
    else if(PointsPolygone->length >=2){
        EdgeSelect[0] = PointsPolygone->head;
        EdgeSelect[1] = PointsPolygone->head->next;
        selectEdge(img,EdgeSelect);
    }
  break;

  //Touche "&"
  case 38 :
    switch (_state)
    {
      //Passe au sommet précedent
      case VERTEX :

        deselectSommet(img,ActualPoint->point.x,ActualPoint->point.y,SaveSelection);

        if(ActualPoint->previous == NULL){
          if(_isClosed){
            ActualPoint = PointsPolygone->end;
          }
        }else
          ActualPoint = ActualPoint->previous;

        selectSommet(img,ActualPoint->point.x,ActualPoint->point.y,SaveSelection);
      break;

      //Passe a l'edge précedente
      case EDGE :
        if(EdgeSelect[0]->previous != NULL){
          I_bresenham(img,EdgeSelect[0]->point.x,EdgeSelect[0]->point.y,EdgeSelect[1]->point.x,EdgeSelect[1]->point.y);
          EdgeSelect[1] = EdgeSelect[0];
          EdgeSelect[0] = EdgeSelect[0]->previous;
          selectEdge(img,EdgeSelect);
        }
      break;
    }
  break;

  //Touche "é"
  case 233 :
    switch (_state)
    {
      //Passe au sommet suivant
      case VERTEX :
        deselectSommet(img,ActualPoint->point.x,ActualPoint->point.y,SaveSelection);

        if(ActualPoint->next == NULL){
          if(_isClosed){
            ActualPoint = PointsPolygone->head;
          }
        }else
          ActualPoint = ActualPoint->next;

        selectSommet(img,ActualPoint->point.x,ActualPoint->point.y,SaveSelection);
      break;

      //Passe a l'edge suivante
      case EDGE :
        if(EdgeSelect[1]->next != NULL){
          I_bresenham(img,EdgeSelect[0]->point.x,EdgeSelect[0]->point.y,EdgeSelect[1]->point.x,EdgeSelect[1]->point.y);
          EdgeSelect[0] = EdgeSelect[1];
          EdgeSelect[1] = EdgeSelect[1]->next;
          selectEdge(img,EdgeSelect);
        }
      break;
    }
  break;

  //Suppr
  case 127 :
    switch (_state)
    {
      //Passe au sommet suivant
      case VERTEX :
        if(ActualPoint != NULL){
          deselectSommet(img,ActualPoint->point.x,ActualPoint->point.y,SaveSelection);
          remove_Point(PointsPolygone, ActualPoint->point.x,ActualPoint->point.y);
          DrawAllListPoints(img,PointsPolygone);
          if(PointsPolygone!=NULL){
            ActualPoint = PointsPolygone->head;
            selectSommet(img,ActualPoint->point.x,ActualPoint->point.y,SaveSelection);
          }
        }
      break;

      default :
        printf("Pas possible dans ce mode \n");
    }
  break;
	}




	glutPostRedisplay();
}

//------------------------------------------------------------------
// Cette fonction permet de réagir au fait que l'utilisateur
// presse une touche spéciale (F1, F2 ... F12, home, end, insert,
// haut, bas, droite, gauche etc).
//------------------------------------------------------------------

void special_CB(int key, int x, int y)
{
	// int mod = glutGetModifiers();

	//int d = 10;
  //TODO: Faire bouger quand fermé
	switch(key)
	{
	case GLUT_KEY_UP    :
    switch (_state) {
      case VERTEX:
        deselectSommet(img,ActualPoint->point.x,ActualPoint->point.y,SaveSelection);
        I_bresenhamDelete(img,ActualPoint);
        MooveSommet(ActualPoint,2);
        if(ActualPoint->previous != NULL)
          I_bresenham(img,ActualPoint->previous->point.x,ActualPoint->previous->point.y,ActualPoint->point.x,ActualPoint->point.y);
        if(ActualPoint->next != NULL)
          I_bresenham(img,ActualPoint->next->point.x,ActualPoint->next->point.y,ActualPoint->point.x,ActualPoint->point.y);
      break;
    }

  //I_move(img,0,d);
  break;
	case GLUT_KEY_DOWN  :
    switch (_state) {
      case VERTEX:
        deselectSommet(img,ActualPoint->point.x,ActualPoint->point.y,SaveSelection);
        I_bresenhamDelete(img,ActualPoint);
        MooveSommet(ActualPoint,4);
        if(ActualPoint->previous != NULL)
          I_bresenham(img,ActualPoint->previous->point.x,ActualPoint->previous->point.y,ActualPoint->point.x,ActualPoint->point.y);
        if(ActualPoint->next != NULL)
          I_bresenham(img,ActualPoint->next->point.x,ActualPoint->next->point.y,ActualPoint->point.x,ActualPoint->point.y);
      break;
    }

  //I_move(img,0,-d);
  break;
	case GLUT_KEY_LEFT  :
    switch (_state) {
      case VERTEX:
        deselectSommet(img,ActualPoint->point.x,ActualPoint->point.y,SaveSelection);
        I_bresenhamDelete(img,ActualPoint);
        MooveSommet(ActualPoint,1);
        if(ActualPoint->previous != NULL)
          I_bresenham(img,ActualPoint->previous->point.x,ActualPoint->previous->point.y,ActualPoint->point.x,ActualPoint->point.y);
        if(ActualPoint->next != NULL)
          I_bresenham(img,ActualPoint->next->point.x,ActualPoint->next->point.y,ActualPoint->point.x,ActualPoint->point.y);
      break;
    }
  //I_move(img,d,0);
  break;
	case GLUT_KEY_RIGHT :
    switch (_state) {
      case VERTEX:
        deselectSommet(img,ActualPoint->point.x,ActualPoint->point.y,SaveSelection);
        I_bresenhamDelete(img,ActualPoint);
        MooveSommet(ActualPoint,3);
        if(ActualPoint->previous != NULL)
          I_bresenham(img,ActualPoint->previous->point.x,ActualPoint->previous->point.y,ActualPoint->point.x,ActualPoint->point.y);
        if(ActualPoint->next != NULL)
          I_bresenham(img,ActualPoint->next->point.x,ActualPoint->next->point.y,ActualPoint->point.x,ActualPoint->point.y);
      break;
    }
  //I_move(img,-d,0); break;
  break;
	default : fprintf(stderr,"special_CB : %d : unknown key.\n",key);
	}
	glutPostRedisplay();
}

//------------------------------------------------------------------------

int _clamp(int x, int inf, int sup)
{
	return (x<inf?inf:(x>sup?sup:x));
}

//------------------------------------------------------------------------

int main(int argc, char **argv)
{
	if((argc!=3)&&(argc!=2))
	{
		fprintf(stderr,"\n\nUsage \t: %s <width> <height>\nou",argv[0]);
		fprintf(stderr,"\t: %s <ppmfilename> \n\n",argv[0]);
		exit(1);
	}
	else
	{
		int largeur, hauteur;
		if(argc==2)
		{
			img = I_read(argv[1]);
			largeur = img->_width;
			hauteur = img->_height;
			img2 = I_new(largeur, hauteur);
		}
		else
		{
			largeur = atoi(argv[1]);
			hauteur = atoi(argv[2]);
			img = I_new(largeur,hauteur);
			img2 = I_new(largeur, hauteur);
		}
		int windowPosX = 900, windowPosY = 100;

		glutInitWindowSize(largeur,hauteur);
		glutInitWindowPosition(windowPosX,windowPosY);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE );
		glutInit(&argc, argv);
		glutCreateWindow(argv[0]);

		glViewport(0, 0, largeur, hauteur);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glOrtho(0,largeur,0,hauteur,-1,1);

		glutDisplayFunc(display_CB);
		glutKeyboardFunc(keyboard_CB);
		glutSpecialFunc(special_CB);
		glutMouseFunc(mouse_CB);
		// glutMotionFunc(mouse_move_CB);
		// glutPassiveMotionFunc(passive_mouse_move_CB);
		glutMainLoop();

    freeList(&PointsPolygone);
    free(ActualPoint);
    free(EdgeSelect[0]);
    free(EdgeSelect[1]);
		return 0;
	}
}
