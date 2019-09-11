#pragma once
#include "BaseItem.h"
#include "ChessPiece.h"
#include <vector>
#include <map>
#include <string>

class ChessBoard : public BaseItem
{
public:
	const static int SIZE = 8;
	const static int CELLS_COUNT = 64;
public:
	ChessBoard();
	~ChessBoard();
private:
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
	const bool existsPieceAt( const int row, const int column );
	const bool isDarkCell( const int row, const int column );
public:
	void parseChessInput( const std::string input, bool& ok ); // special method.
private:
	void clear();
	void initInDefaultPositions();
	void createPiece( const ChessPiece::TYPE type, const int index, const bool isBlack );
private:
	std::vector< ChessPiece > m_pieces;
	std::vector< int > m_positions;
};