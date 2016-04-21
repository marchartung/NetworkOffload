/*
 * NetworkServer.cpp
 *
 *  Created on: 07.04.2016
 *      Author: hartung
 */

#include "../../include/network_impl/NetworkServer.hpp"

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

    bool NetworkServer::initialize(const int& port)
    {
        IPaddress ip;

        if (SDLNet_ResolveHost(&ip, NULL, port) == -1)
        {
            return false;
        }

        _tcpsock = SDLNet_TCP_Open(&ip);
        if (!_tcpsock)
        {
            return false;
        }

        _socket = SDLNet_TCP_Accept(_tcpsock);
        unsigned times = 0;
        while (_socket == NULL && times++ <= _numMaxSleeps)
        {
            SDL_Delay (_sleepTime);
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

