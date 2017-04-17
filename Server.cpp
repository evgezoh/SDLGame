#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <sdkddkver.h>

#include <fstream>
#include <thread>
//#include <mutex>
#include <vector>
#include <string>


SOCKET Connect;
SOCKET *Connections;
SOCKET Listen;

int ClientCount = 0;
const int MaxCountOfThreads = 64;

#define S_WIDTH 440
#define S_HEIGHT 680
#define BALL_SIZE 20
#define BALL_STEP 10

std::vector <std::pair <SOCKET, SOCKET>> players(MaxCountOfThreads / 2, std::pair <SOCKET, SOCKET>(-1, -1));
std::vector <std::pair <int, int>> ballposition(MaxCountOfThreads / 2, std::pair <int, int>(S_HEIGHT / 2 - BALL_SIZE / 2, S_HEIGHT / 2 - BALL_SIZE / 2));
std::vector <int> distance(MaxCountOfThreads, 0);

//void clientThread(int ID);
void clientThread();
void findClients();

void changeBallPosition(int angel, int &BallPosY, int id)
{
	if (!(id % 2))
	{
		if (angel >= 0)
		{
			if (BallPosY >= 0)
				BallPosY-=2;
		}
		if (BallPosY <= 0)
		{
			BallPosY = 0;
		}
		if (angel < 0)
		{
			if (BallPosY <= S_HEIGHT)
				BallPosY+=2;
		}
		if (BallPosY >= S_HEIGHT)
		{
			BallPosY = S_HEIGHT;
		}

		//std::cout << "position has been changed 1" << std::endl;

		ballposition[id / 2].second = S_HEIGHT - BALL_SIZE - BallPosY;
	}
	else
	{
		if (angel >= 0)
		{
			if (BallPosY < S_HEIGHT)
				BallPosY+=2;
		}
		if (BallPosY <= 0)
		{
			BallPosY = 0;
		}
		if (angel < 0)
		{
			if (BallPosY > 0)
				BallPosY-=2;
		}
		if (BallPosY >= S_HEIGHT)
		{
			BallPosY = S_HEIGHT;
		}

		//std::cout << "position has been changed 2" << std::endl;

		ballposition[id / 2].second = S_HEIGHT - BALL_SIZE - BallPosY;
	}
}

struct sendData
{
	int EnemyX;
	int EnemyY;
	int BallX;
	int BallY;
};

int main()
{
	setlocale(LC_ALL, "Russian");
	WSAData data;
	WORD version = MAKEWORD(2, 2);

	int res = WSAStartup(version, &data);
	if (res)
	{
		return 0;
	}

	struct addrinfo hints;
	struct addrinfo *result;

	Connections = new SOCKET(MaxCountOfThreads);
	//Connections = (SOCKET*)calloc(64, sizeof(SOCKET));

	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	getaddrinfo(NULL, "777", &hints, &result);

	Listen = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	bind(Listen, result->ai_addr, result->ai_addrlen);
	listen(Listen, SOMAXCONN);

	freeaddrinfo(result);

	std::cout << "Server start" << std::endl;

	char m_connect[] = "I'm Server";

	std::thread* Th = new std::thread(clientThread);
	std::thread* Th1 = new std::thread(findClients);

	Th->join();
	Th1->join();

	system("Pause");
	return 0;
}

void findClients()
{
	for (;;Sleep(75))
	{
		if (Connect = accept(Listen, NULL, NULL))
		{
			std::cout << "Client " << Connect << " connect... (" << ClientCount << ")" << std::endl;
			Connections[ClientCount] = Connect;

			if (players[ClientCount / 2].first != -1)
			{
				players[ClientCount / 2].second = Connect;
			}
			else
			{
				players[ClientCount / 2].first = Connect;
			}
			ClientCount++;
		}
	}
}

void clientThread()
{
	while (true)
	{
		for (int ID = 0; ID < ClientCount; ID++)
		{

			int BallPosX = S_WIDTH / 2 - BALL_SIZE / 2;
			//int BallPosY = S_HEIGHT / 2 - BALL_SIZE / 2;

			int angel;
			bool collision;

			char ch[1024];

			bool inLoop = true;

			if (recv(Connections[ID], ch, 1024, NULL) > 0)
			{
				Sleep(10);

				std::string str1 = std::string(ch);
				angel = 0;
				int minus = 1;
				int number = 1;
				sendData senddata = { 0,0,0,0 };
				for (int i = 0; i < str1.length(); ++i)
				{
					if (int(str1[i]) == 32)
					{
						number++;
					}
					else
					{
						switch (number)
						{
						case 1: senddata.EnemyX *= 10; senddata.EnemyX += (int(str1[i]) - 48); break;
						case 2: senddata.EnemyY *= 10; senddata.EnemyY += (int(str1[i]) - 48); break;
						case 3: if (int(str1[i]) == 45) { minus = -1; }
								else { angel *= 10; angel += (int(str1[i]) - 48); } break;
						case 4: collision = int(str1[i]) - 48; break;

						}
					}
				}

				angel *= minus;

				if (collision)
				{
					if (ID % 2)
					{
						distance[ID - 1] = 0;
					}
					else
					{
						distance[ID + 1] = 0;
					}
					distance[ID] = 1000;
					collision = false;
				}
				if (distance[ID] > 0)
				{
					distance[ID]--;
					changeBallPosition(angel, ballposition[ID / 2].first, ID);
				}

				if (ID % 2)
				{
					//std::cout << "Second client sent to first" << std::endl;

					std::string toclient = std::to_string(senddata.EnemyX) + " " + std::to_string(senddata.EnemyY) + " "
						+ std::to_string(BallPosX) + " " + std::to_string(ballposition[ID / 2].first);

					std::string toclient1 = std::to_string(1000) + " " + std::to_string(1000) + " "
						+ std::to_string(S_WIDTH - BALL_SIZE - BallPosX) + " " + std::to_string(ballposition[ID / 2].second);

					send(players[ID / 2].first, toclient.c_str(), 25, NULL);
					send(players[ID / 2].second, toclient1.c_str(), 25, NULL);

				}
				else
				{
					//std::cout << "First client sent to second" << std::endl;

					std::string toclient = std::to_string(senddata.EnemyX) + " " + std::to_string(senddata.EnemyY) + " "
						+ std::to_string(S_WIDTH - BALL_SIZE - BallPosX) + " " + std::to_string(ballposition[ID / 2].second);

					std::string toclient1 = std::to_string(1000) + " " + std::to_string(1000) + " "
						+ std::to_string(BallPosX) + " " + std::to_string(ballposition[ID / 2].first);

					if (players[ID / 2].second != -1)
					{
						send(players[ID / 2].second, toclient.c_str(), 25, NULL);
					}
					send(players[ID / 2].first, toclient1.c_str(), 25, NULL);

				}
			}
		}
	}
}
