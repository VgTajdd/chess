#include "ChessGame.h"
#include <assert.h>
#include "../chess/ChessBoard.h"

ChessGame::ChessGame():
	m_board( nullptr ),
	m_finished( false ),
	m_inInBlackTurn( false )
{
	m_board = new ChessBoard();
	m_playerW = new ChessPlayer( false );
	m_playerB = new ChessPlayer( true );

	m_activePlayer = m_playerW;
	m_activePlayer->setActive( true );
}

ChessGame::~ChessGame()
{
	delete m_board;
	delete m_playerW;
	delete m_playerB;
}

void ChessGame::togglePlayerInTurn()
{
	m_inInBlackTurn = !m_inInBlackTurn;
	if ( m_inInBlackTurn )
	{
		m_activePlayer = m_playerB;
	}
	else
	{
		m_activePlayer = m_playerW;
	}
	m_activePlayer->setActive( true );
	m_activePlayer->gotoState( ChessPlayer::STAND );
}

void ChessGame::update( const int dt )
{
	m_activePlayer->update( dt );

	if ( m_activePlayer->getState() == ChessPlayer::ST_END_TURN )
	{
		togglePlayerInTurn();
	}
}

//============================== ChessPlayer ==================================

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
	if ( m_active )
	{
		if ( getState() == BaseItem::STAND )
		{
			gotoState( ChessPlayer::ST_WAIT_FOR_POSSIBLE_MOVEMENTS );
		}
	}
}

void ChessPlayer::gotoState( const int state )
{
	switch ( state )
	{
		case ChessPlayer::ST_WAIT_FOR_POSSIBLE_MOVEMENTS:
			getPossibleMovements();
			break;
		case ChessPlayer::ST_WAIT_FOR_CHOOSING_PIECE_TO_MOVE:
			break;
		case ChessPlayer::ST_WAIT_FOR_CHOOSING_POSITION_TO_MOVE:
			break;
		case ChessPlayer::ST_EVALUATE_POSITION:
			break;
	}
	BaseItem::gotoState( state );
}

void ChessPlayer::getPossibleMovements()
{
	m_possibleMovements.clear();

	// TODO.
}