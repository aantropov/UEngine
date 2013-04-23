/*!
	@file
	@author		Albert Semenov
	@date		09/2009
*/

#include "MyGUI_UEnginePlatform.h"
#include <assert.h>

namespace MyGUI
{

	UEnginePlatform::UEnginePlatform() :
		mIsInitialise(false)
	{
		mRenderManager = new UEngineRenderManager();
		mDataManager = new UEngineDataManager();
		mLogManager = new LogManager();
	}

	UEnginePlatform::~UEnginePlatform()
	{
		assert(!mIsInitialise);
		delete mRenderManager;
		delete mDataManager;
		delete mLogManager;
	}

	void UEnginePlatform::initialise(UEngineImageLoader* _loader, const std::string& _logName)
	{
		assert(!mIsInitialise);
		mIsInitialise = true;

		if (!_logName.empty())
			LogManager::getInstance().createDefaultSource(_logName);

		mRenderManager->initialise(_loader);
		mDataManager->initialise();
	}

	void UEnginePlatform::shutdown()
	{
		assert(mIsInitialise);
		mIsInitialise = false;

		mRenderManager->shutdown();
		mDataManager->shutdown();
	}

	UEngineRenderManager* UEnginePlatform::getRenderManagerPtr()
	{
		assert(mIsInitialise);
		return mRenderManager;
	}

	UEngineDataManager* UEnginePlatform::getDataManagerPtr()
	{
		assert(mIsInitialise);
		return mDataManager;
	}

} // namespace MyGUI
