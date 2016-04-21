/*
 * FmuNetworkFunctions.hpp
 *
 *  Created on: 08.04.2016
 *      Author: hartung
 */

#ifndef INCLUDE_NETWORK_IMPL_SIMNETWORKFUNCTIONS_HPP_
#define INCLUDE_NETWORK_IMPL_SIMNETWORKFUNCTIONS_HPP_

#include "../AdditionalTypes.hpp"

#include <iostream>

namespace NetOff
{
    std::string createStringFromData(const char * data);

    size_t getStringDataSize(const std::string & str);

    size_t saveStringInData(const std::string & str, char * data);

    template<typename T>
    char * saveShiftIntegralInData(const T & val, char* data)
    {
        *reinterpret_cast<T*>(data) = val;
        return data + sizeof(T);
    }

    template<typename T>
    char * saveShiftDirectDataAccessableInData(const T & val, char * data)
    {
        static_assert(std::is_base_of<SharedDataAccessable,T>::value,
                "saveShiftDirectDataAccessableInData(): Invalid template argument. Template must implement DirectDataAccessable.");
        std::shared_ptr<const char> sData = val.data();
        const char * pstart = sData.get(), *pend = sData.get() + val.dataSize();
        std::copy(pstart, pend, data);
        return data + val.dataSize();
    }

    template<typename T>
    char * saveShiftPointerInData(const T * ptr, const size_t & num, char * data)
    {
        if (num == 0)
            return data;
        std::copy(ptr, ptr + num, reinterpret_cast<T*>(data));
        return data + num * sizeof(T);
    }

    template<typename T>
    T getIntegralFromData(const char* data)
    {
        return *reinterpret_cast<const T *>(data);
    }

    template<typename T>
    char * shift(char * in)
    {
        return in + sizeof(T);
    }

    template<typename T>
    char * shift(size_t num, char * in)
    {
        return in + num * sizeof(T);
    }

    template<typename T>
    char * shiftDataAccessable(const T & in, char * p)
    {
        return p + in.dataSize();
    }

    template<typename T>
    const char * shift(const char * in)
    {
        return in + sizeof(T);
    }
}

#endif /* INCLUDE_NETWORK_IMPL_SIMNETWORKFUNCTIONS_HPP_ */
