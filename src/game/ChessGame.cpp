#include "ChessGame.h"
#include <assert.h>
#include <string>
#include <iostream>
#include <ctime>
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

const std::vector< ChessPath* >& ChessGame::getPotentialPaths( const ChessPiece::TYPE type )
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

//============================== ChessPlayer ==================================

ChessPlayer::ChessPlayer( ChessBoard* board, ChessGame* game, const bool isBlack ) :
	BaseItem(),
	m_isBlack( isBlack ),
	m_board( board ),
	m_game( game ),
	m_currentPieceToMoveIndex( -1 ),
	m_currentMovementIndex( -1 ),
	m_isHuman( false )
{}

ChessPlayer::~ChessPlayer()
{
	m_board = nullptr;
	m_game = nullptr;
}

void ChessPlayer::gotoState( const int state )
{
	/*std::cout << "ChessPlayer::gotoState : " << state << std::endl;*/
	BaseItem::gotoState( state );
}

void ChessPlayer::update( const int dt )
{
	switch ( getState() )
	{
		case ChessPlayer::ST_WAIT_FOR_PIECE_DECISION:
			waitForPieceDecision();
			break;
		case ChessPlayer::ST_WAIT_FOR_MOVEMENT_DECISION:
			waitForMovementDecision();
			break;
		case ChessPlayer::ST_WAIT_FOR_PIECE_TO_MOVE:
			waitForPieceToMove( dt );
			break;
		case ChessPlayer::ST_EVALUATE_POSITION:
			evaluateFinalPosition();
			break;
	}
}

void ChessPlayer::startTurn()
{
	m_timerPieceInMovement = 0;
	m_currentPieceToMoveIndex = -1;
	m_currentMovementIndex = -1;
	m_possiblePositions.clear();
	gotoState( ChessPlayer::ST_WAIT_FOR_PIECE_DECISION );
}

void ChessPlayer::endTurn()
{
	gotoState( ChessPlayer::ST_END_TURN );
}

void ChessPlayer::win()
{
	gotoState( ChessPlayer::ST_WIN );
}

void ChessPlayer::evaluateFinalPosition()
{
	const auto& finalPosition = m_possiblePositions[m_currentPieceToMoveIndex][m_currentMovementIndex];

	std::string eatMsg;

	// Check again this place to see if enemy piece will be eaten.
	if ( m_board->existsPieceAt( finalPosition.r, finalPosition.c ) )
	{
		const auto& piece = m_board->pieceAt( finalPosition.r, finalPosition.c );
		assert( piece.isBlack() != m_isBlack );
		const int indexPiece = piece.index();
		m_enemyPiecesToken.push_back( piece.type() );
		m_board->removePiece( indexPiece );

		eatMsg = std::string( "\t => ate enemy " ) + m_game->namePiece( m_enemyPiecesToken.back() );

		// Jake - mate.
		if ( m_enemyPiecesToken.back() == ChessPiece::KING )
		{
			win();
		}
	}

	std::string msg = name() + std::string( " => move " ) + m_game->namePiece( m_board->piece( m_currentPieceToMoveIndex ).type() ) + eatMsg;
	std::cout << msg << std::endl;

	// Save double step if pawn.
	if ( m_board->piece( m_currentPieceToMoveIndex ).type() == ChessPiece::PAWN )
	{
		if ( std::abs( finalPosition.r - m_board->piece( m_currentPieceToMoveIndex ).row() ) == 2 )
		{
			m_pawnsUsedDoubleStep.push_back( m_currentPieceToMoveIndex );
		}
	}

	// Make the movement.
	m_board->movePiece( m_currentPieceToMoveIndex, finalPosition.r, finalPosition.c );

	if ( getState() != ChessPlayer::ST_WIN )
	{
		endTurn();
	}
}

/**
 This method give us a map { indexPiece, [vector of absolute final positions] }
*/
void ChessGame::getPosiblePositions( std::map< int, std::vector< CellNode > >& possiblePositions, const bool playerBlack, const bool onlyEat )
{
	const auto& pieces = m_board->getPieces();
	for ( const auto& [key, piece] : pieces )
	{
		if ( ( piece.isBlack() && playerBlack ) || ( !piece.isBlack() && !playerBlack ) )
		{
			std::vector< CellNode > v = getPossiblePositionsByPiece( piece.index(), playerBlack, onlyEat );
			if ( v.empty() ) continue;
			possiblePositions[piece.index()].insert( possiblePositions[piece.index()].end(), v.begin(), v.end() );
		}
	}
}

