#include "GameObject.h"
#include "ObjectToolbox.h"
#include "GameManager.h"
#include "PlayLayer.h"
USING_NS_CC;

// hi antimatter some of your code was kind of broken so i fixed it up

GameObject::GameObject() {
	unk_0x1b8 = 0;
	unk_0x1bc = 0;
	unk_0x1c0 = false;
	m_glowSprite = nullptr;
	unk_0x1c8 = false;
	unk_0x1c9 = false;
	m_myAction = nullptr;
	unk_0x1d0 = false;
	m_poweredOn = false;
	unk_0x1d4 = 0.0f;
	unk_0x1d8 = 0.0f;
	m_isActive = false;
	m_hasGlow = false;
	unk_0x1de = false;
	m_particleSystem = nullptr;
	m_particleString = "";
	m_particleAdded = false;
	unk_0x204 = false;
	unk_0x218 = false;
	m_hasColor = false;
	m_colorSprite = nullptr;
	m_ignoreScreenCheck = false;
	m_radius = 0.0f;
	m_isRotated = false;
	m_scaleModX = 0.0f;
	m_scaleModY = 0.0f;
	m_ID = 0;
	m_type = None;
	m_sectionIdx = 0;
	m_shouldSpawn = false;
	m_touchTriggered = false;
	m_startPos = CCPointZero;
	m_blendAdditive = false;
	m_frame = "";
	m_usePlayerColor = false;
	m_usePlayerColor2 = false;
	m_isDisabled = false;
	m_useAudioScale = false;
	m_isSleeping = false;
	m_startRotation = 0.0f;
	m_startScaleX = 0.0f;
	m_startScaleY = 0.0f;
	m_shouldHide = false;
	m_spawnXPos = 0;
	m_isInvisible = false;
	m_enterAngle = 0.0f;
	m_enterEffect = 0;
	m_tintDuration = 0.0f;
	m_tintGround = false;
	m_objectKey = 0;
	m_dontTransform = false;
	m_dontFade = false;
	m_dontFadeTinted = false;
	m_isTintObject = false;
	m_hasBeenActivated = false;
	m_stateVar = false;
	m_objectZ = 0;
	m_objectParent = nullptr;
	m_customAudioScale = false;
	m_minAudioScale = 0.0f;
	m_maxAudioScale = 0.0f;
	m_uniqueID = 0;
	m_invisibleMode = false;
	m_glowUseBGColor = false;
	m_useBGColor = false;
	m_useSpecialLight = false;
	m_opacityMod = 1.0f;
	m_glowOpacityMod = 1.0f;
	m_dontShow = false;
	m_editorSelected = false;
	m_copyPlayerColor1 = false;
	m_copyPlayerColor2 = false;
	m_tintObjectsUseBlend = false;
}

bool GameObject::init(const char *spriteName) {
    if (!CCSpritePlus::initWithSpriteFrameName(spriteName)) return false;
    m_objectZ = 2;
    m_opacityMod = 1.0f;
    m_glowOpacityMod = 1.0f;
    m_enterEffect = 0;
    m_frame = spriteName;
    m_shouldSpawn = false;
	// set to 0.1f for now since i just CANNOT figure out what these variables are, probably some inlined functions in CCSprite...
	unk_0x1d4 = getContentSize().width; // collision dimensions, confirmed in original x86 library
	unk_0x1d8 = getContentSize().height;
    m_scaleModX = 1.0f;
    m_scaleModY = 1.0f;
    m_startScaleX = 1.0f;
    m_startScaleY = 1.0f;
    //  m_ID = dword_4B6E6C;
    m_startRotation = 0.0f;
    m_tintColor = ccc3(255, 255, 255);
    m_tintDuration = 0.5f;
    setScaleX(1.0f);
    setScaleY(1.0f);
    m_isActive = false;
    unk_0x204 = true;
    unk_0x218 = true;
    m_tintObjectsUseBlend = true;
    return true;
}

