/*
 * NetworkServer.cpp
 *
 *  Created on: 07.04.2016
 *      Author: Marc Hartung
 */

#include "../../include/network_impl/NetworkServer.hpp"
#include <iostream>

namespace NetOff
{

    NetworkServer::NetworkServer()
            : NetworkMember(),
              _tcpsock(nullptr)
    {

    }

    NetworkServer::~NetworkServer()
    {
        killSocket(_tcpsock);
    }

    bool NetworkServer::initialize(const int & port)
    {
        IPaddress ip;
        SDLNet_Init();

        if (SDLNet_ResolveHost(&ip, NULL, port) == -1)
        {
            std::cout << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
            return false;
        }

        _tcpsock = SDLNet_TCP_Open(&ip);
        if (!_tcpsock)
        {
            std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
            return false;
        }

        _socket = SDLNet_TCP_Accept(_tcpsock);
        unsigned times = 0;
        while (_socket == NULL && times++ <= _numMaxSleeps)
        {
            SDL_Delay(_sleepTime);
            std::cout << "Waiting for client on host " << ip.host << " and port " << port << " ..." << std::endl;
            _socket = SDLNet_TCP_Accept(_tcpsock);
        }

        if (_socket == nullptr)
        {
            return false;
        }
        return true;
    }

    void NetworkServer::deinitialize()
    {
        killSocket(_tcpsock);
        killSocket(_socket);
    }
} /* namespace NetOff */

