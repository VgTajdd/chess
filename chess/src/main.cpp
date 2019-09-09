// problema4

// Programa correspondiente al problema 4, el muestra un tablero de ajedrez desarrollado con OpenGL,
// cuyas piezas son ingresadas en modo de texto, a travez de la cadena initPosition

#include <iostream>
#include <stdlib.h>
#include <string>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define NDEBUG

#include "glUtils/imageloader.h"

using namespace std;

//Cadena que se lee al iniciar
string orden;
//vector equivalente
char letras[64];

void handleKeypress( unsigned char key, int x, int y )
{
	switch ( key )
	{
		case 27: //Tecla Escape
			exit( 0 );
	}
}

GLuint loadTexture( Image* image )
{
	GLuint textureId;
	glGenTextures( 1, &textureId );
	glBindTexture( GL_TEXTURE_2D, textureId );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels );               //The actual pixel data
	return textureId;
}

GLuint _textureId;
GLuint _peonb;
GLuint _peonn;
GLuint _torreb;
GLuint _torren;
GLuint _alfilb;
GLuint _alfiln;
GLuint _caballob;
GLuint _caballon;
GLuint _reinab;
GLuint _reinan;
GLuint _reyb;
GLuint _reyn;


void initRendering()
{
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_NORMALIZE );
	glEnable( GL_COLOR_MATERIAL );
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );//luz de fondo

	//Cargamos las imagenes correspondientes a las texturas de las piezas
	Image* image = loadBMP( "assets/peonb.bmp" );
	_peonb = loadTexture( image );
	delete image;

	Image* image2 = loadBMP( "assets/peonn.bmp" );
	_peonn = loadTexture( image2 );
	delete image2;

	Image* image3 = loadBMP( "assets/torreb.bmp" );
	_torreb = loadTexture( image3 );
	delete image3;

	Image* image4 = loadBMP( "assets/torren.bmp" );
	_torren = loadTexture( image4 );
	delete image4;

	Image* image5 = loadBMP( "assets/alfilb.bmp" );
	_alfilb = loadTexture( image5 );
	delete image5;

	Image* image6 = loadBMP( "assets/alfiln.bmp" );
	_alfiln = loadTexture( image6 );
	delete image6;

	Image* image7 = loadBMP( "assets/caballob.bmp" );
	_caballob = loadTexture( image7 );
	delete image7;

	Image* image8 = loadBMP( "assets/caballon.bmp" );
	_caballon = loadTexture( image8 );
	delete image8;

	Image* image9 = loadBMP( "assets/reinab.bmp" );
	_reinab = loadTexture( image9 );
	delete image9;

	Image* image10 = loadBMP( "assets/reinan.bmp" );
	_reinan = loadTexture( image10 );
	delete image10;

	Image* image11 = loadBMP( "assets/reyb.bmp" );
	_reyb = loadTexture( image11 );
	delete image11;

	Image* image12 = loadBMP( "assets/reyn.bmp" );
	_reyn = loadTexture( image12 );
	delete image12;
}

void handleResize( int w, int h )
{
	glViewport( 0, 0, w, h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45.0, (float) w / (float) h, 1.0, 200.0 );
}

