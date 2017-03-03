#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <ctype.h>
#include <stdio.h>
#include <algorithm>
#include <cmath>
#include <stack>
#include <climits>
#include "clauses.h"

using namespace std;

// Constructor for clauses object (initializes the problem, given a
// file name and runWith value)
Clauses::Clauses(string fName, int rWith) {
  ifstream probStream; // the problem file
  string line; // one line of the input file

  // open file
  probStream.open(fName.c_str(), ios::in);
  if(!probStream.good()) {
    cout << "Error: not able to open file." << endl;
    return;
  }

  this->runWith = rWith;

  // Following block: extracts data from file into Clauses object
  getline(probStream, line);
  getline(probStream, line);
  // advanace stream until empty line
  while(true) {
    if(line.size() == 1 || line.size() == 0) break;
    getline(probStream,line);
  }
  probStream.ignore(666, 'v');
  probStream >> numVars; // read in number of variables
  probStream.ignore(666, 'c');
  probStream >> numClauses; // read in num clauses
  getline(probStream, line);
  getline(probStream, line);
  getline(probStream, line);
  // read in variables
  while(probStream.peek() != 'c') {
    var v;
    probStream >> v.varName;
    probStream >> v.prob;
    probStream >> ws;
    varsList.push_back(v);
  }
  getline(probStream, line);
  // read in clauses
  while(toupper(probStream.peek()) != 'S') {
    clause cl; // make a clause object
    do {
      int a; // make int to read into
      probStream >> a;
      cl.vars.push_back(a); // push back a literal into a clause
      probStream >> ws;
      varsList[abs(a)-1].occ++;
      if( a > 0) {
	varsList[abs(a)-1].occPos++;
      } else {
	varsList[abs(a)-1].occNeg++;
      }
    } while(probStream.peek() != '0');
    probStream.ignore(666, '0'); // ignore last 0 in clauses
    probStream >> ws;
    cl.len = cl.vars.size();
    cl.sat = 0;
    clList.push_back(cl); // push back clause to clause list
  }
}

// debugger function. prints all the clauses
void Clauses::printClauses() {
  for( int i = 0; i < clList.size(); i++) {
    for (int j = 0; j < clList[i].vars.size(); j++) {
      cout << clList[i].vars[j]<< " ";
    }
    cout << "   " << clList[i].sat << endl;
  }
  cout << endl << endl;
}

// debugger function. the current var assignments & occurrences
void Clauses::print_var_assignment() {
  cout << "var assignment" << endl;
  for( int i = 0; i < varsList.size(); i++) {
    cout << "=======================================" << endl;
    cout << varsList[i].varName << endl;
    cout << varsList[i].occ << endl;
    cout << varsList[i].occPos << endl;
    cout << varsList[i].occNeg << endl;
    cout << varsList[i].varVal << endl;
  }
  cout << endl;
}

/* Attempts to find a satisfying variable assignment recursively with
   user-specified heuristic(s)

   Returns: probability of satisfaction*/
float Clauses::solver() {
  // base cases
  int s = checkSat(); // Checks for status of the problem:returns 1
		      // for satisfied, -1 for unsatisfied, 0 for
		      // neither (ie. there are active clauses)
  if(s == 1) {
    return 1.0; // if sat return 1
  } else if (s == -1) {
    return 0.0; // if unsat clause return 0
  }

  int a = 0; // marks index of next variable to split on
  // get next unmarked variable (hasn't yet been split on). If value
  // isn't overwritten later, solver will simply split naively (VOS)
  while(varsList[a].mark == true) {
    a = a+1;
  }

  // use UCP method
  if(runWith == 2 || runWith == 4 || runWith == 5 || runWith == 6 || runWith == 7) {
    float v = ucp(); // looks for UC: if found, recursively calls THIS
		     // function (solver) to solve remainder of problem
    if(v >=  0) { // v is positive if there is a unit clause (negative if not)
      return v; // returns probability of satisfaction
    }
  }

  // use PVE method
  if(runWith == 3 || runWith == 4 || runWith == 5 || runWith == 6 || runWith == 7) {
    float v = pve(); // looks for PV: if found, recursively calls THIS
		     // function (solver) to solve remainder of problem
    if(v >= 0) { // v is positive if there is a pure variable (negative if not)
      return v; // returns probability of satisfaction
    }
  }

  // call splitting heuristics. each selects a variable at index 'a'
  // to split on. Overwrites prior assignment of 'a' (NOT NAIVE)
  if(runWith == 5) {
    a = heur1(a);
  }
  if(runWith == 6) {
    a = heur2(a);
  }
  if(runWith == 7) {
    a = heur3(a);
  }

  //variable splitting
  varsList[a].mark = true; // mark var as split for future recursion
  vector <clause> clListCopy; // for restoring problem after variable split
  vector <var> varsListCopy;
  varsList[a].varVal = true; // set variable value to true
  clListCopy = clList;
  varsListCopy = varsList;
  updateProbTrue(varsList[a]); // update problem with variable set to true
  float prob_with_true = solver(); // call solver recursively
  clList = clListCopy; // restore changes after recursion
  varsList = varsListCopy;

  varsList[a].varVal = false; // sames steps for setting var to false
  clListCopy = clList;
  varsListCopy = varsList;
  updateProbFalse(varsList[a]);
  float prob_with_false = solver();
  clList = clListCopy;
  varsList = varsListCopy;
  varsList[a].mark = false; // unmark variable

  // value return
  if(fabs(varsList[a].prob) >= 1) { // if prob 1 (choice variable)
    float v =  max(prob_with_true, prob_with_false); // take max of true/false split
    return v;
  } else if (varsList[a].prob < 1 && varsList[a].prob > 0) { // if chance var
    float v = varsList[a].prob*prob_with_true + (1-varsList[a].prob)*prob_with_false;
    return v;
 } else {
    cout << varsList[a].prob << endl;
    cout << varsList[a].varName << endl;
    cout << endl << endl << " HUGE PROBLEM" << endl << endl;
  }
  cout << "fix" << endl << endl;
}

