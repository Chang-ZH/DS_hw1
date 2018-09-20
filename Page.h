#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <sstream>
#include <cmath>
#include <map>
#include <algorithm>

using namespace std;
const double page_count = 500;

struct Page{
		string page_name;
		double pr_score;
		int out_links; // the number of the pages pointing "to"
		vector<string> links;// the pages pointing "into"
		vector<string> words;// content
};

struct ORPage{
	string page;
	double pr;
};

/*** Function  ***/
// I/O
void loadPage(vector<Page>&, char*, int);
void writePageRank(double, double, vector<Page>);
void writeReverseIndex(map< string, vector<string> >);
void loadList(vector<string>&, char*);
void writeList(double, double, vector<string>, map< string, vector<string> >, map<string, double>);

// PageRank
vector<int> find_pointingPage(vector<string>); // string to int
double page_rank(double , double*, vector<string>, vector<Page>, int); // return changed diff

// ReverseIndex
map< string, vector<string> > createReverseIndex(map< string, vector<string> >, vector<string>, string);

// Search Engine
vector<string> doAND(vector<string>, map< string, vector<string> >);
vector<string> doOR(vector<string>, map< string, double>, map< string, vector<string> >);

// Sorting
vector<Page> insertionSort(vector<Page>);
void heapSort_insert(vector<ORPage>&, int);
void heapSort_remove(vector<ORPage>&);



