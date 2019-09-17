#pragma once
#include "ChessPiece.h"
#include <vector>
#include <map>

class ChessBoard
{
public:
	const static int SIZE = 8;
	const static int CELLS_COUNT = 64;
	const static int PIECES_COUNT = 32;
public:
	ChessBoard();
	~ChessBoard();
protected:
	std::vector< int > idx_default_pawns;
	std::vector< int > idx_default_rooks;
	std::vector< int > idx_default_bishops;
	std::vector< int > idx_default_knight;
	std::vector< int > idx_default_queen;
	std::vector< int > idx_default_king;
	std::map< ChessPiece::TYPE, std::vector< int > > all_idxs;
public:
	const bool isEmpty() const;
	const ChessPiece& pieceAt( const int row, const int column );
	const ChessPiece& piece( const int indexPiece );
	const bool existsPieceAt( const int row, const int column );
	const bool isDarkCell( const int row, const int column );
	const std::vector< ChessPiece >& getPieces() const;
protected:
	void clear();
	void initInDefaultPositions();
	void createPiece( const ChessPiece::TYPE type, const int index, const bool isBlack );
private:
	std::vector< ChessPiece > m_pieces;
	std::vector< int > m_positions;
};

inline const bool ChessBoard::isDarkCell( const int row, const int column )
{
	return ( ( ( row % 2 ) + ( column % 2 ) ) % 2 == 0 );
}

inline const bool ChessBoard::isEmpty() const
{
	return m_pieces.empty();
}

inline const ChessPiece& ChessBoard::pieceAt( const int row, const int column )
{
	return m_pieces[m_positions[row * SIZE + column]];
}

inline const ChessPiece& ChessBoard::piece( const int indexPiece )
{
	return m_pieces.at( indexPiece );
}

inline const std::vector< ChessPiece >& ChessBoard::getPieces() const
{
	return m_pieces;
}