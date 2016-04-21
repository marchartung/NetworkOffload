/*
 * SimpleSimClient.hpp
 *
 *  Created on: 19.04.2016
 *      Author: hartung
 */

#ifndef TEST_SIMPLESIMCLIENT_HPP_
#define TEST_SIMPLESIMCLIENT_HPP_

#include <cmath>
#include <iostream>


#include <SimulationClient.hpp>

int clientFunc(const std::string & hostname, int port)
{
    NetOff::SimulationClient noFC(hostname, port);

       // test if server can be reached
       if (!noFC.initializeConnection())
           throw std::runtime_error("Couldn't reach important server.");

       // send server data about the fmu u want to calculate and get a handle ID of the fmu
       int funFmu = noFC.addSimulation("/home/of/very/funny/Fmu.fmu");

       // checkout the variables of the fmu
       NetOff::VariableList all = noFC.getVariableNames(funFmu);

       // set the input variables to send (in this case just the first variable of all fmu variables)
       NetOff::VariableList inputVars;
       inputVars.addReal(all.getReals()[2]);
       inputVars.addReal(all.getReals()[3]);
       double inputValue[] = {0.0,0.0};
       // set the output variables, which should be received (in this case all fmu variables)
       NetOff::VariableList outputVars;
       outputVars = all;

       noFC.initializeSimulation(funFmu,inputVars,outputVars,inputValue,nullptr, nullptr);
       NetOff::ValueContainer & data = noFC.getOutputValueContainer(funFmu);

       // start the server
       if (!noFC.start())
           throw std::runtime_error("Couldn't start important server.");

       double t = 0;


       while (t < 10.0)
       {

           if (std::abs(t - 5.0) < 0.01)
           {
               // pause for fun
               noFC.pause();
               //Do something else or just unpause again
               noFC.unpause();
           }

           NetOff::ValueContainer & inputs = noFC.getInputValueContainer(funFmu);
           // set inputs
           inputs.getRealValues()[0] = std::sin(t);
           inputs.getRealValues()[1] = std::cos(t);
           std::cout << "inputs: " << inputs << "\n";
           // send inputs
           noFC.sendInputValues(funFmu, t, inputs);

           // get outputs
           NetOff::ValueContainer & outputs = noFC.recvOutputValues(funFmu, t);  // Don't worry. The won't be any data allocated. And the reference '&' is a must. It's not allowed to be copied

           // do something with outputs
           std::cout << "At time " << t << ": " << outputs << "\n";

           t += 0.1;
       }

       noFC.deinitialize();

       return 0;
}



#endif /* TEST_SIMPLESIMCLIENT_HPP_ */
