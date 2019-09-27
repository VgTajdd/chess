#include "ChessGame.h"
#include "ChessPlayer.h"
#include <assert.h>
#include <iostream>
#include <ctime>
#include <algorithm>
#include "../chess/ChessBoard.h"

ChessGame::ChessGame():
	m_board( nullptr ),
	m_rules( nullptr ),
	m_playerW( nullptr ),
	m_playerB( nullptr ),
	m_activePlayer( nullptr ),
	m_finished( false ),
	m_inInBlackTurn( false )
{
	createGame();
}

ChessGame::~ChessGame()
{
	clear();
}

void ChessGame::clear()
{
	delete m_board;
	delete m_playerW;
	delete m_playerB;
	delete m_rules;
	m_board = nullptr;
	m_rules = nullptr;
	m_playerW = nullptr;
	m_playerB = nullptr;
	m_activePlayer = nullptr;
}

void ChessGame::createGame()
{
	m_board = new ChessBoard();
	m_playerW = new ChessPlayer( m_board, this, false );
	m_playerB = new ChessPlayer( m_board, this, true );
	m_rules = new ChessRules();

	m_activePlayer = m_playerW;
	m_activePlayer->startTurn();

	std::srand( int( std::time( nullptr ) ) );

	std::cout << "-------------ChessGame::createGame---------------" << std::endl;
}

void ChessGame::resetGame()
{
	clear();
	createGame();
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
	m_activePlayer->startTurn();
}

void ChessGame::update( const int dt )
{
	m_activePlayer->update( dt );

	if ( m_activePlayer->getState() == ChessPlayer::ST_END_TURN )
	{
		togglePlayerInTurn();
	}
	else if ( m_activePlayer->getState() == ChessPlayer::ST_WIN && m_config.infiniteLoop() )
	{
		resetGame();
	}
}

const std::vector< ChessPath* >& ChessGame::getPotentialPaths( const ChessPiece::TYPE type ) const
{
	return m_rules->getPaths( type );
}

const char* ChessGame::namePiece( const ChessPiece::TYPE type )
{
	switch ( type )
	{
		case ChessPiece::PAWN: return "PAWN";
		case ChessPiece::QUEEN: return "QUEEN";
		case ChessPiece::ROOK: return "ROOK";
		case ChessPiece::BISHOP: return "BISHOP";
		case ChessPiece::KNIGHT: return "KNIGHT";
		case ChessPiece::KING: return "KING";
		default: return "UNDEFINED";
	}
}

// Helper methods.

void ChessGame::getPossibleAssassinsOf( const int indexPieceVictim, std::vector< int >& assassins ) const
{
	assert( m_board->existsPiece( indexPieceVictim ) );
	const auto& victimPiece = m_board->piece( indexPieceVictim );
	std::map< int, std::vector< CellNode > > possiblePositions;
	getPossiblePositions( possiblePositions, !victimPiece.isBlack(), true, false );
	if ( !possiblePositions.empty() )
	{
		for ( const auto&[indexPiece, positions] : possiblePositions )
		{
			for ( const auto& position : positions )
			{
				CellNode node( victimPiece.row(), victimPiece.column() );
				if ( node == position )
				{
					assassins.push_back( indexPiece );
					break;
				}
			}
		}
	}
}

void ChessGame::getPossibleVictims( std::vector< int >& victims, const bool isBlack, const bool onlySafe ) const
{
	std::map< int, std::vector< CellNode > > possiblePositions;
	getPossiblePositions( possiblePositions, isBlack, true, onlySafe );
	for ( const auto&[indexPiece, positions] : possiblePositions )
	{
		for ( const auto& position : positions )
		{
			if ( m_board->existsPieceAt( position.r, position.c ) )
			{
				const auto& victimPiece = m_board->pieceAt( position.r, position.c );
				assert( victimPiece.isBlack() != isBlack );
				CellNode node( victimPiece.row(), victimPiece.column() );
				victims.push_back( victimPiece.index() );
			}
		}
	}
}

