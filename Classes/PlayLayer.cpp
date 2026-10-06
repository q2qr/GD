#include "PlayLayer.h"
#include "SimpleAudioEngine.h"
#include "GameManager.h"
#include "AppDelegate.h"
#include "LevelTools.h"
#include "PauseLayer.h"
#include "ObjectToolbox.h"
#include "CheckpointObject.h"
#include "GameStatsManager.h"
#include "RetryLevelLayer.h"
#include "GameSoundManager.h"
#include <cmath>
#include <algorithm>
using namespace CocosDenshion;
USING_NS_CC;

// to whoever finds this code, please don't try to fix it.
// forget about it, click off of the github page, and never look back.
// i have made the grave mistake of doing the opposite and continuing to dig myself deeper into this rabbit hole of decompilation.

// i am alone on this barren earth.

static CCArray* splitString(std::string input) // 0x0018c210 (for some reason doesn't have a debug symbol)
{
	CCArray* array = CCArray::create();

	size_t start = 0;
	size_t end = input.find(";");

	while (end != std::string::npos)
	{
		std::string token = input.substr(start, end - start);
		if (!token.empty()) {
			array->addObject(CCString::create(token.c_str()));
		}
		start = end + 1;
		end = input.find(";", start);
	}

	if (start < input.size())
	{
		std::string token = input.substr(start);
		if (!token.empty()) {
			array->addObject(CCString::create(token.c_str()));
		}
	}
	return array;
}

