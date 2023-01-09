# Relaxations Core Stability Alpha Hedonic Games

This code can be used to verify whether an instance of a symmetric $\alpha$-hedonic games exists that is $q$-size core stable and $k$-improvement core stable.

## Initialization
The code uses the Gurobi optimization software, and calls it from C++. More information about how to configure a Gurobi C++ project with Microsoft Visual Studio can be found here: https://support.gurobi.com/hc/en-us/articles/360013194392-How-do-I-configure-a-new-Gurobi-C-project-with-Microsoft-Visual-Studio-2017-   

## Use
All parameters for the code can be modified from the *Source.cpp* file. In that file, you can also declare for which type of S-$\alpha$HG you want to generate an instance, if it exists. Either you can use the function *alpha_hedonic_game* and specify the $\alpha$-vector. A separate function for symmetric fraction hedonic games, symmetric modified fractional hedonic games, and symmetric additively separable hedonic games already exists.