GameObject* GameObject::create(const char* frame)
{
	GameObject *pRet = new GameObject();
	if (pRet && pRet->init(frame))
	{
		pRet->autorelease();
		return pRet;
	}

	CC_SAFE_DELETE(pRet);
	return NULL;
}

GameObject* GameObject::objectFromString(std::string objString)
{
	CCDictionary* objDict = ObjectToolbox::stringSetupToDict(objString);
	if (!objDict) return nullptr;

	if (!objDict->objectForKey("1")) return nullptr;
	int objID = atoi(objDict->valueForKey("1")->getCString());
	if (!objID) return nullptr;

	const char* frame = ObjectToolbox::sharedState()->keyToFrame(objDict->valueForKey("1")->getCString());

	float x = objDict->valueForKey("2")->floatValue();
	float y = objDict->valueForKey("3")->floatValue();
	bool flipX = objDict->objectForKey("4") ? objDict->valueForKey("4")->boolValue() : false;
	bool flipY = objDict->objectForKey("5") ? objDict->valueForKey("5")->boolValue() : false;
	int rotation = objDict->objectForKey("6") ? objDict->valueForKey("6")->intValue() : 0;

	// Unknown custom objects must not assert inside CCSprite::initWithSpriteFrameName.
    if (!frame || !*frame || !CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frame)) return nullptr;
    GameObject* object = GameObject::create(frame);
	if (!object) return nullptr;

	object->setObjectKey(objID);
	object->setFlipX(flipX);
	object->setFlipY(flipY);
	object->setRotation((float)rotation);

	CCPoint startPos = ccp(x, y + 90.0f);
	object->m_startPos = startPos;
	object->setPosition(startPos);

	object->m_isRotated = (rotation % 180 != 0);
	object->customSetup();
	// 1.71 color-trigger properties, confirmed against the APK parser/vtable.
	if (objDict->objectForKey("7"))
		object->m_tintColor = ccc3(objDict->valueForKey("7")->intValue(),
			objDict->valueForKey("8")->intValue(), objDict->valueForKey("9")->intValue());
	if (objDict->objectForKey("10")) object->m_tintDuration = MAX(0.0f, objDict->valueForKey("10")->floatValue());
	object->m_touchTriggered = objDict->valueForKey("11")->boolValue();
	object->m_tintGround = objDict->valueForKey("14")->boolValue();
	object->m_copyPlayerColor1 = objDict->valueForKey("15")->boolValue();
	object->m_copyPlayerColor2 = objDict->valueForKey("16")->boolValue();
	if (objDict->objectForKey("17")) object->m_tintObjectsUseBlend = objDict->valueForKey("17")->boolValue();

	// Detail overlays use the same atlas; PlayLayer attaches them independently
	// so their additive blend can differ from the base object's sprite batch.
	std::string colorFrame = frame;
	size_t suffix = colorFrame.rfind("_001.png");
	if (suffix != std::string::npos) {
		colorFrame.replace(suffix, 8, "_color_001.png");
		if (CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(colorFrame.c_str())) {
			object->m_colorSprite = CCSprite::createWithSpriteFrameName(colorFrame.c_str());
			object->m_hasColor = true;
		}
	}
	object->calculateSpawnXPos();

	return object;

	// old thing i was doing, keeping it here just in case..
	/*CCDictionary* objDict = ObjectToolbox::stringSetupToDict(objString);

	char const* key = objDict->valueForKey("1")->getCString();
	int objID = atoi(key);
	char const* frame = ObjectToolbox::sharedState()->keyToFrame(key);

	if (!objID)
		return nullptr;

	GameObject* object;
	if (objID == 84 || objID == 36 || objID == 141) {
		// object = RingObject::create();
		return nullptr;
	}
	else {
		object = GameObject::create(frame);
		object->setObjectKey(objID);
		object->setPosition(ccp(objDict->valueForKey("2")->floatValue(), objDict->valueForKey("3")->floatValue() + 90.0f));
	}

	object->setObjectKey(objID);
	object->customSetup();

	return object;*/
}

void GameObject::disableObject() {
	m_type = GameObjectType::Decoration;
    m_isDisabled = true;
	m_particleAdded = false;
    m_opacityMod = 0.2f;
}

