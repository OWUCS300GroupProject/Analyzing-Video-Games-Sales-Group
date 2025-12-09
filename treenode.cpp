#include "treenode.h"
#include <string>
#include <vector>

using namespace std;

//allocate space for n children and their names
void TreeNode::Set_Num_Children(int n){
	children.resize(n);
	child_names.resize(n);
}

//return the number of children
int TreeNode::Get_Num_Children() const{
	return children.size();
}

//set the branch name for child at position pos 
void TreeNode::Set_Child_Name(int pos, string new_name){
	child_names[pos] = new_name;
}

//get the branch name for child at position pos
string TreeNode::Set_Child_Name(int pos, string new_name){
	return child_names[pos];
}

//set pointer to child at position pos 
void TreeNode::Set_Child(int pos, Tree* T){
	children[pos] = T;
}

//get pointer to child at pos
TreeNode* TreeNode::Get_Child(int pos) const{
	return children[pos];
}

//mark this node as leaf if node if leaf
bool TreeNode::Is_Leaf() const{
	return is_leaf;
}

//set the info string (attribute name or class label)
void TreeNode::Set_Info(string new_info){
	info = new_info;
}
//get the info string
string TreeNode::Get_Info() const{
	return info;
}