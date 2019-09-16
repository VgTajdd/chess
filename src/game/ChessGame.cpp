#include "ChessGame.h"
#include <assert.h>
#include <vector>
#include "../chess/ChessBoard.h"

ChessGame::ChessGame():
	m_board( nullptr ),
	m_finished( false ),
	m_inInBlackTurn( false )
{
	m_board = new ChessBoard();
	m_playerW = new ChessPlayer( false );
	m_playerB = new ChessPlayer( true );
}

ChessGame::~ChessGame()
{
	delete m_board;
	delete m_playerW;
	delete m_playerB;
}

void ChessGame::update( const int dt )
{
	m_playerW->update( dt );
	m_playerB->update( dt );
}

void ChessGame::start()
{
	//std::vector< ChessPlayer* > players = { m_playerW, m_playerB };
	//int p = 0;
	//while ( !m_finished )
	//{
	//	ChessPlayer* player = players[p];

	//	// TODO

	//	p = ( ++p ) % 2;
	//}
}

// ChessPlayer ================================================================

const int ChessPlayer::absIncrementH( const REL_DIRECTION_H dir ) const
{
	assert( dir == ChessPlayer::REL_DIRECTION_H::LEFT || dir == ChessPlayer::REL_DIRECTION_H::RIGHT );
	int inc = dir == ChessPlayer::REL_DIRECTION_H::LEFT ? -1 : 1;
	if ( m_isBlack ) inc *= -1;
	return inc;
}

const int ChessPlayer::absIncrementV( const REL_DIRECTION_V dir ) const
{
	assert( dir == ChessPlayer::REL_DIRECTION_V::BACK || dir == ChessPlayer::REL_DIRECTION_V::FORWARD );
	int inc = dir == ChessPlayer::REL_DIRECTION_V::BACK ? -1 : 1;
	if ( m_isBlack ) inc *= -1;
	return inc;
}

void ChessPlayer::update( const int dt )
{

}