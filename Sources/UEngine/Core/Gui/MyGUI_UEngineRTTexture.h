/*!
    @file
    @author        Albert Semenov
    @date        12/2009
*/

#ifndef __MYGUI_UEngine_RTTEXTURE_H__
#define __MYGUI_UEngine_RTTEXTURE_H__

#include "mygui 3.2.0/MyGUI_Prerequest.h"
#include "mygui 3.2.0/MyGUI_ITexture.h"
#include "mygui 3.2.0/MyGUI_RenderFormat.h"
#include "mygui 3.2.0/MyGUI_IRenderTarget.h"

namespace MyGUI
{

    class UEngineRTTexture :
        public IRenderTarget
    {
    public:
        UEngineRTTexture(unsigned int _texture);
        virtual ~UEngineRTTexture();

        virtual void begin();
        virtual void end();

        virtual void doRender(IVertexBuffer* _buffer, ITexture* _texture, size_t _count);

        virtual const RenderTargetInfo& getInfo()
        {
            return mRenderTargetInfo;
        }

    private:
        RenderTargetInfo mRenderTargetInfo;
        unsigned int mTextureID;
        int mWidth;
        int mHeight;
        unsigned int mFBOID;
        unsigned int mRBOID;
    };

} // namespace MyGUI

#endif // __MYGUI_UEngine_RTTEXTURE_H__
