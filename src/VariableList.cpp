/*
 * VariableList.cpp
 *
 *  Created on: 18.04.2016
 *      Author: Marc Hartung
 */

#include "VariableList.hpp"
#include "network_impl/SimNetworkFunctions.hpp"

#include <ostream>
#include <string>
#include <algorithm>

namespace NetOff
{

    VariableList::VariableList(const std::shared_ptr<char>& data)
            : VariableList()
    {
        _vars = getVariableListFromData(data.get())._vars;
    }

    VariableList::VariableList()
            : _vars(std::vector<std::vector<std::string>>(3))
    {
    }

    VariableList::VariableList(const std::vector<std::string>& realVars, const std::vector<std::string>& intVars,
                               const std::vector<std::string>& boolVars)
            : _vars(std::vector<std::vector<std::string>>(3))
    {
        _vars[0] = realVars;
        _vars[1] = intVars;
        _vars[2] = boolVars;
    }

    void VariableList::addReal(const std::string & varName)
    {
        _vars[0].push_back(varName);
    }

    void VariableList::addInt(const std::string & varName)
    {
        _vars[1].push_back(varName);
    }

    void VariableList::addBool(const std::string & varName)
    {
        _vars[2].push_back(varName);
    }

    void VariableList::addReals(const std::vector<std::string> & varNames)
    {
        _vars[0].insert(_vars[0].end(), varNames.begin(), varNames.end());
    }

    void VariableList::addInts(const std::vector<std::string> & varNames)
    {
        _vars[1].insert(_vars[1].end(), varNames.begin(), varNames.end());
    }

    void VariableList::addBools(const std::vector<std::string> & varNames)
    {
        _vars[2].insert(_vars[2].end(), varNames.begin(), varNames.end());
    }

    const std::vector<std::string> & VariableList::getReals() const
    {
        return _vars[0];
    }

    const std::vector<std::string> & VariableList::getInts() const
    {
        return _vars[1];
    }

    const std::vector<std::string> & VariableList::getBools() const
    {
        return _vars[2];
    }

    size_t VariableList::dataSize() const
    {
        size_t res = 0;
        res += 3 * sizeof(size_t);  // safe data as: [numReal,numInt,numBool,[numChars,chars]]
        for (auto i : { 0, 1, 2 })
        {
            for (auto& elem : _vars[i])
                res += getStringDataSize(elem);
        }
        return res;
    }

    bool VariableList::empty() const
    {
        return _vars[0].empty() && _vars[1].empty() && _vars[2].empty();
    }

    std::shared_ptr<const char> VariableList::data() const
    {
        std::shared_ptr<char> res(new char[dataSize()], std::default_delete<char[]>());
        saveVariablesTo(res.get());
        return res;
    }

    std::shared_ptr<char> VariableList::data()
    {
        std::shared_ptr<char> res(new char[dataSize()], std::default_delete<char[]>());
        saveVariablesTo(res.get());
        return res;
    }

    void VariableList::saveVariablesTo(char * data) const
    {
        // safe data as: [numReal,numInt,numBool,[numChars,chars]]
        char * curPos = data;
        for (auto i : { 0, 1, 2 })
            curPos = saveShiftIntegralInData<size_t>(_vars[i].size(), curPos);

        for (auto i : { 0, 1, 2 })
        {
            for (auto& elem : _vars[i])
            {
                saveStringInData(elem, curPos);
                curPos += getStringDataSize(elem);
            }
        }
    }

    VariableList VariableList::getVariableListFromData(const char * data)
    {
        VariableList res;
        std::vector<std::vector<std::string>> & vars = res._vars;

        vars = std::vector<std::vector<std::string>>(3);
        const char * curPos = data;
        for (auto i : { 0, 1, 2 })
        {
            vars[i] = std::vector<std::string>(getIntegralFromData<size_t>(curPos));
            curPos = shift<size_t>(curPos);
        }

        for (auto i : { 0, 1, 2 })
        {
            for (size_t j = 0; j < vars[i].size(); ++j)
            {
                vars[i][j] = createStringFromData(curPos);
                curPos += getStringDataSize(vars[i][j]);
            }
        }
        return res;
    }

    std::ostream & operator<<(std::ostream & out, const VariableList & in)
    {
        out << "Real:[";
        for (auto & str : in.getReals())
        {
            out << str << " ";
        }
        out << "]Ints:[";
        for (auto & str : in.getInts())
        {
            out << str << " ";
        }
        out << "]Bools:[";
        for (auto & str : in.getBools())
        {
            out << str << " ";
        }
        out << "]";
        return out;
    }

    bool VariableList::isSubsetOf(const VariableList & in) const
    {
        bool abort;
        for (size_t i = 0; i < _vars.size(); ++i)
        {
            for (const auto & str1 : _vars[i])
            {
                abort = true;
                for (const auto & str2 : in._vars[i])
                    if (str1 == str2)
                    {
                        abort = false;
                        break;
                    }

                if (abort)
                    return false;
            }
        }
        return true;
    }

    size_t VariableList::sizeReals() const
    {
        return _vars[0].size();
    }

    size_t VariableList::sizeInts() const
    {
        return _vars[1].size();
    }

    size_t VariableList::sizeBools() const
    {
        return _vars[2].size();
    }

    size_t VariableList::findRealVariableNameIndex(const std::string & varName) const
    {
        size_t res = 0;
        auto pos = std::find(_vars[0].begin(), _vars[0].end(), varName);
        if (pos != _vars[0].end())
            res = std::distance(_vars[0].begin(), pos);
        return res;
    }

    void VariableList::print() const
    {
        std::cout << "Begin VariableList::print()\n";
        std::cout << "Real variables:\n";
        for (const auto & str1 : _vars[0])
            std::cout << str1 << "\n";

        std::cout << "Integer variables:\n";
        for (const auto & str1 : _vars[1])
            std::cout << str1 << "\n";

        std::cout << "Boolean variables:\n";
        for (const auto & str1 : _vars[2])
            std::cout << str1 << "\n";

        std::cout << "End VariableList::print()\n";
    }

}  // namespace NetOff

