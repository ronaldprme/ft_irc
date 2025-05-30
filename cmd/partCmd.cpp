#include "../server/Irc.hpp"

/**
 * @brief used to leave a channel
 * 
 * PART <channel>
 * 
 * @param ss input string stream
 * @param client client
 * 
 * Numeric Replies:
 * ERR_NOTONCHANNEL (442) - The client is not in the channel
 * ERR_NOSUCHCHANNEL (403) - The channel does not exist
 * ERR_NEEDMOREPARAMS (461) - Missing parameters
 *
 * @see https://www.rfc-editor.org/rfc/rfc2812.html#section-3.2.2
 * 
 */
void Irc::partCmd(istringstream &ss, Client *client)
{
    string channelName;
    ss >> channelName;

    if (channelName.empty())
        return sendMsg(client->getSock(), ERR_NEEDMOREPARAMS(client->getNick(), "PART"));

    Channel *channel = findChannel(channelName);

    if (!channel)
        return sendMsg(client->getSock(), ERR_NOSUCHCHANNEL(client->getNick(), channelName));

    if (!channel->is_member(client))
        return sendMsg(client->getSock(), ERR_NOTONCHANNEL(client->getNick(), channelName));

    channel->send_message(RPL_PART(client->getNick(), client->getUser(), channelName, "Leaving"));

    channel->leave_channel(client);

    if (channel->size() == 0)
        deleteChannel(channelName);
}