#pragma once
#include "BaseItem.h"

class ChessPiece : public BaseItem
{
public:
	enum TYPE
	{
		NONE = 0,
		PAWN = 1,
		ROOK = 2,
		BISHOP = 3,
		KNIGHT = 4,
		QUEEN = 5,
		KING = 6
	};
public:
	ChessPiece( const TYPE type = TYPE::NONE, const bool black = false, const int row = -1, const int col = -1 );
	~ChessPiece();
	const TYPE type() const;
	const int row() const;
	const int column() const;
	const bool isBlack() const;
private:
	void setPosition( const int row, const int column );
private:
	int m_row;
	int m_column;
	TYPE m_type;
	bool m_isBlack;
};