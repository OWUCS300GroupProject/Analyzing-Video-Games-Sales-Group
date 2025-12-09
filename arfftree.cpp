//arfftree.cpp
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath> 
#include <cstdlib>
#include "attribute.h"
#include "instance.h"
#include "treenode.h"

//reads training  and test arff file
//builds a decision tree using info gain


using namespace std;
//////////////////////////////////////////////////////////////////////////////////
//Parse_Data: a helper func to parse one line of data into an Instance
void Parse_Data(string s, const vector<Attribute>&  attribute_list,
		vector<Instance>& example_list){
  Instance cur_instance;
  cur_instance.Set_Attributes(attribute_list);

  stringstream ss;
  ss.str(s);
  int cur_attribute = 0;
  string cur_val;
  
  //reads values seperated by commas
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
//////////////////////////////////////////////////////////////////////////////////
// gets a new attribute from an @attribute line
void New_Attribute(string s, vector<Attribute>& attribute_list){
  Attribute cur_attribute;
  stringstream ss;
  ss.str(s);
  
  string junk;
  ss >> junk; //remove "@attribute"
  
  string name;
  ss >> name;
  cur_attribute.Set_Name(name);
  
  string next; //either "{" or "numeric"
  ss >> next;
  
  if(next == "numeric")
    cur_attribute.Set_Numeric(true);
  else{
	  //nominal attribute read categories between { and }
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

// returns true if all have the same value for
// "check_attribute"
bool Examples_Agree(const vector<Instance>& examples, int check_attribute){
  if(examples.size() == 0)
    return true;
  string value = examples[0].Get_Nominal_Value(check_attribute);
  for(int i = 1; i < examples.size(); i++){
    if(examples[i].Get_Nominal_Value(check_attribute) != value)
      return false;
  }
  return true;
}
///////////////////////////////////////////////////////////////////////////
//write an entropy function 
//pre: param is a vector of probabilities for each class
//returns: entropy
double Entropy(vector<double> param){
	double result = 0.0; //starting the total entropy number at 0
	
	//looping through every possibility
	for(int i = 0; i < param.size(); i++){
		double a = param[i]; //this would get the current probability
	
		//if the probability is greater than 0, we use it
		//part c.if a = 0, skip so we dont do log2(0) which would give nan?
		if(a > 0){
			result -= a * log2(a); //add the term -a*log2(a)to the entropy
		}
	}
	
	return result; //return the computed entropy value
}
/////////////////////////////////////////////////////////////////////////
//Info_Node: entropy of the class labels in "examples"
//uses the last attribute as the class label
double Info_Node(const vector<Attribute>& attribute_list, const vector<Instance>& examples){
    int class_index = attribute_list.size() - 1; // last attribute is classification
    vector<string> class_values = attribute_list[class_index].Get_Category_List();

    //count occurrences of each class
    vector<double> counts(class_values.size(), 0);
    for(int i = 0; i < examples.size(); i++){
        string val = examples[i].Get_Nominal_Value(class_index);
        for(int j = 0; j < class_values.size(); j++){
            if(class_values[j] == val){
                counts[j]++;
            }
        }
    }

    //converting the counts into probabilities
    vector<double> probs;
    double total = examples.size();
	
    for(int i = 0; i < counts.size(); i++){
        if(total > 0){
			probs.push_back(counts[i] / total);//add probability for this category
		}
		else{
			probs.push_back(0.0);// if there are no examples found, probability is 0
	}

    }

    //call entropy based on the probs stored in probs 
    return Entropy(probs);
}
/////////////////////////////////////////////////////////////////////////////
//Info_Attribute: expected entropy if we split this node on
//this is the weighted average of child entropies 
double Info_Attribute(const vector<Attribute>& attribute_list, const vector<Instance>& examples, int cur_attribute) {

    vector<string> categories = attribute_list[cur_attribute].Get_Category_List();
   
   vector<double> child_info(categories.size()); //info for each child
    vector<int> child_counts(categories.size(), 0);//number of examples per child

    //for loop counting examples per category
    for(int i = 0; i < examples.size(); i++){
		//get the val of the cur attribute in the example
        string val = examples[i].Get_Nominal_Value(cur_attribute);

		for (int j = 0; j < categories.size(); j++){
			if (categories[j] == val){
			child_counts[j]++;
            }
        }
    }

    //compute info for each child
    for(int i = 0; i < categories.size(); i++){
        vector<Instance> subset;
		
        for(int j = 0; j < examples.size(); j++){
            if(examples[j].Get_Nominal_Value(cur_attribute) == categories[i]){
                subset.push_back(examples[j]);
            }
        }
        child_info[i] = Info_Node(attribute_list, subset); //info of child
    }

    //weigthed average of child entropies
    double total_examples = examples.size();
    double weighted_info = 0.0;
	
    for(int i = 0; i < categories.size(); i++){
        if(total_examples > 0){
			//fraction of examples in the category are multiplied by the info in the category
			weighted_info += (child_counts[i] / total_examples) * child_info[i];
		}
		else{
			weighted_info += 0.0;  // just add 0 if no examples
		}
	}

    return weighted_info;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//Choose_Node_Attribute: pick the best a
int Choose_Node_Attribute(const vector<Attribute>& attribute_list, const vector<Instance>& examples,const vector<bool>& chosen_attributes){

    double node_info = Info_Node(attribute_list, examples);//entropy before split
    int best_attr = -1;
    double max_gain = -1.0;

    // loop through unchosen attributes (skip last attribute, which is class)
    for(int i = 0; i < attribute_list.size() - 1; i++) { //skip the class attribtute
        if(!chosen_attributes[i]){ //if the attribute is unused
            double attr_info = Info_Attribute(attribute_list, examples, i);//info if split by this attribute
            double gain = node_info - attr_info; //compute the info gain

            if(gain > max_gain){//checking if gain is now bigger than max gain
                max_gain = gain; //if so update max gain
                best_attr = i;//then update the best attribute
            }
        }
    }

    //if no attribute gives positive gain, we don't split it 
    if(max_gain <= 0) 
		return -1;
	else
		return best_attr; //return attribute with the max gain
}

////////////////////////////////////////////////////////////////////////
//foward declaration for helper
string Get_Most_Popular(const vector<Instance>& examples,
                        const vector<Attribute>& attribute_list,
                        int chosen);

//////////////////////////////////////////////////////////////////////////////
//Build_Tree: recursively build the decision tree
TreeNode* Build_Tree(const vector<Attribute>& attribute_list, const vector<Instance>& examples, vector<bool>& chosen_attributes, string parent_choice = "nothing"){
 
  TreeNode* root = new TreeNode; // This is the root of this subtree
  
  int classification_attribute = attribute_list.size() - 1;
 
 //Base Case 1: all examples have the same class -> make a leaf
 if(Examples_Agree(examples, classification_attribute)){ 
    root->Set_Leaf(true);
	
	// if no instances gets to this node, label it by the parent's choice
    if(examples.size() == 0)
      root->Set_Info(parent_choice);
      root->Set_Info(examples[0].Get_Nominal_Value(classification_attribute));
    return root;
  }
  else{
    root->Set_Leaf(false);
	
    // pick an attribute for this node to split on
    int node_attribute = Choose_Node_Attribute(attribute_list, examples, chosen_attributes);

    // did we get an error (-1 means we couldn't choose an attribute)
    if(node_attribute == -1){
      cout << "ERROR: At a node with conflicting data and no attributes remaining" << endl;
      // for now, just make it a leaf with an arbitrary conclusion
      root->Set_Leaf(true);
      root->Set_Info(examples[0].Get_Nominal_Value(classification_attribute));
      return root;
    }
    parent_choice = Get_Most_Popular(examples, attribute_list, classification_attribute);
    chosen_attributes[node_attribute] = true;
    // we have an attribute for the node, so set up the node to handle it
//     cout << "We decide to split on attribute: " << attribute_list[node_attribute].Get_Name() << endl;
    root->Set_Info(attribute_list[node_attribute].Get_Name());
    root->Set_Num_Children(attribute_list[node_attribute].Num_Categories());
    for(int i = 0; i < attribute_list[node_attribute].Num_Categories(); i++){
      // build the child for this category
      TreeNode* child;
      string category_name = attribute_list[node_attribute].Get_Category(i);
      root->Set_Child_Name(i, category_name);
      // find examples that match.  This is super inefficient.
      vector<Instance> child_examples;
      for(int j = 0; j < examples.size(); j++){
				if(examples[j].Get_Nominal_Value(node_attribute) ==
	   			category_name){
	  			child_examples.push_back(examples[j]);
				}
      }
      // we need a _copy_ of the boolean vector so that the subtrees don't
      // mark attributes we haven't seen as chosen
      vector<bool> child_chosen_attributes = chosen_attributes;
      child = Build_Tree(attribute_list, child_examples, child_chosen_attributes, parent_choice);
      root->Set_Child(i, child);
    }
  }
  return root;
}
////////////////////////////////////////////////////////////////////////
// Print_Tree: recursive tree printer
//prints whether a node is leaf or internal and its children
void Print_Tree(TreeNode* root){
  if(root->Is_Leaf()){
    cout << "This node is a leaf, with value " << root->Get_Info() << endl << endl;
  
  }			    			      
  else{
    cout << "This is an internal node, looking at attribute: " << root->Get_Info() << endl;
    for(int i = 0; i < root->Get_Num_Children(); i++){
      cout << "Child " << i << ",  Value: " << root->Get_Child_Name(i) << endl;
      Print_Tree(root->Get_Child(i));
      cout << endl;
    }
    cout << "End of node for attribute: " << root->Get_Info() << endl << endl;
  }
}

/////////////////////////////////////////////////////////////////////////////////
//Traverse_Tree: classify a single example by walking down the tree
//returns the predicted class as a string 
	string Traverse_Tree(TreeNode* root, const Instance& example, const vector<Attribute>& attribute_list){
	
	if(root == 0) 
		return "unkown";
	//leaf -> return its class label
	if(root-> Is_Leaf()){
		//if it is, the info string is the classification of the ezample, so output/return that
		return root-> Get_Info();
	}

	//if were not in the base case, the info string of the node is 
	//the name of the attribute the node cares about (like "outlook")
	string attr_name = root->Get_Info();
	
  // Figure out what that name is (with Get_Info)
  int attr_index = -1;
  for (int i = 0; i < attribute_list.size(); i++){
    if (attribute_list[i].Get_Name() == attr_name){
      attr_index = i;
	  break;
    }
  }

  if(attr_index == -1){
    // attribute couldnt be found in list
    return "unknown";
  }

  // Get the nominal value from the example instance we are looking at (say "sunny")
  string example_val = example.Get_Nominal_Value(attr_index);

  // This (nominal) value is the name of the branch we care about.
  // But we want its corresponding index to know which branch we are going to move into.
  int child_index = -1;
  for(int j = 0; j < root->Get_Num_Children(); j++){
    if (root->Get_Child_Name(j) == example_val){
      child_index = j;
	  break;
    }
  }

  // Make a recursive call on this child of the node
  if(child_index != -1){
	  return "unknown";
  }
  return Traverse_Tree(root->Get_Child(child_index), example, attribute_list);

}

////////////////////////////////////////////////////////////////////
//Count_Nodes: recursive function that returns the number of nodes in the tree
int Count_Nodes(TreeNode* root){
	if(root == 0){ //so if the root is empty, return 0
		return 0;
	}
	
	if(root ->Is_Leaf()){
		return 1;
	}
	
	int total = 1; //count this node 
	for(int i = 0; i < root->Get_Num_Children(); i++){
		total += Count_Nodes(root->Get_Child(i));
	}
	return total;
}
///////////////////////////////////////////////////////////////////////////
//Get_Most_Popular: returns the most common class value in examples
//returns the most popular value in "examples" for attribute "chosen"
string Get_Most_Popular(const vector<Instance>& examples, const vector<Attribute>& attribute_list, int chosen){
  
  vector<string> Category_Names = attribute_list[chosen].Get_Category_List();
  
  vector<int> counts(Category_Names.size(), 0);
  
  //count how many examples of each class 
  for(int i = 0; i < examples.size(); i++){
    string cur_val = examples[i].Get_Nominal_Value(chosen);
    for(int j = 0; j < Category_Names.size(); j++){
      if(Category_Names[j] == cur_val)
				counts[j]++;
	}
  }
  int max = 0;
  int best_pos = -1;
  //find index of most frequent class 
  for(int i = 0; i < counts.size(); i++){
    if(counts[i] > max){
      max = counts[i];
      best_pos = i;
    }
  }
  return Category_Names[best_pos];
}


		   
int main(){
	
	
	cout << "Enter the training filename: " << endl;
	string train_filename;
	cin >> train_filename;
	
  ifstream fin;
  fin.open(train_filename.c_str());
  
  while(!fin){
    cout << "File not found" << endl;
    cout << "Enter the  training filename: ";
    cin >> train_filename;
    fin.clear();
    fin.open(train_filename.data());
  }
  
  vector<Attribute> attribute_list;
  vector<Instance> training_examples;
  
  // file is open
  bool data_mode = false;
  string s;
  int num_read = 0;
  
  while(getline(fin, s)){
    if(s[s.size()-1] == '\r') // grrr
      s = s.substr(0,s.size()-1);
    
    if(s.size() > 0 && s[0] != '%'){ // ignore comments
      if(data_mode){ // it's a line of data
	Parse_Data(s, attribute_list, training_examples);
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
   fin.close();
  }
  
  //build the decision tree
  vector<bool> chosen_attributes(attribute_list.size(), false);

  TreeNode* root = Build_Tree(attribute_list, training_examples, chosen_attributes);

  Print_Tree(root);
	cout << "The tree has " << Count_Nodes(root) << " nodes " << endl;
	
//hw d.) asking for the test files
cout << "Enter the test filename: " << endl;
string test_filename;
cin >> test_filename;

ifstream fin_test;
  fin_test.open(test_filename.data());
  
  while(!fin_test){
    cout << "File not found" << endl;
    cout << "Enter the test filename: ";
    cin >> test_filename;
    fin_test.clear();
    fin_test.open(test_filename.data());
  }

  vector<Instance> test_examples;
  data_mode = false;
  
  while(getline(fin_test, s)){
    if(s[s.size()-1] == '\r')
      s = s.substr(0,s.size()-1);
    if(s.size() > 0 && s[0] != '%'){
      if(data_mode){
        Parse_Data(s, attribute_list, test_examples);
      }
      else{
        if(s.substr(0,5) == "@data")
          data_mode = true;
      }
    }
  }
  fin_test.close();
 
  //training error
  int wrong_train = 0;
  int class_index = attribute_list.size() - 1;
  
  for(int i = 0; i < training_examples.size(); i++){
    string predicted = Traverse_Tree(root, training_examples[i], attribute_list);
    string actual = training_examples[i].Get_Nominal_Value(attribute_list.size() - 1);
    if(predicted != actual) wrong_train++;
  }
  double train_error = (double)wrong_train / training_examples.size();
  cout << "Training error rate: " << train_error << endl;

  //test error
  int wrong_test = 0;
  
  for(int i = 0; i < test_examples.size(); i++){
    string predicted = Traverse_Tree(root, test_examples[i], attribute_list);
    string actual = test_examples[i].Get_Nominal_Value(attribute_list.size() - 1);
    
	if(predicted != actual) 
		wrong_test++;
  }
  
  
  double test_error = wrong_test / test_examples.size();
  
  cout << "Test error rate: " << test_error << endl;

  return 0;
}