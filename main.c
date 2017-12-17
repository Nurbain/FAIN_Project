
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

// Deux images de memes dimensions
Image *img , *img2;
// Coordonnees du dernier clic de souris
int _x = -1, _y = -1;

// Bool : premier ou second clic
int _isFirstClic = 1;

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

ListePoints* PointsPolygone;
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
		I_focusPoint(img,x,img->_height-y);
		int tmp_x = x, tmp_y = img->_height-y;
		printf("mouse_CB : (%d,%d)\n", tmp_x, tmp_y);

		if(!_isFirstClic) {
      DrawNewPoints(img,PointsPolygone,tmp_x,tmp_y);
		}else{
      PointsPolygone = initListPoints();
      push_Back_Point(PointsPolygone,tmp_x,tmp_y);
    }


		_x = tmp_x; _y = tmp_y;
		_isFirstClic = 0;
	}

	glutPostRedisplay();
}

//------------------------------------------------------------------
// Cette fonction permet de réagir au fait que l'utilisateur
// presse une touche (non-spéciale) du clavier.
//------------------------------------------------------------------

void keyboard_CB(unsigned char key, int x, int y)
{
	// fprintf(stderr,"key=%d\n",key);
	switch(key)
	{
	case 27 : exit(1); break;
	case 'z' : I_zoom(img,2.0); break;
	case 'Z' : I_zoom(img,0.5); break;
	case 'i' : I_zoomInit(img); break;
  case 'w' : DrawAllPoints(img,_AllPoints,_AllPointSize); break;
  case 'c' : DrawNewPoints(img,PointsPolygone,PointsPolygone->p_head->point.x,PointsPolygone->p_head->point.y); break;
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

	int d = 10;

	switch(key)
	{
	case GLUT_KEY_UP    : I_move(img,0,d); break;
	case GLUT_KEY_DOWN  : I_move(img,0,-d); break;
	case GLUT_KEY_LEFT  : I_move(img,d,0); break;
	case GLUT_KEY_RIGHT : I_move(img,-d,0); break;
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

			/*Color rouge = C_new(100,0,0);
			Color blanc = C_new(200,200,255);
			I_checker(img,rouge,blanc,50);*/


			/*
			if(argc==3)
			{
				Color rouge = C_new(100,0,0);
				Color blanc = C_new(200,200,255);
				I_checker(img,rouge,blanc,50);
			}
			else
			{
				int xP = _clamp(atoi(argv[3]), 0, largeur);
				int yP = _clamp(atoi(argv[4]), 0, hauteur);
				if (argc==5) {
					I_bresenhamOrigin(img, xP, yP);
				}
				else
				{
					int xQ = _clamp(atoi(argv[5]), 0, largeur);
					int yQ = _clamp(atoi(argv[6]), 0, hauteur);
					I_bresenham(img, xP, yP, xQ, yQ);
				}
			}
			*/
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

    FreeListPoints(PointsPolygone);
    //FreeListPoints(PointsPolygone);
		return 0;
	}
}
