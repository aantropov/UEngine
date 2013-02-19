#pragma once
#include "..\UEngine.h"

template<typename T>
class USingleton {
	
protected:

	static T* instance;
	USingleton(){}

public:
			
		static T* getInstance(){
			if(instance == NULL){
				instance = new T();
			}
			return instance;
		}
		void free(){
			if(instance != NULL)
				delete T;
		}
};


class OnlyOne
{
public:
	static OnlyOne* Instance()
	{
		if(theSingleInstance==0)
			theSingleInstance=new OnlyOne;
		return theSingleInstance;
	}
private:
	static OnlyOne* theSingleInstance;
	OnlyOne(){};
};
OnlyOne* OnlyOne::theSingleInstance=0;