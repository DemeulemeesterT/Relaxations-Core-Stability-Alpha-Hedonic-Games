#include "Run_Configurations.h"
	// In order not to change the main file for every type of hedonic game
	// just call the function that runs the code for the desired type.


int main()
{
	// Below you can fill in the data for which you want to generate an instance, or prove infeasibility:
		// If you fill the vectors with multiple values, then the instances are solved iteratively
	std::vector<int> n_vector{5,6,8,9,11,12,14,15,17,18}; // Number of agents in blocking coalition
	std::vector<int> q_vector{4,4,4,4,4,4,4,4,4,4 };	// Values for q-size core stability
	bool fixed_value = true; // 'true' means checking feasibility for c-improvement core stability
		// Default, put on 'true'. 
		// If 'false', the solver will solve a quadratic program to find the maximal value of k for which a k-improving blocking coaltion exists,
			// But this will require more time.

	std::vector<double> c_vector{2,2,3,3,4,4,5,5,6,6}; //values for c-improvement core stability 
		// (Called k-improvement core stability in the paper)
	
	// Fill in the values of alpha in the symmetric alpha-hedonic game									
	//std::vector<double> alpha{ 0.5,0.5, 0.166666, 0.25, 0.1, 0.1666666, (double)1/14, 0.125, (double)1/18, 0.1, 0 , (double)1/22, 0, (double) 1/14, (double)1/26};
	std::vector<double> alpha;
	for (int i = 0; i < 14; i++) {
		alpha.push_back((double)1 / ((i + 1) * (i + 1)));
	}
	//fractional_hedonic_game(n_vector, q_vector, fixed_value, c_vector);
	additively_separable_hedonic_game(n_vector, q_vector, fixed_value, c_vector);
	//alpha_hedonic_game(n_vector, q_vector, fixed_value, c_vector, alpha);

}