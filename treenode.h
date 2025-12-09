#ifndef TREENODE_H
#define TREENODE_H

#include <string>
#include <vector>

using namespace std;

class TreeNode {
 public:
  // Set how many children this node will have
  void Set_Num_Children(int n);

  // Get how many children this node has
  int Get_Num_Children() const;

  // For child at index pos, set the "name" of the branch
  // e.g. the category string like "Action", "Wii", etc.
  void Set_Child_Name(int pos, string new_name);

  // Get the branch name of child at index pos
  string Get_Child_Name(int pos) const;

  // Set the pointer to a child node at position pos
  void Set_Child(int pos, TreeNode* T);

  // Get the pointer to a child node at position pos
  TreeNode* Get_Child(int pos) const;

  // Mark whether this node is a leaf (true) or not (false)
  void Set_Leaf(bool leaf);

  // Check if this node is a leaf
  bool Is_Leaf() const;

  // Set the info stored in this node:
  //   attribute name (internal node) OR class label (leaf)
  void Set_Info(string new_info);

  // Get the info stored in this node
  string Get_Info() const;

 private:
  string info;                 // attribute name or class label
  bool is_leaf = false;        // true if leaf
  vector<string> child_names;  // branch labels (one per child)
  vector<TreeNode*> children;  // child pointers
};

#endif