const char* GameObject::getBallFrame(int idx) {
    return cocos2d::CCString::createWithFormat("rod_ball_%02d_001.png", idx < 3 ? idx : 3)->getCString();
}

void GameObject::triggerActivated() {
    m_hasBeenActivated = true;
}

void GameObject::removeGlow() {
    if (!m_glowSprite) return;
    m_glowSprite->release();
    m_glowSprite->removeMeAndCleanup();
    m_glowSprite = nullptr;
}

void GameObject::powerOnObject() {
    m_stateVar = true;
    if (!m_poweredOn)
        m_poweredOn = true;
}
void GameObject::powerOffObject() {
    if (m_poweredOn)
        m_poweredOn = false;
}

void GameObject::activateObject() {
    m_shouldHide = false;
    if (m_isActive || m_isSleeping) return;
    
    m_isActive = true;
    if (m_isInvisible) return;
    
    this->setVisible(true);
    if (this->unk_0x1c9)
        PLAY_LAYER->registerStateObject(this);
    
    if (!m_dontShow && m_objectParent)
        m_objectParent->addChild(this, m_objectZ);
    
    if (m_hasGlow)
        PLAY_LAYER->getBatchNodeAdd()->addChild(m_glowSprite);
    
    if (m_hasColor) {
        if (!m_colorSprite->getParent()) PLAY_LAYER->getGameLayer()->addChild(m_colorSprite, 2);
        m_colorSprite->setVisible(true);
    }
    // this = (GameManager *)(*(int (**)(void))(*(_DWORD *)v13 + 0xDC))();
    if (this->unk_0x1d0 && !this->getActionByTag(11) && m_myAction) {
        this->runAction(m_myAction);
    }
}

/*void GameObject::addColorSprite() {
    if (
        ((m_objectKey >= 207 && m_objectKey < 214) ||
         (m_objectKey >= 215 && m_objectKey < 220) ||
         (m_objectKey >= 247 && m_objectKey < 262) ||
         (m_objectKey >= 263 && m_objectKey <= 275))
        && m_hasColor
        ) {
        // replace _001.png with _color_001.png
        std::string colorSpriteFrame = CCString::createWithFormat("%i", m_objectKey)->getCString();
        // this isn't how it works but it also kind of works
        colorSpriteFrame.replace(0, colorSpriteFrame.find("_001.png"), "_color_001.png");
        m_colorSprite = cocos2d::CCSprite::createWithSpriteFrameName(colorSpriteFrame.c_str());
        m_colorSprite->retain();
        m_colorSprite->setPosition(this->getPosition());
        GameManager* gman = GameManager::sharedState();
        if (gman->getEditMode()) {
            m_colorSprite->setOpacity(100);
        }
    }
}*/

void GameObject::setFlipX(bool flipX) {
    CCSpritePlus::setFlipX(flipX);

    if (m_glowSprite) {
        m_glowSprite->setFlipX(flipX);
    }
    if (m_hasColor) {
        m_colorSprite->setFlipX(flipX);
    }
}

void GameObject::setFlipY(bool flipY) {
    CCSpritePlus::setFlipY(flipY);
    if (m_glowSprite) {
        m_glowSprite->setFlipY(flipY);
    }
    if (m_hasColor) {
        m_colorSprite->setFlipY(flipY);
    }
}

void GameObject::setScaleX(float scaleX) {
    CCSpritePlus::setScaleX(scaleX);
    if (m_glowSprite) {
        m_glowSprite->setScaleX(scaleX);
    }
    if (m_hasColor) {
        m_colorSprite->setScaleX(scaleX);
    }
}

void GameObject::setScaleY(float scaleY) {
    CCSpritePlus::setScaleY(scaleY);
    if (m_glowSprite) {
        m_glowSprite->setScaleY(scaleY);
    }
    if (m_hasColor) {
        m_colorSprite->setScaleY(scaleY);
    }
}

void GameObject::resetObject() {
    m_hasBeenActivated = false;
    m_isSleeping = false;
    this->unk_0x1de = false;
}

