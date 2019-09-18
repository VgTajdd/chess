#include "ChessGame.h"
#include <assert.h>
#include <iostream>
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

const std::vector< ChessPath* >& ChessGame::getPotencialMovements( const ChessPiece::TYPE type )
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
		else if ( getState() == ChessPlayer::ST_WAIT_FOR_POSSIBLE_MOVEMENTS )
		{
			gotoState( ChessPlayer::ST_WAIT_FOR_CHOOSING_PIECE_TO_MOVE );
		}
	}
}

void ChessPlayer::gotoState( const int state )
{
	switch ( state )
	{
		case ChessPlayer::ST_WAIT_FOR_POSSIBLE_MOVEMENTS:
			getPosiblePositions();
			break;
		case ChessPlayer::ST_WAIT_FOR_CHOOSING_PIECE_TO_MOVE:
			break;
		case ChessPlayer::ST_WAIT_FOR_CHOOSING_POSITION_TO_MOVE:
			break;
		case ChessPlayer::ST_EVALUATE_POSITION:
			break;
	}
	std::cout << "ChessPlayer::gotoState : " << state << std::endl;
	BaseItem::gotoState( state );
}

void ChessPlayer::getPosiblePositions()
{
	m_possiblePositions.clear();
	const auto& pieces = m_board->getPieces();
	for ( const auto& piece : pieces )
	{
		if ( ( piece.isBlack() && m_isBlack ) || ( !piece.isBlack() && !m_isBlack ) )
		{
			std::vector< CellNode > v = getPossiblePositionsByPiece( piece.index() );
			m_possiblePositions[piece.index()].insert( m_possiblePositions[piece.index()].end(), v.begin(), v.end() );
		}
	}
}

std::vector< CellNode > ChessPlayer::getPossiblePositionsByPiece( const int indexPiece )
{
	std::vector< CellNode > ans;
	const auto& piece = m_board->piece( indexPiece );
	switch ( piece.type() )
	{
		case ChessPiece::TYPE::PAWN:
			ans = getPawnPosiblePositions( indexPiece );
			break;
		case ChessPiece::TYPE::KNIGHT:
			ans = getKnightPossiblePositions( indexPiece );
			break;
		case ChessPiece::TYPE::ROOK:
		case ChessPiece::TYPE::BISHOP:
		case ChessPiece::TYPE::QUEEN:
		case ChessPiece::TYPE::KING:
			ans = getGenericPossiblePositions( indexPiece, piece.type() );
			break;
	}
	return std::move( ans );// improve pawn eat in diagonal and use the double jump.
}

std::vector< CellNode > ChessPlayer::getPawnPosiblePositions( const int indexPiece )
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

	const auto& piece = m_board->piece( indexPiece );

	std::vector< CellNode > ans;

	const auto& potentialPositions = m_game->getPotencialMovements( ChessPiece::PAWN );

	for ( const auto& cp : potentialPositions )
	{
		bool isDiagonal = true;
		if ( cp->totalSteps() == 2 )
		{
			if ( std::find( m_pawnsIndexesUsedDoubleStep.begin(), m_pawnsIndexesUsedDoubleStep.end(), indexPiece) != m_pawnsIndexesUsedDoubleStep.end() )
			{
				break;
			}
			isDiagonal = false;
		}

		const int steps = cp->totalSteps();
		for ( int i = 1; i <= steps; i++ )
		{
			const auto& node = cp->getNode( i );
			int rr = piece.relRow() + node.r;
			int rc = piece.relColumn() + node.c;

			bool isInside = false, noFriend = false, isEmpty = false;

			// Check if cell is inside the board.
			int ar = m_isBlack ? ChessBoard::SIZE - rr - 1 : rr;
			int ac = m_isBlack ? ChessBoard::SIZE - rc - 1 : rc;
			isInside = ( ar >= 0 && ar < ChessBoard::SIZE && ac >= 0 && ac < ChessBoard::SIZE );

			// Check if piece is friend.
			if ( isInside )
			{
				if ( m_board->existsPieceAt( ar, ac ) )
				{
					const auto& piece = m_board->pieceAt( ar, ac );
					noFriend = ( piece.isBlack() != m_isBlack );
				}
				else
				{
					isEmpty = true;
				}
			}

			if ( isDiagonal && isInside && noFriend )
			{
				ans.push_back( CellNode( ar, ac ) );
			}
			else if ( !isDiagonal && isInside && isEmpty )
			{
				ans.push_back( CellNode( ar, ac ) );
			}
			else
			{
				break;
			}
		}
	}
	return ans;
}

