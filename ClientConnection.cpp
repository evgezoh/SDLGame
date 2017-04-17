// Client_cpp.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <thread>
#include "Connection.h"

using namespace std;

struct Enemy_Ball
{
	int EPosX;
	int EPosY;
	int Angel;
	bool Took;
	int BPosX;
	int BPosY;
} enemy_ball{ 0, 0, 0, 0, 0, 0 };

Connection::Connection()
{
	long SUCCESSFUL;
	WSAData WinSockData;
	WORD DLLVersion;
	DLLVersion = MAKEWORD(2, 1);
	SUCCESSFUL = WSAStartup(DLLVersion, &WinSockData);

	string RESPONSE;
	string CONVERTER;
	char MESSAGE[200];

	SOCKADDR_IN ADDRESS;

	sock = socket(AF_INET, SOCK_STREAM, NULL);

	ADDRESS.sin_addr.s_addr = inet_addr("127.0.0.1");
	ADDRESS.sin_family = AF_INET;
	ADDRESS.sin_port = htons(777);

	cout << "Do you want to connect?" << endl;
	getline(cin, RESPONSE);

	RESPONSE[0] = tolower(RESPONSE[0]);

	if (RESPONSE == "n")
	{
		cout << "Ok" << endl;
	}
	else if (RESPONSE == "y")
	{
		connect(sock, (SOCKADDR*)&ADDRESS, sizeof(ADDRESS));

		bool inLoop = true;
		while (inLoop)
		{
			send(sock, (to_string(Player::PosX) + " " + to_string(Player::PosY) + " " + to_string(int(Ball::angel)) + " " + (Ball::took ? to_string(1) : to_string(0))).c_str(), 25, NULL);

			Ball::took = false;

			char str[25] = "message";

			if (recv(sock, str, 25, NULL) > 0)
			{
				std::pair <int, int> Pair;
				enemy_ball = Enemy_Ball{ 0,0,0,0,0,0 };

				int number = 1;
				for (int i = 0; i < ((string)str).length(); ++i)
				{
					if (int(((string)str)[i]) == 32)
					{
						number++;
					}
					else
					{
						switch (number)
						{
						case 1: enemy_ball.EPosX *= 10; enemy_ball.EPosX += (int(((string)str)[i]) - 48); break;
						case 2: enemy_ball.EPosY *= 10; enemy_ball.EPosY += (int(((string)str)[i]) - 48); break;
						case 3: enemy_ball.BPosX *= 10; enemy_ball.BPosX += (int(((string)str)[i]) - 48); break;
						case 4: enemy_ball.BPosY *= 10; enemy_ball.BPosY += (int(((string)str)[i]) - 48); break;
						}
					}
				}

				Ball::PosX = enemy_ball.BPosX;
				Ball::PosY = enemy_ball.BPosY;

				if (enemy_ball.EPosX < 1000)
				{
					Enemy::PosX = enemy_ball.EPosX;
					Enemy::PosY = enemy_ball.EPosY;
				}
			}
			
			else
			{
				std::cout << "Server not found" << std::endl;
			}

		}
	}
	else
	{
		cout << "RESPONSE Error" << endl;
	}
}
