#include "attribute.h"

void Attribute::Set_Name(const string& s){
  name = s;
}

string Attribute::Get_Name() const{
  return name;
}

void Attribute::Set_Numeric(bool b){
  is_numeric = b;
}

bool Attribute::Numeric() const{
  return is_numeric;
}

void Attribute::Add_Category(string s){
  categories.push_back(s);
}

int Attribute::Num_Categories() const{
  return categories.size();
}

// I don't usually like returning a vector from a function, but
// I don't want people having a reference to the real private data either
vector<string> Attribute::Get_Category_List() const{
  return categories;
}

bool Attribute::In_Domain(string new_val){
  if(new_val == "?" || new_val == "N/A" || new_val == "NA")	// missing data is always legal
    return true;
    
  if(is_numeric){		// test whether a string is a (decimal) number
  	bool is_num = true;

  	if (!isdigit(new_val[0]) && new_val[0]!='-')	// leading character has to be a digit or the negative sign
  		is_num = false;
  		
  	int i = 1, num_dot = 0;  		
  	while (is_num && i < new_val.size()){
  		if (new_val[i] == '.')
  			num_dot++;
  			
  		if ((!isdigit(new_val[i]) && new_val[i] != '.') || num_dot > 1)	
  			is_num = false;
  			i++;
  	}
  	
    return is_num;
  }
  else{
  	if(is_open_domain){
  		return true;
 	 }
  
    for(int i = 0; i < categories.size(); i++)
      if(new_val == categories[i])
				return true;
    return false;
  }
}

string Attribute::Get_Category(int num) const{
  return categories[num];
}

void Attribute::Set_Open_Domain(bool b){
	is_open_domain = b;
}

bool Attribute::Is_Open_Domain() const{
	return is_open_domain;
}