const bool ChessGame::isSafeToMoveTo( const int indexPiece, const CellNode& node ) const
{
	assert( m_board->existsPiece( indexPiece ) );
	const auto& piece = m_board->piece( indexPiece );
	CellNode currentPosition( piece.row(), piece.column() );

	int indexPieceE = -1, rowE = -1, columnE = -1;
	ChessPiece::TYPE typeE = ChessPiece::NONE;
	if ( m_board->existsPieceAt( node.r, node.c ) )
	{
		const auto& pieceE = m_board->pieceAt( node.r, node.c );
		if ( pieceE.isBlack() != piece.isBlack() )
		{
			indexPieceE = pieceE.index();
			rowE = pieceE.row();
			columnE = pieceE.column();
			typeE = pieceE.type();
			m_board->removePiece( indexPieceE );
		}
	}

	// Moving temporally.
	m_board->movePieceTo( indexPiece, node.r, node.c );

	bool isSafe = true;
	std::map< int, std::vector< CellNode > > possiblePositions;
	getPossiblePositions( possiblePositions, !piece.isBlack(), false, false );
	if ( !possiblePositions.empty() )
	{
		for ( const auto&[indexPiece, positions] : possiblePositions )
		{
			if ( !isSafe )
			{
				break;
			}
			for ( const auto& position : positions )
			{
				if ( node == position )
				{
					isSafe = false;
					break;
				}
			}
		}
	}

	// Restoring everything.
	m_board->movePieceTo( indexPiece, currentPosition.r, currentPosition.c );
	if ( indexPieceE != -1 )
	{
		m_board->restorePiece( indexPieceE, !piece.isBlack(), typeE, rowE, columnE );
	}

	return isSafe;
}

/**
 This method give us a map { indexPiece, [vector of absolute final positions] }
*/
void ChessGame::getPossiblePositions( std::map< int, std::vector< CellNode > >& possiblePositions, const bool isBlack, const bool onlyEat, const bool onlySafe ) const
{
	const auto& pieces = m_board->getPieces();
	for ( const auto&[indexPiece, piece] : pieces )
	{
		assert( piece.index() == indexPiece );
		if ( ( piece.isBlack() && isBlack ) || ( !piece.isBlack() && !isBlack ) )
		{
			std::vector< CellNode > v = getPossiblePositionsByPiece( piece.index(), onlyEat, onlySafe );
			if ( v.empty() ) continue;
			possiblePositions[piece.index()].insert( possiblePositions[piece.index()].end(), v.begin(), v.end() );
		}
	}
}

std::vector< CellNode > ChessGame::getPossiblePositionsByPiece( const int indexPiece, const bool onlyEat, const bool onlySafe ) const
{
	std::vector< CellNode > ans;
	const auto& piece = m_board->piece( indexPiece );
	switch ( piece.type() )
	{
		case ChessPiece::TYPE::PAWN:
			ans = getPawnPosiblePositions( indexPiece, onlyEat, onlySafe );
			break;
		case ChessPiece::TYPE::KNIGHT:
			ans = getKnightPossiblePositions( indexPiece, onlyEat, onlySafe );
			break;
		case ChessPiece::TYPE::ROOK:
		case ChessPiece::TYPE::BISHOP:
		case ChessPiece::TYPE::QUEEN:
		case ChessPiece::TYPE::KING:
			ans = getGenericPossiblePositions( indexPiece, onlyEat, onlySafe, piece.type() );
			break;
	}
	return std::move( ans );
}

std::vector< CellNode > ChessGame::getPawnPosiblePositions( const int indexPiece, const bool onlyEat, const bool onlySafe ) const
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

	const auto& potentialPaths = getPotentialPaths( ChessPiece::PAWN );

	const auto& p = player( piece.isBlack() );

	for ( const auto& cp : potentialPaths )
	{
		bool isDiagonal = true;
		const int steps = cp->totalSteps();
		if ( steps == 2 )
		{
			if ( std::find( p->pawnsUsedDoubleStep().begin(), p->pawnsUsedDoubleStep().end(), indexPiece ) != p->pawnsUsedDoubleStep().end() )
			{
				break;
			}
			isDiagonal = false;
		}

		for ( int i = 1; i <= steps; i++ )
		{
			const auto& node = cp->getNode( i );
			int rr = piece.relRow() + node.r;
			int rc = piece.relColumn() + node.c;

			bool isInside = false, isBlack = false, isEmpty = false;

			// Check if cell is inside the board.
			int ar = piece.isBlack() ? ChessBoard::SIZE - rr - 1 : rr;
			int ac = piece.isBlack() ? ChessBoard::SIZE - rc - 1 : rc;
			isInside = ( ar >= 0 && ar < ChessBoard::SIZE && ac >= 0 && ac < ChessBoard::SIZE );

			// Check if piece is friend.
			if ( isInside )
			{
				CellNode node( ar, ac );
				bool validNode = false;
				getCellState( node, isEmpty, isBlack );
				if ( onlyEat )
				{
					if ( isDiagonal )
					{
						if ( !isEmpty && ( isBlack != piece.isBlack() ) )
						{
							validNode = true;
						}
					}
				}
				else
				{
					if ( isDiagonal )
					{
						if ( !isEmpty && ( isBlack != piece.isBlack() ) )
						{
							validNode = true;
						}
					}
					else
					{
						if ( !isEmpty )
						{
							break;
						}
						validNode = true;
					}
				}
				if ( validNode )
				{
					if ( onlySafe )
					{
						//if ( isPositionSafe( node, piece.isBlack() ) ) ans.push_back( node );
						if ( isSafeToMoveTo( piece.index(), node ) ) ans.push_back( node );
					}
					else
					{
						ans.push_back( node );
					}
				}
			}
		}
	}
	return ans;
}

