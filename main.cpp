#include "Page.h"

/*** Main  ***/
int main(int argc, char *argv[]){

	double stop_diff = atof(argv[1]);
	double dumping_factor = atof(argv[2]);

	vector<Page> pageList;
	pageList.resize(page_count);
	vector<Page> sorted_pageList;
	pageList.resize(page_count);

	for(int i = 0; i < page_count; i++){
		pageList.at(i).pr_score = 1 / page_count; // pagerank init

		stringstream ss;
		ss << i;
		string num = ss.str();
		pageList.at(i).page_name = "page" + num;
	}

	/*** Read File ***/
	char file_path[] = "web-search-files2/page"; // file path
	loadPage(pageList, file_path, page_count);

	/*** Compute PR  ***/
	// compute outlink count
	for(int i = 0; i < page_count; i ++){
		vector<string> page = pageList.at(i).links;
		vector<int> ins = find_pointingPage(page);
		for(int j = 0; j < ins.size(); j ++){
			int k = ins.at(j);
			pageList.at(k).out_links ++;
		}
	}

	// PageRank
	double diff = stop_diff + 1;
	//int times = 1;
	while(diff >= stop_diff){
		diff = 0;
		//cout << "Round" << times << endl;
		for(int i = 0; i < page_count; i++){
			double *pr_score_before = &pageList.at(i).pr_score;
			diff += page_rank(dumping_factor, pr_score_before, pageList.at(i).links, pageList, page_count);
		}
		//cout << diff << endl;
		//times ++;
	}

	// Insertion Sort
	sorted_pageList = insertionSort(pageList);

	// Output
	writePageRank(dumping_factor, stop_diff, sorted_pageList);

	/*
	for(int i = 0; i < page_count; i ++){
		cout << sorted_pageList.at(i).page_name << " : " << sorted_pageList.at(i).pr_score << endl;
	}
	*/

	/*** ReverseIndex  ***/
	// STL Map is "Red Black Tree", so it will be orded by ASCLL
	map< string, vector<string> > ReverseIndex;
	for(int i = 0; i < page_count; i++){
		// use the "sorted" list as the argument
		ReverseIndex = createReverseIndex(ReverseIndex, sorted_pageList.at(i).words, sorted_pageList.at(i).page_name);
	}
	writeReverseIndex(ReverseIndex);

	/*
	// Map traversal
	for(map< string, vector<string> > ::const_iterator it = ReverseIndex.begin(); it != ReverseIndex.end(); it ++){
			cout << it->first << " ";
		for(int i = 0; i < it->second.size(); i ++){
			if(i == it->second.size() - 1)
				cout << " " << it->second.at(i) << endl;
			else
				cout << " " << it->second.at(i);
		}
	}
	*/

	/*
	vector<string> temp = ReverseIndex["Baker"];
	for(int i = 0; i < temp.size(); i ++){
		cout << temp.at(i) << endl;
	}
	*/

	/*** Search Engine  ***/
	map<string, double> page_pr;
	for(int i = 0; i < page_count; i ++){
		string page_title = sorted_pageList.at(i).page_name;
		double pr_value = sorted_pageList.at(i).pr_score;
		page_pr.insert(pair< string, double >(page_title, pr_value));
	}

	vector<string> List;
	char filename[] = "list.txt";
	loadList(List, filename);
	writeList(dumping_factor, stop_diff, List, ReverseIndex, page_pr);

	while(true){
		char *input;
		input = new char[100];
		cout << "Enter Word: ";
		cin.getline(input, 100);
		//cout << "input is " << input << endl;
		if(!strcmp(input, "*end*")) // the same, return 0
			break;

		vector<string> userInput;
		vector<string> ans_and;
		vector<string> ans_or;

		char *div = strtok(input, " ");
		while(div){
			userInput.push_back(div);
			div = strtok(NULL, " ");
		}
		delete div;

		if(userInput.size() > 1){
			ans_and = doAND(userInput, ReverseIndex);
			ans_or = doOR(userInput, page_pr, ReverseIndex);

			cout << "AND(";
			for(int i = 0; i < userInput.size(); i ++){
				cout << userInput.at(i);
				if(i != userInput.size() - 1)
					cout << " ";
			}
			cout << ") ";
			for(int i = 0; i < ans_and.size(); i ++){
				cout << ans_and.at(i) << " ";
				if(i == ans_and.size() - 1)
					cout << endl;
			}

			cout << "OR(";
			for(int i = 0; i < userInput.size(); i ++){
				cout << userInput.at(i);
				if(i != userInput.size() - 1)
					cout << " ";
			}
			cout << ") ";
			for(int i = 0; i < ans_or.size(); i ++){
				cout << ans_or.at(i) << " ";
				if(i == ans_or.size() - 1)
					cout << endl;
			}
		}else{
			//ans_or = ReverseIndex[userInput.at(0)];
			ans_or = doOR(userInput, page_pr, ReverseIndex);
			for(int i = 0; i < ans_or.size(); i ++){
				cout << ans_or.at(i) << " ";
				if(i == ans_or.size() - 1)
					cout << endl;
			}
		}

		delete [] input;

	}

	/*
	vector<string> test;
	vector<string> ans;
	test.push_back("and");
	test.push_back("Baker");
	test.push_back("his");
	//test.push_back("NASA");
	//test.push_back("pupu");
	ans = doAND(test, ReverseIndex);
	//ans = doOR(test, page_pr, ReverseIndex);
	cout << ans.size() << endl;
	for(int i = 0; i < ans.size(); i++){
		cout << ans.at(i) << endl;
	}
	*/

	return 0;

}

