#ifndef CLAUSES_H
#define CLAUSES_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctype.h>
#include <stdio.h>

using namespace std;

/* Variable Object */
typedef struct _var {
  int varName, occ, occPos, occNeg; // varName and #of times it appears in clauses
  float prob;  // prob of chance variable being true. -1 if choice variable.
  bool varVal; // satisfying variable assignment
  bool mark;   // marks if this variable has been assigned yet
} var;

/* Clause Object */
typedef struct _clause {
  vector <int> vars; // variables within the clause (ought to be named literals...)
  int len; // length of clause
  int sat; // true if an assignment satisfied this clause
} clause;

/* Object that represents the entire problem (all the clauses + variables) */
class Clauses {
 public:
  Clauses(string fName, int rWith); // constructor. takes filename, value for 'runwith' (see below)
  void printClauses(); // debugger function
  void splitter(); // given a method, runs naive/ucp/pve/heuristic and splits on a variable
  float solver(); // attempts to find a satisfying variable assignment
  void updateProbTrue(var v); // updates all the clauses after splitting on a variable
  void updateProbFalse(var v);
  int checkSat(); // checks if the problem is satisfied, unsatisfied, or still active
  void print_var_assignment(); //debugging methods :)
  void print_var_prob(); // debugging (:
  float ucp();
  float pve();
  float update_for_pve_ucp(vector <int> uc); // special case: pve/ucp updates an variable
                                             // at an arbitrary index in the variable list
  vector <var> block_collector(int a); // find vars in same block
  int heur1(int b); // heuristic 1. split on a var in shortes clause (of possible variables)
  int heur2(int b); // 2: split on most popular variable
  int heur3(int b); // 3: split on most popular literal (pos or neg)
  ~Clauses() {};

 private:
  vector <var> varsList; // vector of vars
  vector <clause> clList; // vector of clauses
  int runWith; // what method (UCP PCE heuristics) to run
  int numVars; // number of vars
  int numClauses; // number of clauses
};
#endif
