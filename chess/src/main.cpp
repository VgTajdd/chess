#include <iostream>
#include <string>
#include <ctype.h>
#include "glUtils/imageloader.h"
#include "chess/ChessBoard.h"

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

ChessBoard chessBoard;

void handleKeypress( unsigned char key, int x, int y )
{
	switch ( key )
	{
		case 27: // Escape.
			exit( 0 );
			break;
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

map< ChessPiece::TYPE, GLuint > textureIdByTypeW;
map< ChessPiece::TYPE, GLuint > textureIdByTypeB;

void createTextureId( const char* assetPath, const ChessPiece::TYPE type, const bool isBlack )
{
	Image* image = loadBMP( assetPath );
	if ( isBlack )
	{
		textureIdByTypeB[type] = loadTexture( image );
	}
	else
	{
		textureIdByTypeW[type] = loadTexture( image );
	}
	delete image;
}

void initRendering()
{
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_NORMALIZE );
	glEnable( GL_COLOR_MATERIAL );
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f ); // bg color.

	// Loading Images.
	createTextureId( "assets/peonb.bmp", ChessPiece::TYPE::PAWN, false );
	createTextureId( "assets/peonn.bmp", ChessPiece::TYPE::PAWN, true );
	createTextureId( "assets/torreb.bmp", ChessPiece::TYPE::ROOK, false );
	createTextureId( "assets/torren.bmp", ChessPiece::TYPE::ROOK, true );
	createTextureId( "assets/alfilb.bmp", ChessPiece::TYPE::BISHOP, false );
	createTextureId( "assets/alfiln.bmp", ChessPiece::TYPE::BISHOP, true );
	createTextureId( "assets/caballob.bmp", ChessPiece::TYPE::KNIGHT, false );
	createTextureId( "assets/caballon.bmp", ChessPiece::TYPE::KNIGHT, true );
	createTextureId( "assets/reinab.bmp", ChessPiece::TYPE::QUEEN, false );
	createTextureId( "assets/reinan.bmp", ChessPiece::TYPE::QUEEN, true );
	createTextureId( "assets/reyb.bmp", ChessPiece::TYPE::KING, false );
	createTextureId( "assets/reyn.bmp", ChessPiece::TYPE::KING, true );
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
	glEnd();
	glDisable( GL_TEXTURE_2D );
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

	// Drawing cells/pieces.
	float x, y;
	for ( int i = 0; i < ChessBoard::SIZE; i++ )
	{
		for ( int j = 0; j < ChessBoard::SIZE; j++ )
		{
			x = 3.5f - i;
			y = float( j ) - 3.5f;

			if ( chessBoard.isDarkCell( i, j ) ) // Cell bg color.
			{
				glColor3f( 0.960784f, 0.960784f, 0.862745f );
			}
			else
			{
				glColor3f( 1.0f, 0.7f, 0.3f );
			}

			glBegin( GL_QUADS );
			glNormal3f( 0.0, 0.0f, 1.0f );
			glVertex3f( x - 0.5f, y - 0.5f, 0.0f );
			glVertex3f( x - 0.5f, y + 0.5f, 0.0f );
			glVertex3f( x + 0.5f, y + 0.5f, 0.0f );
			glVertex3f( x + 0.5f, y - 0.5f, 0.0f );
			glEnd();
		}
	}

	// Drawing pieces.
	for ( const auto& piece : chessBoard.getPieces() )
	{
		if ( piece.getState() == ChessPiece::STATE::STATIC )
		{
			x = 3.5f - piece.column();
			y = float( piece.row() ) - 3.5f;
			drawPiece( x, y, piece.isBlack() ? textureIdByTypeB[piece.type()] : textureIdByTypeW[piece.type()] );
		}
	}

	glutSwapBuffers();
	cout << "\nScene rendered...";
}

void drawChessBoard( const string initPosition )
{
	bool ok = false;
	chessBoard.parseChessInput( initPosition, ok );
	if ( !ok )
	{
		cout << "\nInvalid String";
	}
}

int main( int argc, char** argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize( 400, 400 );
	glutCreateWindow( "Chess with OpenGL" );
	initRendering();

	string initPosition( "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" );
	drawChessBoard( initPosition );

	glutDisplayFunc( renderScene );
	glutKeyboardFunc( handleKeypress );
	glutReshapeFunc( handleResize );
	glutMainLoop();

	return 0;
}

