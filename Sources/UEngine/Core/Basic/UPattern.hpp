#pragma once

// Singleton
template<typename T>
class USingleton {
	
protected:

	static T* instance;
	USingleton(){}

public:

		static T* GetInstance();

		static void Free(){
			if(instance != NULL)
				delete instance;
		}
};





