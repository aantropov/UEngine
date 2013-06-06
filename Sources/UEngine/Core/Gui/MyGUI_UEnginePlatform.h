/*!
    @file
    @author        George Evmenov
    @date        07/2009
*/

#ifndef __MYGUI_UEngine_PLATFORM_H__
#define __MYGUI_UEngine_PLATFORM_H__

#include "mygui 3.2.0/MyGUI_Prerequest.h"
#include "MyGUI_UEngineDiagnostic.h"
#include "MyGUI_UEngineRenderManager.h"
#include "MyGUI_UEngineDataManager.h"
#include "MyGUI_UEngineImageLoader.h"
#include "mygui 3.2.0/MyGUI_LogManager.h"

namespace MyGUI
{

    class UEnginePlatform
    {
    public:
        UEnginePlatform();
        ~UEnginePlatform();

        void initialise(UEngineImageLoader* _loader, const std::string& _logName = MYGUI_PLATFORM_LOG_FILENAME);
        void shutdown();

        UEngineRenderManager* getRenderManagerPtr();
        UEngineDataManager* getDataManagerPtr();

    private:
        bool mIsInitialise;
        UEngineRenderManager* mRenderManager;
        UEngineDataManager* mDataManager;
        LogManager* mLogManager;
    };

} // namespace MyGUI

#endif // __MYGUI_UEngine_PLATFORM_H__
