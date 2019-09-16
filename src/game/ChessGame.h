#pragma once
#include <vector>
#include "../chess/BaseItem.h"

class ChessBoard;

class ChessMovement
{
public:
	ChessMovement( const int _indexPiece, const int _relHSteps, const int _relVSteps ) :
		indexPiece( _indexPiece ), relHSteps( _relHSteps ), relVSteps( _relVSteps ) {};
	int indexPiece;
	int relHSteps;
	int relVSteps;
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
	ChessPlayer( const bool isBlack ) : BaseItem(), m_isBlack( isBlack ), m_active( false ) {};
	~ChessPlayer() {};
	void update( const int dt );
	void setActive( const bool value );
	void gotoState( const int state ) override;
	void getPossibleMovements();
	const int absIncrementH( const REL_DIRECTION_H dir ) const;
	const int absIncrementV( const REL_DIRECTION_V dir ) const;
private:
	bool m_isBlack;
	bool m_active;
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
private:
	ChessBoard* m_board;
	ChessPlayer* m_playerW;
	ChessPlayer* m_playerB;
	ChessPlayer* m_activePlayer;
	bool m_finished;
	bool m_inInBlackTurn;
	int m_turnCounter;
};