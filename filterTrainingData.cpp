//work in progress
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

// parses a line of data
void Parse_Data(string s, const vector<Attribute>&  attribute_list,
		vector<Instance>& example_list){
  Instance cur_instance;
  cur_instance.Set_Attributes(attribute_list);

  stringstream ss;
  ss.str(s);
  int cur_attribute = 0;
  string cur_val;
  while(ss >> cur_val){
    if(cur_val[cur_val.size()-1] == ',')
      cur_val = cur_val.substr(0, cur_val.size()-1); // trim off comma
    cur_instance.Add_Value(cur_attribute, cur_val);
    cur_attribute++;
  }
  if(cur_attribute != attribute_list.size()){
    cout << "ERROR: Wrong number of attributes on line: " << s << endl;
    exit(1);
  }
  example_list.push_back(cur_instance);
  
}

// gets a new attribute from an @attribute line
void New_Attribute(string s, vector<Attribute>& attribute_list){
  Attribute cur_attribute;
  stringstream ss;
  ss.str(s);
  string junk;
  ss >> junk; // remove "@attribute"
  string name;
  ss >> name;
  cur_attribute.Set_Name(name);
  string next; // either "{" or "numeric"
  ss >> next;
  if(next == "numeric")
    cur_attribute.Set_Numeric(true);
  else{
    cur_attribute.Set_Numeric(false);
    string temp;
    while(temp != "}"){
      ss >> temp;
      if(temp != "}"){
	if(temp[temp.size()-1] == ',')
	  temp = temp.substr(0, temp.size()-1); // trim off comma
	cur_attribute.Add_Category(temp);
      }
    }
  }
  attribute_list.push_back(cur_attribute);
}
     
void Print_Data(vector<Attribute>& attribute_list, vector<Instance>& examples){
  char choice = 'y';
  while(choice == 'y'){
    cout << "We have " << examples.size() << " examples.  Which would you like? ";
    int cur_example;
    cin >> cur_example;

    cout << "Type 0 to see the all attributes" << endl;
    for(int i = 0; i < attribute_list.size(); i++){
      cout << "Type " << i+1 << " to see value of attribute " << attribute_list[i].Get_Name() << endl;
    }
    int which_attribute;
    cin >> which_attribute;
    for(int i = 0; i < attribute_list.size(); i++){
      if(which_attribute == 0 || which_attribute == i+1){
	if(attribute_list[i].Numeric())
	  cout << attribute_list[i].Get_Name() << ": "
	       << examples[cur_example].Get_Numeric_Value(i) << endl;
	else
	  cout << attribute_list[i].Get_Name() << ": "
	       << examples[cur_example].Get_Nominal_Value(i) << endl;
      }
    }
    cout << "Another? (y/n) ";
    cin >> choice;
  }
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

/*
void Calc_Sum(vector<double>& a, vector<double>& p, vector<double>& c) {
	// Initialize all required summations
    double sum_area = 0.0;
    double sum_price = 0.0;
    double sum_area_squared = 0.0;
    double sum_area_times_price = 0.0;

    for (int i = 0; i < a.size(); ++i) {
        sum_area += a[i];
        sum_price += p[i];
        sum_area_squared += a[i] * a[i];
        sum_area_times_price += a[i] * p[i];
    }
	c.push_back(sum_area);
	c.push_back(sum_price);
	c.push_back(sum_area_squared);
	c.push_back(sum_area_times_price);
}
	
*/
int main(){
	string again = "y";
	while (again == "y") {
			
	  string filename;
	  cout << "Select Training Data to Use: " << endl << "Type 1 for house_price.arff" << endl << "Type 2 for house_price1D.arff" << endl << "Type 3 to enter your own filename." << endl << "Enter your choice: ";
	  cin >> filename;
	  
	  if(filename == "1") 
		  filename = "house_price.arff";
	  else if(filename == "2") 
		  filename = "house_price1D.arff";
	  else if(filename == "3") {
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
	  
	  vector<double> all_area;
	  vector<double> all_price;
	  
	  for(int i = 0; i < examples.size(); i++){
		all_area.push_back(examples[i].Get_Numeric_Value(0));
		all_price.push_back(examples[i].Get_Numeric_Value(attribute_list.size()-1));
	  }
	  
	  vector<double> all_sums;
	  //all_sums[0] = sum of all areas
	  //all_sums[1] = sum of all prices
	  //all_sums[2] = sum of all areas squared
	  //all_sums[3] = sum of all areas * prices
	  Calc_Sum(all_area,all_price,all_sums);
	  int N = examples.size();
	  cout <<"Sum of all Areas: "<< all_sums[0] << endl;
	  cout << "Sum of all Prices: "<< all_sums[1] << endl;
	  cout << "Sum of all Areas^2: "<< all_sums[2] << endl;
	  cout << "Sum of all Areas * Sum of all Prices: " << all_sums[3] << endl;
	  
	  double denominator = (all_sums[0] * all_sums[0]) - (N * all_sums[2]);
	  
	  double w0 = ((all_sums[0] * all_sums[3]) - (all_sums[2] * all_sums[1])) / denominator;
	  double w1 = ((all_sums[0] * all_sums[1]) - (N * all_sums[3])) / denominator;
	  
	  cout << "w0: " << w0 << endl;
	  cout << "w1: " << w1 << endl;
	  
	  double RSS = 0.0;
	  for (int i = 0; i < N; i++) {
		double y_prediction = (w0 + (w1 * all_area[i]));
		RSS += pow(all_price[i] - y_prediction, 2);
	  }
	  cout << "RSS: " << RSS << endl;
	  
	  
	  
	  
	  MatrixXd X(N, attribute_list.size());
	  MatrixXd y(N, 1);

	  for (int i = 0; i < examples.size(); ++i) {
			X(i, 0) = 1.0;
			for (int j = 0; j < attribute_list.size()-1; ++j) {
				X(i, j + 1) = examples[i].Get_Numeric_Value(j);
			}
			y(i, 0) = examples[i].Get_Numeric_Value(attribute_list.size()-1);
		}
	  //cout << "X: " << X << endl;
	  //cout << "y: " << y << endl;
	  
	  MatrixXd w = ((X.transpose() * X).inverse() * X.transpose() * y);
	  
	  cout << endl << "Matrix RSS: " << endl;
	  cout << "Computed Weights:" << endl;
	  //output all w
	  for(int i =0; i < attribute_list.size();i++) {
		cout << "w" << i << ": "<< w(i,0) << endl;
	  }
	  
	  MatrixXd y_prediction_matrix = X * w;
	  MatrixXd residuals = y_prediction_matrix - y;
	  
	  /*double RSS2 = 0.0;
	  for (int i =0; i < attribute_list.size(); i++) {
		  RSS2 += pow(residuals(i,0), 2);
	  }*/  
	  
	  double RSS2 = (residuals.array().square()).sum();
	  
	  cout << "RSS: " << RSS2 << endl;
	  cout << "Again? (y or n)" << endl;
	  cin>> again;
	}
	
  return 0;
}