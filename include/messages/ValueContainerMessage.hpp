/*
 * ValueContainerMessage.hpp
 *
 *  Created on: 18.04.2016
 *      Author: Marc Hartung
 */

#ifndef INCLUDE_MESSAGES_VALUECONTAINERMESSAGE_HPP_
#define INCLUDE_MESSAGES_VALUECONTAINERMESSAGE_HPP_

#include "AbstractMessage.hpp"
#include "../network_impl/SimNetworkFunctions.hpp"
#include "../VariableList.hpp"
#include "../ValueContainer.hpp"

namespace NetOff
{

    template<typename Specifyer>
    class ValueContainerMessage : public AbstractMessage<Specifyer>
    {
     public:

        ValueContainerMessage(const int & simId, const VariableList & vars, const Specifyer & spec)
                : AbstractMessage<Specifyer>(spec),
                  _data(std::shared_ptr<char>(new char[_dataSize])),
                  _dataSize(0),
                  _spec(nullptr),
                  _id(nullptr),
                  _time(nullptr),
                  _container()
        {
            _dataSize = sizeof(Specifyer) + sizeof(int) + sizeof(double)
                    + ValueContainer::calcDataSize(vars.getReals().size(), vars.getInts().size(), vars.getBools().size());
            char * p = _data.get();
            _spec = reinterpret_cast<Specifyer *>(p);
            p = saveShiftIntegralInData<Specifyer>(spec, p);
            _id = reinterpret_cast<int *>(p);
            p = saveShiftIntegralInData(simId, p);
            _time = reinterpret_cast<double *>(p);
            p = saveShiftIntegralInData<double>(0.0,p);
            _container = ValueContainer(_data, p, vars.getReals().size(), vars.getInts().size(), vars.getBools().size(),simId);
        }

        ValueContainerMessage()
                : AbstractMessage<Specifyer>(),
                  _data(std::shared_ptr<char>(new char[_dataSize])),
                  _dataSize(0),
                  _spec(nullptr),
                  _id(nullptr),
                  _time(nullptr),
                  _container()
        {

        }

        ValueContainer & getContainer()
        {
            return _container;
        }

        const ValueContainer & getContainer() const
        {
            return _container;
        }

        void setTime(const double & time)
        {
            *_time = time;
        }

        void setSpecifyer(const Specifyer & spec)
        {
            *_spec = spec;
        }

        const int & getId() const
        {
            return *_id;
        }

        const double & getTime() const
        {
            return *_time;
        }

        const Specifyer getSpecifyer() const
        {
            return *_spec;
        }

        char * data() override
        {
            return _data.get();
        }

        const char * data() const override
        {
            return _data.get();
        }

        size_t dataSize() const override
        {
            return _dataSize;
        }

     private:

        std::shared_ptr<char> _data;
        size_t _dataSize;

        Specifyer * _spec;
        int * _id;
        double * _time;

        ValueContainer _container;

    };
}

#endif /* INCLUDE_MESSAGES_VALUECONTAINERMESSAGE_HPP_ */
