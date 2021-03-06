#include "ChessPiece.h"
#include "ChessBoard.h"

ChessPiece::ChessPiece( const int index, const ChessPiece::TYPE type, const bool black, const int absRow, const int absCol ) :
	BaseItem(),
	m_index( index ),
	m_type( type ),
	m_isBlack( black ),
	m_row( absRow ),
	m_column( absCol )
{}

ChessPiece::~ChessPiece()
{}

const int ChessPiece::relRow() const
{
	return m_isBlack ? ChessBoard::SIZE - m_row - 1 : m_row;
}

const int ChessPiece::relColumn() const
{
	return m_isBlack ? ChessBoard::SIZE - m_column - 1 : m_column;
}