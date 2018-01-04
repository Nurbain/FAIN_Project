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

//###############################################################################
//###############################################################################
//###############################################################################

Point P_new(int x, int y){
	Point point;
	point.x = x;
	point.y = y;

	return point;
}

//------------------ Affiche ligne brisée ------------------

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

//Dessine les droites de bresenham suivant un tableau de point donnés
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


//------------------ Dessin par souris ------------------

//Dessine bresenham entre le dernier des points de la list et x,y
void DrawNewPoints(Image *img, ListePoints* list,int x, int y){
	if(list == NULL){
		return;
	}

	I_bresenham(img, list->end->point.x,list->end->point.y,x,y);
}

//Dessine toutes les bresenham de la liste
void DrawAllListPoints(Image *img, ListePoints* list){
	if(list == NULL || list->length < 2){
		return;
	}

	I_fill(img, C_new(0.f, 0.f, 0.f));
	Points* tmp = list->head;
	while(tmp->next != NULL){
		I_bresenham(img, tmp->point.x,tmp->point.y,tmp->next->point.x,tmp->next->point.y);
		tmp = tmp->next;
	}
}

//------------------ Remplissage scan-line ------------------
int equalColor(Color c1, Color c2){
	if(c1._red != c2._red || c1._blue != c2._blue || c1._green != c2._green){
		return 0;
	}
	else return 1;
}

//Trouve la boundingBox du polygone
void FindBoundingBox(ListePoints* list, Point* boundingBox){
	if(list == NULL){
		return;
	}

	int min_y = 800,min_x= 800,max_y= 0,max_x= 0;

	Points* parcours = list->head;
	while(parcours != NULL){
		if(parcours->point.x <= min_x){
			min_x = parcours->point.x;
		}
		if(parcours->point.y <= min_y){
			min_y = parcours->point.y;
		}
		if(parcours->point.x >= max_x){
			max_x = parcours->point.x;
		}
		if(parcours->point.y >= max_y){
			max_y = parcours->point.y;
		}

		parcours = parcours->next;
	}

	boundingBox[0] = P_new(min_x,min_y);
	boundingBox[1] = P_new(max_x,max_y);
}

int InterieurPloygone(Image *img,ListePoints* list, Point p, int xMax){
	int instersection = 0;
	for(int i = p.x ; i <= xMax ; i++){
		//Interescte un point autre que noir
		if(!equalColor(img->_buffer[i][p.y],C_new(0.f,0.f,0.f))){
			Points* sommet = isVertex(list, i, p.y);
			// Interesction de sommet
			if(sommet != NULL){
				//2.3 document du prof
				if(sommet->previous == NULL && sommet->next == NULL){
					printf("Y'a qu'un sommet la");
				}else if(sommet->previous == NULL){ //si tete
					if(sommet->point.y >= list->end->point.y)
						instersection++;
					if(sommet->point.y >= sommet->next->point.y)
						instersection++;
				}else if(sommet->next == NULL){//si queue
					if(sommet->point.y >= sommet->previous->point.y)
						instersection++;
					if(sommet->point.y >= list->head->point.y)
						instersection++;
				}
				else{
					if(sommet->point.y >= sommet->previous->point.y)
						instersection++;
					if(sommet->point.y >= sommet->next->point.y)
						instersection++;
				}
			}else // Interesection banale
				instersection++;

			//Evite quand une ligne est sur plusieurs pixel
			while(!equalColor(img->_buffer[i+1][p.y],C_new(0.f,0.f,0.f))){
				i++;
			}
		}

	}
	if(instersection%2 == 1){
		return 1;
	}

	return 0;
}

