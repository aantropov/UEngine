//Master class
#pragma once
#include "..\Utils\ExportDef.h"
#include "UIUpdateble.h"

//UNode may be created and stored in scene hierarchy
class UNode : public UIUpdateble
{	
public:
	UENGINE_DECLSPEC UNode();
	
	/*//PreRender is used before child render, it is needed for matrix Saving
	virtual void PreRender(){};
	//PostRender is after parent node render
	virtual void PostRender(){};
	//Render
	virtual void Render(){};
	//Update is one time per frame
	*/
	virtual UENGINE_DECLSPEC void Update(double delta){};
	// Standard RAII
	virtual UENGINE_DECLSPEC void Free(){};
	virtual UENGINE_DECLSPEC ~UNode();
};

