// Pattern
#pragma once
#include "UResource.h"
#include "..\Utils\enum.h"
#include "..\Utils\ExportDef.h"

// Factory is needed for resource creation and management. RAII style
class UShaderProgram;

class UResourceFactory {
	
protected:
	//Element of the vector
	class UElement{
		//id is equal path
		protected:	
			std::string id;
			UResource* resource;
		public:
			
			UElement(UResource* r, std::string path) : resource(r), id(path) {}
			
			UResource* GetResource(){ return resource;}
			void SetResource( UResource* r ){ resource = r; }


			std::string GetId() { return id;}
			void SetId(std::string id) { this->id = id; }
			
			void Free(){ delete resource;}
			~UElement() { Free(); }

	};
	// Main container
	vector<UElement*> resources;

	int unique_id;

public:
	UENGINE_DECLSPEC UResourceFactory(){ unique_id = 0;}
	UENGINE_DECLSPEC ~UResourceFactory(){ ReleaseAll(); }

	//Load resource from hard disk
	UResource UENGINE_DECLSPEC *Get(std::string path);
	bool UENGINE_DECLSPEC Add(std::string path, UResource* res);
	UResource UENGINE_DECLSPEC *Load(std::string path, URESOURCE_TYPE type);
	UShaderProgram UENGINE_DECLSPEC *Load(std::string vp, std::string pp);
	UResource UENGINE_DECLSPEC *Create(URESOURCE_TYPE type);

	// delete by Id
	void UENGINE_DECLSPEC Release(std::string path);	
	// delete by pointer
	void UENGINE_DECLSPEC Release(UResource *resource);
	// delete all elements
	void UENGINE_DECLSPEC ReleaseAll();

};