/*void GameObject::setGlowColor(cocos2d::ccColor3B color) {
    if (m_glowSprite) {
        m_glowSprite->setColor(color);
    }
}*/

void GameObject::setPosition(cocos2d::CCPoint const &position) {
    this->unk_0x218 = true;
    CCSpritePlus::setPosition(position);
    if (m_particleSystem) {
		m_particleSystem->setPosition(position);
    }
}

/*CCRepeatForever* GameObject::createRotateAction(float duration) {
    int sign;
    if (rand() / RAND_MAX < 0.5) {
        sign = -1;
    } else {
        sign = 1;
    }
    return cocos2d::CCRepeatForever::create(cocos2d::CCRotateBy::create(1, duration * sign));
}

void GameObject::setVisible(bool visible) {
    if (this->unk_0x1e8 && this->isVisible() != visible) {
        if (visible) {
            m_particleSystem = PLAY_LAYER->claimParticle(this->unk_0x1e4);
            this->setPosition(this->getPosition());
            if (m_particleSystem) {
                PLAY_LAYER->getGameLayer();
                CCPoint point = this-> + this->unk_0x1ec;
            }
        } else {
            
        }
    }
    cocos2d::CCSprite::setVisible(visible);
}*/

void GameObject::updateState()
{
	if (!m_stateVar) {
		this->powerOffObject();
	}
}

