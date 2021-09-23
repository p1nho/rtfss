#ifndef RTFSS_DEF_ARCH_PROPERTY
#define RTFSS_DEF_ARCH_PROPERTY

#include "arch_node.h"

template <class T> class arch_property{

	private:
		std::map<std::shared_ptr<arch_node>,T,std::owner_less<std::shared_ptr<arch_node>>> nodes;

	public:
		arch_property(){};

		void put(std::shared_ptr<arch_node> node,T &data);
		T* get(std::shared_ptr<arch_node> node);
};

#include "arch_property.tcc" //Implementation

#endif
