#pragma once

#include "ChannelMode.hpp"
#include "../client/Client.hpp"
#include "ChannelInvite.hpp"
#include "../server/Irc.hpp"

class ChannelUser;
class ChannelInvite;

class Channel
{
private:
    string _channelName;
    string _channelTopic;
    ChannelMode _modes;
    map<int, ChannelUser *> _users; // client socket file descriptor and pointer to ChannelUser
    vector<ChannelInvite> _invites;

public:
    explicit Channel(const string &name);
    ~Channel();

    string get_channel_name() const;

    void set_channel_name(const string &channel_name);

    string get_channel_topic() const;

    void set_channel_topic(const string &channel_topic);

    ChannelMode &modes();

    const ChannelMode &modes() const;

    const std::map<int, ChannelUser *> &users() const;

    bool add_client(Client *client);

    bool remove_client(const Client *client);

    bool is_member(const Client *client) const;

    bool is_operator(const Client *client) const;

    void set_operator(const Client *client, bool is_operator);

    bool is_invited(const Client *client) const;

    void invite(const Client *sender, const Client *receiver); // FIXME maybe set this to channel user instead of regular client

    void revoke_invites();

    bool is_full() const;

    size_t size() const;

    void send_private_message(Client *client, const string &message);

    void send_message(const string &message);

    map<int, ChannelUser *> getUsers();

    void leave_channel(Client *client);

    const vector<ChannelInvite> & get_invites() const;

    size_t countOperators();
};
