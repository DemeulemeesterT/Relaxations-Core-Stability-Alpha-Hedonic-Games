#pragma once

#include "gurobi_c++.h"
#include <vector>
#include <stdio.h>

std::vector<std::vector<int>> printCombination(std::vector<int> list, int n, int r);
std::vector<std::vector<int>> combinationUtil(std::vector<std::vector<int>> A, std::vector<int> list, std::vector<int> data, int start, int end, int index, int r);



void fractional_hedonic_game(std::vector<int> n_vector, std::vector<int> q_vector, bool fixed_value, double c);
	// 'n_vector' is a vector containing the number of agents
	// 'q_vector' is a vector containing how large should the size core stability should be?

	// If 'fixed_value' value is true, you can check the feasibility for a fixed value of the improvement factor (c)
	// If this value is false, the solver will optimize towards the maximum value of the improvement factor
	// NOTE: this optimizatino problem is bilinear, and might have troubles proving optimality.
	// Each objective value that is found is, nevertheless, a lower bound on the maximum value

	// If 'fixed_value' is true, value for improvement core stability?

void modified_fractional_hedonic_game(std::vector<int> n_vector, std::vector<int> q_vector, bool fixed_value, double c);
void additively_separable_hedonic_game(std::vector<int> n_vector, std::vector<int> q_vector, bool fixed_value, double c);
