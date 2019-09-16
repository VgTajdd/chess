#pragma once

class ChessBoard;
class ChessPlayer;

class ChessGame
{
public:
	ChessGame();
	~ChessGame();
	void start();
	void update( const int dt );
private:
	ChessBoard* m_board;
	ChessPlayer* m_playerW;
	ChessPlayer* m_playerB;
	bool m_finished;
	bool m_inInBlackTurn;
};

class ChessPlayer
{
public:
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
	ChessPlayer( const bool isBlack ) : m_isBlack( isBlack ) {};
	~ChessPlayer() {};
	void update( const int dt );
	const int absIncrementH( const REL_DIRECTION_H dir ) const;
	const int absIncrementV( const REL_DIRECTION_V dir ) const;
private:
	bool m_isBlack;
};