std::vector< CellNode > ChessGame::getPossiblePositionsByPiece( const int indexPiece, const bool playerBlack, const bool onlyEat )
{
	std::vector< CellNode > ans;
	const auto& piece = m_board->piece( indexPiece );
	switch ( piece.type() )
	{
		case ChessPiece::TYPE::PAWN:
			ans = getPawnPosiblePositions( indexPiece, playerBlack, onlyEat );
			break;
		case ChessPiece::TYPE::KNIGHT:
			ans = getKnightPossiblePositions( indexPiece, playerBlack, onlyEat );
			break;
		case ChessPiece::TYPE::ROOK:
		case ChessPiece::TYPE::BISHOP:
		case ChessPiece::TYPE::QUEEN:
		case ChessPiece::TYPE::KING:
			ans = getGenericPossiblePositions( indexPiece, playerBlack, onlyEat, piece.type() );
			break;
	}
	return std::move( ans );
}

std::vector< CellNode > ChessGame::getPawnPosiblePositions( const int indexPiece, const bool playerBlack, const bool onlyEat )
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

	const auto& p = player( playerBlack );

	for ( const auto& cp : potentialPaths )
	{
		bool isDiagonal = true;
		const int steps = cp->totalSteps();
		if ( steps == 2 )
		{
			if ( std::find( p->pawnsUsedDoubleStep().begin(), p->pawnsUsedDoubleStep().end(), indexPiece) != p->pawnsUsedDoubleStep().end() )
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

			bool isInside = false, noFriend = false, isEmpty = false;

			// Check if cell is inside the board.
			int ar = playerBlack ? ChessBoard::SIZE - rr - 1 : rr;
			int ac = playerBlack ? ChessBoard::SIZE - rc - 1 : rc;
			isInside = ( ar >= 0 && ar < ChessBoard::SIZE && ac >= 0 && ac < ChessBoard::SIZE );

			// Check if piece is friend.
			if ( isInside )
			{
				if ( m_board->existsPieceAt( ar, ac ) )
				{
					const auto& piece = m_board->pieceAt( ar, ac );
					noFriend = ( piece.isBlack() != playerBlack );
				}
				else
				{
					isEmpty = true;
				}
			}
			if ( onlyEat )
			{
				if ( isDiagonal )
				{
					if ( isInside && noFriend )
					{
						ans.push_back( CellNode( ar, ac ) );
					}
				}
			}
			else
			{
				if ( isDiagonal )
				{
					if ( isInside && noFriend )
					{
						ans.push_back( CellNode( ar, ac ) );
					}
				}
				else
				{
					if ( isInside && ( noFriend || isEmpty ) )
					{
						ans.push_back( CellNode( ar, ac ) );
					}
				}
			}
		}
	}
	return ans;
}

std::vector< CellNode > ChessGame::getKnightPossiblePositions( const int indexPiece, const bool playerBlack, const bool onlyEat )
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

		bool isInside = false, noFriend = false, isEmpty = false;

		// Check if cell is inside the board.
		int ar = playerBlack ? ChessBoard::SIZE - rr - 1 : rr;
		int ac = playerBlack ? ChessBoard::SIZE - rc - 1 : rc;
		isInside = ( ar >= 0 && ar < ChessBoard::SIZE && ac >= 0 && ac < ChessBoard::SIZE );

		// Check if piece is friend.
		if ( isInside )
		{
			if ( m_board->existsPieceAt( ar, ac ) )
			{
				const auto& piece = m_board->pieceAt( ar, ac );
				noFriend = ( piece.isBlack() != playerBlack );
			}
			else
			{
				isEmpty = true;
			}
		}

		if ( onlyEat )
		{
			if ( isInside && noFriend )
			{
				ans.push_back( CellNode( ar, ac ) );
			}
		}
		else
		{
			if ( isInside && ( noFriend || isEmpty ) )
			{
				ans.push_back( CellNode( ar, ac ) );
			}
		}
	}

	return ans;
}

