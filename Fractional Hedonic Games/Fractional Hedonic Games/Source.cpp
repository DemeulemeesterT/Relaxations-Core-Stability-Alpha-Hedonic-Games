#include "Run_Configurations.h"
	// In order not to change the main file for every type of hedonic game
	// just call the function that runs the code for the desired type.


int main()
{
	std::vector<int> n_vector{ 4 }; // Number of agents
	std::vector<int> q_vector{ 2 };	// Values for q-size core stability
	bool fixed_value = true; // 'true' means checking feasibility for c-improvement core stability	
	double c = 1.01; // value for c-improvement core stability

	modified_fractional_hedonic_game(n_vector, q_vector, fixed_value, c);

}