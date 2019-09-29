#pragma once
#include <vector>
#include <map>
#include "../chess/BaseItem.h"
#include "../chess/ChessPiece.h"

class ChessBoard;
class ChessGame;
class ChessPlayer;

struct ChessGameSettings
{
	ChessGameSettings( const bool infiniteLoop = true,
					   const unsigned int movementTime = 100,
					   const unsigned int humanPlayers = 0,
					   const unsigned int levelAI = 4,
					   const int decisionTimeAI = 0 ):
		_infiniteLoop( infiniteLoop ),
		_movementTime( movementTime ),
		_humanPlayers( humanPlayers ),
		_levelAI( levelAI ),
		_decisionTimeAI( decisionTimeAI )
	{};
private:
	bool _infiniteLoop;
	unsigned int _movementTime;
	unsigned int _humanPlayers;
	/*--------AI--------
	0: random
	1: eats (if possible) random
	2: eats (if possible) random only safe
	3: eats (if possible) by importance only safe
	4: intelligent
	------------------*/
	unsigned int _levelAI;
	unsigned int _decisionTimeAI;
public:
	const unsigned int humanPlayers() const;
	const bool infiniteLoop() const;
	const unsigned int movementTime() const;
	const unsigned int decisionTimeAI() const;
	const unsigned int levelAI() const;
};

inline const unsigned int ChessGameSettings::humanPlayers() const
{
	return _humanPlayers;
}

inline const bool ChessGameSettings::infiniteLoop() const
{
	return _infiniteLoop;
}

inline const unsigned int ChessGameSettings::movementTime() const
{
	return _movementTime;
}

inline const unsigned int ChessGameSettings::decisionTimeAI() const
{
	return _decisionTimeAI;
}

inline const unsigned int ChessGameSettings::levelAI() const
{
	return _levelAI;
}

struct CellNode
{
	int r;
	int c;
	CellNode() { r = 0; c = 0; }
	CellNode( const int _r, const int _c ) : r( _r ), c( _c ) {}
	bool operator==( const CellNode& a ) const
	{
		return ( r == a.r ) && ( c == a.c );
	}
	CellNode& operator=( const CellNode& a )
	{
		r = a.r;
		c = a.c;
		return *this;
	}
	CellNode operator+( CellNode a ) const
	{
		return { a.r + r, a.c + c };
	}
	CellNode operator-( CellNode a ) const
	{
		return { a.r - r, a.c - c };
	}
};

class LinearPath
{
public:
	LinearPath( const int _relUX, const int _relUY, const int _steps ) :
		relUX( _relUX ), relUY( _relUY ), steps( _steps )
	{};
	CellNode getNode( const int indexNode );
	int relUX;
	int relUY;
	int steps;
};

class ChessPath
{
public:
	ChessPath() : m_totalSteps( 0 ) {};
	~ChessPath();
	ChessPath* addPath( const int _relUX, const int _relUY, const int _steps );
	CellNode getNode( const int indexNode );
	const int totalSteps() const;
	int m_totalSteps;
	std::vector< LinearPath* > m_paths;
};

inline const int ChessPath::totalSteps() const
{
	return m_totalSteps;
}

class ChessRules
{
public:
	ChessRules();
	~ChessRules();
	const std::vector< ChessPath* >& getPaths( const ChessPiece::TYPE type ) const;
	const int getImportance( const ChessPiece::TYPE type ) const;
private:
	ChessPath* addChessPath( const ChessPiece::TYPE type );
private:
	std::map< ChessPiece::TYPE, std::vector< ChessPath* > > m_chessPaths;
};

inline const std::vector< ChessPath* >& ChessRules::getPaths( const ChessPiece::TYPE type ) const
{
	return m_chessPaths.at( type );
}

class ChessGame
{
public:
	ChessGame( const ChessGameSettings& settings );
	~ChessGame();
	void update( const int dt );
	void togglePlayerInTurn();
	void clear();
	void createGame();
	void resetGame();
	const std::vector< ChessPath* >& getPotentialPaths( const ChessPiece::TYPE ) const;
	const ChessGameSettings& settings() const;
	const ChessRules* rules() const;
	static const char* namePiece( const ChessPiece::TYPE );
	const ChessPlayer* const player( const bool isBlack ) const;

	// Helper methods.
	void getPossibleAssassinsOf( const int indexPiece, std::vector< int >& assassins ) const;
	void getPossibleVictims( std::vector< int >& victims, const bool isBlack, const bool onlySafe ) const;
	const bool isSafeToMoveTo( const int indexPiece, const CellNode& node ) const;
	void getPossiblePositions( std::map< int, std::vector< CellNode > >& possiblePositions, const bool isBlack, const bool onlyEat, const bool onlySafe ) const;
	std::vector< CellNode > getPossiblePositionsByPiece( const int indexPiece, const bool onlyEat, const bool onlySafe ) const;
	std::vector< CellNode > getPawnPosiblePositions( const int indexPiece, const bool onlyEat, const bool onlySafe ) const;
	std::vector< CellNode > getKnightPossiblePositions( const int indexPiece, const bool onlyEat, const bool onlySafe ) const;
	std::vector< CellNode > getGenericPossiblePositions( const int indexPiece, const bool onlyEat, const bool onlySafe, const ChessPiece::TYPE type ) const;
	std::pair< int, CellNode > getBlockingFriend( const int indexFriend, const int indexEnemy );
	void getFriendsThatCanReach( std::vector< int >& friends, const CellNode& node, const bool isBlack );
	const int getIndexKing( const bool isBlack ) const;
	void getCellState( const CellNode& node, bool& isEmpy, bool& isBlack ) const;
	const bool isInJake( const bool isBlack ) const;
private:
	ChessGameSettings m_settings;
	ChessBoard* m_board;
	ChessPlayer* m_playerW;
	ChessPlayer* m_playerB;
	ChessPlayer* m_activePlayer;
	ChessRules* m_rules;
	bool m_finished;
	bool m_inInBlackTurn;
	int m_turnCounter;
};

inline const ChessGameSettings& ChessGame::settings() const
{
	return m_settings;
}

inline const ChessRules* ChessGame::rules() const
{
	return m_rules;
}

inline const ChessPlayer* const ChessGame::player( const bool isBlack ) const
{
	return isBlack ? m_playerB : m_playerW;
}