/*** Function  ***/
// I/O
void loadPage(vector<Page> &pageList, char* path, int page_count){
	char line[300]; // as buffer
	fstream fp;

	for(int i = 0; i < page_count; i ++){
		char file[30] = {}; // init
		char num[3];
		sprintf(num, "%d", i); // int to string
		strcat(file, path); // combine path
		strcat(file, num); // combine page num
		
		fp.open(file, ios::in);
		if(!fp){
			cout << "Error!" << endl;
		}else{
			// cout << "Loaded File: " << file << endl;
			
			bool link_end = false;
			while(fp.getline(line, sizeof(line), '\n')){
				// detect ---------------------
				if(!strcmp(line, "---------------------")){ // the same, return 0
					link_end = true;
					continue;
				}else{
					// last line, content of some words
					if(link_end){
						char *div = strtok(line, " ");
						while(div){
							pageList.at(i).words.push_back(div);
							div = strtok(NULL, " ");
						}
					// other pages pointing to this page
					}else{
						pageList.at(i).links.push_back(line);
					}
				}
			}
		}
		fp.close();
	}
}

void writePageRank(double d, double diff, vector<Page> sortedList){
	
	char dumping[10]; // double use 8 bytes in C++
	char difference[10];
	sprintf(dumping, "%lf", d);
	sprintf(difference, "%lf", diff);

	//cout << dumping << endl;
	//cout << difference << endl;
	
	int k = 0;
	for(k = 0; k < (sizeof(difference) / sizeof(char)); k ++){
		if(difference[k] == '1')
			break;
	}
	char a[3] = {dumping[2], dumping[3], '\0'};
	char b[k];
	for(int j = 0; j < k; j ++){
		if(j == k - 1){
			b[j] = '\0';
		}else{
			b[j] = difference[2 + j];
		}
	}
	//cout << k << endl;

	char filename[20] = "";
	strcat(filename, a);
	strcat(filename, "_");
	strcat(filename, b);
	strcat(filename, ".txt");
	
	//cout << filename << endl;

	fstream fp;

	fp.open(filename, ios::out);
	if(!fp){
		cout << "Error!" << endl;
	}else{

		for(int i = 0; i < sortedList.size(); i ++){
			Page cur_page = sortedList.at(i);
			fp << left << setw(10) << cur_page.page_name;
			fp << left << setw(10) << cur_page.out_links;
			fp << left << setw(8) << cur_page.pr_score << '\n';
		}

		fp.close();
	}
}

void writeReverseIndex(map< string, vector<string> > reverseIndex){
	
	char filename[] = "reverseindex.txt";
	fstream fp;

	fp.open(filename, ios::out);
	if(!fp){
		cout << "Error!" << endl;
	}else{

		for(map< string, vector<string> > ::const_iterator it = reverseIndex.begin(); it != reverseIndex.end(); it ++){
			fp << left << setw(15) << it->first;
			
			vector<string> cor_page = it->second;
			for(int i = 0; i < cor_page.size(); i ++){
				fp << cor_page.at(i) << " ";
				if((i == cor_page.size() - 1))
					fp << '\n';
			}
		}

		fp.close();
	}
}

