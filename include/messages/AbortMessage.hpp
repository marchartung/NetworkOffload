/*
 * AbortMessage.hpp
 *
 *  Created on: 19.04.2016
 *      Author: Marc Hartung
 */

#ifndef INCLUDE_MESSAGES_ABORTMESSAGE_HPP_
#define INCLUDE_MESSAGES_ABORTMESSAGE_HPP_

#include "AbstractMessage.hpp"
#include "AdditionalTypes.hpp"

namespace NetOff
{

    class AbortRequestMessage : public AbstractMessage<InitialClientMessageSpecifyer>
    {
     public:
        AbortRequestMessage()
                : AbstractMessage<InitialClientMessageSpecifyer>(InitialClientMessageSpecifyer::CLIENT_INIT_ABORT)
        {

        }
    };
}

#endif /* INCLUDE_MESSAGES_ABORTMESSAGE_HPP_ */
