#pragma once
#include <vector>
#include <map>
#include "../chess/BaseItem.h"
#include "../chess/ChessPiece.h"

class ChessBoard;
class ChessGame;

class LinearPath
{
public:
	LinearPath( const int _relUX, const int _relUY, const int _steps ) :
		relUX( _relUX ), relUY( _relUY ), steps( _steps )
	{};
	int relUX;
	int relUY;
	int steps;
};

class ChessMovement
{
public:
	ChessMovement() {};
	~ChessMovement();
	ChessMovement* addPath( const int _relUX, const int _relUY, const int _steps );
	std::vector< LinearPath* > m_paths;
};

class ChessRules
{
public:
	ChessRules();
	~ChessRules();
	const std::vector< ChessMovement* >& getMovements( const ChessPiece::TYPE type );
private:
	ChessMovement* addMovement( const ChessPiece::TYPE type );
private:
	std::map< ChessPiece::TYPE, std::vector< ChessMovement* > > m_movements;
};

class ChessPlayer : public BaseItem
{
public:
	static const int ST_WAIT_FOR_POSSIBLE_MOVEMENTS = 2;
	static const int ST_WAIT_FOR_CHOOSING_PIECE_TO_MOVE = 3;
	static const int ST_WAIT_FOR_CHOOSING_POSITION_TO_MOVE = 4;
	static const int ST_EVALUATE_POSITION = 5;
	static const int ST_END_TURN = 5;
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
	void update( const int dt );
	void setActive( const bool value );
	void gotoState( const int state ) override;
	void getPossibleMovements();
	std::vector< ChessMovement* > getPossibleMovementsByPiece( const int indexPiece );
	std::vector< ChessMovement* > getPawnPossibleMovements( const int indexPiece );
	const int absIncrementH( const REL_DIRECTION_H dir ) const;
	const int absIncrementV( const REL_DIRECTION_V dir ) const;
private:
	bool m_isBlack;
	bool m_active;
	ChessBoard* m_board;
	ChessGame* m_game;
	std::vector< ChessMovement* > m_possibleMovements;
	std::vector< ChessMovement* > m_doneMovements;
};

inline void ChessPlayer::setActive( const bool value )
{
	m_active = value;
}

class ChessGame
{
public:
	ChessGame();
	~ChessGame();
	void update( const int dt );
	void togglePlayerInTurn();
	const std::vector< ChessMovement* >& getPossibleMovements( const ChessPiece::TYPE );
private:
	ChessBoard* m_board;
	ChessPlayer* m_playerW;
	ChessPlayer* m_playerB;
	ChessPlayer* m_activePlayer;
	ChessRules* m_rules;
	bool m_finished;
	bool m_inInBlackTurn;
	int m_turnCounter;
};