void loadList(vector<string> &list, char* filename){
	
	char line[100]; // as buffer
	fstream fp;
	fp.open(filename, ios::in);
	if(!fp){
		cout << "Error!" << endl;
	}else{
		int i = 0;
		/*
		while(fp.getline(line, sizeof(line), '\n')){
			i++;
		}
		list.resize(i);
		*/

		int j = 0;
		while(fp.getline(line, sizeof(line), '\n')){
			//list.at(i).push_back(line);
			list.push_back(line);
			j ++;
		}
		fp.close();
	}
}

void writeList(double d, double diff, vector<string> Input, map< string, vector<string> > ReverseIndex, map<string, double> page_pr){
	
	char dumping[10]; // double use 8 bytes in C++
	char difference[10];
	sprintf(dumping, "%lf", d);
	sprintf(difference, "%lf", diff);

	int k = 0;
	for(k = 0; k < (sizeof(difference) / sizeof(char)); k ++){
		if(difference[k] == '1')
			break;
	}
	char a[3] = {dumping[2], dumping[3], '\0'};
	char b[k];
	for(int j = 0; j < k; j ++){
		if(j == k - 1){
			b[j] = '\0';
		}else{
			b[j] = difference[2 + j];
		}
	}

	char filename[20] = "result_";
	strcat(filename, a);
	strcat(filename, "_");
	strcat(filename, b);
	strcat(filename, ".txt");
	
	fstream fp;

	fp.open(filename, ios::out);
	if(!fp){
		cout << "Error!" << endl;
	}else{

		for(int p = 0; p < Input.size(); p ++){
			vector<string> userInput;
			vector<string> ans_and;
			vector<string> ans_or;
			
			char *input;
			input = new char[Input.at(p).length() + 1];
			strcpy(input, Input.at(p).c_str());

			char *div = strtok(input, " ");
			while(div){
				userInput.push_back(div);
				div = strtok(NULL, " ");
			}
			delete div;

			if(userInput.size() > 1){
				ans_and = doAND(userInput, ReverseIndex);
				ans_or = doOR(userInput, page_pr, ReverseIndex);

				fp << "AND ";
				for(int i = 0; i < ans_and.size(); i ++){
					fp << ans_and.at(i) << " ";
					if(i == ans_and.size() - 1)
						fp << endl;
				}

				fp << "OR ";
				for(int i = 0; i < ans_or.size(); i ++){
					fp << ans_or.at(i) << " ";
					if(i == ans_or.size() - 1)
						fp << endl;
				}
			}else{

				//ans_or = ReverseIndex[Input.at(0)];
				ans_or = doOR(userInput, page_pr, ReverseIndex);
				for(int i = 0; i < ans_or.size(); i ++){
					fp << ans_or.at(i) << " ";
					if(i == ans_or.size() - 1)
						fp << endl;
				}
			}
			delete [] input;
		}

		fp.close();
	}
}

// PageRank
vector<int> find_pointingPage(vector<string> links){

	vector<int> page_pointing;

	for(int i = 0; i < links.size(); i ++){
		// ***
		char *temp = new char[links.at(i).length() + 1];
		strcpy(temp, links.at(i).c_str());
		// divide token
		int page_id = atoi(strtok(temp, "page"));
		page_pointing.push_back(page_id);
		delete temp;
	}

	return page_pointing;

}

double page_rank(double dumping_factor, double *pr_score, vector<string> page_links, vector<Page> page_list, int page_count){
	
	double init_pr = (1 - dumping_factor) / page_count;
	double diff = 0;

	vector<int> pointingPage_num = find_pointingPage(page_links);
	double pr_before = *pr_score;
	*pr_score = 0;

	for(int j = 0; j < pointingPage_num.size(); j ++){
		int k = pointingPage_num.at(j);
	//	if(k == 500)
	//		return 87;
		*pr_score += page_list.at(k).pr_score / page_list.at(k).out_links;
	}

	*pr_score *= dumping_factor;
	*pr_score += init_pr;

	diff = abs(*pr_score - pr_before);
	return diff;

}

