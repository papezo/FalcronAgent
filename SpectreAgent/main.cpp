#include <iostream>
#include <Windows.h>
#include <thread>

using namespace std;


void HandleClientConnection(HANDLE hPipe)
{
	char buffer[1024];
	DWORD bytesRead;

	while (true)
	{
		BOOL result = ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
		if (!result || bytesRead == 0)
		{
			// uf read fails or pipe is closed, exit the loop
			break;
		}

		buffer[bytesRead] = '\0'; // Null-terminate the string
		cout << "Received from DLL: " << buffer << endl;
	}
}
void StartNamePipeServer()
{
	while (true)
	{
		HANDLE hPipe = CreateNamedPipeW(
			TEXT("\\\\.\\pipe\\SpectreAgentPipe"), // Pipe name
			PIPE_ACCESS_DUPLEX, // Read/Write access
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, // Pipe mode
			PIPE_UNLIMITED_INSTANCES, // Max instances
			1024, 1024, 0, NULL // Buffer sizes
		);

		if (hPipe == INVALID_HANDLE_VALUE)
		{
			cout << "Failed to create named pipe" << endl;
			return;
		}

		cout << "Waiting for client connection..." << endl;

		BOOL isConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

		if (isConnected)
		{
			cout << "Client connected. Spawning handler thread..." << endl;
			thread clientThread(HandleClientConnection, hPipe);

			clientThread.detach();
		}
		else {
			CloseHandle(hPipe);
		}
	}
}


int main()
{
	cout << "Starting named pipe server..." << endl;	

	// Start name pipe server logic here
	StartNamePipeServer();

	return 0;
}