std::vector< CellNode > ChessGame::getKnightPossiblePositions( const int indexPiece, const bool onlyEat, const bool onlySafe ) const
{
	const auto& piece = m_board->piece( indexPiece );

	std::vector< CellNode > ans;

	const auto& potentialPaths = getPotentialPaths( ChessPiece::KNIGHT );

	for ( const auto& cp : potentialPaths )
	{
		const int steps = cp->totalSteps();
		const auto& node = cp->getNode( steps );

		int rr = piece.relRow() + node.r;
		int rc = piece.relColumn() + node.c;

		bool isInside = false, isBlack = false, isEmpty = false;

		// Check if cell is inside the board.
		int ar = piece.isBlack() ? ChessBoard::SIZE - rr - 1 : rr;
		int ac = piece.isBlack() ? ChessBoard::SIZE - rc - 1 : rc;
		isInside = ( ar >= 0 && ar < ChessBoard::SIZE && ac >= 0 && ac < ChessBoard::SIZE );

		// Check if piece is friend.
		if ( isInside )
		{
			CellNode node( ar, ac );
			getCellState( node, isEmpty, isBlack );
			bool validNode = false;
			if ( onlyEat )
			{
				if ( !isEmpty && ( isBlack != piece.isBlack() ) )
				{
					validNode = true;
				}
			}
			else
			{
				if ( ( !isEmpty && ( isBlack != piece.isBlack() ) ) || ( isEmpty ) )
				{
					validNode = true;
				}
			}
			if ( validNode )
			{
				if ( onlySafe )
				{
					//if ( isPositionSafe( node, piece.isBlack() ) ) ans.push_back( node );
					if ( isSafeToMoveTo( piece.index(), node ) ) ans.push_back( node );
				}
				else
				{
					ans.push_back( node );
				}
			}
		}
	}
	return ans;
}

std::vector< CellNode > ChessGame::getGenericPossiblePositions( const int indexPiece, const bool onlyEat, const bool onlySafe, const ChessPiece::TYPE type ) const
{
	const auto& piece = m_board->piece( indexPiece );

	std::vector< CellNode > ans;

	const auto& potentialPaths = getPotentialPaths( type );

	for ( const auto& cp : potentialPaths )
	{
		const int steps = cp->totalSteps();
		for ( int i = 1; i <= steps; i++ )
		{
			const auto& node = cp->getNode( i );
			int rr = piece.relRow() + node.r;
			int rc = piece.relColumn() + node.c;

			bool isInside = false, isBlack = false, isEmpty = false;

			// Check if cell is inside the board.
			int ar = piece.isBlack() ? ChessBoard::SIZE - rr - 1 : rr;
			int ac = piece.isBlack() ? ChessBoard::SIZE - rc - 1 : rc;
			isInside = ( ar >= 0 && ar < ChessBoard::SIZE && ac >= 0 && ac < ChessBoard::SIZE );

			// Check if piece is friend.
			if ( isInside )
			{
				CellNode node( ar, ac );
				getCellState( node, isEmpty, isBlack );

				if ( !isEmpty && ( isBlack == piece.isBlack() ) )
				{
					break;
				}

				if ( onlyEat )
				{
					if ( !isEmpty )
					{
						if ( onlySafe )
						{
							//if ( isPositionSafe( node, piece.isBlack() ) ) ans.push_back( node );
							if ( isSafeToMoveTo( piece.index(), node ) ) ans.push_back( node );
						}
						else
						{
							ans.push_back( node );
						}
						break;
					}
				}
				else
				{
					if ( onlySafe )
					{
						//if ( isPositionSafe( node, piece.isBlack() ) ) ans.push_back( node );
						if ( isSafeToMoveTo( piece.index(), node ) ) ans.push_back( node );
					}
					else
					{
						ans.push_back( node );
					}
					if ( !isEmpty  )
					{
						break;
					}
				}
			}
		}
	}
	return ans;
}

