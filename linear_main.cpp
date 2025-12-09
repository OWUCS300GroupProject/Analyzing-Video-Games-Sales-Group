//hw6.cpp
#include <iostream>
#include <Eigen/Dense>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include "attribute.h"
#include "instance.h"

using namespace std;
using namespace Eigen;

//This file takes in a new arff file that contains critic and user scores for a game. It uses those scores to predict the game sales 

// parses a line of data
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

void Read_File(string& filename, vector<Attribute>& attribute_list,
  vector<Instance>& examples) {
  ifstream fin;
  fin.open(filename.data());
  while(!fin){
    cout << "File not found" << endl;
    cout << "Enter the filename: ";
    cin >> filename;
    fin.clear();
    fin.open(filename.data());
  }
  
  // file is open
  bool data_mode = false;
  string s;
  int num_read = 0;
  while(getline(fin, s)){
    if(s[s.size()-1] == '\r') // grrr
      s = s.substr(0,s.size()-1);
    
    if(s.size() > 0 && s[0] != '%'){ // ignore comments
      if(data_mode){ // it's a line of data
	Parse_Data(s, attribute_list, examples);
      }
      else{
	// then it had better start with an '@'
	if(s[0] != '@'){
	  cout << "ERROR: ILLEGAL LINE: " << s << endl;
	  exit(1);
	}
	// is it @attribute?
	else if(s.substr(0,10) == "@attribute")
	  New_Attribute(s, attribute_list);
	//is it @data?
	else if(s.substr(0,5) == "@data")
	  data_mode = true;
	else{
	  cout << "ERROR: ILLEGAL LINE: " << s << endl;
	  exit(1);
	}
      }
    }
    num_read++;
    cout << num_read << endl;
  }
  fin.close();
}
	

int main(){
	string again = "y";
	while (again == "y") {
			
	  string filename;
	  cout << "Select Training Data to Use: " << endl << "Type 1 for train.arff" << endl << "Type 2 for test.arff" << endl << "Type 3 for output.arff" << endl << "Type 4 to enter your own filename." << endl << "Enter your choice: ";
	  cin >> filename;
	  
	  if(filename == "1") 
		  filename = "train.arff";
	  else if(filename == "2") 
		  filename = "test.arff";
	  else if(filename == "3") 
		  filename = "output.arff";
	  else if(filename == "4") {
		  cout << "Enter the filename: ";
		  cin >> filename;
	  }
	  else{
		  cout << "File not found." << endl;
	  }
	  
	  vector<Attribute> attribute_list;
	  vector<Instance> examples;
	  
	  Read_File(filename, attribute_list, examples);
	  
	  //END OF FILE
	  
	  
	  
	  cout <<"Instances: "<< examples.size() << endl;
	  cout <<"Attribute: "<< attribute_list.size() << endl;
	  
	  int critic_idx = 10;
	int user_idx = 12;
	int global_idx = 9;
	  
	  vector<double> critic_values;
	  vector<double> user_values;
	  vector<double> global_values;
	  
	  for(int i = 0; i < examples.size(); i++){
		critic_values.push_back(examples[i].Get_Numeric_Value(critic_idx));
		user_values.push_back(examples[i].Get_Numeric_Value(user_idx));
		global_values.push_back(examples[i].Get_Numeric_Value(global_idx));
	  }
	  
	  int N = examples.size();
	  
	  MatrixXd X(N, 3);
	  MatrixXd y(N, 1);

	  for (int i = 0; i < N; ++i) {
			X(i, 0) = 1.0; // bias term
			X(i, 1) = critic_values[i];
			X(i, 2) = user_values[i];
			y(i, 0) = global_values[i];
		}
	  //cout << "X: " << X << endl;
	  //cout << "y: " << y << endl;
	  
	  MatrixXd w = ((X.transpose() * X).inverse() * X.transpose() * y);
	  
	  cout << endl << "Matrix RSS: " << endl;
	  cout << "Weights:" << endl;
	  cout << "w0 (bias): " << w(0,0) << endl;
	  cout << "w1 (Critic_Score): " << w(1,0) << endl;
	  cout << "w2 (User_Score): " << w(2,0) << endl;
	  
	  MatrixXd y_prediction_matrix = X * w;
	  MatrixXd residuals = y - y_prediction_matrix;
	  
	  /*double RSS2 = 0.0;
	  for (int i =0; i < attribute_list.size(); i++) {
		  RSS2 += pow(residuals(i,0), 2);
	  }*/  
	  
	  double RSS = (residuals.array().square()).sum();
	  
	  cout << "RSS: " << RSS << endl;
	  
	  
	  
	  int choose;
	  cout <<endl << "1: Run Test data" << endl << "2: Predict new instance" << endl << "0: Quit: ";
	  cin>> choose;
	  
	  
	  if(choose == 1) {
		vector<Attribute> attribute_list_test;
		vector<Instance> test_examples;
		filename = "test.arff";
		Read_File(filename, attribute_list_test, test_examples);
		
		
		double test_RSS = 0.0;

		for(int i = 0; i < test_examples.size(); i++){
			double critic_val = test_examples[i].Get_Numeric_Value(critic_idx);
			double user_val = test_examples[i].Get_Numeric_Value(user_idx);
			double actual = test_examples[i].Get_Numeric_Value(global_idx);

			double predicted_global = w(0,0) + w(1,0) * critic_val + w(2,0) * user_val;

			// Print the prediction and actual value
			cout << "Instance " << i << ":" << endl
				 << "Predicted Global Sales = " << predicted_global << endl
				 << "Actual Global Sales = " << actual << endl << endl;

			// Update RSS
			test_RSS += pow(actual - predicted_global, 2);
		}

		// After the loop, print the total RSS
		cout << "Test RSS: " << test_RSS << endl;
	  }
	  else if (choose == 2) {
		  
	  }
	  else{}
	  cout << "Again? (y or n)" << endl;
	  cin>> again;
	}
	
  return 0;
}