/* Sets the variable V as TRUE, and updates all clauses accordingly
   (marks the clause as satsfied, or erases the literal from the clause) */
void Clauses::updateProbTrue(var v) {
  for(int i = 0; i < clList.size(); i++) { //iterate through clauses
    if(clList[i].sat == 1) continue; // if clause satisfied, do nothing
    for(int j = 0; j < clList[i].vars.size(); j++) { //else, iterate through variables in a clause
      if(v.varName == clList[i].vars[j]) { // if var satisfies clause...
	clList[i].sat = 1; // ...mark the current clause as satisfied

	// update variable counters
	for(int k = 0; k < clList[i].vars.size(); k++) { // iterate through literals in current clause
          varsList[abs(clList[i].vars[k])-1].occ--; // deincrement the count for current variable
          if (clList[i].vars[k] > 0) { // deincrement the positive/negated count for current variable
	    varsList[abs(clList[i].vars[k])-1].occPos--;
	  } else {
	    varsList[abs(clList[i].vars[k])-1].occNeg--;
          }
        }
      } else if(v.varName == -clList[i].vars[j]) { // else, if variable doesn't satisfy the clause (ie. literal negatively)
	varsList[v.varName - 1].occNeg--; // update counts accordingly
	varsList[v.varName - 1].occ--;
	clList[i].vars.erase(clList[i].vars.begin() + j); // erase the variable from the clause
	break; // as soon as we find the variable, we're w/ this clause
      }
    }
  }
}

/* Sets the variable V as FALSE, and updates all clauses accordingly
   (marks the clause as satsfied, or erases the literal from the clause)

   Note: inverse of updateProbTrue*/
void Clauses::updateProbFalse(var v) {
  for(int i = 0; i < clList.size(); i++) { //iterate through clauses
    if(clList[i].sat == 1) continue;
    for(int j = 0; j < clList[i].vars.size(); j++) { // iterate through variables in a clause
      if(v.varName == -clList[i].vars[j]) { // if var satisfies clause
	clList[i].sat = 1; // mark clause as sat

	for(int k = 0; k < clList[i].vars.size(); k++) {
	  varsList[abs(clList[i].vars[k])-1].occ--;
	  if (clList[i].vars[k] > 0) {
	    varsList[abs(clList[i].vars[k])-1].occPos--;
	  } else {
	    varsList[abs(clList[i].vars[k])-1].occNeg--;
	  }
	}
      } else if (v.varName == clList[i].vars[j]) {
	varsList[v.varName - 1].occPos--;
	varsList[v.varName - 1].occ--;
	clList[i].vars.erase(clList[i].vars.begin() + j);
	break;
      }
    }// close first for
  }// close second for
}

