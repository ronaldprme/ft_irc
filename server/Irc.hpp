#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <sys/epoll.h>
#include <fcntl.h>
#include <ctime>

#define END "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"

#define BACKLOG 100
#define MAX_EVENTS 1024
#define BUFFER_SIZE 1024
#define MAX_TCP_PORT 65535
#define MAX_NICK_LENGTH 9
#define TIMEOUT_MS 100

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::find;
using std::getline;
using std::ios;
using std::istringstream;
using std::make_pair;
using std::map;
using std::ofstream;
using std::pair;
using std::runtime_error;
using std::string;
using std::stringstream;
using std::vector;

/*
 * To fully authenticate a client,
 * the server must receive a PASS command,
 * followed by a NICK command and finally a USER command.
 * 
 * USER_AUTH Status and AUTHENTICATED Status are the same for now
 * but offer more flexibility in the future if needed.
*/
enum authState
{
	NO_AUTH,
	PASS_AUTH,
	NICK_AUTH,
	USER_AUTH,
	AUTHENTICATED
};

class Client;
class Channel;
class EpollManager;

#include "serverNumeric.hpp"
#include "../client/Client.hpp"
#include "../epoll/EpollManager.hpp"
#include "../channel/Channel.hpp"
#include "../channel/ChannelUser.hpp"

extern bool running;

int ssLength(istringstream &ss);
void sendMsg(int fd, string msg);
void logger(int type, int data);

class Irc
{
public:
	Irc(void);
	~Irc(void);
	int run_server(char **av);
	// ServerUtils
	Client *findClient(int target);
	Client *findClient(string name);
	Channel *findChannel(string name);
	Channel *createChannel(string name);
	void deleteChannel(string name);
	void deleteClient(Client* client);

private:
	int _port;
	string _serverPassWord;

	int _serverSock;
	EpollManager *epfds;
	map<int, Client *> _clients; // map<fd, Client*>
	map<int, string> requests;
	vector<Channel *> _serverChannels;

	void setPassword(string password);
	void setPort(int port);

	// ServerNetwork
	void initNetwork();
	void setNonBlocking(int fd);
	bool isNewClient(int targetFd);
	void acceptClient(int serverFd);

	void sendResponse(int targetFd);
	void receiveRequest(int targetFd);

	//Commands
	typedef void (Irc::*CommandPtr)(istringstream &line, Client *client);
	map<string, CommandPtr> cmds;

	void sendToChannel(Client *sender, const string &channelName, const string &message);
	void sendToUser(const Client *sender, const string &recipient, const string &message);

	void privmsgCmd(istringstream &ss, Client *client);
	void joinCmd(istringstream &ss, Client *client);
	void partCmd(istringstream &ss, Client *client);
	void topicCmd(istringstream &ss, Client *client);
	void modeCmd(istringstream &ss, Client *client);
	void passCmd(istringstream &ss, Client *client);
	void nickCmd(istringstream &ss, Client *client);
	void userCmd(istringstream &ss, Client *client);
	void inviteCmd(istringstream &ss, Client *client);
	void quitCmd(istringstream &ss, Client *client);
	void kickCmd(istringstream &ss, Client *client);

	//Command to list informations about the channel
	void infoChannel(istringstream &ss, Client *client);

	void saveChannelUsers() const;
	void saveChannels() const;
	void saveRequests() const;
	void saveClients() const;
	void saveChannelInvites() const;
};
