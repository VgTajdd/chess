#pragma once
#include <vector>
#include <map>
#include "../chess/BaseItem.h"
#include "../chess/ChessPiece.h"

class ChessBoard;
class ChessGame;

struct ChessGameConfig
{
	ChessGameConfig() {};
private:
	unsigned int _humanPlayers = 0;
	bool _infiniteLoop = false;
	unsigned int _movementTime = 0; // 1000
	// AI.
	unsigned int _decisionTimeAI = 0;
	/*
	0: random
	1: eats random
	2: eats by importance
	3: intelligent
	*/
	unsigned int _levelAI;
public:
	const unsigned int humanPlayers() const;
	const bool infiniteLoop() const;
	const unsigned int movementTime() const;
	const unsigned int decisionTimeAI() const;
	const unsigned int levelAI() const;
};

inline const unsigned int ChessGameConfig::humanPlayers() const
{
	return _humanPlayers;
}

inline const bool ChessGameConfig::infiniteLoop() const
{
	return _infiniteLoop;
}

inline const unsigned int ChessGameConfig::movementTime() const
{
	return _movementTime;
}

inline const unsigned int ChessGameConfig::decisionTimeAI() const
{
	return _decisionTimeAI;
}

inline const unsigned int ChessGameConfig::levelAI() const
{
	return _levelAI;
}

struct CellNode
{
	int r;
	int c;
	CellNode() { r = 0; c = 0; }
	CellNode( const int _r, const int _c ) : r( _r ), c( _c ) {}
	CellNode& operator=( const CellNode& a )
	{
		r = a.r;
		c = a.c;
		return *this;
	}
	CellNode operator+( CellNode a )
	{
		return { a.r + r, a.c + c };
	}
};

class LinearPath
{
public:
	LinearPath( const int _relUX, const int _relUY, const int _steps ) :
		relUX( _relUX ), relUY( _relUY ), steps( _steps )
	{};
	CellNode getNode( const int indexNode );
	int relUX;
	int relUY;
	int steps;
};

class ChessPath
{
public:
	ChessPath() : m_totalSteps( 0 ) {};
	~ChessPath();
	ChessPath* addPath( const int _relUX, const int _relUY, const int _steps );
	CellNode getNode( const int indexNode );
	const int totalSteps() const;
	int m_totalSteps;
	std::vector< LinearPath* > m_paths;
};

inline const int ChessPath::totalSteps() const
{
	return m_totalSteps;
}

class ChessRules
{
public:
	ChessRules();
	~ChessRules();
	const std::vector< ChessPath* >& getPaths( const ChessPiece::TYPE type );
private:
	ChessPath* addChessPath( const ChessPiece::TYPE type );
private:
	std::map< ChessPiece::TYPE, std::vector< ChessPath* > > m_chessPaths;
};

class ChessPlayer : public BaseItem
{
public:
	static const int ST_WAIT_FOR_POSSIBLE_MOVEMENTS = 1;
	static const int ST_WAIT_FOR_CHOOSING_PIECE_TO_MOVE = 2;
	static const int ST_WAIT_FOR_CHOOSING_POSITION_TO_MOVE = 3;
	static const int ST_WAIT_FOR_PIECE_TO_MOVE = 4;
	static const int ST_EVALUATE_POSITION = 5;
	static const int ST_END_TURN = 6;
	static const int ST_WIN = 7;
	enum REL_DIRECTION_H
	{
		LEFT,
		RIGHT
	};
	enum REL_DIRECTION_V
	{
		FORWARD,
		BACK
	};
public:
	ChessPlayer( ChessBoard* board, ChessGame* game, const bool isBlack );
	~ChessPlayer();
	void gotoState( const int state ) override;
	void update( const int dt );
	void startTurn();
	void endTurn();
	void win();
	void evaluateFinalPosition();
	void getPosiblePositions();
	std::vector< CellNode > getPossiblePositionsByPiece( const int indexPiece );
	std::vector< CellNode > getPawnPosiblePositions( const int indexPiece );
	std::vector< CellNode > getKnightPossiblePositions( const int indexPiece );
	std::vector< CellNode > getGenericPossiblePositions( const int indexPiece, const ChessPiece::TYPE type );
	const char* name() const;
	// Test methods.
	void chooseRandomPieceToMove();
	void chooseRandomPositionToMove();
	void waitForPieceToMove( const int dt );
private:
	bool m_isBlack;
	ChessBoard* m_board;
	ChessGame* m_game;
	std::vector< int > m_pawnsIndexesUsedDoubleStep;
	std::vector< ChessPiece::TYPE > m_enemyPiecesToken;
	// Temporal variables.
	std::map< int, std::vector< CellNode > > m_possiblePositions; // final positions (absolute).
	int m_currentPieceToMoveIndex;
	int m_currentMovementIndex;
	unsigned int m_timerPieceInMovement;
};

class ChessGame
{
public:
	ChessGame();
	~ChessGame();
	void update( const int dt );
	void togglePlayerInTurn();
	void clear();
	void createGame();
	void resetGame();
	const std::vector< ChessPath* >& getPotentialPaths( const ChessPiece::TYPE );
	const ChessGameConfig& config() const;
	static const char* namePiece( const ChessPiece::TYPE );
private:
	ChessGameConfig m_config;
	ChessBoard* m_board;
	ChessPlayer* m_playerW;
	ChessPlayer* m_playerB;
	ChessPlayer* m_activePlayer;
	ChessRules* m_rules;
	bool m_finished;
	bool m_inInBlackTurn;
	int m_turnCounter;
};

inline const ChessGameConfig& ChessGame::config() const
{
	return m_config;
}