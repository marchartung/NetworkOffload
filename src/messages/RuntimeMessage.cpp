/*
 * StartMessage.cpp
 *
 *  Created on: 18.04.2016
 *      Author: Marc Hartung
 */

#include "../../include/messages/RuntimeMessage.hpp"

namespace NetOff
{
    StartRequestMessage::StartRequestMessage()
            : AbstractMessage<InitialClientMessageSpecifyer>(InitialClientMessageSpecifyer::START)
    {
    }

    StartRequestMessage::StartRequestMessage(std::shared_ptr<char> & data)
            : AbstractMessage<InitialClientMessageSpecifyer>(data)
    {
    }

    StartSuccessMessage::StartSuccessMessage()
            : AbstractMessage<InitialServerMessageSpecifyer>(InitialServerMessageSpecifyer::SUCCESS_START)
    {
    }

    StartSuccessMessage::StartSuccessMessage(std::shared_ptr<char> & data)
            : AbstractMessage<InitialServerMessageSpecifyer>(data)
    {
    }

    PauseRequestMessage::PauseRequestMessage()
            : AbstractMessage<ClientMessageSpecifyer>(ClientMessageSpecifyer::PAUSE)
    {
    }

    PauseRequestMessage::PauseRequestMessage(std::shared_ptr<char>& data)
            : AbstractMessage<ClientMessageSpecifyer>(data)
    {
    }

    PauseSuccessMessage::PauseSuccessMessage()
            : AbstractMessage<ServerMessageSpecifyer>(ServerMessageSpecifyer::SUCCESS_PAUSE)
    {
    }

    PauseSuccessMessage::PauseSuccessMessage(std::shared_ptr<char>& data)
            : AbstractMessage<ServerMessageSpecifyer>(data)
    {
    }

    UnpauseRequestMessage::UnpauseRequestMessage()
            : AbstractMessage<ClientMessageSpecifyer>(ClientMessageSpecifyer::UNPAUSE)
    {
    }

    UnpauseRequestMessage::UnpauseRequestMessage(std::shared_ptr<char>& data)
            : AbstractMessage<ClientMessageSpecifyer>(data)
    {
    }

    UnpauseSuccessMessage::UnpauseSuccessMessage()
            : AbstractMessage<ServerMessageSpecifyer>(ServerMessageSpecifyer::SUCCESS_UNPAUSE)
    {
    }

    UnpauseSuccessMessage::UnpauseSuccessMessage(std::shared_ptr<char>& data)
            : AbstractMessage<ServerMessageSpecifyer>(data)
    {
    }

    ResetRequestMessage::ResetRequestMessage()
            : AbstractMessage<ClientMessageSpecifyer>(ClientMessageSpecifyer::RESET)
    {
    }

    ResetRequestMessage::ResetRequestMessage(std::shared_ptr<char>& data)
            : AbstractMessage<ClientMessageSpecifyer>(data)
    {
    }

    ResetSuccessMessage::ResetSuccessMessage()
            : AbstractMessage<ServerMessageSpecifyer>(ServerMessageSpecifyer::SUCCESS_RESET)
    {
    }

    ResetSuccessMessage::ResetSuccessMessage(std::shared_ptr<char>& data)
            : AbstractMessage<ServerMessageSpecifyer>(data)
    {
    }
}

