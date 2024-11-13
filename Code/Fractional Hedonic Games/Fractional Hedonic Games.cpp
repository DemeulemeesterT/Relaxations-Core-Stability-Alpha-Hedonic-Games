#include "Run_Configurations.h"


void fractional_hedonic_game(std::vector<int> n_vector, std::vector<int> q_vector, bool fixed_value, std::vector<double> c_vector) {
	for (int l = 0; l < n_vector.size(); l++) {
		int n = n_vector[l];
		// Size of the blocking coalition
		int q = q_vector[l];
		// q-size core stable

		double c = c_vector[l];

		printf("\n\n\n***************************************\n\tn = %i \t q=%i \t c=%.4f\n***************************************\n\n\n", n, q, c);


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

		// The utility agent i has for agent j
		GRBVar** X = new GRBVar * [n];
		for (int i = 0; i < n; i++) {
			X[i] = new GRBVar[n];
			for (int j = 0; j < n; j++) {
				if (i != j) {
					char name_x[13];
					sprintf_s(name_x, "x_%i_%i", i, j);
					X[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, name_x);
					//X[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_BINARY, name_x);
				}
			}
		}

		// Utility of agent i in the original coalition structure
		GRBVar* V = new GRBVar[n];
		for (int i = 0; i < n; i++) {
			char name_v[13];
			sprintf_s(name_v, "v_%i", i);
			V[i] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, name_v);
		}

		// Now we want to enforce that for each subset of size q or smaller, 
		// at least one agent i should have a utility < q * x_i

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
		int M = 20;
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

		// All edge weights at least 1
		/*for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (i != j) {
					model.addConstr(X[i][j] == 1);
				}
			}
		}*/
		
		//model.addConstr(X[0][1] == 1);

		/*for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (i != j) {
					model.addQConstr(V[j] * X[i][j] == V[i] * X[j][i]);
				}
			}
		}
		*/
		// To avoid that all variables can simply be put equal to zero, require at least one v-variable to be equal to 1;
			// Note that other choices are also possible, and can help to reduce the time required to find a solution.
			// By setting all V[i]'s equal to integer values, the examples also tend to become more understandable
		for (int i = 0; i < n; i++) {
			//model.addConstr(V[i] >= 0.01);
			//model.addConstr(V[i] == (double) 5/6);
		};
		model.addConstr(V[0] == 1);
		//model.addConstr(V[1] == (double)5/6);
		//model.addConstr(V[6] == 0);
		//model.addConstr(V[7] == 0);
		//model.addConstr(V[8] == 0);
		//model.addConstr(V[9] == 0);
		//model.addConstr(V[10] == 1);

		// Test a solution
		/*model.addConstr(X[0][1] == 12.00);
		model.addConstr(X[0][2] == 0.00);
		model.addConstr(X[0][3] == 0.00);
		model.addConstr(X[0][4] == 0.00);
		model.addConstr(X[0][5] == 0.00);
		model.addConstr(X[0][6] == 0.00);
		model.addConstr(X[0][7] == 0.00);
		model.addConstr(X[0][8] == 0.00);
		model.addConstr(X[0][9] == 0.00);*/
		//model.addConstr(X[0][10] == 0.00);
		/*model.addConstr(X[1][2] == 60.00);
		model.addConstr(X[1][3] == 0.00);
		model.addConstr(X[1][4] == 0.00);
		model.addConstr(X[1][5] == 0.00);
		model.addConstr(X[1][6] == 0.00);
		model.addConstr(X[1][7] == 0.00);
		model.addConstr(X[1][8] == 0.00);
		model.addConstr(X[1][9] == 0.00);
		//model.addConstr(X[1][10] == 0.00);
		model.addConstr(X[2][3] == 300.00);
		model.addConstr(X[2][4] == 0.00);
		model.addConstr(X[2][5] == 0.00);
		model.addConstr(X[2][6] == 0.00);
		model.addConstr(X[2][7] == 0.00);
		model.addConstr(X[2][8] == 0.00);
		model.addConstr(X[2][9] == 0.00);
		//model.addConstr(X[2][10] == 0.00);
		model.addConstr(X[3][4] == 1500.00);
		model.addConstr(X[3][5] == 0.00);
		model.addConstr(X[3][6] == 0.00);
		model.addConstr(X[3][7] == 0.00);
		model.addConstr(X[3][8] == 0.00);
		model.addConstr(X[3][9] == 0.00);
		//model.addConstr(X[3][10] == 0.00);
		model.addConstr(X[4][5] == 7500.00);
		model.addConstr(X[4][6] == 0.00);
		model.addConstr(X[4][7] == 0.00);
		model.addConstr(X[4][8] == 0.00);
		model.addConstr(X[4][9] == 0.00);
		//model.addConstr(X[4][10] == 0.00);
		model.addConstr(X[5][6] == 37500.00);
		model.addConstr(X[5][7] == 0.00);
		model.addConstr(X[5][8] == 0.00);
		model.addConstr(X[5][9] == 0.00);
		//model.addConstr(X[5][10] == 0.00);
		//model.addConstr(X[6][7] == 187500.00);
		//model.addConstr(X[6][8] == 0.00);
		//model.addConstr(X[6][9] == 0.00);
		//model.addConstr(X[6][10] == 0.00);
		//.addConstr(X[7][8] == 937500.00);
		//model.addConstr(X[7][9] == 0.00);
		//model.addConstr(X[7][10] == 0.00);
		//model.addConstr(X[8][9] == 4687500.00);
		//model.addConstr(X[8][10] == 0.00);
		//model.addConstr(X[9][10] == 0.00);
		*/

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
			// If the model is infeasible, then we look for the IIS, the minimum set of constraints that is jointly infeasible.
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
				printf("\tV[%i] = %.4f\n", i, V_val[i]);
			}

			printf("\n\n X values:\n");
			for (int i = 0; i < n; i++) {
				for (int j = i + 1; j < n; j++) {
					printf("X[%i][%i] = %.4f\n", i, j, X_val[i][j]);
				}
			}

			if (fixed_value == false) {
				printf("\nALPHA = %.6f\n\n", ALPHA.get(GRB_DoubleAttr_X));
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
		delete[] Y;
		delete[] V;
		for (int j = 0; j < n; j++) {
			delete[] X[j];
		}
		delete[] X;
		delete env;
	}
}
