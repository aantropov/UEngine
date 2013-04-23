/*!
	@file
	@author		George Evmenov
	@date		07/2009
*/

#include "MyGUI_UEngineTexture.h"
#include "MyGUI_UEngineRenderManager.h"
#include "MyGUI_UEngineDiagnostic.h"
#include "MyGUI_UEnginePlatform.h"
#include "MyGUI_UEngineRTTexture.h"

//#include "mygui 3.2.0/GL/glew.h"
#include "../Renderer/URendererHelper.h"

namespace MyGUI
{

	UEngineTexture::UEngineTexture(const std::string& _name, UEngineImageLoader* _loader) :
		mName(_name),
		mTextureID(0),
		mPboID(0),
		mWidth(0),
		mHeight(0),
		mLock(false),
		mPixelFormat(0),
		mDataSize(0),
		mUsage(0),
		mBuffer(0),
		mInternalPixelFormat(0),
		mAccess(0),
		mNumElemBytes(0),
		mImageLoader(_loader),
		mRenderTarget(nullptr)
	{
	}

	UEngineTexture::~UEngineTexture()
	{
		destroy();
	}

	const std::string& UEngineTexture::getName() const
	{
		return mName;
	}

	void UEngineTexture::setUsage(TextureUsage _usage)
	{
		mAccess = 0;
		mUsage = 0;

		if (_usage == TextureUsage::Default)
		{
			mUsage = GL_STATIC_READ_ARB;
			mAccess = GL_READ_ONLY_ARB;
		}
		else if (_usage.isValue(TextureUsage::Static))
		{
			if (_usage.isValue(TextureUsage::Read))
			{
				if (_usage.isValue(TextureUsage::Write))
				{
					mUsage = GL_STATIC_COPY_ARB;
					mAccess = GL_READ_WRITE_ARB;
				}
				else
				{
					mUsage = GL_STATIC_READ_ARB;
					mAccess = GL_READ_ONLY_ARB;
				}
			}
			else if (_usage.isValue(TextureUsage::Write))
			{
				mUsage = GL_STATIC_DRAW_ARB;
				mAccess = GL_WRITE_ONLY_ARB;
			}
		}
		else if (_usage.isValue(TextureUsage::Dynamic))
		{
			if (_usage.isValue(TextureUsage::Read))
			{
				if (_usage.isValue(TextureUsage::Write))
				{
					mUsage = GL_DYNAMIC_COPY_ARB;
					mAccess = GL_READ_WRITE_ARB;
				}
				else
				{
					mUsage = GL_DYNAMIC_READ_ARB;
					mAccess = GL_READ_ONLY_ARB;
				}
			}
			else if (_usage.isValue(TextureUsage::Write))
			{
				mUsage = GL_DYNAMIC_DRAW_ARB;
				mAccess = GL_WRITE_ONLY_ARB;
			}
		}
		else if (_usage.isValue(TextureUsage::Stream))
		{
			if (_usage.isValue(TextureUsage::Read))
			{
				if (_usage.isValue(TextureUsage::Write))
				{
					mUsage = GL_STREAM_COPY_ARB;
					mAccess = GL_READ_WRITE_ARB;
				}
				else
				{
					mUsage = GL_STREAM_READ_ARB;
					mAccess = GL_READ_ONLY_ARB;
				}
			}
			else if (_usage.isValue(TextureUsage::Write))
			{
				mUsage = GL_STREAM_DRAW_ARB;
				mAccess = GL_WRITE_ONLY_ARB;
			}
		}
	}

	void UEngineTexture::createManual(int _width, int _height, TextureUsage _usage, PixelFormat _format)
	{
		createManual(_width, _height, _usage, _format, 0);
	}

	void UEngineTexture::createManual(int _width, int _height, TextureUsage _usage, PixelFormat _format, void* _data)
	{
		MYGUI_PLATFORM_ASSERT(!mTextureID, "Texture already exist");

		//FIXME перенести в метод
		mInternalPixelFormat = 0;
		mPixelFormat = 0;
		mNumElemBytes = 0;
		if (_format == PixelFormat::L8)
		{
			mInternalPixelFormat = GL_LUMINANCE8;
			mPixelFormat = GL_LUMINANCE;
			mNumElemBytes = 1;
		}
		else if (_format == PixelFormat::L8A8)
		{
			mInternalPixelFormat = GL_LUMINANCE8_ALPHA8;
			mPixelFormat = GL_LUMINANCE_ALPHA;
			mNumElemBytes = 2;
		}
		else if (_format == PixelFormat::R8G8B8)
		{
			mInternalPixelFormat = GL_RGB8;
			mPixelFormat = GL_BGR;
			mNumElemBytes = 3;
		}
		else if (_format == PixelFormat::R8G8B8A8)
		{
			mInternalPixelFormat = GL_RGBA8;
			mPixelFormat = GL_BGRA;
			mNumElemBytes = 4;
		}
		else
		{
			MYGUI_PLATFORM_EXCEPT("format not support");
		}

		mWidth = _width;
		mHeight = _height;
		mDataSize = _width * _height * mNumElemBytes;
		setUsage(_usage);
		//MYGUI_PLATFORM_ASSERT(mUsage, "usage format not support");

		mOriginalFormat = _format;
		mOriginalUsage = _usage;

		// Set unpack alignment to one byte
		int alignment = 0;
		glGetIntegerv( GL_UNPACK_ALIGNMENT, &alignment );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

		// создаем тукстуру
		glGenTextures(1, &mTextureID);
		glBindTexture(GL_TEXTURE_2D, mTextureID);
		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, mInternalPixelFormat, mWidth, mHeight, 0, mPixelFormat, GL_UNSIGNED_BYTE, (GLvoid*)_data);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Restore old unpack alignment
		glPixelStorei( GL_UNPACK_ALIGNMENT, alignment );