PlayLayer* PlayLayer::create(GJGameLevel* level)
{
    PlayLayer* pRet = new PlayLayer();
    if (pRet && pRet->init(level))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

PlayLayer::PlayLayer()
{
	m_practiceMode = false;
	m_activeGColorAction = nullptr;
	m_showingHint = false;

	m_levelLength = 0.0f;
	m_realLevelLength = 0.0f;
	m_lastRunPercent = 0;
}

void PlayLayer::onQuit()
{
    this->stopAllActions();
    this->unscheduleAllSelectors();
	SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(false);
	GameManager::sharedState()->returnToLastScene(PLAY_LAYER->getLevel());
	GameManager::sharedState()->fadeInMusic("menuLoop.mp3");
}

void PlayLayer::onExit()
{
	AppDelegate* AppDel = AppDelegate::get();
	if (!AppDel->getPaused())
		AppDel->setPaused(true);
		this->CCLayer::onExit();
}

void PlayLayer::onEnterTransitionDidFinish()
{
	AppDelegate::get()->setPaused(false);
	CCLayer::onEnterTransitionDidFinish();
}

CCScene* PlayLayer::scene(GJGameLevel* level)
{
    CCScene *scene = CCScene::create();
    AppDelegate* pApp = AppDelegate::get();
	pApp->setScenePointer(scene);
    PlayLayer* layer = PlayLayer::create(level);
    scene->addChild(layer);
    scene->setObjType(CCObjectType::PlayLayer);
    return scene;
}

void PlayLayer::createObjectsFromSetup(std::string setup)
{
	m_levelLength = 0.0f;
	m_realLevelLength = 0.0f;

	CCArray* parts = splitString(setup);

	m_levelSettings =
		LevelSettingsObject::objectFromString(
		static_cast<CCString*>(parts->objectAtIndex(0))->getCString()
		);
	m_levelSettings->retain();


	m_levelSettings->updateColors(
		m_player->getGlowColor1(),
		m_player->getGlowColor2());

	m_tintObjectsUseBlend = m_levelSettings->getTintObjectsUseBlend();



	for (unsigned i = 1; i < parts->count(); ++i)
	{
		const char* objStr =
			static_cast<CCString*>(parts->objectAtIndex(i))->getCString();

		GameObject* obj = GameObject::objectFromString(objStr);
		
		if (obj)
		{
			obj->setVisible(!obj->getIsInvisible());

			//if (!obj->getBlendAdditive())


			//obj->setObjectParent(m_batchNode);
			
			int key = obj->getObjectKey();
			if (key == 29 || key == 30 || key == 104 || key == 105 || key == 221)
				m_colorTriggers.push_back(obj);
			if (obj->getPosition().x > m_realLevelLength)
				m_realLevelLength = obj->getPosition().x;
			this->addToSection(obj);
			(obj->getBlendAdditive() ? m_batchNodeAdd : m_batchNode)->addChild(obj);
			if (obj->getColorSprite()) m_gameLayer->addChild(obj->getColorSprite(), 2);

		}

	}

	// The decompiled setup parser omitted the original length bookkeeping. The furthest
	// parsed object gives us a safe progress denominator for the loaded level geometry.
	m_levelLength = m_realLevelLength;
	std::stable_sort(m_colorTriggers.begin(), m_colorTriggers.end(),
		[](GameObject* a, GameObject* b) { return a->getSpawnXPos() < b->getSpawnXPos(); });
}

bool PlayLayer::init(GJGameLevel* level)
{
    if (!CCLayer::init())
        return false;

	bool recordGP = GameManager::sharedState()->getRecordGameplay();
	m_testMode = false;

#pragma region Variables
	float screenRight = CCDirector::sharedDirector()->getScreenRight();
	float screenLeft = CCDirector::sharedDirector()->getScreenLeft();
	float screenTop = CCDirector::sharedDirector()->getScreenTop();
	float screenBottom = CCDirector::sharedDirector()->getScreenBottom();
	float screenMiddle = screenRight * 0.5f;

	m_playbackMode = false;
	m_localLevel = level->getLevelType() == GJLevelType::LocalLevel;
	m_activeEnterEffect = EnterEffect::unk1;
	m_startPos = ccp(0.0f, 105.0f);
	m_attempts = 0;  
	unk_0x220 = 0.0;
	m_jumps = 0;
	m_realLevelLength = 0.0f;
	m_clkTimer = 0.0f;
	m_endTriggered = false;
	m_isResetting = true;

	GameManager::sharedState()->setEditMode(false);
	GameManager::sharedState()->setPlayLayer(this);
	GameManager::sharedState()->setWasHigh(false);

	m_level = level;
	level->retain();

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	m_particlesDictionary = CCDictionary::create();
	m_particlesDictionary->retain();

	unk_0x1e0 = CCDictionary::create();
	unk_0x1e0->retain();

	m_gameLayer = CCLayer::create();
	this->addChild(m_gameLayer, 1);

	unk_0x184 = CCDictionary::create();
	unk_0x184->retain();

	m_checkpoints = CCArray::create();
	m_checkpoints->retain();

	unk_0x130 = CCArray::create();
	unk_0x130->retain();
	
	unk_0x134 = CCArray::create();
	unk_0x134->retain();

	m_effectObjects = CCArray::create();
	m_effectObjects->retain();

	m_hazardsArray = CCArray::create();
	m_hazardsArray->retain();

	m_activeObjects = CCArray::create();
	m_activeObjects->retain();

	unk_0x178 = CCArray::create();
	unk_0x178->retain();

	unk_0x170 = CCArray::create();
	unk_0x170->retain();

	m_stateObjects = CCArray::create();
	m_stateObjects->retain();

	m_bigActionContainer = CCArray::create();
	m_bigActionContainer->retain();

	field_0x1e4 = CCNode::create();
	this->addChild(field_0x1e4);
	field_0x1e4->setVisible(false);

	m_objColorRef = CCSprite::create();
	this->addChild(m_objColorRef);
	m_objColorRef->setVisible(false);

	field_0x1f4 = CCSprite::create();
	this->addChild(field_0x1f4);
	field_0x1f4->setVisible(false);

	field_0x1e8 = CCSprite::create();
	this->addChild(field_0x1e8);
	field_0x1e8->setVisible(false);

	m_gColorRef = CCSprite::create();
	this->addChild(m_gColorRef);
	m_gColorRef->setVisible(false);
#pragma endregion

	CCTextureCache* pTextureCache = CCTextureCache::sharedTextureCache();
	CCTexture2D* texture = pTextureCache->addImage("GJ_GameSheet.png");
	m_batchNode = CCSpriteBatchNode::createWithTexture(texture, 29);
	m_gameLayer->addChild(m_batchNode, 1);

	m_batchNodeAdd = CCSpriteBatchNode::createWithTexture(texture, 29);
	ccBlendFunc blendFunc = { GL_SRC_ALPHA, GL_ONE };
	m_batchNodeAdd->setBlendFunc(blendFunc);
	m_gameLayer->addChild(m_batchNodeAdd, 0);

	m_batchNodeBottom = CCSpriteBatchNode::createWithTexture(texture, 29);
	m_gameLayer->addChild(m_batchNodeBottom, -1);

	m_flyGroundTop = GJFlyGroundLayer::create();
	this->addChild(m_flyGroundTop, 5);
	m_flyGroundBottom = GJFlyGroundLayer::create();
	this->addChild(m_flyGroundBottom, 5);

	float randVal = rand();
	unk_0x1b8 = roundf((randVal / RAND_MAX) + (randVal / RAND_MAX)) + 1;
	
	m_glitter = CCParticleSystemQuad::create("glitterEffect.plist");
	m_glitter->setPositionType(tCCPositionType::kCCPositionTypeFree);
	m_gameLayer->addChild(m_glitter, 0);
	m_glitter->setPosVar(CCPoint((SCREEN_SCALE_F_W * 480.0f) / 1.8f, (SCREEN_SCALE_F_H * 320.0f) * 0.5f));
	m_glitter->stopSystem();

#pragma region Player

	m_player = PlayerObject::create(GameManager::sharedState()->getPlayerFrame(),
		GameManager::sharedState()->getPlayerShip(), nullptr);
	m_player->setColor(GameManager::sharedState()->colorForIdx(GameManager::sharedState()->getPlayerColor()));
	m_player->setSecondColor(GameManager::sharedState()->colorForIdx(GameManager::sharedState()->getPlayerColor2()));
	m_player->updateGlowColor();
	m_batchNode->addChild(m_player, 10);

#pragma endregion

	m_sections = CCArray::create();
	m_sections->retain();

	this->createObjectsFromSetup(m_level->getLevelString());

	if (!m_levelSettings) {
		m_levelSettings = LevelSettingsObject::create();
		m_levelSettings->retain();
	}

	char const* bgSpriteFile = GameManager::sharedState()->getBGTexture(m_levelSettings->getBGIdx());
	m_background = CCSprite::create(bgSpriteFile);
	ccTexParams texParams = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
	m_background->getTexture()->setTexParameters(&texParams);
	this->addChild(m_background, -1);

	m_background->setAnchorPoint(ccp(0, 0));
	m_background->setScale(CCDirector::sharedDirector()->getScreenScaleFactorMax());
	ccBlendFunc bgBlendFunc = { GL_ONE, GL_ZERO };
	m_background->setBlendFunc(bgBlendFunc);
	m_background->setColor(ccc3(40, 125, 255));
	unk_0x15c = m_background->getTextureRect().size.height * m_background->getScale();
	CCRect bgRect = m_background->getTextureRect();
    bgRect.size.width *= ceilf(winSize.width / (bgRect.size.width * m_background->getScale())) + 1;
    bgRect.size.height *= ceilf(winSize.height / (bgRect.size.height * m_background->getScale())) + 1;
    m_background->setTextureRect(bgRect);

	m_ground = GJGroundLayer::create(m_levelSettings->getGIdx());
	m_gameLayer->addChild(m_ground, 4);
	m_rollGroundTop = GJGroundLayer::create(m_levelSettings->getGIdx());
	m_gameLayer->addChild(m_rollGroundTop, 4);
	m_rollGroundBottom = GJGroundLayer::create(m_levelSettings->getGIdx());
	m_gameLayer->addChild(m_rollGroundBottom, 4);
	m_rollGroundBottom->setScaleY(-1.0f);
	
	unk_0x140 = CCSprite::createWithSpriteFrameName("whiteSquare60_001.png");
	unk_0x140->setAnchorPoint(ccp(1, 0));
	unk_0x140->setBlendFunc(bgBlendFunc);
	// unk_0x140->setScaleX((winSize.width + 20.0f) / m_progressBar->unk_0x138);
	/*fVar6 = ((local_124 - 320.0) * 0.5 + 10.0 + 20.0) / m_progressBar->0x13c);
	uVar28 = (**(code **)(*(int *)this->field287_0x140 + 0x48))(this->field287_0x140, fVar6);*/
	unk_0x140->setPosition(ccp(-10, 0));
	unk_0x140->setColor(ccc3(0, 102, 255));
	m_flyGroundTop->addChild(unk_0x140, 5);

	// missing code

	m_uiLayer = UILayer::create();
	this->addChild(m_uiLayer, 10);

	// add other missing code

	m_player->setPosition(m_startPos);

	SimpleAudioEngine* SAE = SimpleAudioEngine::sharedEngine();
	SAE->stopBackgroundMusic(false);
	int audioTrack = m_level->getAudioTrack();
	char const* audioFile = LevelTools::getAudioFileName(audioTrack);
	SAE->preloadBackgroundMusic(audioFile);
	// std::string audioStr = LevelTools::getAudioString(audioTrack);
	// m_audioEffectsLayer = AudioEffectsLayer::create(audioStr);
	// field_0x13c->addChild(m_audioEffectsLayer, 1);
	// m_audioEffectsLayer->setVisible(false);

	m_attemptLabel = CCLabelBMFont::create("Attempt 1", "bigFont.fnt");
	m_gameLayer->addChild(m_attemptLabel, 3);

	runAction(CCSequence::create(
		CCDelayTime::create(1.0f),
		CCCallFunc::create(this, callfunc_selector(PlayLayer::startGame)), nullptr));

	m_cleanReset = true;
	unk_0x1a9 = true;
	this->updateCamera(0.0f);
	m_attemptLabel->setPosition(ccp(m_cameraPos.x + winSize.width * 0.5f, m_cameraPos.y + (winSize.height * 0.5f) + 125.0f));
	
	m_progressBar = CCSprite::create("slidergroove2.png");
	this->addChild(m_progressBar, 10);
	m_progressFill = CCSprite::create("sliderBar2.png");
	unk_0x204 = 8.0f;
	// unk_0x200 = m_progressBar->isDirty() - 4.0f;
	ccTexParams texParams2 = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
	m_progressFill->getTexture()->setTexParameters(&texParams);
	m_progressFill->setColor(m_player->getGlowColor1());
	m_progressBar->addChild(m_progressFill, -1);

	m_progressFill->setAnchorPoint(ccp(0.0f, 0.0f));
	m_progressFill->setPosition(ccp(2.0f, 4.0f));
	m_progressBar->setPosition(ccp(winSize.width * 0.5, winSize.height - 8.0));

	updateProgressbar();
	toggleProgressbar();
	m_player->setVisible(m_testMode);

	tintBackground(m_levelSettings->getStartBGColor(), 0.0f);
	tintGround(m_levelSettings->getStartGColor(), 0.0f);
	tintLine(m_levelSettings->getStartLineColor(), 0.0f);
	tintObjects(m_levelSettings->getStartObjColor(), 0.0f);
	tintColorObjects(m_levelSettings->getStartTintObjColor(), 0.0f);

	updateLevelColors();
	animateOutFlyGround(true);
	animateOutRollGround(true);

	m_player->togglePlayerScale(m_levelSettings->getStartMiniMode());
	int startMode = m_levelSettings->getStartMode();

	if (startMode == 2)
		switchToRollMode(nullptr, true);
	else if (startMode == 3)
		switchToFlyMode(nullptr, true, true);
	/*else
		switchToFlyMode(nullptr, true, false);*/
		
	unk_0x120 = true;
	updateVisibility();
	updateCamera(0.0f);
	toggleAudioRain(false);
	toggleGlitter(false);
	GameManager::sharedState()->resetMusic();
    return true;
}

void PlayLayer::resetLevel()
{
	unk_0x220 = 0;
	m_resetQueued = false;
	m_isResetting = true;
	m_showingEndLayer = false;
	m_endTriggered = false;
	m_didAwardStars = false;
	m_didJump = false;

	unk_0x214 = "";
	unk_0x20c = 0;

	m_uiLayer->enableMenu();

	// this->stopCameraShake();
	m_activeEnterEffect = EnterEffect::unk1;

	this->stopActionByTag(10);
	this->stopActionByTag(11);

	m_cameraMovingX = false;
	m_cameraMovingY = false;

	this->toggleGlitter(false);

	m_playerDead = false;
	unk_0x1a9 = m_cleanReset;
	// this->clearPickedUpItems();

	unk_0x130->removeAllObjects();
	unk_0x1e0->removeAllObjects();

	// unk_0x178 is unused so far so this will be unused for now
	/*for (int i = 0; i < unk_0x178->count(); i++) {
		piVar12 = unk_0x178->objectAtIndex(i);
		piVar12->resetObject();
		piVar12->setEnterEffect(EnterEffect::unk1);
	}*/

	m_flipValue = 0.0;
	// unk_0x1d4 = 0.0;
	m_isFlipped = false;
	// field337_0x1d8 = 1.0;
	this->stopActionByTag(14);

	m_cameraPortal = nullptr;
	//m_audioEffectsLayer->resetAudioVars();
	for (unsigned section = 0; section < m_sections->count(); ++section) {
        CCArray* objects = (CCArray*)m_sections->objectAtIndex(section);
        for (unsigned i = 0; i < objects->count(); ++i)
            ((GameObject*)objects->objectAtIndex(i))->resetObject();
    }
    m_player->resetObject();
	m_nextColorTrigger = 0;
	m_tintObjectsUseBlend = m_levelSettings->getTintObjectsUseBlend();
	tintBackground(m_levelSettings->getStartBGColor(), 0.0f);
	tintGround(m_levelSettings->getStartGColor(), 0.0f);
	tintLine(m_levelSettings->getStartLineColor(), 0.0f);
	tintObjects(m_levelSettings->getStartObjColor(), 0.0f);
	tintColorObjects(m_levelSettings->getStartTintObjColor(), 0.0f);
	updateLevelColors();
	this->animateOutFlyGround(true);
	this->animateOutRollGround(true);

	m_realPlayerPos = m_player->getPosition();
	this->updateCamera(0.0f);
	this->updateVisibility();
    updateAttempts();
	m_isResetting = false;
}

void PlayLayer::fullReset()
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	m_clkTimer = 0.0;
	unk_0x220 = 0.0;
	unk_0x120 = true;
	m_cleanReset = true;
	m_attempts = 0;
	m_jumps = 0;

	if (m_practiceMode)
		togglePracticeMode(false);
	else
		resetLevel();

	m_attemptLabel->setPosition(ccp(m_cameraPos.x + winSize.width * 0.5f, (m_cameraPos.y + winSize.height * 0.5f) + 125.0f));
}

