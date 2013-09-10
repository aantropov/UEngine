#include "..\UEngine.h"

//Constants
const string ULogger:: ULOG_FILE_PATH = "log.txt";
const string UConfig:: UCONFIG_FILE_PATH = "config.xml";

const string UWindow:: UWND_CLASS_NAME = "UWindow";

UResourceFactory UEngine::rf;

bool UEngine::Initialize(){

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
void UEngine:: Stop(){
    UWindow::SetRunning(false);
}
void UEngine::Run(){
    // Main loop
    
    MSG msg;
    
    // Main window cycle
    UWindow::SetActive(true);
    UWindow::SetRunning(true);
    
    // Computing deltaTime
    double deltaTime, beginFrameTime, fixedTimeStep;
    
    deltaTime      = 0.0;
    //it is important for lag fixing
    fixedTimeStep  = 1.0 / 100.0;

    while (UWindow::IsRunning())
    {
        // Proccess messages from quee
        while (PeekMessage(&msg, UWindow::GetHWND(), 0, 0, PM_REMOVE)){
            if (msg.message == WM_QUIT){
                UWindow::SetRunning(false);
                break;
            }
            // TranslateMessage(&msg);
            DispatchMessage(&msg);

        }
        
        //Start time
        beginFrameTime = GetTickCount();

        if (UInput::IsKeyPressed(VK_ESCAPE))
            this->Stop();

        //Process user input
        if(currentScene != NULL)
            currentScene->KeysProccessing();
            
        // If window is runned and active
        if (UWindow::IsRunning() &&  UWindow::IsActive()){
        
        
            OPENGL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
            //OPENGL_CALL(glClear(GL_COLOR_BUFFER_BIT));
            
            //Setup camera
            
            //URenderer::GetInstance()->SetCamera();

            if(currentScene != NULL)
                renderManager->Render(currentScene);

            SwapBuffers( UWindow::GetHDC());

            //Updating delta
            deltaTime += GetTickCount() - beginFrameTime;
            
            //other way
            if(currentScene != NULL)
                currentScene->Update(GetTickCount() - beginFrameTime);

            //fps
            elapsedTime += (float)(GetTickCount() - beginFrameTime)/1000.0f;
            ++fps;

            if (elapsedTime >= 1.0f){
                
                WCHAR buff[50];
                wsprintf(buff, L"UEngine FPS: %u", fps);

                UWindow::SetWindowTitle(buff);
                fps = 0;
                elapsedTime = 0.0f;
            }


            
        }
        //Sleep(2);
    }

    UWindow::SetActive(false);
    UWindow::SetRunning(false);
}
void UEngine::Release(){

    rf.ReleaseAll();

    delete renderManager;
    
    // Release all subsystems
    URenderer::GetInstance()->Release();
    URenderer::Free();

    UScriptEngine::GetInstance()->Release();
    UScriptEngine::Free();

    UConfig::Free();
    ULogger::Free();
}