/*
 * AbstactMessage.hpp
 *
 *  Created on: 18.04.2016
 *      Author: Marc Hartung
 */

#ifndef INCLUDE_MESSAGES_ABSTRACTMESSAGE_HPP_
#define INCLUDE_MESSAGES_ABSTRACTMESSAGE_HPP_

#include "../AdditionalTypes.hpp"

namespace NetOff
{
    template<typename Specifyer>
    class AbstractMessage : public RawDataAccessable
    {
     public:
        AbstractMessage()
     {

     }

        AbstractMessage(const Specifyer & in)
                : req(in)
        {

        }

        AbstractMessage(std::shared_ptr<char> & data)
                : req(*reinterpret_cast<Specifyer *>(data.get()))
        {

        }

        virtual ~AbstractMessage()
        {
        }

        bool testRequest(const Specifyer & should) const
        {
            return req == should;
        }

        virtual char * data()
        {
            return reinterpret_cast<char *>(&req);
        }

        virtual const char * data() const
        {
            return reinterpret_cast<const char *>(&req);
        }

        virtual size_t dataSize() const
        {
            return sizeof(Specifyer);
        }

     protected:

        Specifyer req;
    };
}

#endif /* INCLUDE_MESSAGES_ABSTRACTMESSAGE_HPP_ */
