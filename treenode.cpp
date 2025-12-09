#include "treenode.h"
#include <string>
#include <vector>

using namespace std;

//space for n children and their names
void TreeNode::Set_Num_Children(int n){
    children.resize(n);
    child_names.resize(n);
}

//returns the number of children
int TreeNode::Get_Num_Children() const{
    return children.size();
}

//sets the  branch name for child at index pos
void TreeNode::Set_Child_Name(int pos, string new_name){
    child_names[pos] = new_name;
}

// get branch name for child at index pos
string TreeNode::Get_Child_Name(int pos) const{
    return child_names[pos];
}

// set pointer to child at index pos
void TreeNode::Set_Child(int pos, TreeNode* T){
    children[pos] = T;
}

// get pointer to child at index pos
TreeNode* TreeNode::Get_Child(int pos) const{
    return children[pos];
}

//mark node as leaf or not a leaf
void TreeNode::Set_Leaf(bool leaf){
    is_leaf = leaf;
}

//checks if it is a leaf
bool TreeNode::Is_Leaf() const{
    return is_leaf;
}

//setting info string (attribute name or class label)
void TreeNode::Set_Info(string new_info){
    info = new_info;
}

//get info string 
string TreeNode::Get_Info() const{
    return info;
}
