#include <iostream>
#include <string>
#include <vector>
#include "attribute.h"
#include "instance.h"

using namespace std;

//Pre: platform, genre, and publisher are empty string that need to be established, or already filled.
//examples is the training dataset of instances, attribute_list is a vector of attributes
//Post: This function allows users to filter the dataset based on platform, genre, and publisher. Returns a vector of instances which hold
//the desired filters. This is used to train the model based on related material, rather than the entire training dataset. 
//Note: This can be changed to return a vector of ints to reduce memory.
vector<Instance> Filter(string& platform, string& genre, string& publisher, vector<Instance>& examples, vector<Attribute>& attribute_list) {
	if(platform == "") { 
		cout << "Enter the Platform (Wii, PS4, etc): ";
		getline(cin, platform);
		if(attribute_list[2].In_Domain(platform)) {
			cout << "Platform is invalid";
		}
	}
	if(genre == ""){
		cout << "Enter the Genre (Sports, Platformer, etc): ";
		getline(cin, genre);
		if(attribute_list[4].In_Domain(genre)) {
			cout << "Genre is invalid";
		}
	}
	if(publisher == ""){
		cout << "Enter the Publisher (Nintendo, SquareSoft, etc): ";
		getline(cin, publisher);
		if(attribute_list[5].In_Domain(publisher)) {
			cout << "Publisher is invalid";
		}
	}
	int size = examples.size();
	vector<Instance> filter;
	for(int i=0; i < size; i++) {
		if(platform == examples[i].Get_Nominal_Value(2) || genre == examples[i].Get_Nominal_Value(4) || platform == examples[i].Get_Nominal_Value(5)) {
			filter.push_back(example[i]);
		}
	}
	return filter;
}