void PlayLayer::delayedResetLevel()
{
	if (m_resetQueued)
		resetLevel();
}

void PlayLayer::showRetryLayer()
{
	m_showingEndLayer = true;
	RetryLevelLayer* retryLayer = RetryLevelLayer::create();
	this->addChild(retryLayer, 100);
	retryLayer->showLayer(false);
}

void PlayLayer::startGame()
{
    scheduleUpdate();
	m_cleanReset = true;
	m_player->setVisible(true);
	this->resetLevel();
}

void PlayLayer::pauseGame()
{
	if (!m_endTriggered && !AppDelegate::get()->getPaused()) {	
		m_player->releaseButton(PlayerButton::Jump);
		SimpleAudioEngine::sharedEngine()->pauseAllEffects();
		SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
		PauseLayer* pauseScreen = PauseLayer::create();
		this->getParent()->addChild(pauseScreen, 10);
		this->onExit();
		pauseScreen->customSetup();
	}
}

// updates
void PlayLayer::update(float dt)
{
	float step = dt * 60.0;

	if (!m_player->getIsLocked())
		m_player->setPosition(m_realPlayerPos);

		m_player->setTouchedRing(nullptr);

	for (int i = 0; i < m_stateObjects->count(); ++i)
		((GameObject*)(m_stateObjects->objectAtIndex(i)))->setStateVar(false);

	for (int i = 0; i < m_activeObjects->count(); ++i)
		m_activeObjects->objectAtIndex(i)->update(step);
	
	// Resolve contacts between small movement steps to avoid tunnelling at low frame rates.
    int substeps = (int)ceilf(step * 4.0f);
    substeps = MAX(1, MIN(substeps, 120));
    for (int sub = 0; sub < substeps && !m_player->getIsDead(); ++sub) {
        m_player->update(step / substeps);
        this->checkCollisions(step / substeps);
    }
	if (!m_player->getOnGround()) m_player->deactivateParticle();
	while (!m_playerDead && m_nextColorTrigger < m_colorTriggers.size() &&
		m_colorTriggers[m_nextColorTrigger]->getSpawnXPos() <= m_player->getPosition().x) {
		GameObject* trigger = m_colorTriggers[m_nextColorTrigger++];
		if (!trigger->getTouchTriggered()) trigger->triggerObject();
	}

	if (m_player->isFlying())
		m_player->updateShipRotation(step);

	for (int i = 0; i < m_stateObjects->count(); i = i + 1) {
		((GameObject *)m_stateObjects->objectAtIndex(i))->updateState();
	}

	// weird
	bool isUnlocked = m_player->getIsLocked();
	CCPoint newPlayerPos;
	if (!isUnlocked) {
		m_realPlayerPos = m_player->getPosition();
		float flipVal = m_flipValue;
		if ((flipVal != 0.0f) && (flipVal != 1.0f))
			if (flipVal == -1.0f)
				flipVal = 1.0f - flipVal;
		newPlayerPos = m_player->getPosition() + ccp(flipVal * 150.0f, 0.0f);
		isUnlocked = true;
	}
	else {
		isUnlocked = false;
	}

	updateCamera(step);
	updateVisibility();
	//checkSpawnObjects();
	m_clkTimer += dt;
	//m_audioEffectsLayer->audioStep(dt);
	updateLevelColors();
	if (isUnlocked)
		m_player->setPosition(newPlayerPos);
	updateProgressbar();
	updateEffectPositions();
}


