/*
 * NetworkClient.cpp
 *
 *  Created on: 07.04.2016
 *      Author: hartung
 */

#include "../../include/network_impl/NetworkClient.hpp"

#include <iostream>
namespace NetOff
{

    NetworkClient::NetworkClient()
            : NetworkMember()
    {

    }

    NetworkClient::~NetworkClient()
    {}

    bool NetworkClient::initialize(const std::string& host, const int& port)
    {
        IPaddress ip;

        unsigned times = 0;
        SDLNet_ResolveHost(&ip, host.c_str(), port);
        do
        {
            SDL_Delay(_sleepTime);
            _socket = SDLNet_TCP_Open(&ip);

        } while((_socket == nullptr || SDLNet_ResolveHost(&ip, host.c_str(), port) == -1) && times++ <= _numMaxSleeps);
        if (SDLNet_ResolveHost(&ip, host.c_str(), port) == -1)
        {
            return false;
        }

        if (_socket == nullptr)
        {
            return false;
        }
        return true;
    }

    void NetworkClient::deinitialize()
    {
        killSocket(_socket);
    }

} /* namespace NetOff */