std::pair< int, CellNode > ChessGame::getBlockingFriend( const int indexFriend, const int indexEnemy )
{
	std::pair< int, CellNode > ans( -1, { -1, -1 } );
	assert( m_board->existsPiece( indexFriend ) && m_board->existsPiece( indexEnemy ) );
	const auto& fpiece = m_board->piece( indexFriend );
	const auto& epiece = m_board->piece( indexEnemy );
	CellNode fposition = { fpiece.column(), fpiece.row() };
	CellNode eposition = { epiece.column(), epiece.row() };
	CellNode delta = eposition - fposition;
	int deltaSize = 0;
	if ( delta.c == 0 && std::abs( delta.r ) >= 2 )
	{
		delta.r = delta.r / std::abs( delta.r );
		deltaSize = std::abs( delta.r );
	}
	if ( delta.r == 0 && std::abs( delta.c ) >= 2 )
	{
		delta.c = delta.c / std::abs( delta.c );
		deltaSize = std::abs( delta.c );
	}
	if ( ( std::abs( delta.r ) == std::abs( delta.c ) ) && std::abs( delta.r ) > 2 )
	{
		delta.r = delta.r / std::abs( delta.r );
		delta.c = delta.c / std::abs( delta.c );
		deltaSize = std::abs( delta.r );
	}

	// Iterate over possible cells to see if some friend with less importance can interrupt the path between f and e.
	CellNode testPosition = fposition;
	for ( int i = 1; i < deltaSize; i++ )
	{
		std::vector< int > friends;
		testPosition = testPosition + delta;
		getFriendsThatCanReach( friends, testPosition, fpiece.isBlack() );

		if ( !friends.empty() )
		{
			std::vector< std::pair< int, int > > buffer;
			for ( const int indexHelperFriend : friends )
			{
				assert( m_board->existsPiece( indexHelperFriend ) );
				const auto& piece = m_board->piece( indexHelperFriend );
				if ( m_rules->getImportance( piece.type() ) < m_rules->getImportance( fpiece.type() ) )
				{
					buffer.emplace_back( m_rules->getImportance( piece.type() ), indexHelperFriend );
				}
			}
			if ( !buffer.empty() )
			{
				std::sort( buffer.begin(), buffer.end() );
				ans = { ( *buffer.begin() ).second, testPosition };
				break;
			}
		}
	}
	return ans;
}

void ChessGame::getFriendsThatCanReach( std::vector< int >& friends, const CellNode& node, const bool isBlack )
{
	std::map< int, std::vector< CellNode > > possiblePositions;
	getPossiblePositions( possiblePositions, isBlack, false, false );
	for ( const auto&[index, positions] : possiblePositions )
	{
		for ( const auto& position : positions )
		{
			if ( position == node )
			{
				friends.push_back( index );
				break;
			}
		}
	}
}

const int ChessGame::getIndexKing( const bool isBlack ) const
{
	int indexKing = -1;
	const auto& pieces = m_board->getPieces();
	for ( const auto&[indexPiece, piece] : pieces )
	{
		assert( m_board->existsPiece( indexPiece ) );
		const auto& piece = m_board->piece( indexPiece );
		if ( ( piece.isBlack() == isBlack ) || ( piece.type() == ChessPiece::KING ) )
		{
			indexKing = -1;
			break;
		}
	}
	return indexKing;
}

void ChessGame::getCellState( const CellNode& node, bool& isEmpy, bool& isBlack ) const
{
	if ( m_board->existsPieceAt( node.r, node.c ) )
	{
		isEmpy = false;
		const auto& piece = m_board->pieceAt( node.r, node.c );
		isBlack = piece.isBlack();
	}
	else
	{
		isEmpy = true;
	}
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

const int ChessRules::getImportance( const ChessPiece::TYPE type ) const
{
	int importance = 0;
	switch ( type )
	{
		case ChessPiece::KING: importance = 3; break;
		case ChessPiece::QUEEN: importance = 2; break;
		case ChessPiece::BISHOP: importance = 1; break;
		case ChessPiece::ROOK: importance = 1; break;
		case ChessPiece::KNIGHT: importance = 1; break;
		case ChessPiece::PAWN: importance = 0; break;
	}
	return importance;
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