void PlayLayer::updateAttempts()
{
	m_attempts++;
	GM->setTotalAttempts(GM->getTotalAttempts() + 1);
	char const* attemptString = CCString::createWithFormat("Attempt %i", m_attempts)->getCString();
	m_attemptLabel->setString(attemptString);

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	if (m_attempts != 1)
		m_attemptLabel->setPosition(ccp(m_cameraPos.x + (winSize.width * 0.5f) + 50.0f, m_cameraPos.y + (winSize.height * 0.5f) + 125.0f));
}

void PlayLayer::updateCamera(float dt)
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	float screenHeight = winSize.height;
	CCPoint camPos = m_cameraPos;
	CCPoint playerPos = m_player->getPosition();

	float targetY = camPos.y;

	if (playerPos.y > camPos.y + 120.0f) {
		targetY = playerPos.y - 120.0f;
	}

	if (playerPos.y < camPos.y + 90.0f) {
		targetY = playerPos.y - 90.0f;
	}

	if (dt > 0) camPos.y += (targetY - camPos.y) * MIN(dt / 10.0f, 1.0f);

	float maxY = 1740.0f - screenHeight;
	if (camPos.y < 0.0f) camPos.y = 0.0f;
	else if (camPos.y > maxY) camPos.y = maxY;

	camPos.x = playerPos.x - 125.0f;

	m_cameraPos = camPos;

	CCCamera* camera = m_gameLayer->getCamera();
	camera->setCenterXYZ(camPos.x, camPos.y, 0.0f);
	camera->setEyeXYZ(camPos.x, camPos.y, camera->getZEye());

    m_ground->updateScroll(camPos.x);
    // The background is outside the world camera; emulate its slower parallax.
    CCSize tile = m_background->getTexture()->getContentSize();
    float tileWidth = tile.width * m_background->getScaleX();
    float tileHeight = tile.height * m_background->getScaleY();
    float phaseX = tileWidth > 0 ? fmodf(fmodf(camPos.x * 0.1f, tileWidth) + tileWidth, tileWidth) : 0;
    float phaseY = tileHeight > 0 ? fmodf(fmodf(camPos.y * 0.1f, tileHeight) + tileHeight, tileHeight) : 0;
    m_background->setPosition(ccp(-phaseX, -phaseY));
}

void PlayLayer::updateProgressbar()
{
    if (!m_progressBar || !m_progressFill) return;
    m_progressBar->setVisible(GM->getShowProgressBar());
    float fraction = m_levelLength > 0 ? m_realPlayerPos.x / m_levelLength : 0;
    fraction = MAX(0.0f, MIN(1.0f, fraction));
    m_progressFill->setScaleX((m_progressBar->getContentSize().width - 4.0f) * fraction / m_progressFill->getContentSize().width);
}

void PlayLayer::updateEffectPositions()
{
    
}