std::vector< CellNode > ChessGame::getGenericPossiblePositions( const int indexPiece, const bool playerBlack, const bool onlyEat, const ChessPiece::TYPE type )
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

			bool isInside = false, noFriend = false, isEmpty = false;

			// Check if cell is inside the board.
			int ar = playerBlack ? ChessBoard::SIZE - rr - 1 : rr;
			int ac = playerBlack ? ChessBoard::SIZE - rc - 1 : rc;
			isInside = ( ar >= 0 && ar < ChessBoard::SIZE && ac >= 0 && ac < ChessBoard::SIZE );

			// Check if piece is friend.
			if ( isInside )
			{
				if ( m_board->existsPieceAt( ar, ac ) )
				{
					const auto& piece = m_board->pieceAt( ar, ac );
					noFriend = ( piece.isBlack() != playerBlack );
				}
				else
				{
					isEmpty = true;
				}
			}
			if ( onlyEat )
			{
				if ( isInside && ( noFriend || isEmpty ) )
				{
					if ( noFriend )
					{
						ans.push_back( CellNode( ar, ac ) );
					}
				}
				else
				{
					break;
				}
			}
			else
			{
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
	}
	return ans;
}

void ChessPlayer::chooseRandomPieceToMove()
{
	int random_variable = std::rand();
	double r = double( random_variable ) / double( RAND_MAX );
	int offset = int( ( m_possiblePositions.size() - 1 ) * r );
	for ( const auto& [key, cellNode] : m_possiblePositions )
	{
		if ( offset-- == 0 )
		{
			m_currentPieceToMoveIndex = key;
			break;
		}
	}
	assert( m_currentPieceToMoveIndex != -1 );
}

void ChessPlayer::chooseRandomPositionToMove()
{
	int random_variable = std::rand();
	double r = double( random_variable ) / double( RAND_MAX );
	m_currentMovementIndex = int( ( m_possiblePositions[m_currentPieceToMoveIndex].size() - 1 ) * r );
}

void ChessPlayer::waitForPieceToMove( const int dt )
{
	m_timerPieceInMovement += dt;
	if ( m_timerPieceInMovement >= m_game->config().movementTime() )
	{
		gotoState( ChessPlayer::ST_EVALUATE_POSITION );
	}
}

void ChessPlayer::waitForPieceDecision()
{
	if ( !m_isHuman )
	{
		generateDecision();
	}
	if ( m_currentPieceToMoveIndex != -1 )
	{
		gotoState( ChessPlayer::ST_WAIT_FOR_MOVEMENT_DECISION );
	}
}

void ChessPlayer::waitForMovementDecision()
{
	if ( m_currentMovementIndex != -1 )
	{
		gotoState( ChessPlayer::ST_WAIT_FOR_PIECE_TO_MOVE );
	}
}

const char* ChessPlayer::name() const
{
	return m_isBlack ? "BLACK" : "WHITE";
}

void ChessPlayer::generateDecision()
{
	/*
	This method will set values for this variables:
	- m_currentPieceToMoveIndex
	- m_currentMovementIndex
	based on the current levelAI.
	*/

	switch ( m_game->config().levelAI() )
	{
		case 0: randomDecision(); break;
		case 1: eatRandomDecision(); break;
		case 2: eatByhierarchyDecision(); break;
		case 3: intelligentDecision(); break;
	}

	// TODO: Generate message or validation if something wrong happens.
}

void ChessPlayer::randomDecision()
{
	m_game->getPosiblePositions( m_possiblePositions, m_isBlack, false );
	chooseRandomPieceToMove();
	chooseRandomPositionToMove();
}

void ChessPlayer::eatRandomDecision()
{
	m_game->getPosiblePositions( m_possiblePositions, m_isBlack, true );
	if ( m_possiblePositions.empty() )
	{
		m_game->getPosiblePositions( m_possiblePositions, m_isBlack, false );
	}
	chooseRandomPieceToMove();
	chooseRandomPositionToMove();
}

void ChessPlayer::eatByhierarchyDecision()
{
	// TODO.
}

void ChessPlayer::intelligentDecision()
{
	bool decisionTakken = false;

	// Protection.
	std::vector< int > v, e;
	m_game->getEatables( this, v, e );

	// Make jake.

	// Make jake mate.

	// Eat enemy.
}

void ChessGame::getEatables( const ChessPlayer* player, std::vector< int >& v, std::vector< int >& e )
{
	std::map< int, std::vector< CellNode > > possiblePositions;
	getPosiblePositions( possiblePositions, !player->isBlack(), true );
	if ( !possiblePositions.empty() )
	{
		//...
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

const std::vector< ChessPath* >& ChessRules::getPaths( const ChessPiece::TYPE type )
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