		if (!_data && UEngineRenderManager::getInstance().isPixelBufferObjectSupported())
		{
			//создаем текстурнный буфер
			glGenBuffers(1, &mPboID);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mPboID);
			glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, mDataSize, 0, mUsage);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
		}
	}

	void UEngineTexture::destroy()
	{
		if (mRenderTarget != nullptr)
		{
			delete mRenderTarget;
			mRenderTarget = nullptr;
		}

		if (mTextureID != 0)
		{
			glDeleteTextures(1, &mTextureID);
			mTextureID = 0;
		}
		if (mPboID != 0)
		{
			glDeleteBuffers(1, &mPboID);
			mPboID = 0;
		}

		mWidth = 0;
		mHeight = 0;
		mLock = false;
		mPixelFormat = 0;
		mDataSize = 0;
		mUsage = 0;
		mBuffer = 0;
		mInternalPixelFormat = 0;
		mAccess = 0;
		mNumElemBytes = 0;
		mOriginalFormat = PixelFormat::Unknow;
		mOriginalUsage = TextureUsage::Default;
	}

	void* UEngineTexture::lock(TextureUsage _access)
	{
		MYGUI_PLATFORM_ASSERT(mTextureID, "Texture is not created");

		if (_access == TextureUsage::Read)
		{
			glBindTexture(GL_TEXTURE_2D, mTextureID);

			mBuffer = new unsigned char[mDataSize];
			glGetTexImage(GL_TEXTURE_2D, 0, mPixelFormat, GL_UNSIGNED_BYTE, mBuffer);

			mLock = false;

			return mBuffer;
		}

		// bind the texture
		glBindTexture(GL_TEXTURE_2D, mTextureID);
		if (!UEngineRenderManager::getInstance().isPixelBufferObjectSupported())
		{
			//Fallback if PBO's are not supported
			mBuffer = new unsigned char[mDataSize];
		}
		else
		{
			// bind the PBO
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mPboID);
			
			// Note that glMapBufferARB() causes sync issue.
			// If GPU is working with this buffer, glMapBufferARB() will wait(stall)
			// until GPU to finish its job. To avoid waiting (idle), you can call
			// first glBufferDataARB() with NULL pointer before glMapBufferARB().
			// If you do that, the previous data in PBO will be discarded and
			// glMapBufferARB() returns a new allocated pointer immediately
			// even if GPU is still working with the previous data.
			glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, mDataSize, 0, mUsage);

			// map the buffer object into client's memory
			mBuffer = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mAccess);
			if (!mBuffer)
			{
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
				glBindTexture(GL_TEXTURE_2D, 0);
				MYGUI_PLATFORM_EXCEPT("Error texture lock");
			}
		}

		mLock = true;

		return mBuffer;
	}

	void UEngineTexture::unlock()
	{
		if (!mLock && mBuffer)
		{
			delete mBuffer;
			mBuffer = 0;

			glBindTexture(GL_TEXTURE_2D, 0);

			return;
		}

		MYGUI_PLATFORM_ASSERT(mLock, "Texture is not locked");

		if (!UEngineRenderManager::getInstance().isPixelBufferObjectSupported())
		{
			//Fallback if PBO's are not supported
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, mPixelFormat, GL_UNSIGNED_BYTE, mBuffer);
			delete mBuffer;
		}
		else
		{
			// release the mapped buffer
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);

			// copy pixels from PBO to texture object
			// Use offset instead of ponter.
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, mPixelFormat, GL_UNSIGNED_BYTE, 0);

			// it is good idea to release PBOs with ID 0 after use.
			// Once bound with 0, all pixel operations are back to normal ways.
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
		}
		
		glBindTexture(GL_TEXTURE_2D, 0);
		mBuffer = 0;
		mLock = false;
	}

	void UEngineTexture::loadFromFile(const std::string& _filename)
	{
		destroy();

		if (mImageLoader)
		{
			int width = 0;
			int height = 0;
			PixelFormat format = PixelFormat::Unknow;

			void* data = mImageLoader->loadImage(width, height, format, _filename);
			if (data)
			{
				createManual(width, height, TextureUsage::Static | TextureUsage::Write, format, data);
				delete data;
			}
		}
	}

	void UEngineTexture::saveToFile(const std::string& _filename)
	{
		if (mImageLoader)
		{
			void* data = lock(TextureUsage::Read);
			mImageLoader->saveImage(mWidth, mHeight, mOriginalFormat, data, _filename);
			unlock();
		}
	}

	IRenderTarget* UEngineTexture::getRenderTarget()
	{
		if (mRenderTarget == nullptr)
			mRenderTarget = new UEngineRTTexture(mTextureID);

		return mRenderTarget;
	}

	unsigned int UEngineTexture::getTextureID() const
	{
		return mTextureID;
	}

} // namespace MyGUI
