#include "Irc.hpp"

Irc::Irc(void)
{
    cmds["JOIN"] = &Irc::joinCmd;
    cmds["TOPIC"] = &Irc::topicCmd;
    cmds["PRIVMSG"] = &Irc::privmsgCmd;
    cmds["PASS"] = &Irc::passCmd;
    cmds["PART"] = &Irc::partCmd;
    cmds["MODE"] = &Irc::modeCmd;
    cmds["NICK"] = &Irc::nickCmd;
    cmds["USER"] = &Irc::userCmd;
    cmds["INVITE"] = &Irc::inviteCmd;
    cmds["QUIT"] = &Irc::quitCmd;
    cmds["KICK"] = &Irc::kickCmd;
    cmds["INFO_CHANNEL"] = &Irc::infoChannel;
    cout << CYAN "Server started (Ctrl+c to quit)" END << endl;
}

Irc::~Irc(void)
{
    for (map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
        deleteClient((*it).second);
    for (vector<Channel *>::iterator it = _serverChannels.begin(); it != _serverChannels.end(); it++)
        delete *it;
    if (epfds)
        delete epfds;
    cout << CYAN "Sever terminated" END << endl;
}

void Irc::setPort(int port)
{
    if (port <= 0 || port > MAX_TCP_PORT)
        throw std::runtime_error("Invalid port! Port must be between 1 and 65535.");
    _port = port;
}

void Irc::setPassword(string password)
{
    for (size_t i = 0; i < password.size(); i++)
    {
        if (iswspace(password[i]))
            throw std::runtime_error("Invalid password! No whitespaces allowed.");
        if (password[i] == ',')
            throw std::runtime_error("Invalid password! No commas allowed.");
    }
    _serverPassWord = password;
}

void Irc::saveClients() const
{
    std::string filename_client = "data/clients.csv";
    std::ofstream outfile_client(filename_client.c_str(), std::ios::trunc);
    if (!outfile_client)
    {
        std::cerr << "Error: Unable to open file for writing: " << filename_client << std::endl;
        return;
    }

    // CSV Header
    outfile_client << "fd_cl,nick,auth_state,authenticated,buffer" << std::endl;

    for (std::map<int, Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (!it->second)
            continue; // Ensure pointer is valid

        outfile_client << it->first << ","
                       << it->second->getNick() << ","
                       << it->second->getAuthState() << ","
                       << it->second->isAuthenticated() << ","
                       << "\"" << it->second->getBuffer() << "\"" // Wrap in quotes in case of commas
                       << std::endl;
    }
    outfile_client.close();
}

void Irc::saveRequests() const
{
    std::string filename_requests = "data/requests.csv";
    std::ofstream outFile_requests(filename_requests.c_str());
    if (!outFile_requests)
    {
        std::cerr << "Error: Unable to open file for writing: " << filename_requests << std::endl;
        return;
    }

    // CSV Header
    outFile_requests << "fd_client,string" << std::endl;

    for (std::map<int, std::string>::const_iterator it_r = requests.begin(); it_r != requests.end(); ++it_r)
    {
        outFile_requests << it_r->first << ",\"" << it_r->second << "\"" << std::endl;
    }
    outFile_requests.close();
}

void Irc::saveChannels() const
{
    std::string filename_serverChannel = "data/channels.csv";
    std::ofstream outFile_serverChannel(filename_serverChannel.c_str());
    if (!outFile_serverChannel)
    {
        std::cerr << "Error: Unable to open file for writing: " << filename_serverChannel << std::endl;
        return;
    }

    // CSV Header
    outFile_serverChannel << "ChannelName,Topic, InviteOnly, TopicRestricted, PasswordProtected, UserLimited, UserLimit, Password" << std::endl;

    for (std::vector<Channel *>::const_iterator it_sc = _serverChannels.begin(); it_sc != _serverChannels.end(); ++it_sc)
    {
        Channel *channel = *it_sc;
        if (!channel)
            continue; // Ensure pointer is valid

        outFile_serverChannel
            << channel->get_channel_name() << ","
            << "\"" << channel->get_channel_topic() << "\"," // Wrap in quotes in case of commas
            << channel->modes().is_invite_only() << ","
            << channel->modes().is_topic_restricted() << ","
            << channel->modes().is_password_protected() << ","
            << channel->modes().is_user_limited() << ","
            << channel->modes().get_user_limit() << ","
            << "\"" << channel->modes().get_password() << "\"" // Password might have commas
            << std::endl;
    }
    outFile_serverChannel.close();
}

void Irc::saveChannelUsers() const
{
    std::string filename_channelUser = "data/channel_users.csv";
    std::ofstream outFile_channelUser(filename_channelUser.c_str());
    if (!outFile_channelUser)
    {
        std::cerr << "Error: Unable to open file for writing: " << filename_channelUser << std::endl;
        return;
    }

    // CSV Header
    outFile_channelUser << "ChannelName,UserNick,UserFd,IsOperator,IsInvited" << std::endl;

    for (std::vector<Channel *>::const_iterator it_sc = _serverChannels.begin(); it_sc != _serverChannels.end(); ++it_sc)
    {
        Channel *channel = *it_sc;
        if (!channel)
            continue; // Ensure pointer is valid

        std::map<int, ChannelUser *> users = channel->users();
        for (std::map<int, ChannelUser *>::const_iterator it_cu = users.begin(); it_cu != users.end(); ++it_cu)
        {
            ChannelUser *channelUser = it_cu->second;
            if (!channelUser || !channelUser->get_client())
                continue;

            outFile_channelUser
                << channel->get_channel_name() << ","
                << channelUser->get_client()->getNick() << ","
                << channelUser->get_client()->getSock() << ","
                << channelUser->is_operator() << ","
                << channelUser->is_invited()
                << std::endl;
        }
    }
    outFile_channelUser.close();
}