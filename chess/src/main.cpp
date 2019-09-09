#include <iostream>
#include <string>
#include <ctype.h>
#include "glUtils/imageloader.h"

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

static const int CHESS_SIZE = 8;
static const int CHESS_PIECES_COUNT = 64;

string parsedInitPositions;

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
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f ); // bg color.

	// Loading Images.
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

	Image* imageCHESS_SIZE = loadBMP( "assets/caballon.bmp" );
	_caballon = loadTexture( imageCHESS_SIZE );
	delete imageCHESS_SIZE;

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

void drawPiece( const float x, const float y, const GLuint id )
{
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, id );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glBegin( GL_QUADS );
	glNormal3f( 0.0, 0.0f, 1.0f );

	glTexCoord2f( 0.0f, 0.0f );
	glVertex3f( x - 0.35f, y - 0.35f, 0.01f );

	glTexCoord2f( 0.0f, 1.0f );
	glVertex3f( x - 0.35f, y + 0.35f, 0.01f );

	glTexCoord2f( 1.0f, 1.0f );
	glVertex3f( x + 0.35f, y + 0.35f, 0.01f );

	glTexCoord2f( 1.0f, 0.0f );
	glVertex3f( x + 0.35f, y - 0.35f, 0.01f );
	glDisable( GL_TEXTURE_2D );
	glEnd();
}

string parseChessInput( const string input )
{
	string ans;
	for ( int i = 0; i < input.length(); i++ )
	{
		char c = input[i];
		if ( isdigit( input[i] ) )
		{
			int t = atoi( &c );
			ans.append( string( t, '*' ) );
		}
		else if ( isalpha( input[i] ) )
		{
			ans.append( string( 1, c ) );
		}
	}
	if ( ans.size() != CHESS_PIECES_COUNT )
	{
		ans.clear();
	}
	return ans;
}

void renderScene()
{
	// Initialize camera and lights.
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

	//=========================================================================

	// Creating chess matrix.
	char matrix[CHESS_SIZE][CHESS_SIZE];
	for ( int i = 0; i < CHESS_SIZE; i++ )
	{
		for ( int j = 0; j < CHESS_SIZE; j++ )
		{
			matrix[i][j] = parsedInitPositions[i * CHESS_SIZE + j];
		}
	}

	/*	p = peón
		r = torre
		b = alfil
		n = caballo
		q = reina
		k = rey */

	// Drawing cells/pieces.
	float centrosx[CHESS_SIZE][CHESS_SIZE], centrosy[CHESS_SIZE][CHESS_SIZE];
	for ( int i = 0; i < CHESS_SIZE; i++ )
	{
		for ( int j = 0; j < CHESS_SIZE; j++ )
		{
			glDisable( GL_TEXTURE_2D );
			centrosy[i][j] = 3.5f - i;
			centrosx[i][j] = float( j ) - 3.5f;

			if ( ( ( i % 2 ) + ( j % 2 ) ) % 2 == 0 ) // Cell bg color.
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
			switch ( matrix[i][j] )
			{
				case 'p':
					drawPiece( centrosx[i][j], centrosy[i][j], _peonn );
					break;
				case 'r':
					drawPiece( centrosx[i][j], centrosy[i][j], _torren );
					break;
				case 'b':
					drawPiece( centrosx[i][j], centrosy[i][j], _alfiln );
					break;
				case 'n':
					drawPiece( centrosx[i][j], centrosy[i][j], _caballon );
					break;
				case 'q':
					drawPiece( centrosx[i][j], centrosy[i][j], _reinan );
					break;
				case 'k':
					drawPiece( centrosx[i][j], centrosy[i][j], _reyn );
					break;
				case 'P':
					drawPiece( centrosx[i][j], centrosy[i][j], _peonb );
					break;
				case 'R':
					drawPiece( centrosx[i][j], centrosy[i][j], _torreb );
					break;
				case 'B':
					drawPiece( centrosx[i][j], centrosy[i][j], _alfilb );
					break;
				case 'N':
					drawPiece( centrosx[i][j], centrosy[i][j], _caballob );
					break;
				case 'Q':
					drawPiece( centrosx[i][j], centrosy[i][j], _reinab );
					break;
				case 'K':
					drawPiece( centrosx[i][j], centrosy[i][j], _reyb );
					break;
			}
		}
	}

	glutSwapBuffers();
	cout << "\nScene drown...";
}

void drawChessBoard()
{
	glutDisplayFunc( renderScene );
	glutKeyboardFunc( handleKeypress );
	glutReshapeFunc( handleResize );
	glutMainLoop();
}

int main( int argc, char** argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize( 400, 400 );
	glutCreateWindow( "Chess with OpenGL" );
	initRendering();

	string initPosition( "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" );
	parsedInitPositions = parseChessInput( initPosition );
	drawChessBoard();
	return 0;
}

