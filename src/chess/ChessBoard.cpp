#include "ChessBoard.h"

ChessBoard::ChessBoard() :
idx_default_pawns( { 8, 9, 10, 11, 12, 13, 14, 15 } ),
idx_default_rooks( { 0, 7 } ),
idx_default_knight( { 1, 6 } ),
idx_default_bishops( { 2, 5 } ),
idx_default_queen( { 3 } ),
idx_default_king( { 4 } )
{
	all_idxs.emplace( ChessPiece::TYPE::PAWN, idx_default_pawns );
	all_idxs.emplace( ChessPiece::TYPE::ROOK, idx_default_rooks );
	all_idxs.emplace( ChessPiece::TYPE::KNIGHT, idx_default_knight );
	all_idxs.emplace( ChessPiece::TYPE::BISHOP, idx_default_bishops );
	all_idxs.emplace( ChessPiece::TYPE::QUEEN, idx_default_queen );
	all_idxs.emplace( ChessPiece::TYPE::KING, idx_default_king );

	m_positions.resize( CELLS_COUNT, -1 );
	initInDefaultPositions();
}

ChessBoard::~ChessBoard()
{}

void ChessBoard::initInDefaultPositions()
{
	for ( auto group : all_idxs )
	{
		ChessPiece::TYPE type = group.first;
		for ( auto idx : group.second )
		{
			createPiece( type, idx, false );
			createPiece( type, CELLS_COUNT - idx - 1, true );
		}
	}
}

void ChessBoard::createPiece( const ChessPiece::TYPE type, const int index, const bool isBlack )
{
	int indexPiece = int( m_pieces.size() );
	m_pieces.push_back( ChessPiece( indexPiece, type, isBlack, index / SIZE, index % SIZE ) );
	m_positions[index] = indexPiece;
}

const bool ChessBoard::existsPieceAt( const int row, const int column )
{
	if ( row >= 0 && row < SIZE && column >= 0 && column < SIZE )
	{
		const int idx = m_positions[row * SIZE + column];
		if ( idx >= 0 && idx < int( m_pieces.size() ) )
		{
			return true;
		}
	}
	return false;
}

void ChessBoard::clear()
{
	m_pieces.clear();
	m_positions.clear();
	m_positions.resize( CELLS_COUNT, -1 );
}