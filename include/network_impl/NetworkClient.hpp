/*
 * NetworkClient.hpp
 *
 *  Created on: 07.04.2016
 *      Author: hartung
 */

#ifndef INCLUDE_NETWORK_IMPL_NETWORKCLIENT_HPP_
#define INCLUDE_NETWORK_IMPL_NETWORKCLIENT_HPP_

#include "NetworkMember.hpp"

namespace NetOff
{

    class NetworkClient : public NetworkMember
    {
     public:
        NetworkClient();

        NetworkClient(const NetworkClient &) = delete;

        ~NetworkClient();

        bool initialize(const std::string & host, const int & port);

        void deinitialize();
    };

} /* namespace NetOff */

#endif /* INCLUDE_NETWORK_IMPL_NETWORKCLIENT_HPP_ */
