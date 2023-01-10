# Relaxations Core Stability Alpha Hedonic Games

This code can be used to verify whether an instance of a symmetric $\alpha$-hedonic games exists that is $q$-size core stable and $k$-improvement core stable.

## Initialization
The code uses the Gurobi optimization software, and calls it from C++. Gurobi offers a free [academic license][2]. More information about how to configure a Gurobi C++ project with Microsoft Visual Studio can be found [here][1].

[1]: https://support.gurobi.com/hc/en-us/articles/360013194392-How-do-I-configure-a-new-Gurobi-C-project-with-Microsoft-Visual-Studio-2017-
[2]: https://www.gurobi.com/academia/academic-program-and-licenses/

## Use
Most parameters for the code can be modified from the `Source.cpp` file. In that file, you can also declare for which type of symmetric $\alpha$-hedonic game and for which parameter values you want to generate an instance, if it exists. 
* `q_vector` contains the values of $q$-size core stability for which you want to generate an instance.
* `c_vector` contains the values of $c$-improvement core stability for which you want to generate an instance (denoted by $k$ in the paper).
* `n_vector` contains the sizes of the blocking coalition (denoted by $m$ in the paper).

You can either use the function `alpha_hedonic_game()` and specify the $\alpha$-vector,or use the already existing functions for symmetric fraction hedonic games, symmetric modified fractional hedonic games, and symmetric additively separable hedonic games already exists. Careful, when specifying a type of hedonic game, the value of `M` in the constraints should be chosen with care: too high values will lead to worse LP-relaxations, and hence longer computation times, while too small values will output incorrect results.

In the code, the default setting is that $u_i(\mathcal{C}) = 1$ for all agents $i$ in the blocking coalition, and that the utilities of the agents, which are denoted by the `X`-variables in the code, are real numbers greater than zero. However, both assumptions can be changed in the `alpha_hedonic_game.cpp` file (or the correct file for alternative subclasses of symmetric hedonic games).

* To change $u_i(\mathcal{C})$, comment out the `model.addConstr(V[i] == 1);` line. To restrict the solution space, we recommend fixing `V[i]`, which represents $u_i(\mathcal{C})$, for at least one of the agents.
* To restrict the utilities to be binary, for example, replace the line `X[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, name_x);` by `X[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_BINARY, name_x);`. One could also impose alternative lower or upper bounds on the allowed utilities by modifying the first two values in the declaration of the `X`-variables.
