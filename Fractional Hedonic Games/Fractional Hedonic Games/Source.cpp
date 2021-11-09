#include "Run_Configurations.h"
	// In order not to change the main file for every type of hedonic game
	// just call the function that runs the code for the desired type.


int main()
{
	std::vector<int> n_vector{ 11}; // Number of agents
	std::vector<int> q_vector{ 5};	// Values for q-size core stability
	bool fixed_value = true; // 'true' means checking feasibility for c-improvement core stability	
	std::vector<double> c_vector{ 2.75 }; //values for c - improvement core stability

	friends_and_enemies(n_vector, q_vector, fixed_value, c_vector);

}