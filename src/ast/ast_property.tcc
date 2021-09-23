//#include "ast_property.h"

template <class T> bool ast_property<T>::erase(std::shared_ptr<ast> node){
	auto it=nodes.find(node);
	if(it!=nodes.end()){ //Found, erase
		nodes.erase(it);
		return true;
	}
	else{ //Not found, return false
		return false;
	}
}

template <class T> void ast_property<T>::put(std::shared_ptr<ast> node,T &data){
	
	if(super_parent==nullptr){ //first node getting inserted
		super_parent=ast_property::get_ast_super(node);
	}

	if(check && !check_same_tree(node)){ //not the same tree, do not insert
		//TODO add exception!
		std::cerr<<"AST_PROPERTY NOT SAME TREE"<<std::endl;
	}

	nodes[node]=data; //Creates space in container if key is not already present
	//nodes.insert(std::make_pair(node,data));
}

template <class T> T* ast_property<T>::get(std::shared_ptr<ast> node){

	//Check same tree
	if(!check_same_tree(node)) //not same tree
		return nullptr;

	auto result=nodes.find(node);
	
	//Check if found
	if(result==nodes.end()) //not found
		return nullptr;

	return &(result->second);
}

template <class T> bool ast_property<T>::check_same_tree(std::shared_ptr<ast> node){
	return ast_property::get_ast_super(node)==super_parent;
}

template <class T> std::shared_ptr<ast> ast_property<T>::get_ast_super(std::shared_ptr<ast> node){
	
	while(!node->get_parent().expired()){
		node=node->get_parent().lock();
	}

	return node;
}
