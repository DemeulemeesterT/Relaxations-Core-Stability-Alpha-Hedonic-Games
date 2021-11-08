#include "Run_Configurations.h"
	// In order not to change the main file for every type of hedonic game
	// just call the function that runs the code for the desired type.


int main()
{
	std::vector<int> n_vector{ 8}; // Number of agents
	std::vector<int> q_vector{ 4};	// Values for q-size core stability
	bool fixed_value = true; // 'true' means checking feasibility for c-improvement core stability	
	double c = 2.73; // value for c-improvement core stability

	additively_separable_hedonic_game(n_vector, q_vector, fixed_value, c);

}