void GameObject::customSetup()
{
	// Intrinsic palette/blend flags recovered from 1.71 GameObject::customSetup.
	switch (m_objectKey) {
	case 18: case 19: case 20: case 21: case 37: case 41: case 85: case 86: case 87: 
	case 97: case 110: case 113: case 114: case 115: case 123: case 124: case 125: case 126: 
	case 127: case 128: case 129: case 130: case 131: case 151: case 152: case 153: case 154: 
	case 155: case 156: case 222: case 223: case 224: case 237: case 238: case 239: case 240: 
	case 241: 
		m_usePlayerColor = true; break;
	default: break;
	}
	switch (m_objectKey) {
	case 48: case 49: case 50: case 51: case 52: case 53: case 54: case 60: case 106: 
	case 107: case 132: case 133: case 134: case 136: case 137: case 138: case 139: case 148: 
	case 149: case 150: case 180: case 181: case 182: case 190: case 225: case 226: case 236: 
		m_usePlayerColor2 = true; break;
	default: break;
	}
	switch (m_objectKey) {
	case 157: case 158: case 159: case 227: case 228: case 229: case 230: case 231: case 232: 
	case 233: case 234: case 235: case 242: case 279: case 280: case 281: case 282: case 283: 
	case 284: case 285: 
		m_useBGColor = true; break;
	default: break;
	}
	switch (m_objectKey) {
	case 18: case 19: case 20: case 21: case 41: case 48: case 49: case 50: case 51: 
	case 52: case 53: case 54: case 60: case 85: case 86: case 87: case 97: case 106: 
	case 107: case 110: case 113: case 114: case 115: case 123: case 124: case 125: case 126: 
	case 127: case 128: case 129: case 130: case 131: case 132: case 133: case 134: case 136: 
	case 137: case 138: case 139: case 148: case 149: case 150: case 151: case 152: case 153: 
	case 154: case 155: case 156: case 157: case 158: case 159: case 180: case 181: case 182: 
	case 190: case 222: case 223: case 224: case 225: case 226: case 227: case 228: case 229: 
	case 230: case 231: case 232: case 233: case 234: case 235: case 236: case 237: case 238: 
	case 239: case 240: case 241: case 242: case 279: case 280: case 281: case 282: case 283: 
	case 284: case 285: 
		m_blendAdditive = true; break;
	default: break;
	}
	switch (m_objectKey) {
	case 6: case 7: case 8: case 9: case 14: case 22: case 23: case 24: case 25: 
	case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34: 
	case 39: case 40: case 42: case 43: case 55: case 56: case 57: case 58: case 59: 
	case 61: case 62: case 63: case 64: case 65: case 66: case 68: case 69: case 70: 
	case 71: case 72: case 74: case 75: case 76: case 77: case 78: case 79: case 81: 
	case 82: case 83: case 90: case 91: case 92: case 93: case 94: case 95: case 96: 
	case 100: case 102: case 103: case 104: case 105: case 108: case 109: case 112: case 116: 
	case 117: case 118: case 119: case 121: case 122: case 135: case 144: case 145: case 146: 
	case 147: case 160: case 161: case 162: case 163: case 165: case 166: case 167: case 168: 
	case 169: case 170: case 171: case 172: case 173: case 174: case 175: case 176: case 177: 
	case 178: case 179: case 189: case 192: case 194: case 195: case 196: case 197: case 204: 
	case 205: case 206: case 207: case 208: case 209: case 210: case 212: case 213: case 214: 
	case 215: case 216: case 217: case 218: case 219: case 220: case 221: case 243: case 244: 
	case 247: case 248: case 249: case 250: case 252: case 253: case 254: case 255: case 256: 
	case 257: case 258: case 260: case 261: case 262: case 263: case 264: case 265: case 267: 
	case 268: case 269: case 270: case 271: case 272: case 274: case 275: case 276: case 277: 
	case 278: 
		m_isTintObject = true; break;
	default: break;
	}

	switch (m_objectKey) {
    case 10: m_type = NormalGravityPortal; break;
    case 11: m_type = InvertGravityPortal; break;
    case 12: m_type = CubePortal; break;
    case 13: m_type = ShipPortal; break;
    case 36: m_type = YellowOrb; break;
    case 35: m_type = YellowPad; break;
    case 67: m_type = GravityPad; break;
    case 140: m_type = PinkPad; break;
    case 141: m_type = PinkOrb; break;
    case 99: m_type = SmallPortal; break;
    case 101: m_type = BigPortal; break;
    case 111: m_type = BirdPortal; break;
    case 142: m_type = SecretCoin; break;
    case 143: m_type = unk22; break;
    case 200: case 201: case 202: case 203: m_type = unk21; break;
    case 45: m_type = MirrorPortal; break;
    case 46: m_type = CounterMirrorPortal; break;
    case 47: m_type = BallPortal; break;
    case 84: m_type = BlueOrb; break;
    case 9: case 61: case 135: case 243: case 244:
    case 88: case 89: case 98:
    case 183: case 184: case 185: case 186: case 187: case 188:
        m_type = Hazard; break;
    case 18: case 19: case 20: case 21: case 48: case 49:
    case 113: case 114: case 115: case 129: case 130: case 131:
    case 225: case 226: case 237: case 238: case 239: case 240: case 241:
    case 15: case 16: case 17: case 37: case 38: case 41: case 44:
    case 50: case 51: case 52: case 53: case 54: case 60:
    case 85: case 86: case 87: case 97: case 106: case 107: case 110:
    case 123: case 124: case 125: case 126: case 127: case 128:
    case 132: case 133: case 134: case 136: case 137: case 138: case 139:
    case 150: case 151: case 152: case 153: case 154: case 155: case 156:
    case 157: case 158: case 159: case 180: case 181: case 182: case 190:
    case 211: case 222: case 223: case 224: case 227: case 228: case 229:
    case 230: case 231: case 232: case 233: case 234: case 235: case 236:
    case 242: case 251: case 259: case 266: case 273:
    case 279: case 280: case 281: case 282: case 283: case 284: case 285:
        m_type = Decoration; break;
	case 5:
	case 73:
	case 80:
	case 120:
	case 164:
	case 191:
	case 193:
	case 198:
	case 199:
	case 245:
	case 246:
		m_type = GameObjectType::Decoration;
		m_objectZ = -2;
		break;
	default:
		if (m_frame.find("edit_e", 0) == std::string::npos) {
			m_type = GameObjectType::None;
			break;
		}
		m_type = GameObjectType::Decoration;
		m_shouldSpawn = true;
		unk_0x1c8 = true;
		m_isInvisible = true;
		unk_0x1d4 = 30.0f;
		unk_0x1d8 = 60.0f;
		break;
	case 144: case 145: case 205:
	case 8:
	case 39:
	case 103:
	case 177:
	case 178:
	case 179:
	case 216:
	case 217:
	case 218:
		m_type = GameObjectType::Hazard;
		m_scaleModX = 0.2f;
		m_scaleModY = 0.4f;
		if (m_objectKey - 117U < 3) {
			m_glowUseBGColor = true;
		}
		break;
	}

	// if (((uVar13 < 0xf) && ((1 << (uVar13 & 0xff) & 0x7002U) != 0)) || (m_objectKey == 8)) {
	if (m_objectKey == 8) {
		unk_0x1d4 = 30.0f;
		unk_0x1d8 = 30.0f;
	}

	/*if (m_touchTriggered) {
		m_type = GameObjectType::unk21;
		m_isDisabled = false;
	}*/
}

