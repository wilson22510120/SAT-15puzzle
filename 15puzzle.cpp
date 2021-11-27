#include <iostream>
#include <vector>
#include <fstream>
#include "sat.h"
#include "15puzzle.h"

using namespace std;


int main(int argc, char* argv[])
{
	SatSolver solver;
	solver.initialize();
	solver.assumeRelease();  // Clear assumptions

	char buffer[200];
	int length_of_square, steps;
	fstream fin(argv[1]);
	if (!fin){
		cerr << "error: file notfound!" <<endl;
	}
	fstream fout;
	fout.open(argv[2],ios::out);

	fin >> length_of_square;
	cout << length_of_square << endl;

	fin >> steps;
	cout << steps << endl;
	// cout << length_of_square << endl;
	size_t a, b, c, d;

	// int initial_array[length_of_square][length_of_square];
	int** initial_array;
	initial_array = new int* [length_of_square];
	for(size_t i = 0; i < length_of_square; i++){
		initial_array[i] = new int[length_of_square];
		for(size_t j = 0; j < length_of_square; j++) {
			int tmp;
			fin >> tmp;
			initial_array[i][j] = tmp;
		}
	}

	// for(size_t i = 0; i < 4; i++) {
	// 	for(size_t j = 0; j < 4; j++) {
	// 		cout << initial_array[i][j] << " ";
	// 	}
	// 	cout << endl;
	// }

	// vector<vector<int> > test = neighbor(0,0);
	// for(vector<int> tmp : test){
	// 	cout << test.size() << endl;
	// 	cout << tmp[0] << " " << tmp[1] << endl;
	// }


	// generate vars for the array Si,j,n,t
	// i:row, j:column, n:number(0 denotes empty tile), t:time
	Var _s[4][4][16][81];

	// Var**** _s;
	// _s = new Var***[length_of_square];
	// for(size_t i = 0; i < length_of_square; i++) {
	// 	_s[i] = new Var**[length_of_square];
	// 	for(size_t j = 0; j < length_of_square; j++) {
	// 		_s[i][j] = new Var*[length_of_square ^ 2];
	// 		for(size_t k = 0; k < (length_of_square^2); k++) {
	// 			_s[i][j][k] = new Var[steps];
	// 		}
	// 	}
	// }


	vector<Var> newV;
	size_t totalTime = 0;
	bool oddMoves;
	for(size_t t = 0; t < steps; t++) {
		// 1. create new var
		for(size_t i = 0; i < 4; i++) {
			for(size_t j = 0; j < 4; j++) {
				for(size_t n = 0; n < 16; n++) {
					_s[i][j][n][t] = solver.newVar();
				}
			}
		}
		// 2. basic
		for(size_t i = 0; i < 4; i++) {
			for(size_t j = 0; j < 4; j++) {
				for(size_t n = 0; n < 16; n++) {
					for(size_t m = n + 1 ; m < 16; m++) {
						solver.addCNFnoF(_s[i][j][n][t], true, _s[i][j][m][t], true);
						// cout << "t:" << t << endl; 
						// solver.printStats();
					}
				}
			}
		}

		for(size_t n = 0; n < 16; n++) {
			for(size_t i = 0; i < 4; i++) {
				for(size_t j = 0; j < 4; j++) {
					for(size_t k = 0 ; k < 4; k++) {
						for(size_t l = 0; l < 4; l++) {
							if(4*i + j < 4*k + l) {
								solver.addCNFnoF(_s[i][j][n][t], true, _s[k][l][n][t], true);
							}
						}
					}
				}
			}
		}		
		cout << "22222222" << '\n';
		// 3. One is empty tile
		for(size_t i = 0; i < 4; i++) {
			// break;
			if(t == 0) break;
			for(size_t j = 0; j < 4; j++) {
				// cout << i << j << "n" << t<< endl;

				vector<vector<int> > nb = neighbor(i, j);
				// cout << nb.size() << endl;
				vector<Var> nbVar;
				for(vector<int> v : nb) {
					nbVar.push_back(_s[ v[0] ][ v[1] ][0][t]); // v[0] = k, v[1] = l
				}
				// newV.push_back(solver.newVar());newV.push_back(solver.newVar());
				Var v1 = solver.newVar();
				Var v2 = solver.newVar(); 
				// size() - 2: newV1; size() - 1: newV2
				solver.addCNFn(v1, nbVar);
				// solver.printStats();
				solver.addAigCNF(v2, _s[i][j][0][t], true, v1, false);
				solver.addCNFnoF(_s[i][j][0][t-1], true, v2, false);
			}
		}
		cout << "3333333" << '\n';

		// 4. 14 tiles unchanged
		for(size_t i = 0; i < 4; i++) {
			// break;
			if(t == 0) break;
			for(size_t j = 0; j < 4; j++) {
				Var vf = solver.newVar();
				solver.addAigCNF(vf, _s[i][j][0][t- 1], true, _s[i][j][0][t], true);
				vector<Var> V;
				for(size_t n = 0; n < 16; n++) {
					V.push_back(solver.newVar());
					solver.addEqualCNF(V[n], _s[i][j][n][t], false, _s[i][j][n][t - 1], false);
				}
				Var tmp1 = solver.newVar(), tmp2;
				solver.addAigCNF(tmp1, V[1], false, V[2], false);
				for(size_t n = 3; n < 16; n++) {
					tmp2 = solver.newVar();
					solver.addAigCNF(tmp2, tmp1, false, V[n], false);
					tmp1 = tmp2;
				}
				solver.addAigCNF(vf, tmp1, false, V[0], false);
			}
		}    
		cout << "44444444" << '\n';

		// 5. red implies
		for(size_t i = 0; i < 4; i++) {
			if(t == 0) break;
			for(size_t j = 0; j < 4; j++) {
				for(size_t n = 1; n < 16; n++) {
					vector<vector<int> > nb = neighbor(i, j);
					vector<Var> nbVar;
					for(vector<int> v : nb) {
						nbVar.push_back(_s[ v[0] ][ v[1] ][n][t]); // v[0] = k, v[1] = l
					}
					Var Vr = solver.newVar();
					solver.addCNFn(Vr, nbVar);
					solver.addCNF3noF(_s[i][j][n][t - 1], true, _s[i][j][0][t], true, Vr, false);
				}

			}
		}
		cout << "555555555" << '\n';

		// 6. assert property
		if(t == 0) {
			for(size_t i = 0; i < 4; i++) {
				for(size_t j = 0; j < 4; j++) {
					if(initial_array[i][j] == 0) {
						oddMoves = (i + j) % 2;
					}
					for(size_t n = 0; n < 16; n++) {
						if(initial_array[i][j] == n) {
							solver.assertProperty(_s[i][j][n][t], true);
						}  
						else {
							solver.assertProperty(_s[i][j][n][t], false);
						}                      					
					}
				}
			}
		}
				cout << "66666666" << '\n';

		// 7. no repeats move
		if(t >= 2) {
			for(size_t i = 0; i < 4; i++) {
				for(size_t j = 0; j < 4; j++) {
					for(size_t n = 0; n < 16; n++) {
						solver.addCNF3noF(_s[i][j][n][t - 2], true, _s[i][j][n][t - 1], false, _s[i][j][n][t], true);
					}
				}
			}		
		}
		
		cout << "877777777" << '\n';


		// solve or not
		if(t % 2 == oddMoves) {
			// 7. constraint
			for(size_t i = 0; i < 4; i++) {
				for(size_t j = 0; j < 4; j++) {
					for(size_t n = 0; n < 16; n++) {
						if(i == 3 && j == 3) {
							solver.assumeProperty(_s[i][j][0][t], true);
							continue;
						}
						solver.assumeProperty(_s[i][j][4*i + j + 1][t], true);
					}
				}
			}	

			if(solver.assumpSolve()) {
				// solver.solve();
				cout << t << " SAT"<<endl;
				solver.printStats();
				cout << endl;
				totalTime = t;
				break;
			}
			else {
				cout << t << " UNSAT"<<endl;
				solver.printStats();
				cout << endl;
				solver.assumeRelease();
			}

		}

	}
	if(solver.assumpSolve()) {
		cout << "totaltime:" << totalTime << endl;
		for(size_t t = 0; t <= totalTime; t++) {
			size_t plot[4][4];
			for(size_t i = 0; i < 4; i++) {
				for(size_t j = 0; j < 4; j++) {
					for(size_t n = 0; n < 16; n++) {
						if(solver.getValue(_s[i][j][n][t]) == 1) {
							plot[i][j] = n;
						}
					}
				}
			}	
			cout << "t = " << t	<< ":" << endl;
			for(size_t i  = 0; i < 4; i++) {
				for(size_t j  = 0; j < 4; j++) {
					cout << plot[i][j] << " ";
				}	
				cout << endl;
			}
			cout << endl;	

		}
		
	}

	// for(size_t i = 0; i < length_of_square; i++) {
	// 	for(size_t j = 0; j < length_of_square; j++) {
	// 		for(size_t k = 0; k < (length_of_square^2); k++) {
	// 			delete [] _s[i][j][k];
	// 		}
	// 		delete [] _s[i][j];
	// 	}
	// 	delete [] _s[i];
	// }
	// delete [] _s;
	}









