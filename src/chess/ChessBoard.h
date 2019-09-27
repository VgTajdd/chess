#pragma once
#include "ChessPiece.h"
#include <vector>
#include <map>

class ChessPlayer;
class ChessGame;

class ChessBoard
{
friend class ChessPlayer;
friend class ChessGame;
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
	const ChessPiece& pieceAt( const int row, const int column ) const;
	const ChessPiece& piece( const int indexPiece ) const;
	const bool existsPiece( const int indexPiece ) const;
	const bool existsPieceAt( const int row, const int column ) const;
	const bool isDarkCell( const int row, const int column ) const;
	const std::map< int, ChessPiece >& getPieces() const;
protected:
	void clear();
	void removePiece( const int indexPiece );
	void restorePiece( const int indexPiece, const bool isBlack, const ChessPiece::TYPE type, const int row, const int column );
	void movePieceTo( const int indexPiece, const int row, const int column );
	void initInDefaultPositions();
	void createPiece( const ChessPiece::TYPE type, const int indexPosition, const bool isBlack );
private:
	std::map< int, ChessPiece > m_pieces;
	std::vector< int > m_positions;
};

inline const bool ChessBoard::isDarkCell( const int row, const int column ) const
{
	return ( ( ( row % 2 ) + ( column % 2 ) ) % 2 == 0 );
}

inline const bool ChessBoard::isEmpty() const
{
	return m_pieces.empty();
}
inline const bool ChessBoard::existsPiece( const int indexPiece ) const
{
	return ( m_pieces.find( indexPiece ) != m_pieces.end() );
}

inline const ChessPiece& ChessBoard::pieceAt( const int row, const int column ) const
{
	return m_pieces.at( m_positions.at( row * SIZE + column ) );
}

inline const ChessPiece& ChessBoard::piece( const int indexPiece ) const
{
	return m_pieces.at( indexPiece );
}

inline const std::map< int, ChessPiece >& ChessBoard::getPieces() const
{
	return m_pieces;
}