// ReverseIndex
map< string, vector<string> > createReverseIndex(map< string, vector<string> > reverseIndex,  vector<string> words, string page_num){

	// check the existence of the key
	for(int i = 0; i < words.size(); i ++){
		bool only = true;
		//exist, add to value. continue
		string cur_word = words.at(i);
		if(reverseIndex.find(cur_word) != reverseIndex.end()){
			//check the existence of the value
			map< string, vector<string> > :: iterator it = reverseIndex.find(cur_word);
			vector<string> eachWord_page = it->second;
			for(int j = 0; j < eachWord_page.size(); j ++){
				if(eachWord_page.at(j) == page_num){
					only = false;
					break;
				}
			}
			if(only)
				it->second.push_back(page_num);
		//not exist, new key & value
		}else{
			vector<string> value;
			value.push_back(page_num);
			reverseIndex.insert(pair< string, vector<string> >(cur_word, value));
		}
	}

	return reverseIndex;
}

// Search Engine
vector<string> doAND(vector<string> userInput, map< string, vector<string> > reverseIndex){
	
	int keyword_count = userInput.size();
	string *input;
	input = new string[keyword_count];
	for(int i = 0; i < keyword_count; i ++){
		input[i] = userInput.at(i);
	}

	bool no_result = false;
	vector< vector<string> > result;
	result.resize(keyword_count);
	for(int i = 0; i < keyword_count; i ++){
		string keyword = input[i];
		// if nothing is included, give "none". Then finish doAND
		if(reverseIndex.find(keyword) == reverseIndex.end()){
			//result.at(i).push_back("none");
			no_result = true;
			break;
		}
		result.at(i) = reverseIndex[keyword];
	}
	// no result control
	if(no_result){
		vector<string> noAND;
		noAND.push_back("none");
		return noAND;
	}

	delete [] input;

	// find the least size of the results
	int min_size = 0;
	int min_index = 0;
	for(int i = 0; i < (keyword_count - 1); i ++){
		if(result.at(i).size() < result.at(i + 1).size()){
			min_size = result.at(i).size();
			min_index = i;
		}else{
			min_size = result.at(i + 1).size();
			min_index = i + 1;
		}
	}

	// AND the results only with the least size
	int resultAND[min_size] = {0};
	vector<string> result_min = result.at(min_index);
	for(int i = 0; i < min_size; i ++){
		string temp = result_min.at(i);
		for(int j = 0; j < keyword_count; j ++){
			if(min_index == j){
				resultAND[i] ++;
				continue;
			}
			vector<string>::iterator it;
			vector<string> result_j = result.at(j);
			for(it = result_j.begin(); it != result_j.end(); it ++){
				if(*it == temp)
					resultAND[i] ++;
			}
		}
	}

	vector<string> returnPage;
	returnPage.reserve(min_size);
	int foundPage = 0;
	for(int i = 0; i < min_size; i ++){
		if(foundPage >= 10)
			break;
		if(resultAND[i] == keyword_count){
			returnPage.push_back(result_min.at(i));
			foundPage ++;
		}
	}

	return returnPage;
}

vector<string> doOR(vector<string> userInput, map< string, double> page_pr, map< string, vector<string> > reverseIndex){

	int keyword_count = userInput.size();
	string *input;
	input = new string[keyword_count];
	for(int i = 0; i < keyword_count; i ++){
		input[i] = userInput.at(i);
	}

	vector< vector<string> > result;
	result.resize(keyword_count);
	for(int i = 0; i < keyword_count; i ++){
		string keyword = input[i];
		// if nothing is included, give "none"
		if(reverseIndex.find(keyword) == reverseIndex.end()){
			result.at(i).push_back("none");
			continue;
		}
		result.at(i) = reverseIndex[keyword];
	}
	delete [] input;
	
	// find the largest size of the results
	int max_size = 0;
	int max_index = 0;
	for(int i = 0; i < (keyword_count - 1); i ++){
		if(result.at(i).size() > result.at(i + 1).size()){
			max_size = result.at(i).size();
			max_index = i;
		}else{
			max_size = result.at(i + 1).size();
			max_index = i + 1;
		}
	}

	// OR the results with the largest size, and repeatedly push_back others
	for(int j = 0; j < keyword_count; j ++){
		if(j == max_index)
			continue;
		vector<string>::iterator it;
		vector<string> result_j = result.at(j);
		for(it = result_j.begin(); it != result_j.end(); it ++){
			bool repeat = false;
			string temp = *it;
			// no result control
			if(temp == "none")
				break;
			// dynamically get size
			for(int i = 0; i < result.at(max_index).size(); i ++){
				if(*it == result.at(max_index).at(i)){
					repeat = true;
					break;
				}
			}
			if(repeat){
				continue;
			}else{
				// dinamically inset, chnaging the size meanwhile
				result.at(max_index).push_back(*it);
			}
		}
	}

	// after result_max argumented by OR
	vector<string> result_max = result.at(max_index);
	// no result control
	if(result_max.at(0) == "none"){
		vector<string> noOR;
		noOR.push_back("none");
		return noOR;
	}

	// HeapSort
	vector<ORPage> sorted_or;
	sorted_or.resize(result_max.size() + 1); // as heap
	//sorted_or.at(0) = NULL;
	for(int i = 1; i < sorted_or.size(); i++){
		sorted_or.at(i).page = result_max.at(i - 1);
		sorted_or.at(i).pr = page_pr[result_max.at(i - 1)];
	}
	// from 2, cause 1 must be the root
	for(int i = 2; i < sorted_or.size(); i ++){
		heapSort_insert(sorted_or, i);
	}
	heapSort_remove(sorted_or);

	int return_size = 0;
	if(result_max.size() < 10){
		return_size = result_max.size();
	}else{
		return_size = 10;
	}

	vector<string> returnPage;
	returnPage.reserve(return_size);

	for(int i = 1; i <= return_size; i ++){
		returnPage.push_back(sorted_or.at(i).page);
	}

	return returnPage;
}

