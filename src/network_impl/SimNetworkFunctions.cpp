/*
 * SimNetworkFunctions.cpp
 *
 *  Created on: 18.04.2016
 *      Author: Marc Hartung
 */

#include "../../include/network_impl/SimNetworkFunctions.hpp"

#include <iostream>

namespace NetOff
{
    std::string createStringFromData(const char * data)
    {
        size_t num = getIntegralFromData<size_t>(data);
        const char * start = shift<size_t>(data);
        std::string res(start, num);
        return res;
    }

    size_t getStringDataSize(const std::string & str)
    {
        return sizeof(size_t) + sizeof(char) * (str.size() + 1);  // first is for saving how many chars will be there
    }


    size_t saveStringInData(const std::string & str, char * data)
    {
        char * p = saveShiftIntegralInData<size_t>(str.size(),data);
        for (size_t i = 0; i < str.size(); ++i)
        {
            *p = str[i];
            ++p;
        }
        return getStringDataSize(str);
    }

}
