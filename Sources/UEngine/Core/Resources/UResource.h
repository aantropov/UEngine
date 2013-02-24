//Master class for resources, it should be used in manufactures
#pragma once
#include <vector>
#include <string>
#include <stdio.h>
using namespace std;

class UResourceFactory;

// Base class for resources, RAII hierarchy
class UResource
{	
public:

	// Resource factory
	UResourceFactory* rf;

	virtual bool Load(std::string path){ return true; }
	virtual void Free() {}

	UResource();
	virtual ~UResource();
};