// checks if the problem is satisfied. Code is as follows:
// -1 for unsatisfied var. assignment
// 0 if there are still active problems (neither sat/unsat)
// 1 for satisfactory var. assignment
int Clauses::checkSat() {
  int numSat = 0; // count number of sat clauses
  for( int i = 0; i < clList.size(); i++ ) {
   if (clList[i].vars.size() == 0 && clList[i].sat != 1) {
     return -1; // if any clause has no literals and ISN'T satisfied,
		// the entire problem is unsatsified
   } else if (clList[i].sat == 1) {
     numSat = numSat + 1; // if clause is satisfied, add 1 to count
   }
 }
  if (numSat == clList.size()) return 1; // if all clauses sat, entire problem is satisfied
  return 0; // neither sat or unsat (still have active clauses)
}

/* Attempts to find a unit clause.

   Returns:
   If no unit clause found, returns -1.
   If unit clause found, returns the problem's probability of satisfaction */
float Clauses::ucp() {
  vector <int> uc; // vector with unit clause info: 1st position is
		   // the LITERAL, 2nd position is the VARIABLE NAME
  for(int i = 0; i < clList.size(); i++) { // iterate through clauses
    if(clList[i].sat == 1) continue; // skip clause if satisfied

    if(clList[i].vars.size() == 1) { // if a uc is found...
      uc.push_back(clList[i].vars[0]); // ... save value of LITERAL ...
      uc.push_back(abs(clList[i].vars[0])-1); // ...and save name of VARIABLE
      break;
    }
  }
  if( uc.size() == 0) return -1; // no uc found
  return update_for_pve_ucp(uc); // else, update problem w/ unit clause and
                                 // solve rest of the problem recursively
}

/* Attempts to find a pure variable

   Returns:
   If no pure variable found, returns -1.
   If pure variable found, returns the problem's probability of satisfaction */
float Clauses::pve() {
  vector <int> pv; // vector with pure variable info: 1st position is
		   // the LITERAL, 2nd position is the VARIABLE NAME
  for(int i = 0; i < varsList.size(); i++) { // iterate through all VARIABLES
    if(fabs(varsList[i].prob) < 1) continue; // skip if chance
    if(varsList[i].occ < 1) continue; // skip if it doesn't show up
    if(varsList[i].mark == true) continue; // skip if value already set

    if(varsList[i].occPos == varsList[i].occ) { // if var is PURELY POSITIVE
      pv.push_back(1); // value of literal is TRUE
      pv.push_back(varsList[i].varName -1); // name of VARIABLE
      break;
    }

    if (varsList[i].occNeg == varsList[i].occ) { // if var is PURELY NEGATIVE
      pv.push_back(-1); // lit is FALSE
      pv.push_back(varsList[i].varName - 1); // varname
      break;
    }
  }

  if(pv.size() == 0) return -1; // return -1 if no PV found
  return update_for_pve_ucp(pv); // otherwise, update problem w/ pv and
                                 // solve rest of the problem recursively
}

/* Updates problem given a PURE or UNIT CLAUSE VARIABLE, then
   class solver to recursively solve the remaining problem

   input: a vector with two ints.
          FIRST int holds the sign of the literal (positive or negative)
          SECOND int is the name of the unit/pure variable

   returns: problem satisfaction probability*/
float Clauses::update_for_pve_ucp(vector <int> uc) {
  float p_w_t, p_w_f; // probability with variable set to TRUE or FALSE
  if( uc.size() > 0) { // if there IS a pure variable...
    int a = uc[1]; // NAME of variable
    varsList[a].mark = true; // mark value so it isnt overwritten in recursion

    if(uc[0] > 0) { // if the LITERAL is true...
      varsList[a].varVal = true; // set var to true
      updateProbTrue(varsList[a]); // update appropriately
      p_w_t = solver(); // recursively solve the rest of the problem
      p_w_f = 0; // if the literal is true, prob with false is 0
    } else { // if the literal is false, inverted steps for setting var to false
      varsList[a].varVal = false;
      updateProbFalse(varsList[a]);
      p_w_f = solver();
      p_w_t = 0;
    }
    varsList[a].mark = false;

    //Return probability of satisfaction. Method depends on if current
    //variable is CHANCE or CHOICE
    if(fabs(varsList[a].prob) >= 1) { // choice variable
      float v =  max(p_w_t, p_w_f);
      return v;
    } else { // chance variable
      float v = varsList[a].prob*p_w_t + (1-varsList[a].prob)*p_w_f;
      return v;
    }
  }
}

