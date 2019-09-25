#include "ChessBoard.h"
#include <assert.h>

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

void ChessBoard::createPiece( const ChessPiece::TYPE type, const int indexPosition, const bool isBlack )
{
	int indexPiece = int( m_pieces.size() );
	m_pieces.emplace( indexPiece, ChessPiece( indexPiece, type, isBlack, indexPosition / SIZE, indexPosition % SIZE ) );
	m_positions[indexPosition] = indexPiece;
}

const bool ChessBoard::existsPieceAt( const int row, const int column ) const
{
	assert( row >= 0 && row < SIZE && column >= 0 && column < SIZE );
	if ( row >= 0 && row < SIZE && column >= 0 && column < SIZE )
	{
		const int indexPiece = m_positions.at( row * SIZE + column );
		if ( indexPiece >= 0 )
		{
			return true;
		}
	}
	return false;
}

void ChessBoard::removePiece( const int indexPiece )
{
	const int indexPosition = ( m_pieces.at( indexPiece ).row() * SIZE ) + m_pieces.at( indexPiece ).column();
	m_positions[indexPosition] = -1;
	m_pieces.erase( indexPiece );
}

void ChessBoard::restorePiece( const int indexPiece, const bool isBlack, const ChessPiece::TYPE type, const int row, const int column )
{
	assert( m_pieces.find( indexPiece ) == m_pieces.end() );
	m_pieces.emplace( indexPiece, ChessPiece( indexPiece, type, isBlack, row, column ) );
	const int newIndexPosition = ( row * SIZE ) + column;
	assert( m_positions[newIndexPosition] == -1 );
	m_positions[newIndexPosition] = indexPiece;
}

void ChessBoard::movePiece( const int indexPiece, const int row, const int column )
{
	const int indexPosition = ( m_pieces.at(indexPiece).row() * SIZE ) + m_pieces.at( indexPiece ).column();
	m_positions[indexPosition] = -1;
	m_pieces.at( indexPiece ).setPosition( row, column );
	const int newIndexPosition = ( row * SIZE ) + column;
	assert( m_positions[newIndexPosition] == -1 );
	m_positions[newIndexPosition] = indexPiece;
}

void ChessBoard::clear()
{
	m_pieces.clear();
	m_positions.clear();
	m_positions.resize( CELLS_COUNT, -1 );
}