/*
 * NetworkMember.hpp
 *
 *  Created on: 07.04.2016
 *      Author: Marc Hartung
 */

#ifndef INCLUDE_NETWORK_IMPL_NETWORKMEMBER_HPP_
#define INCLUDE_NETWORK_IMPL_NETWORKMEMBER_HPP_

#include <SDL2/SDL_net.h>
#include <memory>

namespace NetOff
{
    class NetworkMember
    {
     public:
        NetworkMember();

        virtual ~NetworkMember();

        bool send(const char * buffer, const int & num);

        bool recv(char * buffer, const int & num);

        bool variableSend(const char * buffer, const int & num);

        std::shared_ptr<char> variableRecv(size_t * numBytes = nullptr);

        virtual void deinitialize() = 0;

        virtual char getChar() const = 0;

     protected:

        TCPsocket _socket;

        int _maxBuffer;
        int _sleepTime;
        int _numMaxSleeps;

        static void killSocket(TCPsocket & socket);
    };

}

#endif /* INCLUDE_NETWORK_IMPL_NETWORKMEMBER_HPP_ */
