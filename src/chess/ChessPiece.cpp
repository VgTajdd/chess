#include "ChessPiece.h"

ChessPiece::ChessPiece( const ChessPiece::TYPE type, const bool black, const int row, const int col ) :
	BaseItem(),
	m_type( type ),
	m_isBlack( black ),
	m_row( row ),
	m_column( col )
{}

ChessPiece::~ChessPiece()
{}

const ChessPiece::TYPE ChessPiece::type() const
{
	return m_type;
}

const int ChessPiece::row() const
{
	return m_row;
}

const int ChessPiece::column() const
{
	return m_column;
}

const bool ChessPiece::isBlack() const
{
	return m_isBlack;
}

void ChessPiece::setPosition( const int row, const int column )
{
	m_row = row;
	m_column = row;
}