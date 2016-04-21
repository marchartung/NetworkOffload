/*
 * TypeEnum.hpp
 *
 *  Created on: 07.04.2016
 *      Author: hartung
 */

#ifndef INCLUDE_ADDITIONALTYPES_HPP_
#define INCLUDE_ADDITIONALTYPES_HPP_

#include <vector>
#include <tuple>
#include <memory>

namespace NetOff
{
    enum InitialClientMessageSpecifyer
    {
        START = 0,
        ADD_SIM = 1,
        INIT_SIM = 2,
        CLIENT_INIT_ABORT = 3
    };

    enum ClientMessageSpecifyer
    {
        PAUSE = 4,
        UNPAUSE = 5,
        RESET = 6,
        INPUTS = 7,
        CLIENT_ABORT = 8
    };

    enum InitialServerMessageSpecifyer
    {
        SUCCESS_ADD_SIM = 9,
        SUCCESS_START = 10,
        SERVER_INIT_ABORT = 11
    };

    enum ServerMessageSpecifyer
    {
        SUCCESS_SIM_INIT = 12,
        SUCCESS_PAUSE = 13,
        SUCCESS_UNPAUSE = 14,
        SUCCESS_RESET = 15,
        OUTPUTS = 16,
        SERVER_ABORT = 17
    };

    enum CurrentState
    {
        NONE = 0,
        INITED = 1,
        STARTED = 2,
        PAUSED = 3
    };

    class SharedDataAccessable
    {
     public:
        virtual ~SharedDataAccessable()
        {
        }
        virtual std::shared_ptr<char> data() = 0;
        virtual std::shared_ptr<const char> data() const = 0;
        virtual size_t dataSize() const = 0;
    };

    class RawDataAccessable
    {
     public:
        virtual ~RawDataAccessable() { }
        virtual char * data() = 0;
        virtual const char * data() const = 0;
        virtual size_t dataSize() const = 0;
    };

}

#endif /* INCLUDE_ADDITIONALTYPES_HPP_ */
