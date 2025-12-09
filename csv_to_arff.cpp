#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

vector<string> splitCSV(const string& line) {
    vector<string> result;
    string cur;
    bool in_quotes = false;

    for (char c : line) {
        if (c == '"') {
            in_quotes = !in_quotes; // toggle quotes
        } else if (c == ',' && !in_quotes) {
            result.push_back(cur);
            cur = "";
        } else {
            cur += c;
        }
    }
    result.push_back(cur); // last value
    return result;
}

bool isNumeric(const string& s){
	if(s.empty()){
		return false;
	}
	for(char c : s){
		if(!isdigit(c) && c != '.' && c != '-'){
			return false;
		}
	}
	return true;
}

int main(){
	string file_name;
	cout << "Enter CSV file you're trying to convert: ";
	cin >> file_name;
	
	ifstream csv(file_name);
	ofstream arff("output.arff");
	
	while(!csv.is_open()){
		cout << "Failed to open CSV file!\n";
		csv.clear();
		cout << "Try again: ";
		cin >> file_name;
		csv.open(file_name);
	}
	
	string line;
	
	getline(csv, line);
	vector<string> headers = splitCSV(line);
	
	arff << "@relation converted_csv\n";
	for (const auto& h : headers) {
        string trimmed = h;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
        arff << "@attribute " << trimmed << " string\n";
	}
	
	arff << "\n@data\n";
	
	while(getline(csv, line)){
		vector<string> row = splitCSV(line);
        for (size_t i = 0; i < row.size(); ++i) {
            string val = row[i];
            val.erase(0, val.find_first_not_of(" \t"));
            val.erase(val.find_last_not_of(" \t") + 1);
            arff << val;
            if (i != row.size() - 1){
            	arff << ",";
            }
        }
        arff << "\n";
    }

    csv.close();
    arff.close();

    cout << "Conversion complete! Check output.arff\n";
    return 0;
}