void iniciarEscena()
{

	//Inicializa los La camara y las luces.
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glTranslatef( 0.0f, 0.0f, -10.0f );

	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ambientLight );

	GLfloat directedLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat directedLightPos[] = { -10.0f, 15.0f, 20.0f, 0.0f };
	glLightfv( GL_LIGHT0, GL_DIFFUSE, directedLight );
	glLightfv( GL_LIGHT0, GL_POSITION, directedLightPos );

	//Leyenda :
	/*
	p = peón
	r = torre
	b = alfil
	n = caballo
	q = reina
	k = rey*/
	int cont = 0, contin;

	//Creamos un vector equivalente a la cadena initPosition que se ingresa
	for ( int l = 0; l < orden.length(); l++ )
	{

		switch ( orden[l] )
		{
			case 'p':
				contin = cont;
				cont = cont + 1;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = 'p';
				}
				break;
			case 'r':
				contin = cont;
				cont = cont + 1;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = 'r';
				}
				break;
			case 'b':
				contin = cont;
				cont = cont + 1;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = 'b';
				}
				break;
			case 'n':
				contin = cont;
				cont = cont + 1;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = 'n';
				}
				break;
			case 'q':
				contin = cont;
				cont = cont + 1;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = 'q';
				}
				break;
			case 'k':
				contin = cont;
				cont = cont + 1;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = 'k';
				}
				break;

			case 'P':
				contin = cont;
				cont = cont + 1;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = 'P';
				}
				break;
			case 'R':
				contin = cont;
				cont = cont + 1;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = 'R';
				}
				break;
			case 'B':
				contin = cont;
				cont = cont + 1;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = 'B';
				}
				break;
			case 'N':
				contin = cont;
				cont = cont + 1;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = 'N';
				}
				break;
			case 'Q':
				contin = cont;
				cont = cont + 1;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = 'Q';
				}
				break;
			case 'K':
				contin = cont;
				cont = cont + 1;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = 'K';
				}
				break;
			case '1':
				contin = cont;
				cont = cont + 1;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = '*';
				}
				break;
			case '2':
				contin = cont;
				cont = cont + 2;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = '*';
				}
				break;
			case '3':
				contin = cont;
				cont = cont + 3;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = '*';
				}
				break;
			case '4':
				contin = cont;
				cont = cont + 4;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = '*';
				}
				break;
			case '5':
				contin = cont;
				cont = cont + 5;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = '*';
				}
				break;
			case '6':
				contin = cont;
				cont = cont + 6;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = '*';
				}
				break;
			case '7':
				contin = cont;
				cont = cont + 7;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = '*';
				}
				break;
			case '8':
				contin = cont;
				cont = cont + 8;
				for ( int s = contin; s < cont; s++ )
				{
					letras[s] = '*';
				}
				break;

			default:
				break;

		}
	}

	//Creamos una matriz equivalente a la cadena initPosition que se ingresa
	char matlet[8][8];
	cont = 0;
	for ( int i = 0; i < 8; i++ )
	{
		for ( int j = 0; j < 8; j++ )
		{
			matlet[i][j] = letras[cont];
			cont++;
		}
	}

	int matPos[8][8];//representa el tablero
	int valor = 1, valant = 0;
	for ( int i = 0; i < 8; i++ )
	{
		for ( int j = 0; j < 8; j++ )
		{
			valant = valor;
			if ( valor == 1 )
			{
				matPos[i][j] = 0;
				valor = 0;
			}
			else
			{
				matPos[i][j] = 1;
				valor = 1;
			}
			if ( j == 7 )
			{
				valor = valant;
			}
		}
	}
	//calculamos centros de las cuadriculas, y graficamos
	float centrosx[8][8], centrosy[8][8];
	for ( int i = 0; i < 8; i++ )
	{
		for ( int j = 0; j < 8; j++ )
		{
			glDisable( GL_TEXTURE_2D );
			centrosy[i][j] = 3.5 - float( i );
			centrosx[i][j] = float( j ) - 3.5;

			if ( matPos[i][j] == 0 )
			{
				glColor3f( 0.960784f, 0.960784f, 0.862745f );
			}

			else
			{
				glColor3f( 1.0f, 0.7f, 0.3f );
			}

			glBegin( GL_QUADS );
			glNormal3f( 0.0, 0.0f, 1.0f );

			glVertex3f( centrosx[i][j] - 0.5f, centrosy[i][j] - 0.5f, 0.0f );

			glVertex3f( centrosx[i][j] - 0.5f, centrosy[i][j] + 0.5f, 0.0f );

			glVertex3f( centrosx[i][j] + 0.5f, centrosy[i][j] + 0.5f, 0.0f );

			glVertex3f( centrosx[i][j] + 0.5f, centrosy[i][j] - 0.5f, 0.0f );
			glEnd();


			glColor3f( 1.0f, 1.0f, 1.0f );
			switch ( matlet[i][j] )
			{
				///
				case 'p':
					glEnable( GL_TEXTURE_2D );
					glBindTexture( GL_TEXTURE_2D, _peonn );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glBegin( GL_QUADS );
					glNormal3f( 0.0, 0.0f, 1.0f );

					glTexCoord2f( 0.0f, 0.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] - 0.35f, 0.1f );

					glTexCoord2f( 0.0f, 1.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 1.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 0.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] - 0.35f, 0.1f );
					glDisable( GL_TEXTURE_2D );
					glEnd();
					break;
				case 'r':
					glEnable( GL_TEXTURE_2D );
					glBindTexture( GL_TEXTURE_2D, _torren );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glBegin( GL_QUADS );
					glNormal3f( 0.0, 0.0f, 1.0f );

					glTexCoord2f( 0.0f, 0.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] - 0.35f, 0.1f );

					glTexCoord2f( 0.0f, 1.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 1.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 0.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] - 0.35f, 0.1f );
					glDisable( GL_TEXTURE_2D );
					glEnd();
					break;
				case 'b':
					glEnable( GL_TEXTURE_2D );
					glBindTexture( GL_TEXTURE_2D, _alfiln );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glBegin( GL_QUADS );
					glNormal3f( 0.0, 0.0f, 1.0f );

					glTexCoord2f( 0.0f, 0.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] - 0.35f, 0.1f );

					glTexCoord2f( 0.0f, 1.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 1.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 0.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] - 0.35f, 0.1f );
					glDisable( GL_TEXTURE_2D );
					glEnd();
					break;
				case 'n':
					glEnable( GL_TEXTURE_2D );
					glBindTexture( GL_TEXTURE_2D, _caballon );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glBegin( GL_QUADS );
					glNormal3f( 0.0, 0.0f, 1.0f );

					glTexCoord2f( 0.0f, 0.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] - 0.35f, 0.1f );

					glTexCoord2f( 0.0f, 1.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 1.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 0.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] - 0.35f, 0.1f );
					glDisable( GL_TEXTURE_2D );
					glEnd();
					break;
				case 'q':
					glEnable( GL_TEXTURE_2D );
					glBindTexture( GL_TEXTURE_2D, _reinan );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glBegin( GL_QUADS );
					glNormal3f( 0.0, 0.0f, 1.0f );

					glTexCoord2f( 0.0f, 0.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] - 0.35f, 0.1f );

					glTexCoord2f( 0.0f, 1.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 1.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 0.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] - 0.35f, 0.1f );
					glDisable( GL_TEXTURE_2D );
					glEnd();
					break;
				case 'k':
					glEnable( GL_TEXTURE_2D );
					glBindTexture( GL_TEXTURE_2D, _reyn );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glBegin( GL_QUADS );
					glNormal3f( 0.0, 0.0f, 1.0f );

					glTexCoord2f( 0.0f, 0.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] - 0.35f, 0.1f );

					glTexCoord2f( 0.0f, 1.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 1.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 0.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] - 0.35f, 0.1f );
					glDisable( GL_TEXTURE_2D );
					glEnd();
					break;
				case 'P':
					glEnable( GL_TEXTURE_2D );
					glBindTexture( GL_TEXTURE_2D, _peonb );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glBegin( GL_QUADS );
					glNormal3f( 0.0, 0.0f, 1.0f );

					glTexCoord2f( 0.0f, 0.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] - 0.35f, 0.1f );

					glTexCoord2f( 0.0f, 1.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 1.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 0.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] - 0.35f, 0.1f );
					glDisable( GL_TEXTURE_2D );
					glEnd();
					break;
				case 'R':
					glEnable( GL_TEXTURE_2D );
					glBindTexture( GL_TEXTURE_2D, _torreb );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glBegin( GL_QUADS );
					glNormal3f( 0.0, 0.0f, 1.0f );

					glTexCoord2f( 0.0f, 0.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] - 0.35f, 0.1f );

					glTexCoord2f( 0.0f, 1.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 1.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 0.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] - 0.35f, 0.1f );
					glDisable( GL_TEXTURE_2D );
					glEnd();
					break;
				case 'B':
					glEnable( GL_TEXTURE_2D );
					glBindTexture( GL_TEXTURE_2D, _alfilb );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glBegin( GL_QUADS );
					glNormal3f( 0.0, 0.0f, 1.0f );

					glTexCoord2f( 0.0f, 0.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] - 0.35f, 0.1f );

					glTexCoord2f( 0.0f, 1.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 1.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 0.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] - 0.35f, 0.1f );
					glDisable( GL_TEXTURE_2D );
					glEnd();
					break;
				case 'N':
					glEnable( GL_TEXTURE_2D );
					glBindTexture( GL_TEXTURE_2D, _caballob );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glBegin( GL_QUADS );
					glNormal3f( 0.0, 0.0f, 1.0f );

					glTexCoord2f( 0.0f, 0.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] - 0.35f, 0.1f );

					glTexCoord2f( 0.0f, 1.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 1.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 0.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] - 0.35f, 0.1f );
					glDisable( GL_TEXTURE_2D );
					glEnd();
					break;
				case 'Q':
					glEnable( GL_TEXTURE_2D );
					glBindTexture( GL_TEXTURE_2D, _reinab );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glBegin( GL_QUADS );
					glNormal3f( 0.0, 0.0f, 1.0f );

					glTexCoord2f( 0.0f, 0.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] - 0.35f, 0.1f );

					glTexCoord2f( 0.0f, 1.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 1.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 0.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] - 0.35f, 0.1f );
					glDisable( GL_TEXTURE_2D );
					glEnd();
					break;
				case 'K':
					glEnable( GL_TEXTURE_2D );
					glBindTexture( GL_TEXTURE_2D, _reyb );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glBegin( GL_QUADS );
					glNormal3f( 0.0, 0.0f, 1.0f );

					glTexCoord2f( 0.0f, 0.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] - 0.35f, 0.1f );

					glTexCoord2f( 0.0f, 1.0f );
					glVertex3f( centrosx[i][j] - 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 1.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] + 0.35f, 0.1f );

					glTexCoord2f( 1.0f, 0.0f );
					glVertex3f( centrosx[i][j] + 0.35f, centrosy[i][j] - 0.35f, 0.1f );
					glDisable( GL_TEXTURE_2D );
					glEnd();
					break;
				default:
					break;
			}

		}
	}

	glutSwapBuffers();
	cout << "\nDibujando escena...";
}

void drawChessBoard( string initPosition )
{
	orden = initPosition;
	glutDisplayFunc( iniciarEscena );
	glutKeyboardFunc( handleKeypress );
	glutReshapeFunc( handleResize );
	glutMainLoop();
}

int main( int argc, char** argv )
{
	cout << "**********" << endl;
	cout << "PROBLEMA 4" << endl;
	cout << "**********" << endl;
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize( 400, 400 );
	glutCreateWindow( "Ajedrez con OpenGl" );
	initRendering();

	string initPosition( "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" );
	drawChessBoard( initPosition );
	return 0;
}

