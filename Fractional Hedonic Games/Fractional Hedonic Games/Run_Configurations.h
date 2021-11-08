#pragma once

#include "gurobi_c++.h"
#include <vector>
#include <stdio.h>

void fractional_hedonic_game(std::vector<int> n_vector, std::vector<int> q_vector, bool fixed_value, double c);
	// 'n_vector' is a vector containing the number of agents
	// 'q_vector' is a vector containing how large should the size core stability should be?

	// If 'fixed_value' value is true, you can check the feasibility for a fixed value of the improvement factor (c)
	// If this value is false, the solver will optimize towards the maximum value of the improvement factor
	// NOTE: this optimizatino problem is bilinear, and might have troubles proving optimality.
	// Each objective value that is found is, nevertheless, a lower bound on the maximum value

	// If 'fixed_value' is true, value for improvement core stability?

void modified_fractional_hedonic_game(std::vector<int> n_vector, std::vector<int> q_vector, bool fixed_value, double c);




// *************************************************************
// *****CODE FOR GENERATING ALL COMBINATIONS********************
// *************************************************************

// Found code for combinations here: https://www.geeksforgeeks.org/print-all-possible-combinations-of-r-elements-in-a-given-array-of-size-n/

// Program to print all combination of size r in an array of size n
std::vector<std::vector<int>> combinationUtil(std::vector<std::vector<int>> A, std::vector<int> list, std::vector<int> data, int start, int end,
	int index, int r);

// The main function that prints all combinations of size r
// in arr[] of size n. This function mainly uses combinationUtil()
std::vector<std::vector<int>> printCombination(std::vector<int> list, int n, int r)
{
	// A temporary array to store all combination one by one
	std::vector<int> data(r);

	// Create empty vector to fill with all combinations
	std::vector<std::vector<int>> A(0, std::vector<int>(r));

	// Return a vector of vectors that contain all possible combinations of size r
	A = combinationUtil(A, list, data, 0, n - 1, 0, r);

	return A;
}

/* arr[]  ---> Input Array
   data[] ---> Temporary array to store current combination
   start & end ---> Staring and Ending indexes in arr[]
   index  ---> Current index in data[]
   r ---> Size of a combination to be printed */
std::vector<std::vector<int>> combinationUtil(std::vector<std::vector<int>> A, std::vector<int> list, std::vector<int> data, int start, int end,
	int index, int r) {

	// Current combination is ready to be printed, print it
	if (index == r)
	{
		std::vector<int> empty(r);
		for (int j = 0; j < r; j++) {
			empty[j] = data[j];
		}
		A.push_back(empty);
		return A;
	}

	// replace index with all possible elements. The condition
	// "end-i+1 >= r-index" makes sure that including one element
	// at index will make a combination with remaining elements
	// at remaining positions
	for (int i = start; i <= end && end - i + 1 >= r - index; i++)
	{
		data[index] = list[i];
		A = combinationUtil(A, list, data, i + 1, end, index + 1, r);
	}
	return A;
}

