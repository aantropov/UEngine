#pragma once
#include "..\Basic\USingleton.hpp"
#include "..\Resources\uxmlfile.h"

// Load parameters from the configuration file
class UConfig :	public USingleton<UConfig>{

	// Path to log file
	static const string UCONFIG_FILE_PATH;
	// Config file
	UXMLFile uCfg;

	UConfig();

public:
	
	// Singleton method
	static UConfig* GetInstance();

	//Returns value of the parameter
	std::string GetParam(std::string param_path);

	~UConfig();	
};

// Singleton
//UConfig* UConfig::instance = NULL;