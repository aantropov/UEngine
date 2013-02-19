#pragma once
#include "..\GameMechanics\UComponent.h"

class UScript :	public UComponent{

public:
	
	virtual void Update(double delta){}

	UScript();
	~UScript();	
};

