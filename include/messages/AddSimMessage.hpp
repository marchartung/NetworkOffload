/*
 * AddSimMessage.hpp
 *
 *  Created on: 18.04.2016
 *      Author: Marc Hartung
 */

#ifndef INCLUDE_MESSAGES_ADDSIMMESSAGE_HPP_
#define INCLUDE_MESSAGES_ADDSIMMESSAGE_HPP_

#include "AbstractMessage.hpp"
#include "../AdditionalTypes.hpp"
#include "../VariableList.hpp"

namespace NetOff
{
    class AddSimRequestMessage : public AbstractMessage<InitialClientMessageSpecifyer>
    {

     public:

        AddSimRequestMessage(const int & id, const std::string & path);

        AddSimRequestMessage(std::shared_ptr<char> & data);

        char * data() override;

        const char * data() const override;

        size_t dataSize() const override;

        int getSimId();

        std::string getPath();

     private:
        std::shared_ptr<char> _data;
        size_t _dataSize;
    };

    class AddSimSuccessMessage : public AbstractMessage<InitialServerMessageSpecifyer>
    {

     public:

        AddSimSuccessMessage(const int & id, const VariableList & inputs, const VariableList & outputs);

        AddSimSuccessMessage(std::shared_ptr<char> & data);

        char * data() override;

        const char * data() const override;

        size_t dataSize() const override;

        const int & getSimId() const;

        VariableList getInputVariableList() const;

        VariableList getOutputVariableList() const;

        std::string getPath();

     private:
        std::shared_ptr<char> _data;
        size_t _dataSize;
    };

}

#endif /* INCLUDE_MESSAGES_ADDSIMMESSAGE_HPP_ */
