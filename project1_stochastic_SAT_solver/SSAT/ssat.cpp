#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctype.h>
#include <stdio.h>
#include "ssat.h"
#include "clauses.h"
#include <dirent.h>
#include "rtimer.h"
using namespace std;

int main(int argc, char** argv) {
  float start, end, time;
  int runWith;
  string fName;
  vector <string> testFiles;

  while(true) {
    cout << endl << "=============================" << endl;
    cout << "Choose how to solve problem: " << endl;
    cout << "1. Naively with VOS" << endl;
    cout << "2. With UCP" << endl;
    cout << "3. with PVE" << endl;
    cout << "4. With UCP and PVE" << endl;
    cout << "5. Heuristic 1 (Split variable in smallest clause)" << endl;
    cout << "6. Heuristic 2 (Split variable with most total literals in active clauses" << endl;
    cout << "7. Heuristic 3 (split varialbe with most positive or negated literals in active clauses" << endl;
    cout << "8. Run all 126 tests!!! (yeah, you heard correctly)" <<endl;
    cout << "Enter your choice: " << endl;
    cin >> runWith;
    if (runWith > 8 || runWith < 1) {
      while(runWith > 8 || runWith < 1) {
	cout << "Invalid input, try again." << endl;
	cin >> runWith;
      }
    }
    if(runWith == 8) break;
    cout << endl << "Enter the test file name you want to run (no quotes)" << endl;
    cin >> fName;
    break;
  }

  // grab all
  // This code was modified from a Stack Overflow thread. see README.txt
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (".")) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      if(ent->d_name[0] == '.') continue;
      if(ent->d_name[0] != 'T') continue;
      string s = string(ent->d_name);
      testFiles.push_back(s);
    }
    closedir (dir);
  } else {
    cout << "Could not open directory" << endl;
    return 0;
  }

  if(runWith == 8) {
    for(int i = 0; i < testFiles.size(); i++) {
      cout << "=========================" << endl << endl;
      cout << " Tests for: " << testFiles[i] << endl;
      int j = 3;
      while(j < 8) {
	if(j == 1) cout << "VOS" << endl;
	if(j == 2) cout << "UCP" << endl;
	if(j == 3) cout << "PVE" << endl;
	if(j == 4) cout << "UCP & PVE" << endl;
	if(j == 5) cout << "UCP, PVE & HEUR1" << endl;
	if(j == 6) cout << "UCP, PVE & HEUR2" << endl;
	if(j == 7) cout << "UCP, PVE & HEUR3" << endl;
	runWith = j;
	Clauses cl(testFiles.at(i), runWith);
	Rtimer rt1;
	rt_start(rt1);
	float f = cl.solver();
	rt_stop(rt1);
	char buf [1024];
	rt_sprint(buf,rt1);
	cout << "Probability of Satisfaction: " << f << endl;
	cout << "Run Time: " << buf << endl << endl;
	j++;
	break;
      }
    }
  } else{
    Clauses cl(fName.c_str(), runWith);
    cout << endl << "for file: " << fName << endl;
    Rtimer rt1;
    rt_start(rt1);
    start = clock();
    float f = cl.solver();
    end = clock();
    rt_stop(rt1);
    char buf [1024];
    rt_sprint(buf,rt1);
    cout << "Probability of Satisfaction: " << f << endl;
    //cout << "Run Time: " << buf << endl << endl;
    cout << endl << "Run Time 2: " << (end - start)/CLOCKS_PER_SEC << endl << endl;
  }
  return 0;
}
