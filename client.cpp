#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <thread>

class TCPClient
{
private:
	std::string name{};
	int serverPort{};
	int connectionPeriod{};
	int clientSocket{};
	bool connectEstablished = false;

public:
	TCPClient( const std::string &clientName, int port, int period ) :
		name( clientName ), serverPort( port ), connectionPeriod( period )
	{
	}

	void Connect();
	void Run();
};

void TCPClient::Connect()
{
	clientSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if( clientSocket == -1 )
	{
		std::cerr << "Failed to create socket.\n";
		return;
	}

	sockaddr_in serverAddress{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons( serverPort );
	serverAddress.sin_addr.s_addr = inet_addr( "127.0.0.1" );

	int conStatus = connect( clientSocket, (struct sockaddr *)&serverAddress, sizeof( serverAddress ) );
	if( conStatus == -1 )
	{
		std::cerr << "Connection failed.\n";
		return;
	}

	connectEstablished = true;
}
void TCPClient::Run()
{
	if( !connectEstablished )
	{
		return;
	}

	std::string timestamp_str;
	timestamp_str.reserve( 64 );
	while( 1 )
	{
		timestamp_str.clear();

		auto now = std::chrono::system_clock::now();
		auto now_time = std::chrono::system_clock::to_time_t( now );
		auto *now_tm = std::localtime( &now_time );
		auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>( now.time_since_epoch() ) % 1000;

		char timestamp[32];
		std::strftime( timestamp, sizeof( timestamp ), "[%Y-%m-%d %H:%M:%S.", now_tm );
		timestamp_str = timestamp + std::to_string( now_ms.count() ) + "] ";
		timestamp_str.append( name );

		send( clientSocket, timestamp_str.data(), timestamp_str.length(), 0 );
		std::cout << timestamp_str << "\n";

		std::this_thread::sleep_for( std::chrono::seconds{ connectionPeriod } );
	}

	close( clientSocket );
}
int main( int argc, char *argv[] )
{
	if( argc < 4 )
	{
		std::cerr << "Usage: client <name> <server_port> <connection_period>\n";
		return 1;
	}

	std::string name( argv[1] );
	int serverPort = atoi( argv[2] );
	int connectionPeriod = atoi( argv[3] );

	TCPClient client( name, serverPort, connectionPeriod );
	client.Connect();
	client.Run();

	return 0;
}
