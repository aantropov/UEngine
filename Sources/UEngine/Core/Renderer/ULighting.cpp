#include "ULighting.h"
#include "..\UEngine.h"

UDefferedLighting::UDefferedLighting()
{
    fb.Initialize();
    postfb.Initialize();

    lighting = dynamic_cast<UPostEffect*>(UEngine::rf.Load(UConfig::GetInstance()->GetParam("/xml/config/post_effects/deffered_lighting/"), URESOURCE_POST_EFFECT));

    colorScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
    depthScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
    normalScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
    diffuseScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
    //ambientScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
    specularScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
    resScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
    resSceneA = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
    resSceneB = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
    positionScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
    
    colorScene->name = "colorScene";
    depthScene->name = "depthScene";
    normalScene->name = "normalScene";
    diffuseScene->name = "diffuseScene";
    //ambientScene->name = "ambientScene";
    specularScene->name = "specularScene";
    positionScene->name = "positionScene";
    resScene->name = "colorScene";
    resSceneA->name = "previousScene";
    resSceneB->name = "previousScene";

    auto render = URenderer::GetInstance();

    colorScene->Create(render->GetWidth(), URenderer::GetInstance()->GetHeight(), UTEXTURE_COLOR);
    normalScene->Create(render->GetWidth(), render->GetHeight(), UTEXTURE_COLOR);
    diffuseScene->Create(render->GetWidth(), render->GetHeight(), UTEXTURE_COLOR);
    //ambientScene->Create(render->GetWidth(), render->GetHeight(), UTEXTURE_COLOR);
    specularScene->Create(render->GetWidth(), render->GetHeight(), UTEXTURE_COLOR);
    resScene->Create(render->GetWidth(), render->GetHeight(), UTEXTURE_COLOR);
    resSceneA->Create(render->GetWidth(), render->GetHeight(), UTEXTURE_COLOR);
    resSceneB->Create(render->GetWidth(), render->GetHeight(), UTEXTURE_COLOR);
    positionScene->Create(render->GetWidth(), render->GetHeight(), UTEXTURE_FLOAT32);
    depthScene->Create(render->GetWidth(), render->GetHeight(), UTEXTURE_DEPTH);

    fb.BindTexture(depthScene, UFB_ATTACHMENT_DEPTH);
    fb.BindTexture(colorScene, UFB_ATTACHMENT_COLOR0);
    fb.BindTexture(normalScene, UFB_ATTACHMENT_COLOR1);
    fb.BindTexture(diffuseScene, UFB_ATTACHMENT_COLOR2);
    //fb.BindTexture(ambientScene, UFB_ATTACHMENT_COLOR3);
    fb.BindTexture(specularScene, UFB_ATTACHMENT_COLOR3);
    fb.BindTexture(positionScene, UFB_ATTACHMENT_COLOR4);

    lighting->AddTexture(colorScene, 0);
    lighting->AddTexture(depthScene, 1);
    lighting->AddTexture(normalScene, 2);
    lighting->AddTexture(diffuseScene, 3);
    //lighting->AddTexture(ambientScene, 4);
    lighting->AddTexture(specularScene, 4);
    lighting->AddTexture(positionScene, 5);
}

