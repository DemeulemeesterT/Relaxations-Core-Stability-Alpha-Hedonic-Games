#include "gurobi_c++.h"
#include <vector>
#include <stdio.h>


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

int main()
{
	int n = 5;
	// Size of the blocking coalition
	int q = 4;
	// q-size core stable

	bool fixed_value = false;
		// If this value is true, you can check the feasibility for a fixed value of the improvement factor (c)
		// If this value is false, the solver will optimize towards the maximum value of the improvement factor
			// NOTE: this optimizatino problem is bilinear, and might have troubles proving optimality.
			// Each objective value that is found is, nevertheless, a lower bound on the maximum value
	
	double c = 1.2;
	// c-core stable

	std::vector<int> list(n);
	for (int i = 0; i < n; i++) {
		list[i] = i;
	}

	GRBEnv* env = NULL;

	env = new GRBEnv();
	env->set(GRB_StringParam_LogFile, "HedonicGames.log");
	GRBModel model = GRBModel(*env);
	model.set(GRB_IntParam_PoolSolutions, 10); // Limit the number of solutions that will be stored.
	model.set(GRB_DoubleParam_PoolGap, 0.0001); // Limit the search space by setting a gap for the worst possible solution that will be accepted

	// In case we want to maximize the improvement fraction
	char name_alpha[13];
	sprintf_s(name_alpha, "alpha");
	GRBVar ALPHA = model.addVar(1.0, 2.0, 0.0, GRB_CONTINUOUS, name_alpha);
	GRBLinExpr obj = ALPHA;
	model.setObjective(obj, GRB_MAXIMIZE);

	GRBVar** X = new GRBVar * [n];
	for (int i = 0; i < n; i++) {
		X[i] = new GRBVar[n];
		for (int j = 0; j < n; j++) {
			if (i != j) {
				char name_x[13];
				sprintf_s(name_x, "x_%i_%i", i, j);
				X[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, name_x);
			}
		}
	}

	GRBVar* V = new GRBVar[n];
	for (int i = 0; i < n; i++) {
		char name_v[13];
		sprintf_s(name_v, "v_%i", i);
		V[i] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, name_v);
	}

	// Now we want to enforce that for each subset of size q or smaller, 
	// at least one agent i should be lesser than q * x_i

	// To do this, first find all subsets of size q or smaller
	std::vector<std::vector<int>> A;
	for (int k = 2; k < q + 1; k++) {
		std::vector<std::vector<int>> temp = printCombination(list, n, k);
		for (int i = 0; i < temp.size(); i++) {
			A.push_back(temp[i]);
		}
	}

	// Now, for each subset, one of q constraints should hold.
	// Binary variable y for whether constraint holds.

	// Number of y-var needed:
	int y_needed = 0;
	for (int i = 0; i < A.size(); i++) {
		y_needed += A[i].size();
	}

	GRBVar* Y = new GRBVar[y_needed];
	for (int i = 0; i < y_needed; i++) {
		char name_y[13];
		sprintf_s(name_y, "y_%i", i);
		Y[i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, name_y);
	}

	// Big number M
	int M = 1000;
	int counter = 0;

	//printf("\n\n\n\n\n\n\nATTENTION, NOT ALL CONSTRAINTS FOR THE SUBSETS ADDED \n\n\n\n\n\n\n");

	for (int i = 0; i < A.size(); i++) {
		GRBLinExpr con2;
		for (int k = 0; k < A[i].size(); k++) {
			GRBLinExpr con;
			for (int l = 0; l < A[i].size(); l++) {
				if (A[i][l] != A[i][k]) {
					con += X[A[i][k]][A[i][l]];
				}
			}
			con -= A[i].size() * V[A[i][k]];
			model.addConstr(con <= M * (1 - Y[counter]));
			con2 += Y[counter];
			counter++;
		}
		// At least one of these q constraints should hold
		model.addConstr(con2 >= 1);
	}

	// Add constraints that X[i][j] == X[j][i]
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i != j) {
				model.addConstr(X[i][j] == X[j][i]);
			}
		}
	}
	model.addConstr(X[0][1] == 1);

	/*for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i != j) {
				model.addQConstr(V[j] * X[i][j] == V[i] * X[j][i]);
			}
		}
	}
	*/
	// To avoid that all variables can simply be put equal to zero, require all v-variables to be at least 1;
	for (int i = 0; i < n; i++) {
		//model.addConstr(V[i] >= 0.01);
		//model.addConstr(V[i] == 1);
	}
	//model.addConstr(V[0] == 0.666);
	//model.addConstr(V[1] == 0.666);
	//model.addConstr(V[2] == 0.666);
	//model.addConstr(V[3] == 0.666);
	//model.addConstr(V[4] == 0.666);
	//model.addConstr(V[5] == 0.67);


	//model.addConstr(V[2] == 3);

	// Test whether there exists a solution in which no 'partitioning' of the X-variables holds for the constraints for n=5, q=3
	/*model.addConstr(Y[20] + Y[35] <= 1);
	model.addConstr(Y[21] + Y[44] <= 1);
	model.addConstr(Y[22] + Y[47] <= 1);
	model.addConstr(Y[23] + Y[32] <= 1);
	model.addConstr(Y[24] + Y[41] <= 1);
	model.addConstr(Y[25] + Y[48] <= 1);
	model.addConstr(Y[26] + Y[29] <= 1);
	model.addConstr(Y[27] + Y[38] <= 1);
	model.addConstr(Y[28] + Y[49] <= 1);
	model.addConstr(Y[30] + Y[42] <= 1);
	model.addConstr(Y[31] + Y[45] <= 1);
	model.addConstr(Y[33] + Y[39] <= 1);
	model.addConstr(Y[34] + Y[46] <= 1);
	model.addConstr(Y[36] + Y[40] <= 1);
	model.addConstr(Y[37] + Y[43] <= 1);*/


	if (fixed_value == true) {
		// Then also add the constraints that the new coalition is actually blocking
		for (int i = 0; i < n; i++) {
			GRBLinExpr con;
			for (int k = 0; k < n; k++) {
				if (i != k) {
					con += X[i][k];
				}
			}
			model.addConstr(con >= c * n * V[i]);
			// NOTE: we want a strict inequality here, but GUROBI doesn't support it
		}
	}
	else {
		// Then also add the constraints that the new coalition is actually blocking
		for (int i = 0; i < n; i++) {
			GRBLinExpr con;
			for (int k = 0; k < n; k++) {
				if (i != k) {
					con += X[i][k];
				}
			}
			model.addQConstr(con >= ALPHA * n * V[i]);
		}

		model.set(GRB_IntParam_NonConvex, 2);
	}

	model.write("HedonicGames.lp");
	model.optimize();

	int status = model.get(GRB_IntAttr_Status);
	if (status == 3) {
		model.computeIIS();
		model.write("model_IIS.ilp");
	}
	else {
		// Get solution values
		std::vector<std::vector<double>> X_val(n, std::vector<double>(n, -1));
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (i != j) {
					X_val[i][j] = X[i][j].get(GRB_DoubleAttr_X);
					X_val[j][i] = X[j][i].get(GRB_DoubleAttr_X);
				}
			}
		}

		std::vector<bool> Y_val(y_needed, 0);
		for (int i = 0; i < y_needed; i++) {
			Y_val[i] = (bool)Y[i].get(GRB_DoubleAttr_X);
		}

		std::vector<double> V_val(n, 0);
		for (int i = 0; i < n; i++) {
			V_val[i] = V[i].get(GRB_DoubleAttr_X);
		}

		printf("V values:\n");
		for (int i = 0; i < n; i++) {
			printf("\tV[%i] = %.2f\n", i, V_val[i]);
		}

		printf("\n\n X values:\n");
		for (int i = 0; i < n; i++) {
			for (int j = i + 1; j < n; j++) {
				printf("X[%i][%i] = %.2f\n", i, j, X_val[i][j]);
			}
		}

		if (fixed_value == false) {
			printf("\nALPHA = %.5f\n\n", ALPHA.get(GRB_DoubleAttr_X));
		}

		/*printf("\n\n Y values:\n");
		for (int i = 0; i < y_needed; i++) {
			printf("Y[%i] = %i\n", i, (int)Y_val[i]);
		}*/

		int sum_0 = 0;
		int sum_1 = 0;
		int sum_2 = 0;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (i != j) {
					if (X_val[i][j] == 0) {
						sum_0++;
					}
					if (X_val[i][j] == 1) {
						sum_1++;
					}
					if (X_val[i][j] == 2) {
						sum_2++;
					}
				}
			}
		}
		printf("Number of X-values equal to 0: %i\n", sum_0 / 2);
		printf("Number of X-values equal to 1: %i\n", sum_1 / 2);
		printf("Number of X-values equal to 2: %i\n", sum_2 / 2);
	}

}