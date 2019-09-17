#include "ChessGame.h"
#include <assert.h>
#include "../chess/ChessBoard.h"

ChessGame::ChessGame():
	m_board( nullptr ),
	m_rules( nullptr ),
	m_finished( false ),
	m_inInBlackTurn( false )
{
	m_board = new ChessBoard();
	m_playerW = new ChessPlayer( m_board, false );
	m_playerB = new ChessPlayer( m_board, true );
	m_rules = new ChessRules();

	m_activePlayer = m_playerW;
	m_activePlayer->setActive( true );
}

ChessGame::~ChessGame()
{
	delete m_board;
	delete m_playerW;
	delete m_playerB;
	delete m_rules;
	m_activePlayer = nullptr;
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

ChessPlayer::ChessPlayer( ChessBoard* board, const bool isBlack ) :
	BaseItem(),
	m_isBlack( isBlack ),
	m_active( false ),
	m_board( board )
{}

ChessPlayer::~ChessPlayer()
{
	m_board = nullptr;
}

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
	/*std::vector< int > idxs;*/
	const auto& pieces = m_board->getPieces();
	for ( const auto& piece : pieces )
	{
		if ( ( piece.isBlack() && m_isBlack ) || ( !piece.isBlack() && !m_isBlack ) )
		{
			/*idxs.push_back( piece.index() );*/
			std::vector< ChessMovement* > v = getPossibleMovementsByPiece( piece.index() );
			m_possibleMovements.insert( m_possibleMovements.end(), v.begin(), v.end() );
		}
	}
}

std::vector< ChessMovement* > ChessPlayer::getPossibleMovementsByPiece( const int indexPiece )
{
	std::vector< ChessMovement* > ans;
	const auto& piece = m_board->piece( indexPiece );
	switch ( piece.type() )
	{
		case ChessPiece::TYPE::PAWN:
			ans = getPawnPossibleMovements( indexPiece );
			break;
	}
	return std::move( ans );
}

std::vector< ChessMovement* > ChessPlayer::getPawnPossibleMovements( const int indexPiece )
{
	/*
	1- Define the final position. (I can make vectors for paths for each type)
	2- Check if the path is empty (if knight pass this step). (I can make vectors for paths for each type)
	3- Add movement to array.
	*/

	std::vector< ChessMovement* > ans;
	const auto& piece = m_board->piece( indexPiece );

	// 1 position forward.
	int absR = piece.row() + absIncrementV( ChessPlayer::FORWARD );
	int absC = piece.column();
	m_board->existsPieceAt( absR, absC );

	// 2 positions forward.
	absR = piece.row() + absIncrementV( ChessPlayer::FORWARD );
	m_board->existsPieceAt( absR, absC );
	return std::move( ans );
}

//============================== ChessRules ===================================

ChessRules::ChessRules()
{
	// create vectors
}

ChessRules::~ChessRules()
{
	// delete pointers in vectors.
}
