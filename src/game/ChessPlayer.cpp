#include "ChessGame.h"
#include "ChessPlayer.h"
#include <assert.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <set>
#include "../chess/ChessBoard.h"

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
			eatMsg.append( " [[ JAKE MATE ]]" );
		}
	}

	std::string msg = "| " + m_preMessage + " | " + name() + std::string( " => move " ) + m_game->namePiece( m_board->piece( m_currentPieceToMoveIndex ).type() ) + eatMsg;
	std::cout << msg << std::endl;
	m_preMessage.clear();

	// Save double step if pawn.
	if ( m_board->piece( m_currentPieceToMoveIndex ).type() == ChessPiece::PAWN )
	{
		if ( std::abs( finalPosition.r - m_board->piece( m_currentPieceToMoveIndex ).row() ) == 2 )
		{
			m_pawnsUsedDoubleStep.push_back( m_currentPieceToMoveIndex );
		}
	}

	// Make the movement.
	m_board->movePieceTo( m_currentPieceToMoveIndex, finalPosition.r, finalPosition.c );

	if ( getState() != ChessPlayer::ST_WIN )
	{
		endTurn();
	}
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
		case 2: eatRandomDecisionSafe(); break;
		case 3: eatByHierarchyDecisionSafe(); break;
		case 4: intelligentDecision(); break;
	}

	// TODO: Generate message or validation if something wrong happens.
}

void ChessPlayer::randomDecision()
{
	m_game->getPossiblePositions( m_possiblePositions, m_isBlack, false, false );
	chooseRandomPieceToMove();
	chooseRandomPositionToMove();
}

void ChessPlayer::eatRandomDecision()
{
	m_game->getPossiblePositions( m_possiblePositions, m_isBlack, true, false );
	if ( m_possiblePositions.empty() )
	{
		m_game->getPossiblePositions( m_possiblePositions, m_isBlack, false, false );
	}
	chooseRandomPieceToMove();
	chooseRandomPositionToMove();
}

void ChessPlayer::eatRandomDecisionSafe()
{
	m_game->getPossiblePositions( m_possiblePositions, m_isBlack, true, true );
	if ( m_possiblePositions.empty() )
	{
		m_game->getPossiblePositions( m_possiblePositions, m_isBlack, true, false );
	}
	if ( m_possiblePositions.empty() )
	{
		m_game->getPossiblePositions( m_possiblePositions, m_isBlack, false, false );
	}
	chooseRandomPieceToMove();
	chooseRandomPositionToMove();
}

void ChessPlayer::eatByHierarchyDecisionSafe()
{
	std::vector< int > possibleVictims;
	m_game->getPossibleVictims( possibleVictims, m_isBlack, true );
	if ( !possibleVictims.empty() )
	{
		std::vector< std::pair< int, int > > buffer;
		for ( const int indexVictim : possibleVictims )
		{
			assert( m_board->existsPiece( indexVictim ) );
			const auto& piece = m_board->piece( indexVictim );
			buffer.emplace_back( m_game->rules()->getImportance( piece.type() ), piece.index() );
		}
		std::sort( buffer.rbegin(), buffer.rend() );
		int indexVictim = buffer[0].second; // More important victim.
		std::vector< int > assassins;
		m_game->getPossibleAssassinsOf( indexVictim, assassins );
		if ( !assassins.empty() )
		{
			m_currentPieceToMoveIndex = assassins[0];
			const auto& possiblePositions = m_game->getPossiblePositionsByPiece( m_currentPieceToMoveIndex, true, true );
			for ( int i = 0; i < possiblePositions.size(); i++ )
			{
				if ( m_board->existsPieceAt( possiblePositions[i].r, possiblePositions[i].c ) )
				{
					const auto& victimPiece = m_board->pieceAt( possiblePositions[i].r, possiblePositions[i].c );
					if ( victimPiece.index() == m_currentPieceToMoveIndex )
					{
						m_currentMovementIndex = i;
						break;
					}
				}
			}
			if ( m_currentMovementIndex == -1 )
			{
				m_currentPieceToMoveIndex = -1;
			}
			else
			{
				m_game->getPossiblePositions( m_possiblePositions, m_isBlack, true, true );
				return;
			}
		}
	}

	eatRandomDecisionSafe();
}

