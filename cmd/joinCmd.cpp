
#include "../server/Irc.hpp"

static bool verifyChannelmodes(Channel *tarChannel, Client *client, istringstream &ss)
{
	string pass;
	ss >> pass;

	if (tarChannel->modes().is_invite_only() && !tarChannel->is_invited(client))
		return (sendMsg(client->getSock(), ERR_INVITEONLYCHAN(client->getNick(), tarChannel->get_channel_name())), 1);
	else if (tarChannel->modes().get_user_limit() && tarChannel->is_full())
		return (sendMsg(client->getSock(), ERR_CHANNELISFULL(client->getNick(), tarChannel->get_channel_name())), 1);
	else if (tarChannel->modes().is_password_protected() && (pass != tarChannel->modes().get_password()))
		return (sendMsg(client->getSock(), ERR_BADCHANNELKEY(client->getNick(), tarChannel->get_channel_name())), 1);

	return 0;
}

void Irc::joinCmd(istringstream &ss, Client *client)
{
	string msg;
	string channelName;
	if (!(ss >> channelName))
		return sendMsg(client->getSock(), ERR_NEEDMOREPARAMS(client->getNick(), "JOIN"));

	if (channelName.empty() || channelName[0] != '#')
		return sendMsg(client->getSock(), ERR_NOSUCHCHANNEL(client->getNick(), channelName));

	Channel *tarChannel;
	if ((tarChannel = findChannel(channelName)))
	{
		if (tarChannel->is_member(client))
		{
			sendMsg(client->getSock(), "Already in this channel\n\r");
			return;
		}
		if (!verifyChannelmodes(tarChannel, client, ss))
		{
			tarChannel->add_client(client);
			cout << "Send to client fd: " << client->getSock() << endl;
			tarChannel->send_message(RPL_JOIN(client->getNick(), client->getUser(), channelName, string("realname"))); //!FIXME realname is not implemented?
		}
		return;
	}
	tarChannel = createChannel(channelName);
	tarChannel->add_client(client);

	cout << "Send to client fd: " << client->getSock() << endl;
	tarChannel->send_message(RPL_JOIN(client->getNick(), client->getUser(), channelName, string("realname"))); //!FIXME realname is not implemented?
}