int IntersectSuperieur(ListePoints* list, Points* sommet){
	if(sommet != NULL){
		//2.3 document du prof
		if(sommet->previous == NULL && sommet->next == NULL){
			printf("Y'a qu'un sommet la");
		}else if(sommet->previous == NULL){ //si tete
			if(sommet->point.y >= list->end->point.y)
				return 1;
			if(sommet->point.y >= sommet->next->point.y)
				return 1;
		}else if(sommet->next == NULL){//si queue
			if(sommet->point.y >= sommet->previous->point.y)
				return 1;
			if(sommet->point.y >= list->head->point.y)
				return 1;
		}
		else{
			if(sommet->point.y >= sommet->previous->point.y)
				return 1;
			if(sommet->point.y >= sommet->next->point.y)
				return 1;
		}
		return 0;
	}else
		return 0;

}

int isSameAbscisseAsSommet(ListePoints* list,int x){
	if(list == NULL){
		return 0;
	}
	Points* tmp = list->head;

	while(tmp != NULL){
		if(tmp->point.x == x)
			return 1;
		tmp = tmp->next;
	}

	return 0;
}

/*
3.2
void fill(Image *img,ListePoints* list){
	Point boundingBox[2];
	FindBoundingBox(list,boundingBox);
	for(int j = boundingBox[0].y; j <= boundingBox[1].y ; j++){
		for(int i = boundingBox[0].x; i <= boundingBox[1].x ; i++){
			Point actuel = P_new(i,j);
			if(InterieurPloygone(img,list,actuel,boundingBox[1].x)){
				I_plot(img,i,j);
			}
		}
	}
}*/

/* 3.3 1)
void fill(Image *img,ListePoints* list){
	Point boundingBox[2];
	FindBoundingBox(list,boundingBox);
	int Xintersect[100];
	for(int j = boundingBox[0].y; j <= boundingBox[1].y ; j++){
		int index = 0;
		//On regarde les intersections sur cette scan-ligne
		for(int i = boundingBox[0].x; i <= boundingBox[1].x ; i++){
			if(!equalColor(img->_buffer[i][j],C_new(0.f,0.f,0.f))){
				Points* sommet = isVertex(list,i,j);
				if(sommet != NULL){
					//Interescte ailleur que l'extremité superieur
					if(IntersectSuperieur(list,sommet)){
						Xintersect[index] = i;
						index++;
					}
				}else{
					Xintersect[index] = i;
					index++;
				}
				//Evite quand une ligne est sur plusieurs pixel
				while(!equalColor(img->_buffer[i+1][j],C_new(0.f,0.f,0.f))){
					i++;
				}
			}
		}
		int est_allume = 0;
		for(int x = boundingBox[0].x; x <= boundingBox[1].x ; x++ ){
			for(int k = 0;k<index;k++){
				if(Xintersect[k] == x){
					est_allume = !est_allume;
				}
			}

			if(est_allume){
				I_plot(img,x,j);
			}
		}
	}
}*/

void fill(Image *img,ListePoints* list){
	Point boundingBox[2];
	FindBoundingBox(list,boundingBox);
	int Xintersect[100];
	for(int j = boundingBox[0].y; j <= boundingBox[1].y ; j++){
		int index = 0;
		//On regarde les intersections sur cette scan-ligne
		for(int i = boundingBox[0].x; i <= boundingBox[1].x ; i++){
			if(!equalColor(img->_buffer[i][j],C_new(0.f,0.f,0.f))){
				Points* sommet = isVertex(list,i,j);
				if(sommet != NULL){
					//Interescte ailleur que l'extremité superieur
					if(IntersectSuperieur(list,sommet)){
						Xintersect[index] = i;
						index++;
					}
				}else{
					Xintersect[index] = i;
					index++;
				}
				//Evite quand une ligne est sur plusieurs pixel
				while(!equalColor(img->_buffer[i+1][j],C_new(0.f,0.f,0.f))){
					i++;
				}
			}
		}
		int est_allume = 0;
		for(int x = boundingBox[0].x; x <= boundingBox[1].x ; x++ ){
			for(int k = 0;k<index;k++){
				if(Xintersect[k] == x){
					est_allume = !est_allume;
				}
			}

			if(est_allume){
				I_plot(img,x,j);
			}
		}
	}
}