std::vector< CellNode > ChessPlayer::getKnightPossiblePositions( const int indexPiece )
{
	const auto& piece = m_board->piece( indexPiece );

	std::vector< CellNode > ans;

	const auto& potentialPositions = m_game->getPotencialMovements( ChessPiece::KNIGHT );

	for ( const auto& cp : potentialPositions )
	{
		const int steps = cp->totalSteps();
		const auto& node = cp->getNode( steps );

		int rr = piece.relRow() + node.r;
		int rc = piece.relColumn() + node.c;

		bool isInside = false, noFriend = false, isEmpty = false;

		// Check if cell is inside the board.
		int ar = m_isBlack ? ChessBoard::SIZE - rr - 1 : rr;
		int ac = m_isBlack ? ChessBoard::SIZE - rc - 1 : rc;
		isInside = ( ar >= 0 && ar < ChessBoard::SIZE && ac >= 0 && ac < ChessBoard::SIZE );

		// Check if piece is friend.
		if ( isInside )
		{
			if ( m_board->existsPieceAt( ar, ac ) )
			{
				const auto& piece = m_board->pieceAt( ar, ac );
				noFriend = ( piece.isBlack() != m_isBlack );
			}
			else
			{
				isEmpty = true;
			}
		}

		if ( isInside && ( noFriend || isEmpty ) )
		{
			ans.push_back( CellNode( ar, ac ) );
		}
		else
		{
			break;
		}
	}

	return ans;
}

std::vector< CellNode > ChessPlayer::getGenericPossiblePositions( const int indexPiece, const ChessPiece::TYPE type )
{
	const auto& piece = m_board->piece( indexPiece );

	std::vector< CellNode > ans;

	const auto& potentialPositions = m_game->getPotencialMovements( type );

	for ( const auto& cp : potentialPositions )
	{
		const int steps = cp->totalSteps();
		for ( int i = 1; i <= steps; i++ )
		{
			const auto& node = cp->getNode( i );
			int rr = piece.relRow() + node.r;
			int rc = piece.relColumn() + node.c;

			bool isInside = false, noFriend = false, isEmpty = false;

			// Check if cell is inside the board.
			int ar = m_isBlack ? ChessBoard::SIZE - rr - 1 : rr;
			int ac = m_isBlack ? ChessBoard::SIZE - rc - 1 : rc;
			isInside = ( ar >= 0 && ar < ChessBoard::SIZE && ac >= 0 && ac < ChessBoard::SIZE );

			// Check if piece is friend.
			if ( isInside )
			{
				if ( m_board->existsPieceAt( ar, ac ) )
				{
					const auto& piece = m_board->pieceAt( ar, ac );
					noFriend = ( piece.isBlack() != m_isBlack );
				}
				else
				{
					isEmpty = true;
				}
			}

			if ( isInside && ( noFriend || isEmpty ) )
			{
				ans.push_back( CellNode( ar, ac ) );
			}
			else
			{
				break;
			}
		}
	}
	return ans;
}

//============================== ChessRules ===================================

