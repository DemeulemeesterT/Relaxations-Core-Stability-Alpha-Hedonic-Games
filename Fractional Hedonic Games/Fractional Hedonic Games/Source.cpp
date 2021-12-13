#include "Run_Configurations.h"
	// In order not to change the main file for every type of hedonic game
	// just call the function that runs the code for the desired type.


int main()
{
	std::vector<int> n_vector{ 6}; // Number of agents
	std::vector<int> q_vector{ 2};	// Values for q-size core stability
	bool fixed_value = true; // 'true' means checking feasibility for c-improvement core stability	
	std::vector<double> c_vector{ 0.55}; //values for c - improvement core stability
	//std::vector<double> alpha{ 0.5,0.5, 0.166666, 0.25, 0.1, 0.1666666, (double)1/14, 0.125, (double)1/18, 0.1, 0 , (double)1/22, 0, (double) 1/14, (double)1/26};
	std::vector<double> alpha;
	for (int i = 0; i < 14; i++) {
		alpha.push_back((double)1 / ((i + 1) * (i + 1)));
	}
	alpha_hedonic_game(n_vector, q_vector, fixed_value, c_vector, alpha);

}