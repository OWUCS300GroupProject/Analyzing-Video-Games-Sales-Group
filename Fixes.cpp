# in your arfftree_compiled.cpp
void Modify_Class(vector<Instance>& examples){

	for(int i = 0; i < examples.size(); i++){
		
		string cur_val_str = examples[i].Get_String_Value(examples[i].Get_Num_Attributes()-1);
		double cur_val = stod(cur_val_str); 
		double replace = floor(cur_val/5.0)* 5.0;
		
		//new category is the highest multiple of 5 that can go into the previous sales price
		examples[i].Add_Value(examples[i].Get_Num_Attributes()-1, to_string(replace_int));
		
	}
	
}

#in your instance.cpp
string Instance::Get_String_Value(int attribute_num) const{
	if(Is_Valid_Attribute(attribute_num)){
		return Element_List[attribute_num];
	}
	else{
		cout << "Error- illegal attribute number: " << attribute_num << endl;
		exit(1);
	}
}

#in your instance.h
string Get_String_Value(int attribute_num) const;