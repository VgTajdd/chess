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
	m_playerW = new ChessPlayer( m_board, this, false );
	m_playerB = new ChessPlayer( m_board, this, true );
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

const std::vector< ChessMovement* >& ChessGame::getPossibleMovements( const ChessPiece::TYPE type )
{
	return m_rules->getMovements( type );
}

//============================== ChessPlayer ==================================

ChessPlayer::ChessPlayer( ChessBoard* board, ChessGame* game, const bool isBlack ) :
	BaseItem(),
	m_isBlack( isBlack ),
	m_active( false ),
	m_board( board ),
	m_game( game )
{}

ChessPlayer::~ChessPlayer()
{
	m_board = nullptr;
	m_game = nullptr;
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
	const auto& pieces = m_board->getPieces();
	for ( const auto& piece : pieces )
	{
		if ( ( piece.isBlack() && m_isBlack ) || ( !piece.isBlack() && !m_isBlack ) )
		{
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
	if ( use-only-final-position )
	{ 1 y 2 -> add }
	else
	{
		for (fp = next node in path)
		{
			1 y 2 -> add
		}
	}
	1- fp exists inside of map?
	2- fp has no friendly piece
	*/

	std::vector< ChessMovement* > ans;
	ans = m_game->getPossibleMovements( ChessPiece::PAWN );

	// TODO.

	return ans;
}

//============================== ChessRules ===================================

ChessRules::ChessRules()
{
	addMovement( ChessPiece::PAWN )->addPath( 0, 1, 2 );
	addMovement( ChessPiece::PAWN )->addPath( -1, 1, 1 );
	addMovement( ChessPiece::PAWN )->addPath( 1, 1, 1 );

	addMovement( ChessPiece::ROOK )->addPath( -1, 0, 8 );
	addMovement( ChessPiece::ROOK )->addPath( 1, 0, 8 );
	addMovement( ChessPiece::ROOK )->addPath( 0, 1, 8 );
	addMovement( ChessPiece::ROOK )->addPath( 0, -1, 8 );

	addMovement( ChessPiece::BISHOP )->addPath( -1, 1, 8 );
	addMovement( ChessPiece::BISHOP )->addPath( 1, -1, 8 );
	addMovement( ChessPiece::BISHOP )->addPath( -1, -1, 8 );
	addMovement( ChessPiece::BISHOP )->addPath( 1, 1, 8 );

	addMovement( ChessPiece::KNIGHT )->addPath( -1, 0, 1 )->addPath( 0, 1, 2 );
	addMovement( ChessPiece::KNIGHT )->addPath( -1, 0, 1 )->addPath( 0, -1, 2 );
	addMovement( ChessPiece::KNIGHT )->addPath( 1, 0, 1 )->addPath( 0, 1, 2 );
	addMovement( ChessPiece::KNIGHT )->addPath( 1, 0, 1 )->addPath( 0, -1, 2 );
	addMovement( ChessPiece::KNIGHT )->addPath( -1, 0, 2 )->addPath( 0, 1, 1 );
	addMovement( ChessPiece::KNIGHT )->addPath( -1, 0, 2 )->addPath( 0, -1, 1 );
	addMovement( ChessPiece::KNIGHT )->addPath( 1, 0, 2 )->addPath( 0, 1, 1 );
	addMovement( ChessPiece::KNIGHT )->addPath( 1, 0, 2 )->addPath( 0, -1, 1 );

	addMovement( ChessPiece::QUEEN )->addPath( -1, 0, 8 );
	addMovement( ChessPiece::QUEEN )->addPath( 1, 0, 8 );
	addMovement( ChessPiece::QUEEN )->addPath( 0, 1, 8 );
	addMovement( ChessPiece::QUEEN )->addPath( 0, -1, 8 );
	addMovement( ChessPiece::QUEEN )->addPath( -1, 1, 8 );
	addMovement( ChessPiece::QUEEN )->addPath( 1, -1, 8 );
	addMovement( ChessPiece::QUEEN )->addPath( -1, -1, 8 );
	addMovement( ChessPiece::QUEEN )->addPath( 1, 1, 8 );

	addMovement( ChessPiece::KING )->addPath( -1, 0, 1 );
	addMovement( ChessPiece::KING )->addPath( 1, 0, 1 );
	addMovement( ChessPiece::KING )->addPath( 0, 1, 1 );
	addMovement( ChessPiece::KING )->addPath( 0, -1, 1 );
	addMovement( ChessPiece::KING )->addPath( -1, 1, 1 );
	addMovement( ChessPiece::KING )->addPath( 1, -1, 1 );
	addMovement( ChessPiece::KING )->addPath( -1, -1, 1 );
	addMovement( ChessPiece::KING )->addPath( 1, 1, 1 );
}

ChessRules::~ChessRules()
{
	for ( auto movementsByType : m_movements )
	{
		for ( auto movement : movementsByType.second )
		{
			delete movement;
			movement = nullptr;
		}
		movementsByType.second.clear();
	}
	m_movements.clear();
}

ChessMovement* ChessRules::addMovement( const ChessPiece::TYPE type )
{
	ChessMovement* movement = new ChessMovement();
	m_movements[type].push_back( movement );
	return movement;
}

const std::vector< ChessMovement* >& ChessRules::getMovements( const ChessPiece::TYPE type )
{
	return m_movements[type];
}

//============================== ChessMovement ================================

ChessMovement::~ChessMovement()
{
	for ( auto path : m_paths )
	{
		delete path;
		path = nullptr;
	}
	m_paths.clear();
}

ChessMovement* ChessMovement::addPath( const int _relUX, const int _relUY, const int _steps )
{
	m_paths.push_back( new LinearPath( _relUX, _relUY, _steps ) );
	return this;
}