//Probleme quand un sommet est sur plusieurs pixel
//Sommet sur l'horizontale vérifier que les précédents et suivant sont de chaque coté
//Remplie le polygone avec la méthode scan-line
void fillByScanLine(Image *img,ListePoints* list){
	Point boundingBox[2];
	FindBoundingBox(list,boundingBox);

	printf(" min : %d %d    max :%d %d\n", boundingBox[0].x,boundingBox[0].y,boundingBox[1].x,boundingBox[1].y);
	int instersection = 0;
	for(int j = boundingBox[0].y; j <= boundingBox[1].y ; j++){
		instersection=0;
		for(int i = boundingBox[0].x; i <= boundingBox[1].x ; i++){
			if(!equalColor(img->_buffer[i][j],C_new(0.f,0.f,0.f))){
				if(!isVertex(list,i,j)){
					instersection++;
					while(!equalColor(img->_buffer[i+1][j],C_new(0.f,0.f,0.f))){
						i++;
					}
				}
			}else{
				if(instersection%2 == 1){
					I_plot(img,i,j);
				}
			}
		}
	}

}

//Verifie que le point donné est un somet de la liste
Points* isVertex(ListePoints* list, int x, int y){
	if(list == NULL){
		return NULL;
	}
	Points* tmp = list->head;

	while(tmp != NULL){
		if(tmp->point.x == x && tmp->point.y == y)
			return tmp;
		tmp = tmp->next;
	}

	return NULL;
}

//------------------ Insert et Suppr Sommets ------------------
// Utilisation d'une liste doublement chainé

//Init la liste chainée
ListePoints* initListPoints(){
	ListePoints* newlist = malloc(sizeof *newlist);
	if(newlist != NULL){
		newlist->length = 0;
		newlist->end = NULL;
		newlist->head = NULL;
	}
	return newlist;
}

//Ajoute un sommet a la fin
ListePoints* push_Back_Point(ListePoints* list, int x, int y){

	if(list != NULL){
		Points *newpoint = malloc(sizeof *newpoint);
		if(newpoint != NULL){
			newpoint->point = P_new(x,y);
			newpoint->next= NULL;
			if(list->end == NULL){
				newpoint->previous = NULL;
				list->head = newpoint;
				list->end = newpoint;
			}
			else{
				list->end->next = newpoint;
				newpoint->previous = list->end;
				list->end = newpoint;
			}
			list->length++;
		}
	}

	return list;
}

//Supprime un sommet a la fin
ListePoints* remove_Back_Point(ListePoints* list){
	if(list != NULL){
		Points* tmp = list->end;
		list->end = tmp->previous;
		list->end->next = NULL;
		free(tmp);
	}
	return list;
}

//Supprime le sommet trouvé
ListePoints* remove_Point(ListePoints* list, int x, int y){
	if(list != NULL){
		Points* tmp = list->head;
		int found = 0;
		while(tmp != NULL && !found){
			if(tmp->point.x == x && tmp->point.y == y){
				if(tmp->previous == NULL){
					list->head = tmp->next;
					list->head->previous = NULL;
				}
				else if(tmp->next == NULL){
					list->end = tmp->previous;
					list->end->next = NULL;
				}
				else{
					tmp->previous->next = tmp->next;
					tmp->next->previous = tmp->previous;
				}
				free(tmp);
				list->length--;
				found = 1;
			}
			else{
				tmp = tmp->next;
			}
		}
		return list;
	}
	return list;
}

//Insert le sommet
ListePoints* insert_Point(ListePoints* list, Points** edgePoint){
	if(list == NULL ||edgePoint == NULL){
		return list;
	}

	Points* tmp = list->head;
	while(tmp != NULL){
		if(tmp->point.x == edgePoint[0]->point.x && tmp->point.y == edgePoint[0]->point.y){
			//On verfifie que le point suivant est bien celui donné en argument
			if(tmp->next != NULL && tmp->next->point.x == edgePoint[1]->point.x && tmp->next->point.y == edgePoint[1]->point.y){
				Points *newpoint = malloc(sizeof *newpoint);
				if(newpoint != NULL){
					int x = (edgePoint[0]->point.x + edgePoint[1]->point.x) /2;
					int y = (edgePoint[0]->point.y + edgePoint[1]->point.y) /2;
					newpoint->point = P_new(x,y);
					tmp->next->previous = newpoint;
					newpoint->next = tmp->next;
					tmp->next = newpoint;
					newpoint->previous = tmp;
					edgePoint[1]=newpoint;
					return list;
				}
				return list;
			}
		}
		tmp = tmp->next;
	}
	return list;
}

