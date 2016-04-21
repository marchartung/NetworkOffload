/*
 * VariableList.hpp
 *
 *  Created on: 18.04.2016
 *      Author: hartung
 */

#ifndef INCLUDE_VARIABLELIST_HPP_
#define INCLUDE_VARIABLELIST_HPP_

#include "AdditionalTypes.hpp"
#include <iostream>

namespace NetOff
{
    class VariableList : public SharedDataAccessable
    {
     public:

        VariableList();

        VariableList(const std::vector<std::string> & realVars, const std::vector<std::string> & intVars, const std::vector<std::string> & boolVars);

        void addReal(const std::string & varName);
        void addInt(const std::string & varName);
        void addBool(const std::string & varName);

        const std::vector<std::string> & getReals() const;
        const std::vector<std::string> & getInts() const;
        const std::vector<std::string> & getBools() const;

        bool empty() const;

        size_t dataSize() const;

        std::shared_ptr<const char> data() const;

        std::shared_ptr<char> data();

        void saveVariablesTo(char * data) const;

        static VariableList getVariableListFromData(const char * data);

        friend std::ostream & operator<<(std::ostream & out, const VariableList & in);

     private:
        std::vector<std::vector<std::string>> _vars;

        VariableList(const std::shared_ptr<char> & data);
    };
}
#endif /* INCLUDE_VARIABLELIST_HPP_ */
