#include "../server/Irc.hpp"

/**
 * @brief Checks if a nickname is valid.
 *
 * A nickname must:
 * - Not be empty.
 * - Not exceed the maximum nickname length.
 * - Not contain any invalid characters.
 * - Start with an alphabetic character, an underscore, or a hyphen.
 * 
 * @param nick The nickname to validate.
 * 
 * @return True if the nickname is valid, false otherwise.
*/
bool	is_valid_nick(const string &nick)
{
	if (nick.empty() || nick.length() > MAX_NICK_LENGTH)
		return (false);

	const std::string invalidChars = " ,*?!@.#&:$";
	if (nick.find_first_of(invalidChars) != std::string::npos)
		return (false);

	if (!isalpha(nick[0]) && nick[0] != '_' && nick[0] != '-')
		return (false);

	return (true);
}

/**
 * @brief Handles the NICK command.
 *
 * Syntax:
 * - Set Nick:    NICK <new_nick>
 *
 * @param ss Input string stream containing the command arguments.
 * @param client The client issuing the command.
 *
 * Numeric Replies:
 * - ERR_NONICKNAMEGIVEN (431) - No nickname given.
 * - ERR_ERRONEUSNICKNAME (432) - Erroneous nickname.
 * - ERR_NICKNAMEINUSE (433) - Nickname is already in use.
 * - RPL (NICK) - Nickname change message.
 * 
 *  @see https://www.rfc-editor.org/rfc/rfc2812.html#section-3.1.2
 */

void Irc::nickCmd(istringstream &ss, Client *client)
{
	string			newNick;
	Channel			*channel = NULL;

	if (client->getPassWord().empty())
		return (sendMsg(client->getSock(),
				NOTICE_MSG("Error: A password is required to access this server. Please use the PASS command before continuing.")));
	if (ssLength(ss) < 1 || !(ss >> newNick))
		return (sendMsg(client->getSock(), ERR_NONICKNAMEGIVEN("*")));
	if (!is_valid_nick(newNick))
		return (sendMsg(client->getSock(), ERR_ERRONEUSNICKNAME("*", newNick)));
	if (findClient(newNick))
		return (sendMsg(client->getSock(), ERR_NICKNAMEINUSE(newNick)));
	
	const string	oldNick = client->getNick();
	client->setNick(newNick);
	
	if (!oldNick.empty()) // if it is a rename
		sendMsg(client->getSock(), RPL_NICK(oldNick, client->getUser(), client->getNick()));


	std::vector<Channel *>::iterator it;
	for (it = _serverChannels.begin(); it != _serverChannels.end(); ++it)
	{
		channel = *it;
		if (channel->is_member(client))
		{
			// send RPL_NICK to all members of the channels the client is in
			channel->send_private_message(client, RPL_NICK(oldNick, client->getUser(),client->getNick()));
		}
		else
		{
			cout << client->getNick() << " is not in " << channel->get_channel_name() << endl;
		}
	}
}
