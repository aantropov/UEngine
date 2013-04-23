/*!
	@file
	@author		George Evmenov
	@date		07/2009
*/

#include "MyGUI_UEngineRenderManager.h"
#include "MyGUI_UEngineTexture.h"
#include "MyGUI_UEngineVertexBuffer.h"
#include "MyGUI_UEngineDiagnostic.h"
#include "mygui 3.2.0/MyGUI_VertexData.h"
#include "mygui 3.2.0/MyGUI_Gui.h"
#include "mygui 3.2.0/MyGUI_Timer.h"

//#include "mygui 3.2.0/GL/glew.h"
#include "../Renderer/URendererHelper.h"

namespace MyGUI
{

	UEngineRenderManager::UEngineRenderManager() :
		mUpdate(false),
		mImageLoader(nullptr),
		mPboIsSupported(false),
		mIsInitialise(false)
	{
	}

	void UEngineRenderManager::initialise(UEngineImageLoader* _loader)
	{
		MYGUI_PLATFORM_ASSERT(!mIsInitialise, getClassTypeName() << " initialised twice");
		MYGUI_PLATFORM_LOG(Info, "* Initialise: " << getClassTypeName());

		mVertexFormat = VertexColourType::ColourABGR;

		mUpdate = false;
		mImageLoader = _loader;

		//glewInit();

		mPboIsSupported = true;//gl IsExtensionSupported("GL_EXT_pixel_buffer_object") != 0;

		MYGUI_PLATFORM_LOG(Info, getClassTypeName() << " successfully initialized");
		mIsInitialise = true;
	}

	void UEngineRenderManager::shutdown()
	{
		MYGUI_PLATFORM_ASSERT(mIsInitialise, getClassTypeName() << " is not initialised");
		MYGUI_PLATFORM_LOG(Info, "* Shutdown: " << getClassTypeName());

		destroyAllResources();

		MYGUI_PLATFORM_LOG(Info, getClassTypeName() << " successfully shutdown");
		mIsInitialise = false;
	}

	IVertexBuffer* UEngineRenderManager::createVertexBuffer()
	{
		return new UEngineVertexBuffer();
	}

	void UEngineRenderManager::destroyVertexBuffer(IVertexBuffer* _buffer)
	{
		delete _buffer;
	}

	void UEngineRenderManager::doRender(IVertexBuffer* _buffer, ITexture* _texture, size_t _count)
	{
		UEngineVertexBuffer* buffer = static_cast<UEngineVertexBuffer*>(_buffer);
		unsigned int buffer_id = buffer->getBufferID();
		MYGUI_PLATFORM_ASSERT(buffer_id, "Vertex buffer is not created");

		unsigned int texture_id = 0;
		if (_texture)
		{
			UEngineTexture* texture = static_cast<UEngineTexture*>(_texture);
			texture_id = texture->getTextureID();
			//MYGUI_PLATFORM_ASSERT(texture_id, "Texture is not created");
		}

		glBindTexture(GL_TEXTURE_2D, texture_id);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer_id);

		// enable vertex arrays
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// before draw, specify vertex and index arrays with their offsets
		size_t offset = 0;
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offset);
		offset += (sizeof(float) * 3);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offset);
		offset += (4);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offset);

		glDrawArrays(GL_TRIANGLES, 0, _count);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void UEngineRenderManager::begin()
	{
		//save current attributes
		glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glPolygonMode(GL_FRONT, GL_FILL);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_FOG);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);

		//glFrontFace(GL_CW);
		//glCullFace(GL_BACK);
		//glEnable(GL_CULL_FACE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
	}

	void UEngineRenderManager::end()
	{
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		//restore former attributes
		glPopAttrib();
		glPopClientAttrib();
	}

	const RenderTargetInfo& UEngineRenderManager::getInfo()
	{
		return mInfo;
	}

	const IntSize& UEngineRenderManager::getViewSize() const
	{
		return mViewSize;
	}

	VertexColourType UEngineRenderManager::getVertexFormat()
	{
		return mVertexFormat;
	}

	void UEngineRenderManager::drawOneFrame()
	{
		Gui* gui = Gui::getInstancePtr();
		if (gui == nullptr)
			return;

		static Timer timer;
		static unsigned long last_time = timer.getMilliseconds();
		unsigned long now_time = timer.getMilliseconds();
		unsigned long time = now_time - last_time;

		onFrameEvent((float)((double)(time) / (double)1000));

		last_time = now_time;

		begin();
		onRenderToTarget(this, mUpdate);
		end();

		mUpdate = false;
	}

	void UEngineRenderManager::setViewSize(int _width, int _height)
	{
		if (_height == 0)
			_height = 1;
		if (_width == 0)
			_width = 1;

		mViewSize.set(_width, _height);

		mInfo.maximumDepth = 1;
		mInfo.hOffset = 0;
		mInfo.vOffset = 0;
		mInfo.aspectCoef = float(mViewSize.height) / float(mViewSize.width);
		mInfo.pixScaleX = 1.0f / float(mViewSize.width);
		mInfo.pixScaleY = 1.0f / float(mViewSize.height);

		onResizeView(mViewSize);
		mUpdate = true;
	}

	bool UEngineRenderManager::isPixelBufferObjectSupported() const
	{
		return mPboIsSupported;
	}

	ITexture* UEngineRenderManager::createTexture(const std::string& _name)
	{
		MapTexture::const_iterator item = mTextures.find(_name);
		MYGUI_PLATFORM_ASSERT(item == mTextures.end(), "Texture '" << _name << "' already exist");

		UEngineTexture* texture = new UEngineTexture(_name, mImageLoader);
		mTextures[_name] = texture;
		return texture;
	}

	void UEngineRenderManager::destroyTexture(ITexture* _texture)
	{
		if (_texture == nullptr)
			return;

		MapTexture::iterator item = mTextures.find(_texture->getName());
		MYGUI_PLATFORM_ASSERT(item != mTextures.end(), "Texture '" << _texture->getName() << "' not found");

		mTextures.erase(item);
		delete _texture;
	}

	ITexture* UEngineRenderManager::getTexture(const std::string& _name)
	{
		MapTexture::const_iterator item = mTextures.find(_name);
		if (item == mTextures.end())
			return nullptr;
		return item->second;
	}

	void UEngineRenderManager::destroyAllResources()
	{
		for (MapTexture::const_iterator item = mTextures.begin(); item != mTextures.end(); ++item)
		{
			delete item->second;
		}
		mTextures.clear();
	}

} // namespace MyGUI
