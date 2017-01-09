/*
 * ValueContainer.cpp
 *
 *  Created on: 07.04.2016
 *      Author: Marc Hartung
 */

#include "ValueContainer.hpp"
#include <algorithm>

namespace NetOff
{

    ValueContainer::ValueContainer()
            : _data(nullptr),
              _dataPtr(nullptr),
              _realData(nullptr),
              _intData(nullptr),
              _boolData(nullptr),
              _numReal(0),
              _numInt(0),
              _numBool(0),
              _dataSize(0),
              _id(-1)
    {
    }

    ValueContainer::ValueContainer(std::shared_ptr<char> data, char * containerStart, size_t numReals, size_t numInts,
                                   size_t numBools, const int & id)
            : _data(std::move(data)),
              _dataPtr(containerStart),
              _realData(nullptr),
              _intData(nullptr),
              _boolData(nullptr),
              _numReal(numReals),
              _numInt(numInts),
              _numBool(numBools),
              _dataSize(calcDataSize()),
              _id(id)
    {
        setPointers();
    }

    ValueContainer::ValueContainer(ValueContainer && in)
            : ValueContainer()
    {
        *this = std::move(in);
    }

    ValueContainer & ValueContainer::operator=(ValueContainer && vc)
    {
        std::swap(_data, vc._data);
        std::swap(_dataSize, vc._dataSize);
        std::swap(_numReal, vc._numReal);
        std::swap(_numInt, vc._numInt);
        std::swap(_numBool, vc._numBool);
        std::swap(_data, vc._data);
        std::swap(_realData, vc._realData);
        std::swap(_intData, vc._intData);
        std::swap(_boolData, vc._boolData);
        std::swap(_id, vc._id);
        return *this;
    }

    void ValueContainer::setRealValues(const double * values)
    {
        std::copy(values, values + _numReal, _realData);
    }

    void ValueContainer::setIntValues(const int * values)
    {
        std::copy(values, values + _numInt, _intData);
    }

    void ValueContainer::setBoolValues(const char * values)
    {
        std::copy(values, values + _numBool, _boolData);
    }

    const double * ValueContainer::getRealValues() const
    {
        return _realData;
    }

    const int * ValueContainer::getIntValues() const
    {
        return _intData;
    }

    const char * ValueContainer::getBoolValues() const
    {
        return _boolData;
    }

    double * ValueContainer::getRealValues()
    {
        return _realData;
    }

    int * ValueContainer::getIntValues()
    {
        return _intData;
    }

    char * ValueContainer::getBoolValues()
    {
        return _boolData;
    }

    size_t ValueContainer::sizeReal() const
    {
        return _numReal;
    }

    size_t ValueContainer::sizeInt() const
    {
        return _numInt;
    }

    size_t ValueContainer::sizeBool() const
    {
        return _numBool;
    }

    size_t ValueContainer::dataSize() const
    {
        return _dataSize;
    }

    const int & ValueContainer::getSimId() const
    {
        return _id;
    }

    char * ValueContainer::data()
    {
        return _dataPtr;
    }

    void ValueContainer::init(size_t numReal, size_t numInt, size_t numBool)
    {
        std::tie(_numReal, _numInt, _numBool) = std::tie(numReal, numInt, numBool);
        _dataSize = calcDataSize();
        _data = std::shared_ptr<char>(new char[_dataSize], std::default_delete<char[]>());
        _id = -1;
        setPointers();
    }

    void ValueContainer::reinit(size_t numReal, size_t numInt, size_t numBool)
    {
        std::shared_ptr<char> oldData = _data;
        double * oldReal = _realData;
        int * oldInt = _intData;
        char * oldBool = _boolData;
        size_t oldRealNum = _numReal;
        size_t oldIntNum = _numInt;
        size_t oldBoolNum = _numBool;

        init(numReal, numInt, numBool);

        std::copy(oldReal, oldReal + std::min(oldRealNum, _numReal), _realData);
        std::copy(oldInt, oldInt + std::min(oldIntNum, _numInt), _intData);
        std::copy(oldBool, oldBool + std::min(oldBoolNum, _numBool), _boolData);
    }

    std::ostream & operator<<(std::ostream & stream, const ValueContainer & container)
    {
        stream << "[ Real:[";
        if (container._numReal > 0)
        {
            for (size_t i = 0; i < container._numReal - 1; ++i)
            {
                stream << container._realData[i] << ", ";

            }
            stream << container._realData[container._numReal - 1];
        }
        stream << "], Int:[";
        if (container._numInt > 0)
        {
            for (size_t i = 0; i < container._numInt - 1; ++i)
            {
                stream << container._intData[i] << ", ";

            }
            stream << container._intData[container._numInt - 1];
        }
        stream << "], Bool:[";
        if (container._numBool > 0)
        {
            for (size_t i = 0; i < container._numBool - 1; ++i)
            {
                stream << container._boolData[i] << ", ";

            }
            stream << container._boolData[container._numBool - 1];
        }
        stream << "]]";

        return stream;
    }

    void ValueContainer::setPointers()
    {
        _realData = reinterpret_cast<double *>(_dataPtr);
        _intData = reinterpret_cast<int *>(_realData + _numReal);
        _boolData = reinterpret_cast<char *>(_intData + _numInt);
    }

    const char * ValueContainer::data() const
    {
        return _dataPtr;
    }

    size_t ValueContainer::calcDataSize() const
    {
        return calcDataSize(_numReal, _numInt, _numBool);
    }

    size_t ValueContainer::calcDataSize(size_t numReal, size_t numInt, size_t numBool)
    {
        return numReal * sizeof(double) + numInt * sizeof(int) + numBool * sizeof(char);
    }

}  // namespace NetOff
