#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>
#include <thread>
#include <fstream>

class TCPServer
{
private:
	int serverPort;
	std::ofstream logFile;
	std::mutex logMutex;
	int serverSocket{};

public:
	explicit TCPServer( int port ) : serverPort( port )
	{
	}

	void Start();

private:
	void handleClient( int clientSocket );
	void logMessage( const std::string &message );
};

void TCPServer::handleClient( int clientSocket )
{
	char buffer[256] = { 0 };
	auto bytesRead = 0u;

	while( ( bytesRead = recv( clientSocket, buffer, sizeof( buffer ), 0 ) ) > 0 )
	{
		if( bytesRead <= 0 )
		{
			std::cerr << "Error reading from client.\n";
			close( clientSocket );
			return;
		}

		logMessage( { buffer } );
	}

	close( clientSocket );
}

void TCPServer::logMessage( const std::string &message )
{
	std::lock_guard<std::mutex> lock( logMutex );
	logFile.open( "log.txt", std::ios::app );
	if( logFile.is_open() )
	{
		logFile << message << "\n";
		std::cout << message << "\n";
	}
	logFile.close();
}
void TCPServer::Start()
{
	serverSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if( serverSocket == -1 )
	{
		std::cerr << "Failed to create socket.\n";
		return;
	}

	sockaddr_in serverAddress{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl( INADDR_ANY );
	serverAddress.sin_port = htons( serverPort );

	if( bind( serverSocket, (struct sockaddr *)&serverAddress, sizeof( serverAddress ) ) < 0 )
	{
		std::cerr << "Bind failed.\n";
		return;
	}

	if( listen( serverSocket, 5 ) < 0 )
	{
		std::cerr << "Listen failed.\n";
		return;
	}

	std::cout << "Server started. Listening to port " << serverPort << "\n";

	while( 1 )
	{
		int clientSocket = accept( serverSocket, nullptr, nullptr );
		if( clientSocket < 0 )
		{
			std::cerr << "Accept failed.\n";
			continue;
		}

		std::thread clientThread( &TCPServer::handleClient, this, clientSocket );
		clientThread.detach();
	}
}
int main( int argc, char *argv[] )
{
	if( argc < 2 )
	{
		std::cerr << "Usage: server <port>\n";
		return 1;
	}

	int serverPort = atoi( argv[1] );

	TCPServer server( serverPort );
	server.Start();

	return 0;
}
