#include <ilcplex/ilocplex.h>
#include <math.h>

#include <fstream>
#include <iostream>
#include <vector>

#define M 100000
#define RUNNING_HOURS 5

ILOSTLBEGIN

typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<NumVarMatrix> NumVar3Matrix;
typedef IloArray<IloBoolVarArray> BoolVarMatrix;
typedef IloArray<BoolVarMatrix> BoolVar3Matrix;
ILOMIPINFOCALLBACK4(timeIntervalCallback, IloCplex, cplex, ofstream &, fp, IloNum, start_time,
                    const std::string &, instance_name) {
    int better_flag = 0;
    IloNum time_elapsed = cplex.getCplexTime() - start_time;
    if (time_elapsed / 3600 > RUNNING_HOURS) {
        // if (hasIncumbent())
        // {
        //     fp << instance_name << "," << getIncumbentObjValue() << endl;
        // }
        abort();
    }
}
struct Vehicle;
struct Demand;
struct Vehicle {
    double Q = 0;
    double c = 0;
};

struct Demand {
    double x = 0;
    double y = 0;
    double e = 0;
    double l = 0;
    double m = 0;
    double s = 0;
};

std::vector<std::string> str_split(char *source, const std::string &delim) {
    std::vector<std::string> res;
    if (std::strcmp(source, "") == 0) {
        return res;
    }
    char *d = new char[delim.length() + 1];
    strcpy(d, delim.c_str());
    char *p = strtok(source, d);
    while (p) {
        std::string s = p;
        res.push_back(s);
        p = strtok(NULL, d);
    }
    return res;
}

double euclidean_distance_calc(double x1, double y1, double x2, double y2) {
    return pow(pow(x1 - x2, 2) + pow(y1 - y2, 2), 0.5);
}

void read_instance(const IloEnv &env, const std::string &file_name, IloInt &nodes_no,
                   IloInt &vehicles_no, IloNumArray &e, IloNumArray &l, IloNumArray &m,
                   IloNumArray &s, IloNumArray2 &tt, IloNumArray &Q, IloNumArray &c, IloInt &p,
                   IloInt &w, IloInt &f, IloInt &T_max) {
    ifstream file(file_name, std::ios::in);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file " << file_name << std::endl;
        return;
    }
    std::vector<Vehicle> vehicles;
    std::vector<Demand> demands;
    char context[256];
    std::string mode = "";
    while (!file.eof()) {
        file.getline(context, 200);
        if (std::strcmp(context, "VEHICLES") == 0) {
            mode = "vehicles";
        } else if (std::strcmp(context, "PARAM") == 0) {
            mode = "parameters";
        } else if (std::strcmp(context, "DEMANDS") == 0) {
            mode = "demands";
        } else if (mode == "parameters") {
            std::vector<std::string> attributes = str_split(context, ",");
            if (attributes.size() < 1) {
                continue;
            }
            p = std::stoi(attributes[0]);
            w = std::stoi(attributes[1]);
            f = std::stoi(attributes[2]);
            T_max = std::stoi(attributes[3]);
        } else if (mode == "vehicles") {
            std::vector<std::string> attributes = str_split(context, ",");
            if (attributes.size() < 1) {
                continue;
            }
            Vehicle vehicle;
            vehicle.Q = std::stod(attributes[0]);
            vehicle.c = std::stod(attributes[1]);
            vehicles.push_back(vehicle);
        } else if (mode == "demands") {
            std::vector<std::string> attributes = str_split(context, ",");
            if (attributes.size() < 1) {
                continue;
            }
            Demand demand;
            demand.x = std::stod(attributes[1]);
            demand.y = std::stod(attributes[2]);
            demand.e = std::stod(attributes[3]);
            demand.l = std::stod(attributes[4]);
            demand.m = std::stod(attributes[7]);
            demand.s = std::stod(attributes[8]);
            demands.push_back(demand);
        }
    }
    nodes_no = demands.size();
    vehicles_no = vehicles.size();
    e = IloNumArray(env, nodes_no);
    l = IloNumArray(env, nodes_no);
    m = IloNumArray(env, nodes_no);
    s = IloNumArray(env, nodes_no);
    Q = IloNumArray(env, vehicles_no);
    c = IloNumArray(env, vehicles_no);

    tt = IloNumArray2(env, nodes_no);
    for (int i = 0; i < nodes_no; i++) {
        tt[i] = IloNumArray(env, nodes_no);
        e[i] = demands[i].e;
        l[i] = demands[i].l;
        m[i] = demands[i].m;
        s[i] = demands[i].s;
        for (int j = 0; j < nodes_no; j++) {
            tt[i][j] = int(euclidean_distance_calc(demands[i].x, demands[i].y, demands[j].x,
                                                   demands[j].y) *
                           10) /
                       10.0;
        }
    }
    for (int k = 0; k < vehicles_no; k++) {
        Q[k] = vehicles[k].Q;
        c[k] = vehicles[k].c;
    }
    return;
}