//------------------ Séléction par clavier ------------------
#define Size_Select 3

//VERTEX
//Dessine le carré qui montre la selection
void selectSommet(Image* img,int x, int y, Color* save){
	Color c = C_new(255.f, 0.f, 0.f);
	int index = 0;
	for(int j=x-Size_Select; j<x+Size_Select;j++){
		for(int i=y-Size_Select; i<y+Size_Select;i++){
			save[index] = img->_buffer[j][i];
			index++;
			I_plotColor(img,j,i,c);
		}
	}
}

//Redessine comme avant la selection du sommet
void deselectSommet(Image* img,int x, int y,Color* save){
	if(save == NULL){
		return;
	}

	int index = 0;
	for(int j=x-Size_Select ; j<x+Size_Select;j++){
		for(int i=y-Size_Select; i<y+Size_Select;i++){
			I_plotColor(img,j,i,save[index]);
			index++;
		}
	}
}

//Bouge suivant la direction le x ou y du point actuel
void MooveSommet(Points* Actuel, int direction){
	switch (direction) {
		case 1:
			Actuel->point.x = Actuel->point.x-1;
		break;
		case 2:
			Actuel->point.y = Actuel->point.y+1;
		break;
		case 3:
			Actuel->point.x = Actuel->point.x+1;
		break;
		case 4:
			Actuel->point.y = Actuel->point.y-1;
		break;
	}
}

//Efface les lignes lié au point
void I_bresenhamDelete(Image *img, Points* ActualPoint){
	if(ActualPoint == NULL)
		return;

	Color tmp = img->_current_color;
	img->_current_color = C_new(0.f, 0.f, 0.f);

	if(ActualPoint->previous != NULL)
		I_bresenham(img,ActualPoint->previous->point.x,ActualPoint->previous->point.y,ActualPoint->point.x,ActualPoint->point.y);

	if(ActualPoint->next != NULL)
		I_bresenham(img,ActualPoint->point.x,ActualPoint->point.y,ActualPoint->next->point.x,ActualPoint->next->point.y);

	img->_current_color = tmp;
}

//EDGE
//Dessine l'edge en couleur
void selectEdge(Image* img,Points** edgePoint){

	Color tmp = img->_current_color;
	img->_current_color = C_new(255.f, 0.f, 0.f);

	I_bresenham(img,edgePoint[0]->point.x,edgePoint[0]->point.y,edgePoint[1]->point.x,edgePoint[1]->point.y);

	img->_current_color = tmp;
}

//------------------ Séléction par souris ------------------
//Calcul la distance euclidienne entre deux point
float distanceVertex(Point a, Point b){
	float distance = (a.x-b.x) * (a.x-b.x) + (a.y-b.y) * (a.y-b.y);
	return sqrt(distance);
}

//Choisi le point le plus pret de coordonées données
Points* closestVertex(ListePoints* list, Points* ActualPoint, int x, int y){
	if(list == NULL || list->head == NULL){
		return ActualPoint;
	}
	Point clicMouse = P_new(x,y);
	Points* save = list->head;
	ActualPoint = save;
	float minDistance = distanceVertex(save->point,clicMouse);
	while(save != NULL){
		float tmpDistance = distanceVertex(save->point,clicMouse);
		if(tmpDistance<minDistance){
			minDistance = tmpDistance;
			ActualPoint = save;
		}
		save = save->next;
	}
	return ActualPoint;
}


//Choisi la droite la plus pret d'un point
void closestEdge(ListePoints* list, Points** edgePoint, int x, int y){

}
