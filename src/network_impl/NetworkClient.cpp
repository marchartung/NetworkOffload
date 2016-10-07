/*
 * NetworkClient.cpp
 *
 *  Created on: 07.04.2016
 *      Author: Marc Hartung
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
    {
    }

    bool NetworkClient::initialize(const std::string & host, const int & port)
    {
        IPaddress ip;
        SDLNet_Init();

        if (SDLNet_ResolveHost(&ip, host.c_str(), port) == -1)
        {
            std::cout << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
            return false;
        }

        unsigned times = 0;
        do
        {
            SDL_Delay(_sleepTime);
            _socket = SDLNet_TCP_Open(&ip);
            std::cout << "Waiting for server on host " << ip.host << " and port " << port << " ..." << std::endl;
        }
        while ((_socket == nullptr || SDLNet_ResolveHost(&ip, host.c_str(), port) == -1) && (times++ <= (unsigned int) _numMaxSleeps));

        if (SDLNet_ResolveHost(&ip, host.c_str(), port) == -1)
        {
            std::cout << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
            return false;
        }

        if (_socket == nullptr)
        {
            std::cout << "no socket" << std::endl;
            return false;
        }
        return true;
    }

    void NetworkClient::deinitialize()
    {
        killSocket(_socket);
    }

} /* namespace NetOff */
