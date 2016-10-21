/*
 * ValueContainer.hpp
 *
 *  Created on: 07.04.2016
 *      Author: Marc Hartung
 */

#ifndef INCLUDE_VALUECONTAINER_HPP_
#define INCLUDE_VALUECONTAINER_HPP_

#include <memory>
#include <iostream>
#include <vector>

#include "AdditionalTypes.hpp"

namespace NetOff
{

    class ValueContainer : public RawDataAccessable
    {
        template<typename T>
        friend class ValueContainerMessage;
     public:

        ValueContainer();

        ValueContainer(ValueContainer && in);

        ValueContainer & operator=(ValueContainer && vc);

        ~ValueContainer();

        void setRealValues(const double * values);
        void setIntValues(const int * values);
        void setBoolValues(const char * values);

        const double * getRealValues() const;
        const int * getIntValues() const;
        const char * getBoolValues() const;

        double * getRealValues();
        int * getIntValues();
        char * getBoolValues();

        size_t sizeReal() const;
        size_t sizeInt() const;
        size_t sizeBool() const;
        size_t dataSize() const;

        const int & getSimId() const;

        char * data();

        const char * data() const;

        friend std::ostream & operator<<(std::ostream & stream, const ValueContainer & container);

     private:

        std::shared_ptr<char> _data;
        char * _dataPtr;
        double * _realData;
        int * _intData;
        char * _boolData;

        size_t _numReal;
        size_t _numInt;
        size_t _numBool;

        size_t _dataSize;

        int _id;

        ValueContainer(const std::shared_ptr<char> & data, char * containerStart, size_t numReals, size_t numInts,
                       size_t numBools, const int & id);

        void init(size_t numReal, size_t numInt, size_t numBool);

        void reinit(size_t numReal, size_t numInt, size_t numBool);

        void setPointers();

        size_t calcDataSize() const;

        static size_t calcDataSize(size_t numReal, size_t numInt, size_t numBool);

    };

}
#endif /* INCLUDE_VALUECONTAINER_HPP_ */
