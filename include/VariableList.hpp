/*
 * VariableList.hpp
 *
 *  Created on: 18.04.2016
 *      Author: Marc Hartung
 */

#ifndef INCLUDE_VARIABLELIST_HPP_
#define INCLUDE_VARIABLELIST_HPP_

#include "AdditionalTypes.hpp"
#include <iostream>
#include <vector>

namespace NetOff
{
    /*! \brief
     *
     * todo Marc!
     */
    class VariableList : public SharedDataAccessable
    {
     public:

        VariableList();

        VariableList(const std::vector<std::string> & realVars, const std::vector<std::string> & intVars, const std::vector<std::string> & boolVars);

        /*! \brief Adds a real variable to the container.
         *
         * @param varName Name of the variable.
         */
        void addReal(const std::string & varName);

        /*! \brief Adds a integer variable to the container.
         *
         * @param varName Name of the variable.
         */
        void addInt(const std::string & varName);

        /*! \brief Adds a boolean variable to the container.
         *
         * @param varName Name of the variable.
         */
        void addBool(const std::string & varName);

        /*! \brief Adds a vector of real variables to the container.
         *
         * @param varNames Vector of the variable names.
         */
        void addReals(const std::vector<std::string> & varNames);

        /*! \brief Adds a vector of integer variables to the container.
         *
         * @param varNames Vector of the variable names.
         */
        void addInts(const std::vector<std::string> & varNames);

        /*! \brief Adds a vector of boolean variables to the container.
         *
         * @param varNames Vector of the variable names.
         */
        void addBools(const std::vector<std::string> & varNames);

        const std::vector<std::string> & getReals() const;
        const std::vector<std::string> & getInts() const;
        const std::vector<std::string> & getBools() const;

        /*! Returns size of real container, i.e., number of stored real variable names. */
        size_t sizeReals() const;
        size_t sizeInts() const;
        size_t sizeBools() const;

        /*! \brief Returns true, if the container is empty. */
        bool empty() const;

        size_t dataSize() const;

        std::shared_ptr<const char> data() const;

        std::shared_ptr<char> data();

        void saveVariablesTo(char * data) const;

        static VariableList getVariableListFromData(const char * data);

        friend std::ostream & operator<<(std::ostream & out, const VariableList & in);

        bool isSubsetOf(const VariableList & in) const;

        size_t findRealVariableNameIndex(const std::string & varName) const;

     private:
        /*! _vars[0] = Vector containing names of real variables.
         *  _vars[1] = Vector containing names of integer variables.
         *  _vars[2] = Vector containing names of boolean variables.
         */
        std::vector<std::vector<std::string>> _vars;

        VariableList(const std::shared_ptr<char> & data);
    };
}
#endif /* INCLUDE_VARIABLELIST_HPP_ */
