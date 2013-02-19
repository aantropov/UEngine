//Xml files reading
#pragma once

#include <vector>
#include <string>
#include <stdio.h>
using namespace std;

#include "..\Utils\enum.h"
#include "UResource.h"
#include "..\Utils\ULogger.h"

// XNLfile resource
class UXMLFile :
	public UResource
{
private:

	FILE* fXml;
	std::string sPath;

	// Every element has name and value
	struct UXMLNode{
		UXMLNode(std::string name1, std::string value1) : name(name1), value(value1) {}
		std::string name;
		std::string value;
	};

	// Open xml file
	bool OpenFile(std::string path);	
	// Close file
	void CloseFile();

	// Sorted container which contains all elements/
	vector<UXMLNode> elements;

public:

	UXMLFile();
	~UXMLFile();

	//Get file path
	std::string GetPath();

	// Create map of all elements in .xml
	bool Load(std::string path);
	
	// Free
	void Free(){}

	/* Get value by "path": /a/b/c
	for xml:
 	<a><a1></a1><b><c>Test<c></b></a>
	result:Test
	*/
	std::string GetElement(std::string path);
	vector<string> GetElements(std::string key);
	bool isExistElement(string key);
};

