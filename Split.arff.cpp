#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <sstream>
#include "instance.h"
#include "attribute.h"

using namespace std;

void New_Attribute(const string& s, vector<Attribute>& att){
	stringstream ss(s);
	string at_sign, att_name;
	
	ss >> at_sign; // should be "@attribute"
	
	if(at_sign != "@attribute"){
		cout << "Error: Line does not start with @attribute.\n";
		return;
	}
	
	ss >> att_name; // the name of the attribute
	
	if(att_name.empty()){
		cout << "Error: Attribute name is missing.\n";
		return;
	}
	
	string rest;
	getline(ss, rest);
	
	rest.erase(0,rest.find_first_not_of(" \t"));
	
	Attribute attr;
	attr.Set_Name(att_name);
	
	//Case 1: Numeric
	if(rest == "numeric"){
		attr.Set_Numeric(true);
		att.push_back(attr);
		return;
	}	
	
	//Case 2: Nominal
	if(rest.front() == '{' && rest.back() == '}'){
		attr.Set_Numeric(false);
		
		//Remove {}
		rest = rest.substr(1, rest.size() - 2);
		
		stringstream cat_stream(rest);
		string category;
		while(getline(cat_stream, category, ',')){
			//Trim each category
			category.erase(0, category.find_first_not_of(" \t"));
			category.erase(category.find_last_not_of(" \t") + 1);
			
			if(!category.empty()){
				attr.Add_Category(category);
			}
		}
		att.push_back(attr);
		return;
	}
	if(rest == "string"){
		attr.Set_Numeric(false);
		attr.Set_Open_Domain(true);
		att.push_back(attr);
		return;
	}
	//If here, its not valid
	cout << "Error: Malformed nominal attribute definition.\n";
}

void Parse_Data(const string& s, const vector<Attribute>& attribute_list, vector<Instance>& instance_list){
	stringstream ss(s);
	string value;
	vector<string> values;
	
	//Split by commas
	while(getline(ss, value, ',')){
		//Trim whitespace
		value.erase(0, value.find_first_not_of(" \t"));
		value.erase(value.find_last_not_of(" \t") + 1);
		
		values.push_back(value);
	}
	
	//Check number of values and match with attributes
	if(values.size() != attribute_list.size()){
		cout << "Error: Data line has " << values.size() 
		<< " values, but " << attribute_list.size() << " attributes expected.\n";
		return;
	}
	
	//Create a new instance and set the attribute to it
	Instance inst;
	inst.Set_Attributes(attribute_list);
	
	//Add each instances value
	for(int i = 0; i < values.size(); i++){
		if(!inst.Add_Value(i, values[i])){
			cout << "Invalid value at attribute #" << i << ": " << values[i] << endl;
			return; //Dont use this instance
		}
	}
	
	//Add the complete instance
	instance_list.push_back(inst);
}

void Write_ARFF(const string& filename,
				const vector<Attribute>& attributes,
				const vector<Instance>& instances){
	ofstream fout(filename);
	if(!fout.is_open()){
		cout << "Error opening file for writing: " << filename << endl;
		return;
	}				
	
	fout << "@relation converted_csv\n";
	
	for(const auto& att : attributes){
    	fout << "@attribute " << att.Get_Name() << " ";
    	if(att.Numeric()) fout << "numeric\n";
    	else if(att.Is_Open_Domain()) fout << "string\n";
    	else {
        	fout << "{";
        	const auto& cats = att.Get_Category_List();
        	for(size_t i = 0; i < cats.size(); ++i){
            	fout << cats[i];
            	if(i != cats.size() - 1) fout << ",";
        	}
        	fout << "}\n";
    	}
	}

	// Write data
	fout << "\n@data\n";
	for(const auto& inst : instances){
    	int n = inst.Get_Num_Attributes();
    	for(int i = 0; i < n; i++){
    		if(inst.Is_Numeric_Attribute(i)){
    			fout << inst.Get_Numeric_Value(i);
    		}
    		else{
    			fout << inst.Get_Nominal_Value(i);
    		}
    		if(i != n -1){
    			fout << ",";
    		}
    	}
    	fout << "\n";
    }
	fout.close();
}

int main(){
	string inputFile;
	cout << "Enter ARFF filename: ";
	cin >> inputFile;
	
	ifstream fin(inputFile);
	while(!fin.is_open()){
		cout << "Error opening file.\n";
		fin.clear();
		cout << "Try again: ";
		cin >> inputFile;
		fin.open(inputFile);
	}
	
	vector<Attribute> attributes;
	vector<Instance> instances;
	
	bool data_mode = false;
	string s;
	while(getline(fin, s)){
		s.erase(0, s.find_first_not_of(" \t"));
		s.erase(s.find_last_not_of(" \t") + 1);
		if(s.size() > 0 && s[0] != '%'){ // ignore line if empty or comment
			if(data_mode){ // we're hopefully reading a line of data
				if(s[0] == '@'){
					cout << "Error: Getting a command instead of data" << endl;
				}
				else{
					Parse_Data(s, attributes, instances); // we will have to rewrite this later.
				}
			}
			else{	// not in data mode yet
				if(s == "@data"){
					data_mode = true;
				}
				else if(s.substr(0, 9) == "@relation"){
					continue;
				}
				else if(s.substr(0, 10) == "@attribute"){
					New_Attribute(s, attributes); // we'll have to write this later.
				}
				else{
					cout << "Error: None of your data works (Check your arff file)" << endl;
				} 
			} // End else
		} // end "if I shouldn't skip this line"
	} // end while
	
	random_device rd;
	mt19937 g(rd());
	vector<Instance> shuffled_instances = instances;
	shuffle(shuffled_instances.begin(), shuffled_instances.end(), g);
	
	size_t split_index = (2 * shuffled_instances.size()) / 3;
	vector<Instance> train(shuffled_instances.begin(), shuffled_instances.begin() + split_index);
	vector<Instance> test(shuffled_instances.begin() + split_index, shuffled_instances.end());
	
	Write_ARFF("train.arff", attributes, train);
	Write_ARFF("test.arff", attributes, test);
	
	cout << " ARFF split complete: train.arff and  test.arff\n";
	
	return 0;
}