ChessRules::ChessRules()
{
	addChessPath( ChessPiece::PAWN )->addPath( 0, 1, 2 );
	addChessPath( ChessPiece::PAWN )->addPath( -1, 1, 1 );
	addChessPath( ChessPiece::PAWN )->addPath( 1, 1, 1 );

	addChessPath( ChessPiece::ROOK )->addPath( -1, 0, 8 );
	addChessPath( ChessPiece::ROOK )->addPath( 1, 0, 8 );
	addChessPath( ChessPiece::ROOK )->addPath( 0, 1, 8 );
	addChessPath( ChessPiece::ROOK )->addPath( 0, -1, 8 );

	addChessPath( ChessPiece::BISHOP )->addPath( -1, 1, 8 );
	addChessPath( ChessPiece::BISHOP )->addPath( 1, -1, 8 );
	addChessPath( ChessPiece::BISHOP )->addPath( -1, -1, 8 );
	addChessPath( ChessPiece::BISHOP )->addPath( 1, 1, 8 );

	addChessPath( ChessPiece::KNIGHT )->addPath( -1, 0, 1 )->addPath( 0, 1, 2 );
	addChessPath( ChessPiece::KNIGHT )->addPath( -1, 0, 1 )->addPath( 0, -1, 2 );
	addChessPath( ChessPiece::KNIGHT )->addPath( 1, 0, 1 )->addPath( 0, 1, 2 );
	addChessPath( ChessPiece::KNIGHT )->addPath( 1, 0, 1 )->addPath( 0, -1, 2 );
	addChessPath( ChessPiece::KNIGHT )->addPath( -1, 0, 2 )->addPath( 0, 1, 1 );
	addChessPath( ChessPiece::KNIGHT )->addPath( -1, 0, 2 )->addPath( 0, -1, 1 );
	addChessPath( ChessPiece::KNIGHT )->addPath( 1, 0, 2 )->addPath( 0, 1, 1 );
	addChessPath( ChessPiece::KNIGHT )->addPath( 1, 0, 2 )->addPath( 0, -1, 1 );

	addChessPath( ChessPiece::QUEEN )->addPath( -1, 0, 8 );
	addChessPath( ChessPiece::QUEEN )->addPath( 1, 0, 8 );
	addChessPath( ChessPiece::QUEEN )->addPath( 0, 1, 8 );
	addChessPath( ChessPiece::QUEEN )->addPath( 0, -1, 8 );
	addChessPath( ChessPiece::QUEEN )->addPath( -1, 1, 8 );
	addChessPath( ChessPiece::QUEEN )->addPath( 1, -1, 8 );
	addChessPath( ChessPiece::QUEEN )->addPath( -1, -1, 8 );
	addChessPath( ChessPiece::QUEEN )->addPath( 1, 1, 8 );

	addChessPath( ChessPiece::KING )->addPath( -1, 0, 1 );
	addChessPath( ChessPiece::KING )->addPath( 1, 0, 1 );
	addChessPath( ChessPiece::KING )->addPath( 0, 1, 1 );
	addChessPath( ChessPiece::KING )->addPath( 0, -1, 1 );
	addChessPath( ChessPiece::KING )->addPath( -1, 1, 1 );
	addChessPath( ChessPiece::KING )->addPath( 1, -1, 1 );
	addChessPath( ChessPiece::KING )->addPath( -1, -1, 1 );
	addChessPath( ChessPiece::KING )->addPath( 1, 1, 1 );
}

ChessRules::~ChessRules()
{
	for ( auto movementsByType : m_chessPaths )
	{
		for ( auto movement : movementsByType.second )
		{
			delete movement;
			movement = nullptr;
		}
		movementsByType.second.clear();
	}
	m_chessPaths.clear();
}

ChessPath* ChessRules::addChessPath( const ChessPiece::TYPE type )
{
	ChessPath* cp = new ChessPath();
	m_chessPaths[type].push_back( cp );
	return cp;
}

const std::vector< ChessPath* >& ChessRules::getMovements( const ChessPiece::TYPE type )
{
	return m_chessPaths[type];
}

//============================== ChessPath ====================================

ChessPath::~ChessPath()
{
	for ( auto path : m_paths )
	{
		delete path;
		path = nullptr;
	}
	m_paths.clear();
}

ChessPath* ChessPath::addPath( const int _relUX, const int _relUY, const int _steps )
{
	m_paths.push_back( new LinearPath( _relUX, _relUY, _steps ) );
	m_totalSteps += _steps;
	return this;
}

CellNode ChessPath::getNode( const int indexNode )
{
	int offset = 0;
	CellNode ans;
	for ( const auto path : m_paths )
	{
		if ( indexNode < path->steps + offset )
		{
			CellNode currentNode = path->getNode( indexNode - offset );
			ans = ans + currentNode;
			break;
		}
		offset += path->steps;
		CellNode lastNode = path->getNode( path->steps );
		ans = ans + lastNode;
	}
	return ans;
}

//============================== LinearPath ===================================

CellNode LinearPath::getNode( const int indexNode )
{
	return CellNode( relUY * indexNode, relUX * indexNode );
}