// Sorting
vector<Page> insertionSort(vector<Page> pageList){
	
	vector<Page> sortedList;
	sortedList.reserve(page_count);

	sortedList.push_back(pageList.at(0));// insert 1 element to init
	for(int i = 1; i < pageList.size(); i ++){
		double insertPage_pr = pageList.at(i).pr_score;
		Page insertPage = pageList.at(i);
		for(int j = 0; j < sortedList.size(); j ++){
			double sortedPage_pr = sortedList.at(j).pr_score;

			if(insertPage_pr > sortedPage_pr){
				Page swap_temp = sortedList.at(j);
				sortedList.at(j) = insertPage;
				insertPage = swap_temp;

				insertPage_pr = insertPage.pr_score;
			}

		}
		sortedList.push_back(insertPage);
	}

	return sortedList;
}

void heapSort_insert(vector<ORPage> &orPage, int node){
	int parent = 0;
	// bottom-up
	while(parent != 1){
		// odd, right child
		if(node % 2){
			parent = (node - 1) / 2;
			double node_pr = orPage.at(node).pr;
			double parent_pr = orPage.at(parent).pr;
			// swap
			if(node_pr < parent_pr){
				ORPage temp = orPage.at(parent);
				orPage.at(parent) = orPage.at(node);
				orPage.at(node) = temp;
				node = parent;
			}else{
				break;
			}
		// even, left child
		}else{
			parent = node / 2;
			double node_pr = orPage.at(node).pr;
			double parent_pr = orPage.at(parent).pr;
			// swap
			if(node_pr < parent_pr){
				ORPage temp = orPage.at(parent);
				orPage.at(parent) = orPage.at(node);
				orPage.at(node) = temp;
				node = parent;
			}else{
				break;
			}
		}
	}
}

void heapSort_remove(vector<ORPage> &orPage){
	int heap_size = orPage.size() - 1;

	vector<ORPage>:: iterator it;
	// from index 1, to the last( end() - 1 )
	for(it = orPage.end() - 1; it != (orPage.begin() + 1); it --){
		
		// swap the root and the last leaf
		ORPage temp = orPage.at(1);
		orPage.at(1) = *it;
		*it = temp;
		heap_size --;

		int parent = 1;
		// make sure the path has no child with left leaf
		while((2 * parent <= heap_size)){
			int child = 2 * parent; //init
			// if there are 2 children, choose the less
			// whether exist the right child
			if((2 * parent + 1) <= heap_size){
				int child1 = 2 * parent;
				int child2 = 2 * parent + 1;
				if(orPage.at(child1).pr > orPage.at(child2).pr){
					child = child2;
				}
			}

			double parent_pr = orPage.at(parent).pr;
			double child_pr = orPage.at(child).pr;
			//swap
			if(parent_pr > child_pr){
				ORPage temp2 = orPage.at(parent);
				orPage.at(parent) = orPage.at(child);
				orPage.at(child) = temp2;
				// up-down
				parent = child;
			}else{
				break;
			}
		}
	}
}