void ChessPlayer::intelligentDecision()
{
	m_game->getPossiblePositions( m_possiblePositions, m_isBlack, false, false );

	bool decisionTaken = false;
	int indexFriend = -1; // To protect.
	std::vector< int > possibleAssassins;
	bool multipleEnemiesForCurrentFriend = false;

	// Protection.
	{
		std::map< int, std::vector< CellNode > > possiblePositions;
		m_game->getPossiblePositions( possiblePositions, !m_isBlack, true, false );

		if ( !possiblePositions.empty() )
		{
			if ( possiblePositions.size() == 1 )
			{
				const auto& pair = *possiblePositions.begin();
				possibleAssassins.push_back( pair.first );
				const auto& finalPositions = pair.second;
				if ( finalPositions.size() == 1 )
				{
					if ( m_board->existsPieceAt( finalPositions[0].r, finalPositions[0].c ) )
					{
						indexFriend = m_board->pieceAt( finalPositions[0].r, finalPositions[0].c ).index();
					}
				}
				else
				{
					std::vector< std::pair< int, int > > bufferFriends;
					for ( const auto& fp : finalPositions )
					{
						if ( m_board->existsPieceAt( fp.r, fp.c ) )
						{
							const auto& piece = m_board->pieceAt( fp.r, fp.c );
							bufferFriends.emplace_back( m_game->rules()->getImportance( piece.type() ), piece.index() );
						}
					}
					std::sort( bufferFriends.rbegin(), bufferFriends.rend() );
					indexFriend = bufferFriends[0].second; // More important friend.
				}
			}
			else
			{
				std::set< int > uniqueFriends;
				std::vector< std::pair< int, int > > bufferFriends;
				for ( const auto&[key, finalPositions] : possiblePositions )
				{
					for ( const auto& fp : finalPositions )
					{
						if ( m_board->existsPieceAt( fp.r, fp.c ) )
						{
							const auto& piece = m_board->pieceAt( fp.r, fp.c );
							assert( piece.isBlack() == m_isBlack );
							uniqueFriends.emplace( piece.index() );
							bufferFriends.emplace_back( m_game->rules()->getImportance( piece.type() ), piece.index() );
						}
					}
				}
				std::sort( bufferFriends.rbegin(), bufferFriends.rend() );


				if ( uniqueFriends.size() == 1 )
				{
					indexFriend = ( *uniqueFriends.begin() );
					for ( const auto& [indexAssassin, positions] : possiblePositions )
					{
						possibleAssassins.push_back( indexAssassin );
					}
					multipleEnemiesForCurrentFriend = true;
				}
				else
				{
					indexFriend = bufferFriends[0].second; // More important friend.
					m_game->getPossibleAssassinsOf( indexFriend, possibleAssassins );
					assert( possibleAssassins.size() > 0 );
					multipleEnemiesForCurrentFriend = ( possibleAssassins.size() != 1 );
				}
			}

			/////////

			bool isThereAHorse = false;
			if ( ( !possibleAssassins.empty() ) && ( indexFriend != -1 ) )
			{
				// Step 1 - Eat posible assassin.
				for ( const auto& ie : possibleAssassins )
				{
					std::vector< int > myAssassins;
					m_game->getPossibleAssassinsOf( ie, myAssassins );
					if ( m_board->existsPiece( ie ) )
					{
						const auto& ep = m_board->piece( ie );
						if ( ep.type() == ChessPiece::KNIGHT )
						{
							isThereAHorse = true;
						}
						if ( m_game->isPositionSafe( CellNode( ep.row(), ep.column() ), m_isBlack ) )
						{
							if ( myAssassins.empty() )
							{
								continue;
							}
							m_currentPieceToMoveIndex = myAssassins[0];
							for ( int i = 0; i < m_possiblePositions[m_currentPieceToMoveIndex].size(); i++ )
							{
								auto pos = m_possiblePositions[m_currentPieceToMoveIndex][i];
								if ( pos == CellNode( ep.row(), ep.column() ) )
								{
									m_currentMovementIndex = i;
									break;
								}
							}
							if ( m_currentMovementIndex != -1 && m_currentPieceToMoveIndex != -1 )
							{
								m_preMessage = "Eat posible assassin";
								decisionTaken = true;
								break;
							}
							else
							{
								m_currentMovementIndex = m_currentPieceToMoveIndex = -1;
							}
						}
					}
				}

				if ( decisionTaken ) return;

				// Step 2 - Move to a safe place.
				const auto& safePositions = m_game->getPossiblePositionsByPiece( indexFriend, false, true );
				if ( !safePositions.empty() )
				{
					for ( int i = 0; i < m_possiblePositions[indexFriend].size(); i++ )
					{
						const auto& p = m_possiblePositions[indexFriend][i];
						if ( p == safePositions[0] )
						{
							m_preMessage = "Move to a safe place";
							assert( possibleAssassins.size() > 0 );
							decisionTaken = true;
							m_currentPieceToMoveIndex = indexFriend;
							m_currentMovementIndex = i;
							break;
						}
					}
				}

				if ( decisionTaken ) return;

				// Step 3 - Block enemy movement.
				if ( !isThereAHorse && ( possibleAssassins.size() == 1 ) )
				{
					auto pairIndexPosition = m_game->getBlockingFriend( indexFriend, possibleAssassins[0] );
					if ( pairIndexPosition.first != -1 )
					{
						for ( int i = 0; i < m_possiblePositions[pairIndexPosition.first].size(); i++ )
						{
							const auto& p = m_possiblePositions[pairIndexPosition.first][i];
							if ( pairIndexPosition.second == p )
							{
								m_preMessage = "Block enemy movement";
								decisionTaken = true;
								m_currentPieceToMoveIndex = pairIndexPosition.first;
								m_currentMovementIndex = i;
								break;
							}
						}
					}
				}

			}
		}
	}

	if ( decisionTaken ) return;

	// Make jake.
	{
		std::map< int, std::vector< CellNode > > possiblePositions;
		m_game->getPossiblePositions( possiblePositions, m_isBlack, false, true );
		for ( const auto&[indexPiece, positions] : possiblePositions )
		{
			if ( decisionTaken ) break;
			assert( m_board->existsPiece( indexPiece ) );
			const auto& piece = m_board->piece( indexPiece );
			if ( piece.type() == ChessPiece::PAWN || piece.type() == ChessPiece::KING )
			{
				continue;
			}
			CellNode currentPosition( piece.row(), piece.column() );
			for ( const auto& position : positions )
			{
				assert( m_game->isPositionSafe( position, m_isBlack ) );
				int indexPieceE = -1, rowE = -1, columnE = -1;
				ChessPiece::TYPE typeE = ChessPiece::NONE;
				if ( m_board->existsPieceAt( position.r, position.c ) )
				{
					const auto& pieceE = m_board->pieceAt( position.r, position.c );
					assert( pieceE.isBlack() != m_isBlack );
					indexPieceE = pieceE.index();
					rowE = pieceE.row();
					columnE = pieceE.column();
					typeE = pieceE.type();
					m_board->removePiece( indexPieceE );
				}

				// Moving temporally.
				m_board->movePieceTo( indexPiece, position.r, position.c );

				std::vector< int > victims;
				m_game->getPossibleVictims( victims, m_isBlack, false );
				for ( const int i : victims )
				{
					assert( m_board->existsPiece( i ) );
					const auto& victimPiece = m_board->piece( i );
					assert( victimPiece.isBlack() != m_isBlack );
					assert( m_game->isPositionSafe( CellNode( victimPiece.row(), victimPiece.column() ), m_isBlack ) );
					if ( victimPiece.type() == ChessPiece::KING )
					{
						// Only choose the first one opportunity to make jake.
						m_currentPieceToMoveIndex = indexPiece;
						break;
					}
				}

				if ( m_currentPieceToMoveIndex != -1 )
				{
					for ( int i = 0; i < m_possiblePositions[m_currentPieceToMoveIndex].size(); i++ )
					{
						const auto& p = m_possiblePositions[m_currentPieceToMoveIndex][i];
						if ( p == position )
						{
							decisionTaken = true;
							m_currentMovementIndex = i;
							m_preMessage = "JAKE!";

							std::vector< int > possibleAssassins;
							m_game->getPossibleAssassinsOf( m_currentPieceToMoveIndex, possibleAssassins );
							assert( possibleAssassins.empty() );
							break;
						}
					}
					if ( m_currentMovementIndex == -1 )
					{
						m_currentPieceToMoveIndex = -1;
					}
				}

				// Restoring everything.
				m_board->movePieceTo( indexPiece, currentPosition.r, currentPosition.c );
				if ( indexPieceE != -1 )
				{
					m_board->restorePiece( indexPieceE, !m_isBlack, typeE, rowE, columnE );
				}

				if ( decisionTaken ) break;
			}
		}
	}

	if ( decisionTaken ) return;

	// Make jake mate.
	{
		std::vector< int > victims;
		m_game->getPossibleVictims( victims, m_isBlack, false );
		int enemyKingIndex = -1;
		CellNode enemyKingPosition;
		bool jakeMatePossible = false;
		for ( const int i : victims )
		{
			assert( m_board->existsPiece( i ) );
			assert( m_board->piece( i ).isBlack() != m_isBlack );
			if ( m_board->piece( i ).type() == ChessPiece::KING )
			{
				// Only choose the first one opportunity to make jake.
				jakeMatePossible = true;
				const auto& enemyKing = m_board->piece( i );
				enemyKingPosition = CellNode( enemyKing.row(), enemyKing.column() );
				enemyKingIndex = i;
				break;
			}
		}

		if ( jakeMatePossible )
		{
			for ( const auto&[indexPiece, positions] : m_possiblePositions )
			{
				if ( decisionTaken )
				{
					break;
				}
				for ( int i = 0; i < positions.size(); i++ )
				{
					if ( positions[i] == enemyKingPosition )
					{
						m_currentPieceToMoveIndex = indexPiece;
						m_currentMovementIndex = i;
						decisionTaken = true;
						m_preMessage = "JAKE MATE!";
						break;
					}
				}
			}
		}
	}

	if ( decisionTaken ) return;

	// Eat enemy.

	if ( decisionTaken ) return;

	m_preMessage = "Random decision";

	// Move randomly.
	chooseRandomPieceToMove();
	chooseRandomPositionToMove();
}