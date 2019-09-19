#pragma once
#include "BaseItem.h"

class ChessBoard;

class ChessPiece : public BaseItem
{
friend class ChessBoard;
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
	ChessPiece( const int index, const TYPE type = TYPE::NONE, const bool black = false, const int row = -1, const int col = -1 );
	~ChessPiece();
	const int index() const;
	const TYPE type() const;
	const int row() const;
	const int column() const;
	const int relRow() const;
	const int relColumn() const;
	const bool isBlack() const;
private:
	void setPosition( const int row, const int column );
private:
	TYPE m_type;
	bool m_isBlack;
	int m_index;
	int m_row;
	int m_column;
};

inline const int ChessPiece::index() const
{
	return m_index;
}

inline const ChessPiece::TYPE ChessPiece::type() const
{
	return m_type;
}

inline const int ChessPiece::row() const
{
	return m_row;
}

inline const int ChessPiece::column() const
{
	return m_column;
}

inline const bool ChessPiece::isBlack() const
{
	return m_isBlack;
}

inline void ChessPiece::setPosition( const int row, const int column )
{
	m_row = row;
	m_column = column;
}