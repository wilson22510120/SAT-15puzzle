#include <iostream>
#include <vector>
#include <fstream>
#include "sat.h"

using namespace std;



// void reportResult(const SatSolver& solver, bool result, unsigned number_of_node)
// {
//     solver.printStats();
//     cout << (result? "SAT" : "UNSAT") << endl;
//     if (result) {
//         for (size_t i = 0; i < number_of_node; ++i){
//             for (size_t j = 0; j < number_of_node; ++j){
//                 cout << solver.getValue(_e[i][j]->getVar()) << endl;
//             }
//         }
//     }
// }

int main(int argc, char* argv[])
{
    SatSolver solver;
    solver.initialize();
    solver.assumeRelease();  // Clear assumptions
    //read in the info of the graph
    char buffer[200];
    char kind_of_graph;
    int _number_of_node,_number_of_edge;
    int w,s,e;
    
    fstream fin(argv[1]);
    if (!fin){
        cout<<"error"<<endl;
    }
    fstream fout;
    fout.open(argv[2],ios::out);
    fin>>kind_of_graph;
    fin>>_number_of_node;
    fin>>_number_of_edge;
    const int number_of_node = _number_of_node;
    const int number_of_edge = _number_of_edge;
    bool edge[number_of_node][number_of_node];
    vector <unsigned> start[number_of_node];
    vector <unsigned> end[number_of_node];
    for (unsigned i = 0; i < number_of_node; i++){
       for (unsigned j = 0; j < number_of_node; j++){
           edge[i][j] = false;
       }
   }
    while (fin >> s >> e >> w){
        edge[s][e] = true;
        edge[e][s] = true;
        start[s].push_back(e);
        end[e].push_back(s);
    }

   //make the relation of the nodes
   
   

   //generate vars for the new directed graph
   Var _e[number_of_node][number_of_node];
   
   for (unsigned i = 0; i < number_of_node; i++){
       for (unsigned j = 0; j < number_of_node; j++){
           _e[i][j] = solver.newVar();
           if (edge[i][j] == false){
               solver.assumeProperty(_e[i][j],false);
           }
       }
   }

   // 1.add constraint that the bidirectional edges cannot be chosen simultaneously
   vector <Var> constraint;
   unsigned other_end;
   Var newV[number_of_edge * 20];
   for(unsigned i = 0; i < 20 * number_of_node;i++){
       newV[i] = solver.newVar();
   }
   size_t counter = 0;
   for (unsigned i = 0; i < number_of_node; i++){
       for (unsigned j = i; j < number_of_node; j++){
           if (edge[i][j] == true){
               solver.addCNF(newV[counter],_e[i][j],true, _e[j][i], true);
               counter ++;
               assert(counter <= 20 * number_of_node);
               solver.assumeProperty(newV[counter],true);
           }
       }
   }

   //2.add the constraint that only one outward and one inward edge would be true 
   //
   //(2.1) this part is for C(n,2)
   for (unsigned i = 0; i < number_of_node; i++){
       for (unsigned j = 0; j < start[i].size(); j++){
            for ( unsigned k = j+1; k < start[i].size(); k++){
                solver.addCNF(newV[counter],_e[i][start[i][j]],true, _e[i][start[i][k]], true);
                solver.assumeProperty(newV[counter],true);
                counter ++;
                assert(counter <= 20 * number_of_node);
            }
       }
   }

   for (unsigned i = 0; i < number_of_node; i++){
       for( unsigned j = 0; j < end[i].size(); i++){
           for( unsigned k = j + 1; k < end[i].size(); k++){
               solver.addCNF(newV[counter],_e[i][end[i][j]],true, _e[i][end[i][k]], true);
               solver.assumeProperty(newV[counter],true);
               counter ++;
               assert(counter <= 20 * number_of_node);
           }
       }
   }
    //(2.2) this part is for or all of the literals
    bool flag = false;
    unsigned _i = 0,_j = 0;
    for (unsigned i = 0; i < number_of_node; i++){
       for (unsigned j = 0; j < start[i].size(); j++){
           if (start[i].size() < 2){
               cerr<<"unsat, since there is a node with only one edge";
           }
           else{
               if (edge[i][j] == true && flag == false){
                   _i = i;
                   _j = j;
                   flag = true;
               }
               else if (edge[i][j] == true){
                   solver.addCNF(newV[counter],_e[_i][_j],false, _e[i][j], false);
                   counter++;
                   _i = i;
                   _j = j;
                   assert(counter <= 20 * number_of_node);
               }
           }
       }
       solver.assumeProperty(newV[counter - 1],true);
    }

    //3.check that there is no multi-cycle in the answer if there is one, add it into constraint

    //used for position table
    Var U[number_of_node][number_of_node];
    for (unsigned i = 0; i < number_of_node; i++){
       for (unsigned j = 0; j < number_of_node; j++){
           U[i][j] = solver.newVar();
       }
   }
    // set the position table constraint
    //constraint3,4
    for (unsigned i = 0; i < number_of_node; i++){
        solver.addCNF(newV[counter],_e[0][i],true,U[i][1],true);
        solver.assumeProperty(newV[counter],true);
        counter++;
        assert(counter <= 20 * number_of_node);

        solver.addCNF(newV[counter],_e[i][0],true,U[i][number_of_node - 1],true);
        solver.assumeProperty(newV[counter],true);
        counter++;
        assert(counter <= 20 * number_of_node);
    }

    //constraint 5
    for (unsigned i = 1; i < number_of_node; i++){
        for (unsigned j = 1; j < number_of_node; j++){
            for( unsigned p = 1; p < number_of_node - 1; p++){
                solver.addCNF(newV[counter],_e[i][j],true,U[i][p],true);
                counter++;
                assert(counter <= 20 * number_of_node);
                solver.addCNF(newV[counter],newV[counter - 1],false,U[j][p + 1],true);
                solver.assumeProperty(newV[counter],true);
                counter++;
                assert(counter <= 20 * number_of_node);
            }
        }
    }


//    genProofModel(solver);
   
   bool result;
   
 
   
   // solver.assumeProperty(newV, true);  // k = 1

   result = solver.assumpSolve();
   solver.printStats();
    cout << (result? "SAT" : "UNSAT") << endl;
    if (result) {
        for (size_t i = 0; i < number_of_node; ++i){
            for (size_t j = 0; j < number_of_node; ++j){
                cout << solver.getValue(_e[i][j]);
            }
            cout << "\n";
        }
    }
//    reportResult(solver, result);

   cout << endl << endl << "======================" << endl;

   // // k = Solve(Gate(3) & !Gate(7))
   // newV = solver.newVar();
   // solver.addAigCNF(newV, gates[3]->getVar(), false, gates[7]->getVar(), true);
   // solver.assumeRelease();  // Clear assumptions
   // solver.assumeProperty(newV, true);  // k = 1
   // result = solver.assumpSolve();
   // reportResult(solver, result);
}