void PlayLayer::updateLevelColors()
{
	m_ground->getGroundSprite()->setColor(m_gColorRef->getColor());
	m_rollGroundTop->getGroundSprite()->setColor(m_gColorRef->getColor());
	m_rollGroundBottom->getGroundSprite()->setColor(m_gColorRef->getColor());
	m_rollGroundTop->getLine()->setColor(m_ground->getLine()->getColor());
	m_rollGroundBottom->getLine()->setColor(m_ground->getLine()->getColor());
	unk_0x140->setColor(m_gColorRef->getColor());
	for (unsigned section = 0; section < m_sections->count(); ++section) {
		CCArray* objects = (CCArray*)m_sections->objectAtIndex(section);
		for (unsigned i = 0; i < objects->count(); ++i) {
			GameObject* obj = (GameObject*)objects->objectAtIndex(i);
			ccColor3B color = obj->getIsTintObject() ? m_objColorRef->getColor() : ccc3(255, 255, 255);
			if (obj->getUsePlayerColor()) color = m_player->getGlowColor1();
			else if (obj->getUsePlayerColor2()) color = m_player->getGlowColor2();
			else if (obj->getUseBGColor()) color = m_background->getColor();
			obj->setColor(color);
			if (obj->getColorSprite()) {
				obj->getColorSprite()->setPosition(obj->getPosition());
				obj->getColorSprite()->setRotation(obj->getRotation());
				obj->getColorSprite()->setScaleX(obj->getScaleX());
				obj->getColorSprite()->setScaleY(obj->getScaleY());
				obj->getColorSprite()->setFlipX(obj->isFlipX());
				obj->getColorSprite()->setFlipY(obj->isFlipY());
				obj->getColorSprite()->setVisible(obj->isVisible());
				obj->getColorSprite()->setOpacity(obj->getOpacity());
				obj->getColorSprite()->setColor(field_0x1f4->getColor());
				obj->getColorSprite()->setBlendFunc(m_tintObjectsUseBlend ? ccBlendFunc{GL_SRC_ALPHA, GL_ONE} : ccBlendFunc{GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
			}
		}
	}
}

void PlayLayer::tintBackground(ccColor3B color, float duration)
{
	m_background->stopAllActions();
	if (duration <= 0.0f) m_background->setColor(color);
	else m_background->runAction(CCTintTo::create(duration, color.r, color.g, color.b));
}

void PlayLayer::tintGround(ccColor3B color, float duration)
{
	m_gColorRef->stopAllActions();
	ColorAction* cAction = ColorAction::create(this->getGColor(), color, duration, m_clkTimer);
	this->setActiveGColorAction(cAction);

	if (duration <= 0.0f)
		m_gColorRef->setColor(color);
	else {
		CCTintTo* tintAction = CCTintTo::create(duration, color.r, color.g, color.b);
		m_gColorRef->runAction(tintAction);
	}
}

void PlayLayer::tintLine(ccColor3B color, float duration)
{
	CCSprite* line = m_ground->getLine();
	line->stopAllActions();
	if (duration <= 0.0f) line->setColor(color);
	else line->runAction(CCTintTo::create(duration, color.r, color.g, color.b));
}

void PlayLayer::tintObjects(ccColor3B color, float duration)
{
	m_objColorRef->stopAllActions();
	if (duration <= 0.0f) m_objColorRef->setColor(color);
	else m_objColorRef->runAction(CCTintTo::create(duration, color.r, color.g, color.b));
}

void PlayLayer::tintColorObjects(ccColor3B color, float duration)
{
	field_0x1f4->stopAllActions();
	if (duration <= 0.0f) field_0x1f4->setColor(color);
	else field_0x1f4->runAction(CCTintTo::create(duration, color.r, color.g, color.b));
}

ccColor3B PlayLayer::getLineColor()
{
	return m_ground->getLine()->getColor();
}

ccColor3B PlayLayer::getGColor()
{
	return m_ground->getGroundSprite()->getColor();
}


void PlayLayer::setActiveGColorAction(ColorAction* action)
{
	if (m_activeGColorAction != action) {
		if (action != nullptr)
			action->retain();

		if (m_activeGColorAction != nullptr)
			m_activeGColorAction->release();

			m_activeGColorAction = action;
	}
}


// toggles
void PlayLayer::toggleGlitter(bool visible)
{
	if (GameManager::sharedState()->getPerformanceMode())
		return;

	if (visible)
		m_glitter->resumeSystem();
	else
		m_glitter->stopSystem();
}

void PlayLayer::togglePracticeMode(bool practice)
{
	if (m_practiceMode == practice)
		return;

	m_practiceMode = practice;
	m_uiLayer->toggleCheckpointsMenu(practice);
	if (practice) {
		SimpleAudioEngine* SAE = SimpleAudioEngine::sharedEngine();
		SAE->pauseBackgroundMusic();
		SAE->playBackgroundMusic("StayInsideMe.mp3", true);
		return;
	}
	//while (int idx = m_checkpoints->count(), idx != 0) {
		// removeLastCheckpoint();
	//}
	m_cleanReset = true;
	resetLevel();
}

void PlayLayer::toggleProgressbar()
{
	m_progressBar->setVisible(GameManager::sharedState()->getShowProgressBar());
}

void PlayLayer::toggleAudioRain(bool toggle)
{
	// m_audioEffectsLayer->setRainActive(toggle);
}

void PlayLayer::registerStateObject(GameObject* obj)
{
	if (!m_stateObjects->containsObject(obj)) {
		m_stateObjects->addObject(obj);
	}
}

void PlayLayer::resume()
{
	AppDelegate* pApp = AppDelegate::get();
	GameManager* pGameManager = GameManager::sharedState();
	SimpleAudioEngine* SAE = SimpleAudioEngine::sharedEngine();

	if (!pApp->getPaused()) {
		return;
	}

	pApp->setPaused(false);
	this->onEnter();
	SAE->resumeAllEffects();
	if (((pGameManager->getRecordGameplay() != false) && (!m_practiceMode)) && (!m_testMode)) {
		//this->tryStartRecord();
	}
	if (!m_practiceMode) {
		if (m_player->getPosition().x <= 0.0f) {
			return;
		}
		if (!SAE->isBackgroundMusicPlaying()) {
			char const* audioFile = LevelTools::getAudioFileName(m_level->getAudioTrack());
			SAE->playBackgroundMusic(audioFile, false);
		}
		//SAE->setBackgroundMusicTime(timeForXPos(m_player->getPosition().x, false));
	}
	SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
	return;
}

void PlayLayer::resumeAndRestart()
{
	AppDelegate* pApp = AppDelegate::get();
	if (pApp->getPaused()) {
		pApp->setPaused(false);
		this->onEnter();
		this->resetLevel();
	}
}

void PlayLayer::updateVisibility()
{

}

void PlayLayer::addToSection(GameObject* obj)
{
	unsigned int targetSection = sectionForPos(obj->getPosition());
	if (m_sections->count() < targetSection + 1) {
		while (m_sections->count() < targetSection + 1) {
			m_sections->addObject(CCArray::create());
		}
	}
	CCArray* section = (CCArray*)m_sections->objectAtIndex(targetSection);
	section->addObject(obj);
	obj->setSectionIdx(targetSection);
}

void PlayLayer::removeObjectFromSection(GameObject* obj)
{
	m_sections->removeObject(m_sections->objectAtIndex(obj->getSectionIdx()), true);
}

void PlayLayer::switchToFlyMode(GameObject* obj, bool param_1, bool param_2)
{
	this->exitRollMode();

	if (obj) {
		m_player->setPortalP(obj->getPosition());
		m_player->setPortalObject(obj);
		m_cameraPortal = obj;
	}

	if (param_2)
        m_player->toggleBirdMode(true);
    else
        m_player->toggleFlyMode(true);

	this->toggleGlitter(true);

	// incomplete
}

void PlayLayer::switchToRollMode(GameObject* obj, bool)
{

}

void PlayLayer::exitAirMode()
{
	this->toggleGlitter(false);
	this->animateOutFlyGround(false);
	m_cameraMovingY = true;
}

void PlayLayer::exitBirdMode()
{
	m_player->toggleBirdMode(false);
	this->exitAirMode();
}

void PlayLayer::exitFlyMode()
{
	m_player->toggleFlyMode(false);
	this->exitAirMode();
}

void PlayLayer::exitRollMode()
{
	m_player->toggleRollMode(false);
	this->animateOutRollGround(false);
}

void PlayLayer::animateInFlyGround(bool instant)
{
	if (!m_flyGroundActive) {
		m_flyGroundActive = true;
		m_flyGroundTop->deactivateGround();
		m_flyGroundBottom->deactivateGround();
		m_flyGroundTop->setVisible(true);
		m_flyGroundBottom->setVisible(true);

		if (!instant) {
			CCMoveTo* moveAction = CCMoveTo::create(0.5f, ccp(0.0f, unk_0x1a0));
			CCEaseInOut* easeMove = CCEaseInOut::create(moveAction, 2.0f);

			CCMoveTo* moveAction2 = CCMoveTo::create(0.5f, ccp(0.0f, unk_0x1a4));
			CCEaseInOut* easeMove2 = CCEaseInOut::create(moveAction2, 2.0f);

			m_flyGroundTop->runAction(easeMove);
			m_flyGroundBottom->runAction(easeMove2);

			m_flyGroundTop->fadeOutGround(0.5f);
			m_flyGroundBottom->fadeOutGround(0.5f);
		}
		else {
			m_flyGroundTop->setPosition(ccp(0.0f, unk_0x1a0));
			m_flyGroundBottom->setPosition(ccp(0.0f, unk_0x1a4));
			m_flyGroundTop->showGround();
			m_flyGroundBottom->showGround();
		}
	}
}

void PlayLayer::animateOutFlyGround(bool instant)
{
	/*m_flyGroundActive = false;
	CCPoint groundTopPos = ccp(0.0f, CCDirector::sharedDirector()->getScreenBottom() - 2.0f);
	CCPoint groundBottomPos = ccp(0.0f, CCDirector::sharedDirector()->getScreenTop() + 2.0f);
	m_flyGroundTop->deactivateGround();
	m_flyGroundBottom->deactivateGround();

	if (instant) {
		animateOutFlyGroundFinished();
		m_flyGroundTop->setPosition(groundTopPos);
		m_flyGroundBottom->setPosition(groundBottomPos);
	}
	else {
		CCMoveTo* moveAction = CCMoveTo::create(0.4f, groundBottomPos);
		CCEaseInOut* easeMove = CCEaseInOut::create(moveAction, 1.5f);

		CCMoveTo* moveAction2 = CCMoveTo::create(0.4f, groundBottomPos);
		CCEaseInOut* easeMove2 = CCEaseInOut::create(moveAction2, 1.5f);

		CCDelayTime* delay = CCDelayTime::create(0.6f);
		CCSequence* doneSequence = CCSequence::create(delay, CCCallFunc::create(this, callfunc_selector(PlayLayer::animateOutFlyGroundFinished)), nullptr);

		m_flyGroundTop->runAction(easeMove);
		m_flyGroundBottom->runAction(easeMove2);
		m_flyGroundBottom->runAction(doneSequence);

		m_flyGroundTop->fadeOutGround(0.4f);
		m_flyGroundBottom->fadeOutGround(0.4f);
	}*/
}

void PlayLayer::animateOutFlyGroundFinished()
{
	m_flyGroundTop->setVisible(false);
	m_flyGroundBottom->setVisible(false);
}

void PlayLayer::animateInRollGround(bool instant)
{
	if (!m_rollGroundActive) {
		m_rollGroundActive = true;
		m_rollGroundTop->setVisible(true);
		m_rollGroundBottom->setVisible(true);
		m_rollGroundTop->deactivateGround();
		m_rollGroundBottom->deactivateGround();

		if (!instant) {
			CCMoveTo* moveAction = CCMoveTo::create(0.5f, ccp(0.0f, unk_0x190));
			CCEaseInOut* easeMove = CCEaseInOut::create(moveAction, 2.0f);

			CCMoveTo* moveAction2 = CCMoveTo::create(0.5f, ccp(0.0f, unk_0x194));
			CCEaseInOut* easeMove2 = CCEaseInOut::create(moveAction2, 2.0f);

			m_rollGroundTop->runAction(easeMove);
			m_rollGroundBottom->runAction(easeMove2);

			m_rollGroundTop->fadeOutGround(0.5f);
			m_rollGroundBottom->fadeOutGround(0.5f);
		}
		else {
			m_rollGroundTop->setPosition(ccp(0.0f, unk_0x190));
			m_rollGroundBottom->setPosition(ccp(0.0f, unk_0x194));
			m_rollGroundTop->showGround();
			m_rollGroundBottom->showGround();
		}
	}
}

void PlayLayer::animateOutRollGround(bool instant)
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	m_rollGroundActive = false;
	float groundYPos = m_rollGroundTop->getGroundSprite()->getPosition().y;
	CCPoint groundTopPos = ccp(0.0f, (CCDirector::sharedDirector()->getScreenBottom() - 2.0f) - groundYPos);
	CCPoint groundBottomPos = ccp(0.0f, (CCDirector::sharedDirector()->getScreenTop() + 2.0f) - (winSize.height - groundYPos));
	m_rollGroundTop->deactivateGround();
	m_rollGroundBottom->deactivateGround();

	if (instant) {
		animateOutRollGroundFinished();
		m_rollGroundTop->setPosition(groundTopPos);
		m_rollGroundBottom->setPosition(groundBottomPos);
	}
	else {
		CCMoveTo* moveAction = CCMoveTo::create(0.4f, groundTopPos);
		CCEaseInOut* easeMove = CCEaseInOut::create(moveAction, 1.5f);

		CCMoveTo* moveAction2 = CCMoveTo::create(0.4f, groundTopPos);
		CCEaseInOut* easeMove2 = CCEaseInOut::create(moveAction2, 1.5f);

		CCDelayTime* delay = CCDelayTime::create(0.6f);
		CCSequence* doneSequence = CCSequence::create(delay, CCCallFunc::create(this, callfunc_selector(PlayLayer::animateOutRollGroundFinished)), nullptr);
		
		m_rollGroundTop->runAction(easeMove);
		m_rollGroundBottom->runAction(easeMove2);
		m_rollGroundBottom->runAction(doneSequence);

		m_rollGroundTop->fadeOutGround(0.4f);
		m_rollGroundBottom->fadeOutGround(0.4f);
	}
}

void PlayLayer::animateOutRollGroundFinished()
{
	m_rollGroundTop->setVisible(false);
	m_rollGroundBottom->setVisible(false);
}

void PlayLayer::checkCollisions(float dt)
{
    const float floorY = 90.0f + 15.0f * m_player->getPlayerScale();
    if (!m_player->isFlying() && m_player->getPosition().y <= floorY) {
        if (m_player->getGravityFlipped()) { destroyPlayer(); return; }
        m_player->setPosition(ccp(m_player->getPosition().x, floorY));
        m_player->hitGround(false);
    }
    if (m_player->getPosition().y > 1890.0f) { destroyPlayer(); return; }
    int section = sectionForPos(m_player->getPosition());
    for (int idx = MAX(0, section - 1); idx <= section + 1 && idx < (int)m_sections->count(); ++idx) {
        CCArray* objects = (CCArray*)m_sections->objectAtIndex(idx);
        for (unsigned i = 0; i < objects->count(); ++i) {
            GameObject* obj = (GameObject*)objects->objectAtIndex(i);
            if (obj->getTouchTriggered() && !obj->getHasBeenActivated() &&
                m_player->getObjectRect().intersectsRect(obj->getObjectRect())) obj->triggerObject();
            if (obj->getIsSleeping() || obj->getIsDisabled()) continue;
            if (!m_player->getObjectRect().intersectsRect(obj->getObjectRect())) continue;
            switch (obj->getType()) {
                case Hazard: destroyPlayer(); return;
                case None: case unk22: m_player->collidedWithObject(dt, obj); break;
                case NormalGravityPortal: m_player->flipGravity(false, false); break;
                case InvertGravityPortal: m_player->flipGravity(true, false); break;
                case ShipPortal: if (!m_player->getFlyMode()) switchToFlyMode(obj, false, false); break;
                case CubePortal: exitFlyMode(); exitBirdMode(); exitRollMode(); break;
                case SmallPortal: m_player->togglePlayerScale(true); break;
                case BigPortal: m_player->togglePlayerScale(false); break;
                case BallPortal: if (!m_player->getRollMode()) { exitFlyMode(); exitBirdMode(); m_player->toggleRollMode(true); } break;
                case YellowPad: case GravityPad: case PinkPad:
                    if (!obj->getHasBeenActivated()) {
                        obj->triggerActivated();
                        if (obj->getType() == GravityPad) m_player->flipGravity(!m_player->getGravityFlipped(), false);
                        m_player->m_yVelocity = (obj->getType() == PinkPad ? 10.0f : 15.0f) * m_player->flipMod();
                        m_player->setOnGround(false);
                    }
                    break;
                case YellowOrb: case BlueOrb: case PinkOrb:
                    m_player->setTouchedRing(obj);
                    m_player->ringJump();
                    break;
                default: break; // Decoration and editor triggers have no solid hitbox.
            }
            if (m_player->getIsDead()) return;
        }
    }
}

int PlayLayer::sectionForPos(CCPoint point)
{
	return (int)floorf(point.x / 100.0f);
}

void PlayLayer::recordAction(bool pressed)
{
	if (m_localLevel) {
		if (pressed)
			field391_0x211 = true;
		else
			field392_0x212 = true;
	}
}

bool PlayLayer::isFlipping()
{
	if (m_flipValue == 0.0) {
		return false;
	}
	return m_flipValue != 1.0;
}

// weird function
void PlayLayer::destroyPlayer()
{
	if (!m_player->getIsLocked() && !m_playerDead) {
		if (!m_showingHint && (m_level->getLevelID() == 1) && !m_player->getHasJumped() && m_attempts > 1)
			this->showHint();

		if (!m_showingHint && (m_level->getLevelID() == 3) && !m_player->getHasRingJumped() && m_attempts > 1)
			this->showHint();
		
		int lastRunPercent = 0;
		if (m_levelLength > 0.0f && std::isfinite(m_levelLength)) {
			float runPercent = (m_player->getPosition().x / m_levelLength) * 100.0f;
			if (std::isfinite(runPercent)) {
				if (runPercent < 0.0f)
					runPercent = 0.0f;
				else if (runPercent > 100.0f)
					runPercent = 100.0f;
				lastRunPercent = static_cast<int>(runPercent);
			}
		}

		bool newBest = true;
		m_playerDead = true;
		m_player->playerDestroyed();

		// pfVar3 = m_player->getPosition();
		if (!m_testMode) {
			if (m_practiceMode || (lastRunPercent <= m_level->getNormalPercent()))
				newBest = false;

			m_level->savePercentage(lastRunPercent, m_practiceMode);
			if (m_level->getLevelType() == GJLevelType::MainLevel)
				GM->reportPercentageForLevel(m_level->getLevelID(), lastRunPercent, m_practiceMode);
		}
		else
			newBest = false;

		if (!m_practiceMode)
			m_lastRunPercent = lastRunPercent;
			
		if (!m_practiceMode)
			SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(false);

		GameSoundManager::sharedManager()->playEffect("explode_11.ogg", 1.0f, 0.0f, 0.65f);
		CCSequence* sequence;
		if ((!GameManager::sharedState()->getAutoRetryLevel()) && (!m_practiceMode)) {
			PLAY_LAYER->getUILayer()->disableMenu();
			sequence = CCSequence::create(CCDelayTime::create(1.0f), CCCallFunc::create(this, callfunc_selector(PlayLayer::showRetryLayer)), nullptr);
		}
		else {
			m_resetQueued = true;
			float delayTime;
			if (newBest) {
				delayTime = 1.4f;
				this->showNewBest();
			}
			else
				delayTime = 1.0f;

			this->stopActionByTag(16);
			sequence = CCSequence::create(CCDelayTime::create(delayTime), CCCallFunc::create(this, callfunc_selector(PlayLayer::delayedResetLevel)), nullptr);
			sequence->setTag(16);
		}

		this->runAction(sequence);
	}
}

void PlayLayer::showHint()
{
	m_showingHint = true;
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	float delayTime;
	float scale;
	char const* string;

	if (m_level->getLevelID() == 1) {
		delayTime = 3.0f;
		scale = 0.7f;
		string = "Tap to jump over the spikes";
	}
	else {
		delayTime = 4.0f;
		scale = 0.6f;
		string = "Tap while touching a ring to jump mid air";
	}

	CCLabelBMFont* hintLabel = CCLabelBMFont::create(string, "bigFont.fnt");
	hintLabel->setScale(scale);
	this->addChild(hintLabel, 3);
	hintLabel->setPosition(ccp(winSize.width * 0.5f, (winSize.height * 0.5f) + 60.0f));
	hintLabel->setOpacity(0);

	CCFadeIn* fadeIn = CCFadeIn::create(0.5f);
	CCDelayTime* delay = CCDelayTime::create(delayTime);
	CCFadeOut* fadeOut = CCFadeOut::create(0.5f);
	CCCallFunc* callback = CCCallFunc::create(hintLabel, callfunc_selector(CCNode::removeMeAndCleanup));
	CCSequence* sequence = CCSequence::create(fadeIn, delay, fadeOut, callback, nullptr);

	hintLabel->runAction(sequence);
}

void PlayLayer::showNewBest()
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	CCPoint newBestPos = ccp(winSize.width * 0.5f, (winSize.height * 0.5f) + 20.0f);

	CCNode* newBestNode = CCNode::create();
	this->addChild(newBestNode, 15);
	newBestNode->setPosition(newBestPos);

	CCSprite* newBestSpr = CCSprite::createWithSpriteFrameName("GJ_newBest_001.png");
	newBestNode->addChild(newBestSpr);
	newBestSpr->setAnchorPoint(ccp(0.5f, 0.0f));

	char const* pctString = CCString::createWithFormat("%i%%", m_lastRunPercent)->getCString();
	CCLabelBMFont* pctLabel = CCLabelBMFont::create(pctString, "bigFont.fnt");
	pctLabel->setAnchorPoint(ccp(0.5f, 1.0f));
	newBestNode->addChild(pctLabel);
	newBestNode->setScale(0.01f);

	// this isn't exactly accurate, but at least it doesn't crash...
	CCEaseElasticOut* easeElasticOut = CCEaseElasticOut::create(CCScaleTo::create(0.4f, 1.0f), 0.6f);
	CCDelayTime* delay = CCDelayTime::create(0.7f);
	CCEaseIn* easeIn = CCEaseIn::create(CCScaleTo::create(0.4f, 0.01f, 0.01f), 2.0f);
	CCHide* hide = CCHide::create();
	CCCallFunc* callback = CCCallFunc::create(newBestNode, callfunc_selector(CCNode::removeMeAndCleanup));
	CCSequence* sequence = CCSequence::create(easeElasticOut, delay, easeIn, hide, callback, nullptr);

	newBestNode->runAction(sequence);
}

