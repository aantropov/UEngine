/*!
	@file
	@author		George Evmenov
	@date		07/2009
*/

#ifndef __MYGUI_UEngine_RENDER_MANAGER_H__
#define __MYGUI_UEngine_RENDER_MANAGER_H__

#include "mygui 3.2.0/MyGUI_Prerequest.h"
#include "mygui 3.2.0/MyGUI_RenderFormat.h"
#include "mygui 3.2.0/MyGUI_IVertexBuffer.h"
#include "mygui 3.2.0/MyGUI_RenderManager.h"
#include "MyGUI_UEngineImageLoader.h"

namespace MyGUI
{

	class UEngineRenderManager :
		public RenderManager,
		public IRenderTarget
	{
	public:
		UEngineRenderManager();

		void initialise(UEngineImageLoader* _loader = 0);
		void shutdown();

		static UEngineRenderManager& getInstance()
		{
			return *getInstancePtr();
		}
		static UEngineRenderManager* getInstancePtr()
		{
			return static_cast<UEngineRenderManager*>(RenderManager::getInstancePtr());
		}

		/** @see UEngineRenderManager::getViewSize */
		virtual const IntSize& getViewSize() const;

		/** @see UEngineRenderManager::getVertexFormat */
		virtual VertexColourType getVertexFormat();

		/** @see UEngineRenderManager::createVertexBuffer */
		virtual IVertexBuffer* createVertexBuffer();
		/** @see UEngineRenderManager::destroyVertexBuffer */
		virtual void destroyVertexBuffer(IVertexBuffer* _buffer);

		/** @see UEngineRenderManager::createTexture */
		virtual ITexture* createTexture(const std::string& _name);
		/** @see UEngineRenderManager::destroyTexture */
		virtual void destroyTexture(ITexture* _texture);
		/** @see UEngineRenderManager::getTexture */
		virtual ITexture* getTexture(const std::string& _name);


		/** @see IRenderTarget::begin */
		virtual void begin();
		/** @see IRenderTarget::end */
		virtual void end();
		/** @see IRenderTarget::doRender */
		virtual void doRender(IVertexBuffer* _buffer, ITexture* _texture, size_t _count);
		/** @see IRenderTarget::getInfo */
		virtual const RenderTargetInfo& getInfo();


	/*internal:*/
		void drawOneFrame();
		void setViewSize(int _width, int _height);
		bool isPixelBufferObjectSupported() const;

	private:
		void destroyAllResources();

	private:
		IntSize mViewSize;
		bool mUpdate;
		VertexColourType mVertexFormat;
		RenderTargetInfo mInfo;

		typedef std::map<std::string, ITexture*> MapTexture;
		MapTexture mTextures;
		UEngineImageLoader* mImageLoader;
		bool mPboIsSupported;

		bool mIsInitialise;
	};

} // namespace MyGUI

#endif // __MYGUI_UEngine_RENDER_MANAGER_H__
