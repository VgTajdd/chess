#pragma once
#include <vector>
#include <map>
#include "../chess/BaseItem.h"

class ChessBoard;
class ChessGame;
struct CellNode;

class ChessPlayer : public BaseItem
{
public:
	static const int ST_WAIT_FOR_PIECE_DECISION = 1;
	static const int ST_WAIT_FOR_MOVEMENT_DECISION = 2;
	static const int ST_WAIT_FOR_PIECE_TO_MOVE = 3;
	static const int ST_EVALUATE_POSITION = 4;
	static const int ST_END_TURN = 5;
	static const int ST_WIN = 6;
public:
	ChessPlayer( ChessBoard* board, ChessGame* game, const bool isBlack );
	~ChessPlayer();
	void gotoState( const int state ) override;
	void update( const int dt );
	void startTurn();
	void endTurn();
	void win();
	void evaluateFinalPosition();

	// Decision methods.
	void generateDecision();
	void randomDecision();
	void eatRandomDecision();
	void eatRandomDecisionSafe();
	void eatByHierarchyDecisionSafe();
	void intelligentDecision();

	const bool protect();
	const bool makeJake();
	const bool makeJakeMate();
	const bool eatEnemySafe(); // Eat enemy (safe way).
	const bool eatEnemyNotSafe(); // Eat enemy (not safe way).
	const bool moveLessImportant(); // Move the less important piece to a safe place.

	void waitForPieceDecision();
	void waitForMovementDecision();
	void waitForPieceToMove( const int dt );

	const char* name() const;
	const bool isBlack() const;
	const std::vector< int >& pawnsUsedDoubleStep() const;

	// Test methods.
	void chooseRandomPieceToMove();
	void chooseRandomPositionToMove();
private:
	bool m_isBlack;
	bool m_isHuman;
	ChessBoard* m_board;
	ChessGame* m_game;
	std::vector< int > m_pawnsUsedDoubleStep;
	std::vector< ChessPiece::TYPE > m_enemyPiecesToken;

	// Temporal variables.
	std::map< int, std::vector< CellNode > > m_possiblePositions; // final positions (absolute).
	int m_currentPieceToMoveIndex;
	int m_currentMovementIndex;
	unsigned int m_timerPieceInMovement;
	std::string m_preMessage;
};

inline const std::vector< int >& ChessPlayer::pawnsUsedDoubleStep() const
{
	return m_pawnsUsedDoubleStep;
}

inline const bool ChessPlayer::isBlack() const
{
	return m_isBlack;
}