UTexture* UDefferedLighting::Render(UScene *scene, UCamera camera)
{
    auto render = URenderer::GetInstance();
    render->BindFBO(&fb);
    GLenum buffers[] = { UFB_ATTACHMENT_COLOR0, UFB_ATTACHMENT_COLOR1, UFB_ATTACHMENT_COLOR2,UFB_ATTACHMENT_COLOR3, UFB_ATTACHMENT_COLOR4 };
    glDrawBuffers(5, buffers);

    glViewport(0, 0, colorScene->GetWidth(), colorScene->GetHeight());
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);

    scene->Render(URENDER_DEFFERED, camera);
    render->UnbindFBO();

    OPENGL_CHECK_FOR_ERRORS();

    auto lights = scene->GetLights();
    auto lightParams = render->GetCurrentScene()->lightParams;

    //Update frustum
    /*
    UVertex* vertices = reinterpret_cast<UVertex*>(lighting->vb.Lock());
    
    mat4 view = render->currentCamera.GetView();
    mat4 viewProjection = render->currentCamera.GetProjection() * view;
    mat4 viewProjectionInv = inverse(viewProjection);

    for (int i = 0; i < lighting->vb.GetNum(); i++)
    {
        vec3 pos = vertices[i].GetPosition();
        vec4 frustumCorner = vec4(pos.x, pos.y, 1.0f, 1.0f);        
        frustumCorner = viewProjectionInv * frustumCorner;
        frustumCorner /= frustumCorner.w;

        vertices[i].SetNormal(frustumCorner);
    }

    lighting->vb.Unlock();
    */

    int mod2 = 0;
    for (unsigned int i = 0; i < lightParams.count; i++)
    {
        lighting->material.ClearUniformUnits();
        lighting->AddTexture(colorScene, 0);
        lighting->AddTexture(depthScene, 1);
        lighting->AddTexture(normalScene, 2);
        lighting->AddTexture(diffuseScene, 3);
        //lighting->AddTexture(ambientScene, 4);
        lighting->AddTexture(specularScene, 4);
        lighting->AddTexture(positionScene, 5);

        if (mod2 % 2 == 0)
        {
            lighting->AddTexture(resSceneA, 7);
        }
        else if (mod2 % 2 == 1)
        {
            lighting->AddTexture(resSceneB, 7);
        }

        lighting->material.params["lightIndex"] = UUniformParam((float)i);

        if (lightParams.count == 1)
            lighting->material.params["state"] = UUniformParam(3.0f);
        else if (mod2 == 0)
            lighting->material.params["state"] = UUniformParam(0.0f);
        else if (mod2 == lightParams.count - 1)
            lighting->material.params["state"] = UUniformParam(2.0f);
        else
            lighting->material.params["state"] = UUniformParam(1.0f);

        //////////////////////////////////////
        render->BindFBO(&postfb);
        postfb.BindTexture(mod2 == lightParams.count - 1 ? resScene : (mod2 % 2 == 0 ? resSceneB : resSceneA), UFB_ATTACHMENT_COLOR0);

        OPENGL_CHECK_FOR_ERRORS();
                
        lighting->Render(URENDER_DEFFERED, i);
        render->UnbindFBO();
        //////////////////////////////////////

        mod2++;
    }

    OPENGL_CHECK_FOR_ERRORS();
    return resScene;
}

UForwardLighting::UForwardLighting()
{
    fb.Initialize();
    postfb.Initialize();

    depthScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
    normalScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));
    resScene = dynamic_cast<UTexture*>(UEngine::rf.Create(URESOURCE_TEXTURE));

    depthScene->name = "depthScene";
    normalScene->name = "normalScene";
    resScene->name = "colorScene";

    auto render = URenderer::GetInstance();

    normalScene->Create(render->GetWidth(), render->GetHeight(), UTEXTURE_FLOAT);
    resScene->Create(render->GetWidth(), render->GetHeight(), UTEXTURE_COLOR);
    depthScene->Create(render->GetWidth(), render->GetHeight(), UTEXTURE_DEPTH);

    fb.BindTexture(depthScene, UFB_ATTACHMENT_DEPTH);
    fb.BindTexture(resScene, UFB_ATTACHMENT_COLOR0);
    fb.BindTexture(normalScene, UFB_ATTACHMENT_COLOR1);
}

UTexture* UForwardLighting::Render(UScene *scene, UCamera camera)
{
    auto render = URenderer::GetInstance();
    //color, depth
    render->BindFBO(&fb);
    fb.BindTexture(resScene, UFB_ATTACHMENT_COLOR0);
    fb.BindTexture(depthScene, UFB_ATTACHMENT_DEPTH);

    glViewport(0, 0, resScene->GetWidth(), resScene->GetHeight());
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);

    scene->Render(URENDER_FORWARD, camera);
    URenderer::GetInstance()->UnbindFBO();

    // normal    
    URenderer::GetInstance()->BindFBO(&fb);
    fb.BindTexture(normalScene, UFB_ATTACHMENT_COLOR0);

    glViewport(0, 0, normalScene->GetWidth(), normalScene->GetHeight());
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    scene->Render(URENDER_NORMAL, camera);
    URenderer::GetInstance()->UnbindFBO();
    return resScene;
}