#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main() {
	vector<ofstream> file_vec;
	for(int i = 0; i < 4; i++) {
		ofstream tmp_file;
		string file_name = "hello";
		file_name = file_name + to_string(i);
		tmp_file.open(file_name);
		file_vec.push_back(tmp_file);
	}
/*
	for(int i = 0; i < 4; i++) {
		file_vec[i].close();
	}	
*/
	return 0;
}
