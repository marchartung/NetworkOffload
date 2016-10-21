/*
 * StartMessage.hpp
 *
 *  Created on: 18.04.2016
 *      Author: Marc Hartung
 */

#ifndef INCLUDE_MESSAGES_RUNTIMEMESSAGE_HPP_
#define INCLUDE_MESSAGES_RUNTIMEMESSAGE_HPP_

#include "AbstractMessage.hpp"

namespace NetOff
{

    class StartRequestMessage : public AbstractMessage<InitialClientMessageSpecifyer>
    {
     public:
        StartRequestMessage();

        StartRequestMessage(std::shared_ptr<char> & data);
    };

    class StartSuccessMessage : public AbstractMessage<InitialServerMessageSpecifyer>
    {
     public:
        StartSuccessMessage();

        StartSuccessMessage(std::shared_ptr<char> & data);
    };

    class PauseRequestMessage : public AbstractMessage<ClientMessageSpecifyer>
    {
     public:
        PauseRequestMessage();

        PauseRequestMessage(std::shared_ptr<char> & data);
    };

    class PauseSuccessMessage : public AbstractMessage<ServerMessageSpecifyer>
    {
     public:
        PauseSuccessMessage();

        PauseSuccessMessage(std::shared_ptr<char> & data);
    };

    class UnpauseRequestMessage : public AbstractMessage<ClientMessageSpecifyer>
    {
     public:
        UnpauseRequestMessage();

        UnpauseRequestMessage(std::shared_ptr<char> & data);
    };

    class UnpauseSuccessMessage : public AbstractMessage<ServerMessageSpecifyer>
    {
     public:
        UnpauseSuccessMessage();

        UnpauseSuccessMessage(std::shared_ptr<char> & data);
    };

    class ResetRequestMessage : public AbstractMessage<ClientMessageSpecifyer>
    {
     public:
        ResetRequestMessage();

        ResetRequestMessage(std::shared_ptr<char> & data);
    };

    class ResetSuccessMessage : public AbstractMessage<ServerMessageSpecifyer>
    {
     public:
        ResetSuccessMessage();

        ResetSuccessMessage(std::shared_ptr<char> & data);
    };

}

#endif /* INCLUDE_MESSAGES_RUNTIMEMESSAGE_HPP_ */
