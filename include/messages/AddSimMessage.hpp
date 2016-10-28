/*
 * AddSimMessage.hpp
 *
 *  Created on: 18.04.2016
 *      Author: Marc Hartung
 */

#ifndef INCLUDE_MESSAGES_ADDSIMMESSAGE_HPP_
#define INCLUDE_MESSAGES_ADDSIMMESSAGE_HPP_

#include "messages/AbstractMessage.hpp"
#include "AdditionalTypes.hpp"
#include "VariableList.hpp"

#include <memory>
#include <string>

namespace NetOff
{

    class AddSimRequestMessage : public AbstractMessage<InitialClientMessageSpecifyer>
    {
     public:

        AddSimRequestMessage(const int & id, const std::string & path);

        AddSimRequestMessage(std::shared_ptr<char> & data);

        AddSimRequestMessage(const AddSimRequestMessage& rhs) = default;

        AddSimRequestMessage& operator=(const AddSimRequestMessage& rhs) = default;

        ~AddSimRequestMessage() = default;

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

        AddSimSuccessMessage(const AddSimSuccessMessage& rhs) = default;

        AddSimSuccessMessage& operator=(const AddSimSuccessMessage& rhs) = default;

        ~AddSimSuccessMessage() = default;

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
