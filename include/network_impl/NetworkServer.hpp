/*
 * NetworkServer.hpp
 *
 *  Created on: 07.04.2016
 *      Author: Marc Hartung
 */

#ifndef INCLUDE_NETWORK_IMPL_NETWORKSERVER_HPP_
#define INCLUDE_NETWORK_IMPL_NETWORKSERVER_HPP_

#include "NetworkMember.hpp"

namespace NetOff
{

    class NetworkServer : public NetworkMember
    {
     public:
        NetworkServer();

        NetworkServer(const NetworkServer &) = delete;

        virtual ~NetworkServer();

        bool initialize(const int & port);

        void deinitialize() override;


        virtual char getChar() const
        {
            return 'S';
        }

     private:
        TCPsocket _tcpsock;

    };

} /* namespace NetOff */

#endif /* INCLUDE_NETWORK_IMPL_NETWORKSERVER_HPP_ */
