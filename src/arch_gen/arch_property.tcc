//#include "arch_property.h"

template <class T> void arch_property<T>::put(std::shared_ptr<arch_node> node,T &data){

	nodes[node]=data; //Creates space in container if key is not already present
}

template <class T> T* arch_property<T>::get(std::shared_ptr<arch_node> node){

	auto result=nodes.find(node);
	
	//Check if found
	if(result==nodes.end()) //not found
		return nullptr;

	return &(result->second);
}
