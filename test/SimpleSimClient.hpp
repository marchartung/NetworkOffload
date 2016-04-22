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

int clientFunc(const std::string & hostname, int port) {
	NetOff::SimulationClient noFC(hostname, port);

	// test if server can be reached
	if (!noFC.initializeConnection())
		throw std::runtime_error("Couldn't reach important server.");

	// send server data about the sim u want to calculate and get a handle of the sim
	int funSim = noFC.addSimulation("/home/of/very/funny/Sim.sim");

	// checkout the variables of the sim
	NetOff::VariableList all = noFC.getVariableNames(funSim);

	// set the input variables to send (in this case just the 3rd,4th variable of all sim variables)
	NetOff::VariableList inputVars;
	inputVars.addReal(all.getReals()[2]); // add the 3rd variable
	inputVars.addReal(all.getReals()[3]); // add the 4th variabl
	double inputValue[] = { 0.0, 0.0 }; // for all inputs added inputs, the client needs to set initial values

	// set the output variables, which should be received (in this case all sim variables)
	NetOff::VariableList outputVars;
	outputVars = all;

	// initialize the added simulation:
	noFC.initializeSimulation(funSim, inputVars, outputVars, inputValue,
			nullptr, nullptr);

	// no its possible to access the containers of the initialized simulation
	NetOff::ValueContainer & data = noFC.getOutputValueContainer(funSim);

	// start the server, only possible if all added simulations are initialized
	if (!noFC.start())
		throw std::runtime_error("Couldn't start important server.");
	double t = 0;

	while (t < 10.0) {

		if (std::abs(t - 5.0) < 0.01) {
			// pause for fun
			noFC.pause();
			//Do something else or just unpause again
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