CCRect GameObject::getObjectRect()
{
	return getObjectRect2(m_scaleModX, m_scaleModY);
}

CCRect GameObject::getObjectRect(float scaleModX, float scaleModY)
{
	CCSize objSize = CCSizeMake(unk_0x1d4 * fabsf(getScaleX()), unk_0x1d8 * fabsf(getScaleY()));

	float newSizeWidth = scaleModX * objSize.width;
	float newSizeHeight = scaleModY * objSize.height;

	objSize.width = scaleModX * objSize.width;
	objSize.height = scaleModY * objSize.height;

	// This inverses the values (i assume this was changed in update 2.0 when free rotate was added)
	if (m_isRotated) {
		objSize.width = newSizeHeight;
		objSize.height = newSizeWidth;
	}

	CCPoint realPos = getRealPosition();
	return CCRectMake(realPos.x - objSize.width * 0.5f, realPos.y - objSize.height * 0.5f, objSize.width, objSize.height);
}

CCRect GameObject::getObjectRect2(float scaleModX, float scaleModY)
{
	if (unk_0x218) {
		unk_0x218 = true;
		unk_0x208 = getObjectRect(scaleModX, scaleModY);
	}
	return unk_0x208;
}

void GameObject::createAndAddParticle(int objType, char const* file, int zOrder, cocos2d::tCCPositionType positionType)
{
	PLAY_LAYER->createParticle(objType, file, zOrder, positionType);
	m_particleString = PLAY_LAYER->getParticleKey(objType, file, zOrder, positionType);
	m_particleAdded = true;
}

void GameObject::triggerObject()
{
	if (m_hasBeenActivated || !PLAY_LAYER) return;
	ccColor3B color = m_tintColor;
	if (m_copyPlayerColor1) color = PLAY_LAYER->getPlayer()->getGlowColor1();
	else if (m_copyPlayerColor2) color = PLAY_LAYER->getPlayer()->getGlowColor2();
	switch (m_objectKey) {
	case 29:
		PLAY_LAYER->tintBackground(color, m_tintDuration);
		if (m_tintGround) PLAY_LAYER->tintGround(color, m_tintDuration);
		break;
	case 30: PLAY_LAYER->tintGround(color, m_tintDuration); break;
	case 104: PLAY_LAYER->tintLine(color, m_tintDuration); break;
	case 105: PLAY_LAYER->tintObjects(color, m_tintDuration); break;
	case 221:
		PLAY_LAYER->updateTintObjectsUseBlend(m_tintObjectsUseBlend);
		PLAY_LAYER->tintColorObjects(color, m_tintDuration);
		break;
	default: return;
	}
	triggerActivated();
}

void GameObject::deactivateObject()
{
	// todo
}

CCRect GameObject::getObjectTextureRect()
{
	// todo
	return CCRect();
}

CCPoint GameObject::getRealPosition()
{
	return m_startPos;
}

void GameObject::setStartPos(CCPoint position)
{
	m_startPos = position;
	this->setPosition(position);
}

std::string GameObject::getSaveString()
{
	// this is like a pretty big function and i don't know if it's worth decompiling yet
	return "";
}

void GameObject::calculateSpawnXPos()
{
	m_spawnXPos = m_startPos.x;
}
