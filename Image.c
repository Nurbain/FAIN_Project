/*====================================================*\
  Vendredi 8 novembre 2013
  Arash HABIBI
  Image.c
\*====================================================*/

#include "Image.h"

//------------------------------------------------------------------------

Color C_new(float red, float green, float blue)
{
	Color c;
	c._red = red;
	c._green = green;
	c._blue = blue;
	return c;
}

//------------------------------------------------------------------------

void C_check(Color c, char *message)
{
	fprintf(stderr,"%s : %f %f %f\n",message,c._red,c._green,c._blue);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

Image* I_new(int width, int height)
{
	Image *img_new = (Image*)malloc(sizeof(Image));
	img_new->_width = width;
	img_new->_height = height;
	img_new->_xzoom = 0;
	img_new->_yzoom = 0;
	img_new->_zoom = 1.0;

	img_new->_xoffset=0;
	img_new->_yoffset=0;

	img_new->_current_color = C_new(255,255,255);

	img_new->_buffer = (Color**)calloc(width,sizeof(Color*));

	int x;
	for(x=0;x<width;x++)
		img_new->_buffer[x] = (Color*)calloc(height,sizeof(Color));

	return img_new;
}

//------------------------------------------------------------------------

static void _plot(Image *img, int x, int y, Color c)
{
	img->_buffer[x][y] = c;
}

//-----

static int _isPpm(char *imagefilename)
{
	FILE *imagefile;
	imagefile = fopen(imagefilename,"r");
	if(imagefile==NULL) {perror(imagefilename); exit(1); }

	else
	{
		int c1 = fgetc(imagefile);
		int c2 = fgetc(imagefile);
		fclose(imagefile);

		if((c1=='P')&&(c2=='6'))	return 1;
		else						return 0;
	}
}

//-----

Image* I_read(char *imagefilename)
{
	Image *img;
	char command[100];

	if(_isPpm(imagefilename))	sprintf(command,"cp %s input.ppm",imagefilename);
	else					sprintf(command,"convert %s input.ppm",imagefilename);

	int stat = system(command);
	if(stat!=0)
	{
		fprintf(stderr,"Convert : %s -> input.ppm impossible conversion.\n", imagefilename);
		exit(1);
	}
	else
	{
		Ppm ppm = PPM_nouv("input.ppm", PPM_LECTURE);
		system("rm input.ppm");

		fprintf(stderr,"%d x %d\n",PPM_largeur(ppm),PPM_hauteur(ppm));

		if(ppm!=NULL)
		{
			img = I_new(PPM_largeur(ppm),PPM_hauteur(ppm));
			int nb_bits=ppm->_nb_bits;
			int valmax = ppm->_valmax;

			int nb_pixels = img->_width*img->_height;

			if(nb_bits <= 8)
			{
				unsigned char *donnees = (unsigned char*)calloc(3*nb_pixels,sizeof(unsigned char));
				PPM_lectureDonneesChar(ppm, donnees);

				int x,y;
				for(y=0;y<img->_height;y++)
					for(x=0;x<img->_width;x++)
					{
						int indice = (img->_height-y)*img->_width + x;
						Color c = C_new((1.0*donnees[3*indice  ])/valmax,
										(1.0*donnees[3*indice+1])/valmax,
										(1.0*donnees[3*indice+2])/valmax);
						_plot(img,x,y,c);
					}
			}
			else
			{
				unsigned short *donnees = (unsigned short*)calloc(3*nb_pixels,sizeof(unsigned short));
				PPM_lectureDonneesShort(ppm, donnees);
				int x,y;
				for(y=0;y<img->_height;y++)
					for(x=0;x<img->_width;x++)
					{
						int indice = (img->_height-y)*img->_width + x;
						Color c = C_new((1.0*donnees[3*indice  ])/valmax,
										(1.0*donnees[3*indice+1])/valmax,
										(1.0*donnees[3*indice+2])/valmax);
						img->_buffer[x][y] = c;
					}
			}
			PPM_fermeture(ppm);
			return(img);
		}
		else
			return(NULL);
	}
}

//------------------------------------------------------------------------

void I_fill(Image *img, Color c)
{
	int x,y;
	for(x=0;x<img->_width;x++)
		for(y=0;y<img->_height;y++)
			img->_buffer[x][y]=c;
}

//------------------------------------------------------------------------

void I_checker(Image *img, Color c1, Color c2, int step)
{
	int x,y;
	for(x=0;x<img->_width;x++)
		for(y=0;y<img->_height;y++)
		{
			int n_x = x/step;
			int n_y = y/step;
			if((n_x+n_y)%2==0)	_plot(img,x,y,c1);
			else				_plot(img,x,y,c2);
		}
}

//------------------------------------------------------------------------

void I_changeColor(Image *img, Color c)
{
	img->_current_color = c;
}

//------------------------------------------------------------------------

void I_plot(Image *img, int x, int y)
{
	if((x>=0)&&(x<img->_width)&&
	   (y>=0)&&(y<img->_height))
		img->_buffer[x][y] = img->_current_color;
	else
	{
		fprintf(stderr,"I_plot : ERROR !!!\n");
		fprintf(stderr,"x (=%d) must be in the [%d,%d] range and\n", x, 0, img->_width);
		fprintf(stderr,"y (=%d) must be in the [%d,%d] range\n", y, 0, img->_height);
	}
}

//------------------------------------------------------------------------

void I_plotColor(Image *img, int x, int y, Color c)
{
	if((x>=0)&&(x<img->_width)&&
	   (y>=0)&&(y<img->_height))
		img->_buffer[x][y] = c;
	else
	{
		fprintf(stderr,"I_plotColor : ERROR !!!\n");
		fprintf(stderr,"x (=%d) must be in the [%d,%d] range and\n", x, 0, img->_width);
		fprintf(stderr,"y (=%d) must be in the [%d,%d] range\n", y, 0, img->_height);
	}
}

//------------------------------------------------------------------------
// Changement de repère

static void _windowToImage(Image *img, int xwin, int ywin, int *ximg, int *yimg)
{

	*ximg = img->_xoffset + img->_xzoom + (xwin-img->_xzoom) / img->_zoom;
	*yimg = img->_yoffset + img->_yzoom + (ywin-img->_yzoom) / img->_zoom;
}

//-----
// Changement de repère inverse
/*
static void _imageToWindow(Image *img, int ximg, int yimg, int *xwin, int *ywin)
{

	*xwin = img->_xoffset + img->_xzoom + (ximg-img->_xzoom-img->_xoffset) * img->_zoom;
	*ywin = img->_yoffset + img->_yzoom + (yimg-img->_yzoom-img->_yoffset) * img->_zoom;
}
*/
//-----

void I_focusPoint(Image *img, int xwin, int ywin)
{
	int dx = xwin - img->_xzoom;
	int dy = ywin - img->_yzoom;
	img->_xoffset -= dx*(1-1.0/img->_zoom);
	img->_yoffset -= dy*(1-1.0/img->_zoom);
	img->_xzoom = xwin;
	img->_yzoom = ywin;
}

//------------------------------------------------------------------------

void I_zoomInit(Image *img)
{
	img->_xoffset = 0;
	img->_yoffset = 0;
	img->_zoom = 1.0;
}

//------------------------------------------------------------------------

void I_zoom(Image *img, double zoom_coef)
{
	img->_zoom = img->_zoom * zoom_coef;
}

//------------------------------------------------------------------------

void I_move(Image *img, int x, int y)
{
	img->_xoffset += x;
	img->_yoffset += y;
}

//------------------------------------------------------------------------

void I_draw(Image *img)
{
	glBegin(GL_POINTS);
	int xwin, ywin, ximg, yimg;
	for(xwin=0;xwin<img->_width;xwin++)
		for(ywin=0;ywin<img->_height;ywin++)
		{
			_windowToImage(img, xwin, ywin, &ximg, &yimg);
			Color c;
			if((ximg>=0)&&(ximg<img->_width)&&
			   (yimg>=0)&&(yimg<img->_height))
				c = img->_buffer[ximg][yimg];
			else
				c = C_new(0,0,0);

			glColor3f(c._red,c._green,c._blue);
			glVertex2i(xwin,ywin);
		}
	glEnd();
}

//------------------------------------------------------------------------


//------------------------------------------------------------------------
//------------------- 	FONCTION BRESENHAM     ---------------------------
//------------------------------------------------------------------------
//Fonctions vu en TD

//Dessine dans le 1er octant, x et y supposé y appartenant
void I_bresenhamOrigin(Image *img, int x, int y) {
	if (!img) {
		return;
	}

	int dx = x, dy = y;
	int incrd1 = 2*dy, incrd2 = 2*(dy - dx);
	int x_cur = 0, y_cur = 0;
	int d = 2*dy - dx;

	do {
		I_plot(img, x_cur, y_cur);
		x_cur += 1;
		if (d < 0) {
			d += incrd1;
		} else {
			y_cur += 1;
			d += incrd2;
		}
	} while (x_cur < x);
}


//Ramene une droite venant du Nieme octant dans le 1er
void I_bresenhamZ2_to_1Oct(int xA, int yA, int xB, int yB, int *O1xA, int *O1yA, int *O1xB, int *O1yB) {
	int _O1xA = xA, _O1xB = xB, _O1yA = yA, _O1yB = yB;
	int dx = xB-xA, dy = yB-yA;
	if (dx < 0) {
		_O1xA *= -1; _O1xB *= -1;
		dx = _O1xB - _O1xA;
	}
	if (dy < 0) {
		_O1yA *= -1; _O1yB *= -1;
		dy = _O1yB - _O1yA;
	}
	if (dy > dx) {
		int tmp;
		tmp = _O1xA;
		_O1xA = _O1yA;
		_O1yA = tmp;
		tmp = _O1xB;
		_O1xB = _O1yB;
		_O1yB = tmp;
	}
	*O1xA = _O1xA; *O1yA = _O1yA; *O1xB = _O1xB; *O1yB = _O1yB;
}

//Ramene la droite du 1er octant a sa place d'origine
void I_bresenham1Oct_to_Z2(int xA, int yA, int xB, int yB, int O1x, int O1y, int *x, int *y) {
	int _x, _y;
	if (abs(xB - xA) > abs(yB - yA)) {
		_x = O1x; _y = O1y;
	} else {
		_x = O1y; _y = O1x;
	}
	if (xB > xA) {
		*x = _x;
	} else {
		*x = -_x;
	}
	if (yB > yA) {
		*y = _y;
	} else {
		*y = -_y;
	}
}

//Dessine la droite de bresenham
void I_bresenham(Image *img, int xA, int yA, int xB, int yB) {
	if (!img) {
		return;
	}

	int O1xA, O1yA, O1xB, O1yB;
	I_bresenhamZ2_to_1Oct(xA, yA, xB, yB, &O1xA, &O1yA, &O1xB, &O1yB);
	int dx = O1xB - O1xA, dy = O1yB - O1yA;
	int incrd1 = 2*dy, incrd2 = 2*(dy - dx);
	int O1x = O1xA, O1y = O1yA, d = 2*dy - dx, x, y;
	do {
		I_bresenham1Oct_to_Z2(xA, yA, xB, yB, O1x, O1y, &x, &y);
		I_plot(img, x, y);
		O1x++;
		if (d < 0) {
			d += incrd1;
		} else {
			O1y++; d+= incrd2;
		}
	} while (O1x < O1xB);
}


void DrawAllPoints(Image *img, int points[], int pointsSize){
	if (!img) {
		return;
	}

	if (pointsSize%2!=0) {
		printf("Mauvaise longueur de point donné");
		return;
	}

	for(int i=2;i<pointsSize;i=i+2){
		I_bresenham(img,points[i-2],points[i-1],points[i],points[i+1]);
	}
}