int main(int argc, char **argv) {
    IloInt nodes_no, vehicles_no, p = 0, w = 10000, f = 1000, T_max;
    IloNumArray e, l, m, s, c, Q;
    IloNumArray2 tt;
    IloEnv env;
    read_instance(env, argv[1], nodes_no, vehicles_no, e, l, m, s, tt, Q, c, p, w, f, T_max);
    // std::cout << p << "," << w << "," << f << "," << T_max << std::endl;
    // std::cout << nodes_no << ", " << vehicles_no << std::endl;
    // for (int i = 0; i < nodes_no; i++) {
    //     std::cout << i << " " << e[i] << " " << l[i] << " " << m[i] << " " << s[i] << std::endl;
    // }
    // for (int i = 0; i < vehicles_no; i++) {
    //     std::cout << i << " " << Q[i] << " " << c[i] << std::endl;
    // }
    // for (int i = 0; i < nodes_no; i++) {
    //     for (int j = 0; j < nodes_no; j++) {
    //         std::cout << tt[i][j] << " ";
    //     }
    //     std::cout << std::endl;
    // }
    try {
        IloModel model(env);
        // Variable Define
        BoolVar3Matrix x(env, vehicles_no);
        for (int k = 0; k < vehicles_no; k++) {
            x[k] = BoolVarMatrix(env, nodes_no);
            for (int i = 0; i < nodes_no; i++) {
                x[k][i] = IloBoolVarArray(env, nodes_no);
                for (int j = 0; j < nodes_no; j++) {
                    x[k][i][j] = IloBoolVar(
                        env, std::string("x_{" + std::to_string(i) + "," + std::to_string(j) + "," +
                                         std::to_string(k) + "}")
                                 .c_str());
                }
            }
        }
        IloNumVarArray u(env, nodes_no);
        IloNumVarArray at(env, nodes_no);
        IloNumVarArray dt(env, nodes_no);
        for (int i = 0; i < nodes_no; i++) {
            u[i] = IloBoolVar(env, std::string("u_" + std::to_string(i)).c_str());
            at[i] = IloNumVar(env, 0, IloInfinity, IloNumVar::Float,
                              std::string("at_" + std::to_string(i)).c_str());
            dt[i] = IloNumVar(env, 0, IloInfinity, IloNumVar::Float,
                              std::string("dt_" + std::to_string(i)).c_str());
        }

        // Objective Function
        IloExpr profits(env);
        IloExpr traveling_cost(env);
        IloExpr outsourcing_cost(env);
        for (int k = 0; k < vehicles_no; k++) {
            for (int i = 0; i < nodes_no; i++) {
                for (int j = 0; j < nodes_no; j++) {
                    profits += p * m[i] * x[k][i][j];
                }
            }
        }
        for (int k = 0; k < vehicles_no; k++) {
            for (int i = 0; i < nodes_no; i++) {
                for (int j = 0; j < nodes_no; j++) {
                    traveling_cost += c[k] * tt[i][j] * x[k][i][j];
                }
            }
        }
        for (int i = 0; i < nodes_no; i++) {
            outsourcing_cost += u[i] * w;
        }
        model.add(IloMaximize(env, profits - traveling_cost - outsourcing_cost));
        profits.end();
        traveling_cost.end();
        outsourcing_cost.end();

        // Constraints

        // depot_loop
        for (int k = 0; k < vehicles_no; k++) {
            IloExpr depotOut(env);
            for (int i = 0; i < nodes_no; i++) {
                depotOut += x[k][0][i];
            }
            IloExpr depotIn(env);
            for (int i = 0; i < nodes_no; i++) {
                depotIn += x[k][i][0];
            }
            model.add(depotOut == depotIn);
            model.add(depotOut <= 1);
            model.add(depotIn <= 1);
            depotOut.end();
            depotIn.end();
        }

        // visit
        for (int i = 1; i < nodes_no; i++) {
            IloExpr nodesVisit(env);
            for (int k = 0; k < vehicles_no; k++) {
                for (int j = 0; j < nodes_no; j++) {
                    if (i == j) continue;
                    nodesVisit += x[k][i][j];
                }
            }
            model.add(nodesVisit + u[i] == 1);
            nodesVisit.end();
        }

        // flow constraint
        for (int k = 0; k < vehicles_no; k++) {
            for (int i = 0; i < nodes_no; i++) {
                IloExpr flowIn(env);
                IloExpr flowOut(env);
                for (int j = 0; j < nodes_no; j++) {
                    // if (i == j)
                    //     continue;
                    flowIn += x[k][j][i];
                }
                for (int j = 0; j < nodes_no; j++) {
                    // if (i == j)
                    //     continue;
                    flowOut += x[k][i][j];
                }
                model.add(flowIn == flowOut);
                flowIn.end();
                flowOut.end();
            }
        }

        // continuity of arrival time for every node
        for (int i = 0; i < nodes_no; i++) {
            for (int j = 1; j < nodes_no; j++) {
                // if (i == j)
                //     continue;
                IloExpr arcVisit(env);
                for (int k = 0; k < vehicles_no; k++) {
                    arcVisit += x[k][i][j];
                }
                model.add(at[j] >= dt[i] + tt[i][j] - M * (1 - arcVisit));
                arcVisit.end();
            }
        }
        // for (int i = 0; i < nodes_no; i++)
        // {
        //     for (int j = 1; j < nodes_no; j++)
        //     {
        //         // if (i == j)
        //         //     continue;
        //         IloExpr arcVisit(env);
        //         for (int k = 0; k < vehicles_no; k++)
        //         {
        //             for (int t = r[j]; t <= d[j]; t++)
        //             {
        //                 arcVisit += x[t][k][i][j];
        //             }
        //         }
        //         model.add(at[j] <= dt[i] + tt[i][j] + M * (1 - arcVisit));
        //         arcVisit.end();
        //     }
        // }

        // time window
        for (int i = 1; i < nodes_no; i++) {
            model.add(at[i] <= l[i]);
            model.add(at[i] >= e[i]);
        }

        // departure time constraints
        for (int i = 1; i < nodes_no; i++) {
            model.add(dt[i] == at[i] + s[i]);
        }

        // capacity constraint
        for (int k = 0; k < vehicles_no; k++) {
            IloExpr curLoad(env);
            for (int i = 0; i < nodes_no; i++) {
                for (int j = 0; j < nodes_no; j++) {
                    curLoad += x[k][i][j] * m[i];
                }
            }
            model.add(curLoad <= Q[k]);
            curLoad.end();
        }

        // binary variable constraint
        for (int k = 0; k < vehicles_no; k++) {
            for (int i = 0; i < nodes_no; i++) {
                for (int j = 0; j < nodes_no; j++) {
                    model.add(x[k][i][j] >= 0);
                    model.add(x[k][i][j] <= 1);
                }
            }
        }
        for (int i = 0; i < nodes_no; i++) {
            model.add(u[i] >= 0);
            model.add(u[i] <= 1);
        }

        // Optimize
        IloCplex cplex(model);
        // Parameter setting
        cplex.setParam(IloCplex::Param::MIP::Strategy::File, 3);
        cplex.setParam(IloCplex::Param::WorkDir, "./");
        std::string instance_name = str_split(argv[1], "__")[0];
        ofstream file("./results.txt", std::ios::app);
        IloNum start_time = cplex.getCplexTime();
        // CallBack
        cplex.use(timeIntervalCallback(env, cplex, file, start_time, instance_name));
        cplex.solve();
        // cplex.exportModel("best_route.lp");
        env.out() << "Solution status = " << cplex.getStatus() << endl;
        env.out() << "Solution value  = " << cplex.getObjValue() << endl;
        file << instance_name << "," << -cplex.getObjValue() << ","
             << cplex.getCplexTime() - start_time << "," << cplex.getStatus() << std::endl;
        file.close();
        ofstream detail_res("./detail_res/" + instance_name, std::ios::out);
        // display the value of variable
        for (int k = 0; k < vehicles_no; k++) {
            detail_res << "vehicle " << k << ": ";
            for (int i = 0; i < nodes_no; i++) {
                if (i == 0) {
                    detail_res << i;
                }
                for (int j = 0; j < nodes_no; j++) {
                    if (cplex.getValue(x[k][i][j]) > 0.1) {
                        detail_res << " -> " << j;
                        if (j == 0) {
                            i = nodes_no;
                        } else {
                            i = j - 1;
                        }
                        break;
                    }
                }
            }
            detail_res << std::endl;
        }
        detail_res << std::endl;
        // for (int i = 1; i < nodes_no; i++)
        // {
        //     std::cout << i << ": ";
        //     double sum = 0;
        //     for (int k = 0; k < vehicles_no; k++)
        //     {
        //         for (int j = 0; j < nodes_no; j++)
        //         {
        //             if (i == j)
        //                 continue;
        //             for (int t = r[i]; t <= d[i]; t++)
        //             {
        //                 std::cout << cplex.getValue(x[t][k][i][j]) << " ";
        //                 sum += cplex.getValue(x[t][k][i][j]);
        //             }
        //         }
        //     }
        //     std::cout << ", " << sum << " + " << cplex.getValue(u[i]) << std::endl;
        // }
        // std::cout << "demands serving: " << std::endl;
        // for (int i = 1; i < nodes_no; i++)
        // {
        //     for (int t = 1; t <= horizon; t++)
        //     {
        //         for (int k = 0; k < vehicles_no; k++)
        //         {
        //             for (int j = 0; j < nodes_no; j++)
        //             {
        //                 if (cplex.getValue(x[t][k][i][j]) > 0)
        //                 {
        //                     std::cout << i << " served by " << t << " " << k << " " << j <<
        //                     std::endl;
        //                 }
        //             }
        //         }
        //     }
        // }

        detail_res << "outsourced demands: ";
        for (int i = 0; i < nodes_no; i++) {
            if (cplex.getValue(u[i]) == 1) {
                detail_res << i << " ";
            }
        }
        detail_res << std::endl;

        detail_res << "arrival time: 0 ";
        for (int i = 1; i < nodes_no; i++) {
            detail_res << cplex.getValue(at[i]) << " ";
        }
        detail_res << endl;

        detail_res << "departure time: 0 ";
        for (int i = 1; i < nodes_no; i++) {
            detail_res << cplex.getValue(dt[i]) << " ";
        }

        detail_res << endl;
    } catch (IloException &ex) {
        cerr << "Error: " << ex << endl;
    } catch (...) {
        cerr << "Error" << endl;
    }
    env.end();
    return 0;
}
