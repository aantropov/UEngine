#include "..\UEngine.h"

//Constants
const string ULogger:: ULOG_FILE_PATH = "log.txt";
const string UConfig:: UCONFIG_FILE_PATH = "config.xml";

const string UWindow:: UWND_CLASS_NAME = "UWindow";

UResourceFactory UEngine::rf;

bool UEngine::Initialize()
{
    elapsedTime = 0;
    fps = 0;

    if(UWindow::IsRunning())
        return true;

    ULogger::GetInstance()->Message("UEngine: start UEngine initialisation");
    
    if(URenderer::GetInstance()->Initialize())
        ULogger::GetInstance()->Message("UEngine: UEngine is initialized succesfully");
    
    renderManager = new URenderManager();

    return true;
}

void UEngine:: Stop()
{
    UWindow::SetRunning(false);
}

void UEngine::Run()
{
    MSG msg;
    
    UWindow::SetActive(true);
    UWindow::SetRunning(true);
    
    double deltaTime, beginFrameTime, fixedTimeStep;
    
	auto render = URenderer::GetInstance();	

    deltaTime      = 0.0;
    //it is important for lag fixing
    fixedTimeStep  = 1.0 / 100.0;

    while (UWindow::IsRunning())
    {
        while (PeekMessage(&msg, UWindow::GetHWND(), 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
			{
                UWindow::SetRunning(false);
                break;
            }
            DispatchMessage(&msg);
        }
        
        beginFrameTime = GetTickCount();

        if (UInput::IsKeyPressed(VK_ESCAPE))
            this->Stop();

        if(currentScene != NULL)
            currentScene->KeysProccessing();
            
        if (UWindow::IsRunning() &&  UWindow::IsActive())
        {
            OPENGL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        
			render->drawCalls = 0;

            if(currentScene != NULL)
                renderManager->Render(currentScene);

            SwapBuffers(UWindow::GetHDC());

            deltaTime += GetTickCount() - beginFrameTime;
            
            if(currentScene != NULL)
                currentScene->Update(GetTickCount() - beginFrameTime);

            elapsedTime += (float)(GetTickCount() - beginFrameTime)/1000.0f;
            ++fps;

            if (elapsedTime >= 1.0f)
            {
                WCHAR buff[50];
				wsprintf(buff, L"UEngine FPS: %u, Draw Calls: %u", fps, render->drawCalls);

                UWindow::SetWindowTitle(buff);
                fps = 0;
                elapsedTime = 0.0f;
            }
        }
    }

    UWindow::SetActive(false);
    UWindow::SetRunning(false);
}

void UEngine::Release()
{
    rf.ReleaseAll();
    delete renderManager;
    
    URenderer::GetInstance()->Release();
    URenderer::Free();

    UScriptEngine::GetInstance()->Release();
    UScriptEngine::Free();

    UConfig::Free();
    ULogger::Free();
}