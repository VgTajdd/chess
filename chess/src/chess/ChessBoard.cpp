#include "ChessBoard.h"

using namespace std;

ChessBoard::ChessBoard() : BaseItem(),
idx_default_pawns( { 8, 9, 10, 11, 12, 13, 14, 15 } ),
idx_default_rooks( { 0, 7} ),
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
			createPiece( type, idx, true );
			createPiece( type, CELLS_COUNT - idx - 1, false );
		}
	}
}

void ChessBoard::createPiece( const ChessPiece::TYPE type, const int index, const bool isBlack )
{
	m_pieces.push_back( ChessPiece( type, isBlack, index / SIZE, index % SIZE ) );
	m_positions[index] = int( m_pieces.size() ) - 1;
}

const bool ChessBoard::isEmpty() const
{
	return m_pieces.empty();
}

const ChessPiece& ChessBoard::pieceAt( const int row, const int column )
{
	return m_pieces[m_positions[row * SIZE + column]];
}

const bool ChessBoard::existsPieceAt( const int row, const int column )
{
	if ( row >= 0 && row < SIZE && column >= 0 && column < SIZE )
	{
		const int idx = m_positions[row * SIZE + column];
		if ( idx >= 0 == idx < m_pieces.size() )
		{
			return true;
		}
	}
	return false;
}

const bool ChessBoard::isDarkCell( const int row, const int column )
{
	return ( ( ( row % 2 ) + ( column % 2 ) ) % 2 == 0 );
}

void ChessBoard::parseChessInput( const string input, bool& ok )
{
	string ans;
	ok = false;
	for ( int i = 0; i < input.length(); i++ )
	{
		char c = input[i];
		if ( isdigit( input[i] ) )
		{
			int t = atoi( &c );
			ans.append( string( t, '*' ) );
		}
		else if ( isalpha( input[i] ) )
		{
			ans.append( string( 1, c ) );
		}
	}

	// Validations.
	if ( ans.size() == CELLS_COUNT )
	{
		map< ChessPiece::TYPE, vector< int > > freq_b, freq_w;
		for ( int i = 0; i < ans.length(); i++ )
		{
			char c = tolower( ans[i] );
			ChessPiece::TYPE type = ChessPiece::TYPE::NONE;
			switch ( c )
			{
				case 'p': type = ChessPiece::TYPE::PAWN; break;
				case 'r': type = ChessPiece::TYPE::ROOK; break;
				case 'b': type = ChessPiece::TYPE::BISHOP; break;
				case 'n': type = ChessPiece::TYPE::KNIGHT; break;
				case 'q': type = ChessPiece::TYPE::QUEEN; break;
				case 'k': type = ChessPiece::TYPE::KING; break;
				default:  type = ChessPiece::TYPE::NONE; break;
			}
			if ( type != ChessPiece::TYPE::NONE )
			{
				if ( islower( ans[i] ) ) // Lower -> white.
				{
					freq_w[type].push_back( i );
				}
				else
				{
					freq_b[type].push_back( i );
				}
			}
		}
		bool invalidFreqs = false;
		for ( auto group : all_idxs )
		{
			auto type = group.first;
			if ( ( freq_b[type].size() != group.second.size() )
				 || ( freq_w[type].size() != group.second.size() ) )
			{
				invalidFreqs = true;
				break;
			}
		}
		if ( !invalidFreqs )
		{
			ok = true;

			// Creating new chess.
			m_positions.resize( CELLS_COUNT, -1 );
			for ( auto group : freq_b )
			{
				ChessPiece::TYPE type = group.first;
				for ( auto idx : group.second )
				{
					createPiece( type, idx, true );
				}
			}
			for ( auto group : freq_w )
			{
				ChessPiece::TYPE type = group.first;
				for ( auto idx : group.second )
				{
					createPiece( type, idx, false );
				}
			}
		}
	}
}

void ChessBoard::clear()
{
	m_pieces.clear();
	m_positions.clear();
	m_positions.resize( CELLS_COUNT, -1 );
}