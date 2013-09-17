#include "UXMLFile.h"
//#include "URendererHelper.h"
//#include "UEngine.h"

UXMLFile::UXMLFile()
{
    fXml = NULL;
}

UXMLFile::~UXMLFile()
{
}

std::string UXMLFile:: GetPath()
{
    return sPath;
}

bool UXMLFile::OpenFile(string path)
{
    fopen_s(&fXml, path.c_str() , "rb");
    if(fXml == NULL)
    {
        ULogger::GetInstance()->Message("XMLFile: Can`t open file " + path, ULOG_MSG_ERROR);    
        return false;
    }

    sPath = path;
    ULogger::GetInstance()->Message("XMLFile: " + path + " is opened");

    return true;
}

void UXMLFile::CloseFile()
{
    if(fXml != NULL)
    {
        fclose(fXml);
        ULogger::GetInstance()->Message("XMLFile: " + sPath + " is closed");
    }
}

string UXMLFile::GetElement(string key)
{
    for(unsigned int i = 0; i < elements.size(); i++)
        if(key == elements[i].name)
            return elements[i].value;
    return NULL;
}


int UXMLFile::GetElementi(std::string param_path) 
{
	return std::atoi(GetElement(param_path).c_str());
}

float UXMLFile::GetElementf(std::string param_path) 
{
    return (float)std::atof(GetElement(param_path).c_str());
}

bool UXMLFile::isExistElement(string key)
{
    for(unsigned int i = 0; i < elements.size(); i++)
        if(key == elements[i].name)
            return true;
    return false;
}

vector<std::string> UXMLFile::GetElements(std::string key)
{
    vector<std::string> res;
    for(unsigned int i = 0; i < elements.size(); i++)
        if(key == elements[i].name)
            res.push_back(elements[i].value);
    return res;
}

bool UXMLFile::Load(string path)
{
    if(!OpenFile(path))
        return false;

    string temp;
    string currentMapTag = "/";
    string lastWord;
    string value;

    char buffer[UE_MAXCHAR];
    memset(buffer, '\0' , UE_MAXCHAR);

    try
    {
        while(!feof(fXml)){
            // Get line
            temp = std::fgets(buffer, MAXCHAR, fXml);

            // Parsing
            int startWord = -1;
            int startTag = -1;
            int endTag = -1;
            bool isEndingTag = false;

            for(unsigned int i = 0; i < temp.length() ; i++){            

                // if it is a new tag
                if(temp[i] == '<' && startTag == -1){
                    if(temp[i+1] == '/'){
                        startWord = -1;
                        isEndingTag = true;
                        startTag = i+2;
                    }
                    else
                        startTag = i+1;
                }
                //value forming
                if(startWord != -1 && temp[i] != 13 && temp[i] != 10)
                    value += temp[i];

                // if end of tag
                if(temp[i] == '>' && startTag != -1){

                    // tag name
                    lastWord = temp.substr(startTag, i - startTag);

                    // if it is an ending tag we should delete last tag
                    if(!isEndingTag){
                        currentMapTag += lastWord + "/";
                        startWord = i+1;
                    }
                    else{
                        // has no value
                        if(value != "")
                            elements.push_back(UXMLNode(currentMapTag, value));
                        value = "";
                        currentMapTag = currentMapTag.substr(0,currentMapTag.length()-lastWord.length()-1);
                    }
                    //Initialise
                    startTag = -1;
                    //endTag = -1;
                    endTag = false;                
                }                
            }                
        }
        CloseFile();

    }
    catch(exception ex)
    {
        ULogger::GetInstance()->Message("UXMLFile: Uncorrect xml" ,ULOG_MSG_ERROR);        
    }
    return true;
}