/* Given a variable, returns all other variables of the same block */
vector <var> Clauses::block_collector(int varname) {
  vector <var> vars_block;

  if(fabs(varsList[varname].prob) >= 1) { // if the current block is a CHOICE block
    // collect the other variables within the block
    while(fabs(varsList[varname].prob) >= 1 && varname < varsList.size()) {
      vars_block.push_back(varsList[varname]);
      varname++;
    }
  } else { // if the current block is a CHANCE block
    // collect the other variables within the block
    while(fabs(varsList[varname].prob) < 1 && varname < varsList.size()) {
      vars_block.push_back(varsList[varname]);
      varname++;
    }
  }

  return vars_block;
}

/* Heuristic 1: Find the smallest clause and split on its variables

   Params: the next variable (So we know what block we're in)
   Returns: the name/index of a variable in the smallest clause */
int Clauses::heur1(int a) {
  int min_len = INT_MAX; // length of shortest clause
  int min_cl_index; // index of the min clause
  bool has_possible_var; // true if a clause has a splittable var

  vector <var> vars_block; // vars from the current choice/chance block
  vars_block = block_collector(a);

  // THIS for loop finds the shortest clause
  for(int i = 0; i < clList.size(); i++) { // iterate through clauses
    has_possible_var = false; // reset this bool
    if (clList[i].sat == 1) continue; // if clause is already satisfied, skip

    for(int j = 0; j < clList[i].vars.size(); j++) { // LITerate through its LITerals
      for(int k = 0; k < vars_block.size(); k++) { // iterate through current block
	if(vars_block[k].mark == true) continue; // if the var has been set, skip
	// if the literal in the clause is in the current block...
	if(vars_block[k].varName == abs(clList[i].vars[j])) {
	  has_possible_var = true; // ...we found a variable to split on!!
	  break;
	}
      } // close third for
      if(has_possible_var == true) break;
    } // close second for
    if(has_possible_var == false) continue;

    // if a new shortest clause has been found, update min_len
    if(clList[i].vars.size() < min_len) {
      min_len = clList[i].vars.size();
      min_cl_index = i;
    }
  } // close first for


  // THIS for loop picks a variable in the shortest clause
  // iterate though vars in the shortest clause
  for(int l = 0; l < clList[min_cl_index].vars.size(); l++) {
    for(int m = 0; m < vars_block.size(); m++) { // iter through other var in the block
      if(vars_block[m].mark == true) continue; // if the var has been assigned, skip

      // if the literal in the clause is in the current block...
      if(vars_block[m].varName == abs(clList[min_cl_index].vars[l])) {
	return vars_block[m].varName - 1; //...return that variable to split on!
      }
    }
  }

  return a; // if no shortest clause found, use naive method
}

/* Heuristic 2: Split variable that appears the most in all clauses

   Params: the next variable (So we know what block we're in)
   Returns: the name/index of the most popular variable */
int Clauses::heur2(int a) {
  vector <var> vars_block; // vars from the current choice/chance block
  vars_block = block_collector(a);


  int max_total = vars_block[0].occ;
  int max_name = vars_block[0].varName;

  // find the var with most occurences within our current block
  for(int i = 0; i < vars_block.size(); i++) { // iterate through vars in block
    if(vars_block[i].mark == true) continue; // if var has been set, skip
    if(max_total < vars_block[i].occ) { // if we found a new max...
      max_total = vars_block[i].occ; // ...set our variables accordingly
      max_name = vars_block[i].varName;
    }
  }
  return max_name - 1; // returns most popular variable to split on
}

/* Heuristic 3: Split on LITERAL that appears the most in all clauses

   Params: the next variable (So we know what block we're in)
   Returns: the name/index of the most popular variable */
int Clauses::heur3(int a) {
  vector <var> vars_block; // vars from the current choice/chance block
  vars_block = block_collector(a);

  int max_name = vars_block[0].varName;
  int max_pos_neg;
  // set max_pos_neg to whichever literal has more occurences (pos or neg)
  if(vars_block[0].occPos > vars_block[0].occNeg) {
    max_pos_neg = vars_block[0].occPos;
  } else {
    max_pos_neg = vars_block[0].occNeg;
  }

  // find the LITERAL with most occurences within our current block
  for(int i = 0; i < vars_block.size(); i++) { // iter through our block
    if(vars_block[i].mark == true) continue; // if already set, skip

    // if a literal shows up MORE times than max_pos_neg, update accordingly
    if(max_pos_neg < vars_block[i].occPos) {
      max_pos_neg = vars_block[i].occPos;
      max_name = vars_block[i].varName;
    }
    if(max_pos_neg < vars_block[i].occNeg) {
      max_pos_neg = vars_block[i].occNeg;
      max_name = vars_block[i].varName;
    }
  }
  return max_name - 1; // returns the most popular literal to split on
}
