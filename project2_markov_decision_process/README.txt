Implementation of value iteration and policy iteration to solve MDP problem (grid world). Only the MDP.java is my own work. All other .java files are open source to do matrix functions (from jama).

To run: compile MDP.java
input string for executable: 
	
	discountFactor   maxStateUtilityError   keyLossProbability   positiveTerminalReward   negativeTerminalReward   stepCost   solutionTechnique

for example:

	java MDP 0.99 1e-6 0.5 1 -1 -0.04 v

for value iteration, and:

	java MDP 0.99 1e-6 0.5 1 -1 -0.04 p

for policy iteration.
