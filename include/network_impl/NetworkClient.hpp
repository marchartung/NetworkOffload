/*
 * NetworkClient.hpp
 *
 *  Created on: 07.04.2016
 *      Author: Marc Hartung
 */

#ifndef INCLUDE_NETWORK_IMPL_NETWORKCLIENT_HPP_
#define INCLUDE_NETWORK_IMPL_NETWORKCLIENT_HPP_

#include "NetworkMember.hpp"
#include <string>

namespace NetOff
{

    class NetworkClient : public NetworkMember
    {
     public:
        NetworkClient();

        NetworkClient(const NetworkClient &) = delete;

        NetworkClient& operator=(const NetworkClient &) = delete;

        ~NetworkClient();

        bool initialize(const std::string & host, const int & port);

        void deinitialize() override;

        char getChar() const override;
    };

} /* namespace NetOff */

#endif /* INCLUDE_NETWORK_IMPL_NETWORKCLIENT_HPP_ */