std::string PlayLayer::getParticleKey(int objType, char const* file, int zOrder, cocos2d::tCCPositionType positionType)
{
	return CCString::createWithFormat("%i%s%i%i", objType, file, zOrder, positionType)->getCString();
}

/*std::string PlayLayer::getParticleKey2(std::string pKey)
{
	return CCString::createWithFormat("%s%s", pKey, )->getCString();
}*/

void PlayLayer::createParticle(int objType, char const* file, int zOrder, cocos2d::tCCPositionType positionType)
{
	GameManager* pGameManager = GameManager::sharedState();

	if (!pGameManager->getPerformanceMode()) {
		std::string particleKey = this->getParticleKey(objType, file, zOrder, positionType);
		if (!m_particlesDictionary->objectForKey(particleKey)) {
			// TODO: add better names to these variables
			CCArray* pCVar3 = CCArray::create();
			CCArray* pCVar4 = CCArray::create();
			m_particlesDictionary->setObject(pCVar3, particleKey);
			// m_particlesDictionary->setObject(pCVar4, this->getParticleKey2(particleKey));
		}
	}
}

void PlayLayer::playSpeedParticle(float timeMod)
{
	// todo
}

void PlayLayer::moveCameraToPos(cocos2d::CCPoint pos)
{
	cameraMoveX(pos.y, 1.2f, 1.8f);
	cameraMoveY(pos.x, 1.2f, 1.8f);
}

