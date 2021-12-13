#include "Run_Configurations.h"

// The difference for Modified Fractional Hedonic Games is that the utility of an agent
// is only divided by the number of agents in the coalition, EXCLUDING HIMSELF

void additively_separable_hedonic_game(std::vector<int> n_vector, std::vector<int> q_vector, bool fixed_value, std::vector<double> c_vector) {
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
		env->set(GRB_StringParam_LogFile, "HedonicGames_ASHG.log");
		GRBModel model = GRBModel(*env);
		model.set(GRB_IntParam_PoolSolutions, 10); // Limit the number of solutions that will be stored.
		model.set(GRB_DoubleParam_MIPGap, 0.000001);
		//model.set(GRB_DoubleParam_PoolGap, 0.0000001); // Limit the search space by setting a gap for the worst possible solution that will be accepted

		// In case we want to maximize the improvement fraction
		char name_alpha[13];
		sprintf_s(name_alpha, "alpha");
		GRBVar ALPHA = model.addVar(1.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, name_alpha);

		GRBVar** X = new GRBVar * [n];
		for (int i = 0; i < n; i++) {
			X[i] = new GRBVar[n];
			for (int j = 0; j < n; j++) {
				if (i != j) {
					char name_x[13];
					sprintf_s(name_x, "x_%i_%i", i, j);
					X[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, name_x);
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

		// Big number M, should be upper bounded by the sum of the valuations (because of 2-size core stability)
		// Fixing one of the valuations to 1, a quick guess is that none of the valuations is larger than 4
		int M = 4*q;
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
				con -= V[A[i][k]];
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

		// To avoid that all variables can simply be put equal to zero, require all v-variables to be at least 1;
		for (int i = 0; i < n; i++) {
			//model.addConstr(V[i] >= 0.01);
			//model.addConstr(V[i] == 1);
		}
		/*model.addConstr(V[0] == 1);
		model.addConstr(V[1] == 1);
		model.addConstr(V[2] == 1);
		model.addConstr(V[3] == 1);
		model.addConstr(V[4] == 1);
		model.addConstr(V[5] == 1);
		model.addConstr(V[6] == 1);
		model.addConstr(V[7] == 1);
		model.addConstr(V[8] == 1);
		model.addConstr(V[9] == 1);*/
		//model.addConstr(V[10] == 1);


		if (fixed_value == true) {
			// Then also add the constraints that the new coalition is actually blocking
			for (int i = 0; i < n; i++) {
				GRBLinExpr con;
				for (int k = 0; k < n; k++) {
					if (i != k) {
						con += X[i][k];
					}
				}
				model.addConstr(con >= c * V[i]);
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
				model.addQConstr(con >= ALPHA * V[i]);
			}

			GRBLinExpr obj = ALPHA;
			model.setObjective(obj, GRB_MAXIMIZE);

			model.set(GRB_IntParam_NonConvex, 2);
		}

		model.write("HedonicGames.lp");
		model.optimize();

		int status = model.get(GRB_IntAttr_Status);
		if (status == 3) {
			//model.computeIIS();
			//model.write("model_IIS.ilp");
		}
		else if (status == 4) {
			printf("\nInfeasible or unbounded...\n");
			//model.computeIIS();
			//model.write("model_IIS.ilp");
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
				printf("\tV[%i] = %.6f\n", i, V_val[i]);
			}

			printf("\n\n X values:\n");
			for (int i = 0; i < n; i++) {
				for (int j = i + 1; j < n; j++) {
					printf("X[%i][%i] = %.6f\n", i, j, X_val[i][j]);
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