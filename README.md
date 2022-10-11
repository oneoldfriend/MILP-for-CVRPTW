# MILP-For-CVRPTW
## install cplex
refer to https://www.ibm.com/support/pages/installation-ibm-ilog-cplex-optimization-studio-linux-platforms
## make the c++ project
modify the following fields so that the value is aligned with your environment.
1. SYSTEM: x86-64_osx, x86-64_linux, x86-64_windows
2. CPLEXDIR: the directories where CPLEX and CONCERT are installed.
3. CONCERTDIR: the directories where CPLEX and CONCERT are installed.
type "make milp" in the CMD to make the project.
## run the python file
type "python main.py --instance ${instances_path}" in the CMD to run the cplex solver.
## instances modification
1. profit $p$ per unit demands are set to 5, outsourcing cost $w$ per demand are set to 1000, overtime wages $f$ per unit time are set to 2.