void PlayLayer::cameraMoveX(float value, float duration, float rate)
{
	this->stopActionByTag(10);
	m_cameraMovingY = true;
	CCEaseInOut* ease = CCEaseInOut::create(
		CCActionTween::create(duration, "cTX", m_cameraPos.x, value),
		rate);
	ease->setTag(10);
	this->runAction(ease);
}

void PlayLayer::cameraMoveY(float value, float duration, float rate)
{
	this->stopActionByTag(11);
	m_cameraMovingY = true;
	CCEaseInOut* ease = CCEaseInOut::create(
		CCActionTween::create(duration, "cTY", m_cameraPos.y, value),
		rate);
	ease->setTag(11);
	this->runAction(ease);
}

CheckpointObject* PlayLayer::createCheckpoint()
{
	CheckpointObject* check = CheckpointObject::create();
	m_player->saveToCheckpoint(check);
	check->setTimeStamp(m_clkTimer);

	// todo

	return check;
}


void PlayLayer::storeCheckpoint(CheckpointObject* check)
{
	m_checkpoints->addObject(check);
	addToSection(check->getObject());
}

void PlayLayer::markCheckpoint()
{
	if (!m_player->getIsDead()) {
		CheckpointObject* check = createCheckpoint();
		storeCheckpoint(check);
		check->getObject()->activateObject();
	}
}

bool PlayLayer::hasUniqueCoin(GameObject* obj)
{
	char const* key = m_level->getCoinKey(obj->getUniqueID());
	return GameStatsManager::sharedState()->hasUniqueItem(key);
}

void PlayLayer::incrementJumps()
{ 
	m_didJump = true;
	GameStatsManager::sharedState()->incrementStat("1");
	m_jumps++;
	m_level->setJumps(m_level->getJumps() + 1);
}
