/*!
    @file
    @author        George Evmenov
    @date        07/2009
*/

#include "MyGUI_UEngineDataManager.h"
#include "MyGUI_UEngineDiagnostic.h"
#include "mygui 3.2.0/MyGUI_DataFileStream.h"
#include "mygui 3.2.0/FileSystemInfo/FileSystemInfo.h"
#include <fstream>

namespace MyGUI
{

    UEngineDataManager::UEngineDataManager() :
        mIsInitialise(false)
    {
    }

    void UEngineDataManager::initialise()
    {
        MYGUI_PLATFORM_ASSERT(!mIsInitialise, getClassTypeName() << " initialised twice");
        MYGUI_PLATFORM_LOG(Info, "* Initialise: " << getClassTypeName());

        MYGUI_PLATFORM_LOG(Info, getClassTypeName() << " successfully initialized");
        mIsInitialise = true;
    }

    void UEngineDataManager::shutdown()
    {
        MYGUI_PLATFORM_ASSERT(mIsInitialise, getClassTypeName() << " is not initialised");
        MYGUI_PLATFORM_LOG(Info, "* Shutdown: " << getClassTypeName());

        MYGUI_PLATFORM_LOG(Info, getClassTypeName() << " successfully shutdown");
        mIsInitialise = false;
    }

    IDataStream* UEngineDataManager::getData(const std::string& _name)
    {
        std::string filepath = getDataPath(_name);
        if (filepath.empty())
            return nullptr;

        std::ifstream* stream = new std::ifstream();
        stream->open(filepath.c_str(), std::ios_base::binary);

        if (!stream->is_open())
        {
            delete stream;
            return nullptr;
        }

        DataFileStream* data = new DataFileStream(stream);

        return data;
    }

    bool UEngineDataManager::isDataExist(const std::string& _name)
    {
        const VectorString& files = getDataListNames(_name);
        return files.size() == 1;
    }

    const VectorString& UEngineDataManager::getDataListNames(const std::string& _pattern)
    {
        static VectorString result;
        common::VectorWString wresult;
        result.clear();

        for (VectorArhivInfo::const_iterator item = mPaths.begin(); item != mPaths.end(); ++item)
        {
            common::scanFolder(wresult, (*item).name, (*item).recursive, MyGUI::UString(_pattern).asWStr(), false);
        }

        for (common::VectorWString::const_iterator item = wresult.begin(); item != wresult.end(); ++item)
        {
            result.push_back(MyGUI::UString(*item).asUTF8());
        }

        return result;
    }

    const std::string& UEngineDataManager::getDataPath(const std::string& _name)
    {
        static std::string path;
        VectorString result;
        common::VectorWString wresult;

        for (VectorArhivInfo::const_iterator item = mPaths.begin(); item != mPaths.end(); ++item)
        {
            common::scanFolder(wresult, (*item).name, (*item).recursive, MyGUI::UString(_name).asWStr(), true);
        }

        for (common::VectorWString::const_iterator item = wresult.begin(); item != wresult.end(); ++item)
        {
            result.push_back(MyGUI::UString(*item).asUTF8());
        }

        path = result.size() == 1 ? result[0] : "";
        return path;
    }

    void UEngineDataManager::addResourceLocation(const std::string& _name, bool _recursive)
    {
        ArhivInfo info;
        info.name = MyGUI::UString(_name).asWStr();
        info.recursive = _recursive;
        mPaths.push_back(info);
    }

} // namespace MyGUI
