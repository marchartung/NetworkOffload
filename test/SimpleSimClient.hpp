/*
 * SimpleSimClient.hpp
 *
 *  Created on: 19.04.2016
 *      Author: Marc Hartung
 */

#ifndef TEST_SIMPLESIMCLIENT_HPP_
#define TEST_SIMPLESIMCLIENT_HPP_

#include <cmath>
#include <iostream>

#include "../include/SimulationClient.hpp"

int clientFunc(const std::string & hostname, int port) {
	NetOff::SimulationClient noFC(hostname, port);

	// test if server can be reached
	if (!noFC.initializeConnection())
		throw std::runtime_error("Couldn't reach important server.");

	// send server data about the sim you want to calculate and get a handle of the sim
	int funSim = noFC.addSimulation("/home/of/very/funny/Sim.sim");

	// checkout the variables of the sim
    NetOff::VariableList allInputs = noFC.getPossibleInputVariableNames(funSim);
	NetOff::VariableList allOutputs = noFC.getPossibleOuputVariableNames(funSim);

	// set the input variables to send (in this case just the 1st and 2nd variable of all sim variables)
	NetOff::VariableList inputVars;
	inputVars.addReal(allInputs.getReals()[0]); // add the 1st variable
	inputVars.addReal(allInputs.getReals()[1]); // add the 2d variable
	double inputValue[] = { 0.0, 0.0 }; // for all inputs added inputs, the client needs to set initial values

	// set the output variables, which should be received (in this case all sim variables)
	NetOff::VariableList outputVars;
	outputVars = allOutputs;

	// initialize the added simulation:
	noFC.initializeSimulation(funSim, inputVars, outputVars, inputValue,
			nullptr, nullptr);
	noFC.getSimulationFile(funSim,"./test_src.txt","./test_target.txt");

	// now its possible to access the containers of the initialized simulation
	NetOff::ValueContainer & data = noFC.getOutputValueContainer(funSim);

	// start the server, only possible if all added simulations are initialized
	if (!noFC.start())
		throw std::runtime_error("Couldn't start important server.");
	double t = 0;

	while (t < 10.0) {

		if (std::abs(t - 5.0) < 0.01) {
			// pause for fun
			noFC.pause();
			// do something else or just unpause again
			noFC.unpause();
		}

		NetOff::ValueContainer & inputs = noFC.getInputValueContainer(funSim);
		// set inputs
		inputs.getRealValues()[0] = std::sin(t);
		inputs.getRealValues()[1] = std::cos(t);
		std::cout << "inputs: " << inputs << "\n";

		// after start() its possible to use the communication functions
		// send inputs
		noFC.sendInputValues(funSim, t, inputs);

		// receive outputs
		NetOff::ValueContainer & outputs = noFC.recvOutputValues(funSim, t); // Don't worry. The won't be any data allocated. And the reference '&' is a must. It's not allowed to be copied

		// do something with outputs
		std::cout << "At time " << t << ": " << outputs << "\n";

		t += 0.1;
	}
	// stop the connection:
	noFC.deinitialize();

	return 0;
}

#endif /* TEST_SIMPLESIMCLIENT_HPP_ */
