#include "../../../game/ChessGame.h"
#include <thread>
#include <chrono>

class Application
{
public:
	Application();
	~Application();
public:
	void start();
private:
	bool m_finished;
	std::chrono::steady_clock::time_point m_lastTime;
	ChessGame* m_game;
};

Application::Application():
	m_finished( false ),
	m_game( nullptr )
{
	m_game = new ChessGame();
	m_game->start();
}

Application::~Application()
{
	delete m_game;
}

void Application::start()
{
	m_lastTime = std::chrono::steady_clock::now();
	while ( !m_finished )
	{
		std::chrono::milliseconds delta( 16 );
		std::this_thread::sleep_for( delta );
		if ( m_game != nullptr )
		{
			std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
			int dt = int( std::chrono::duration_cast<std::chrono::milliseconds>( now - m_lastTime ).count() );
			m_game->update( dt );
			m_lastTime = now;
		}
	}
}

int main()
{
	Application app;
	app.start();
}