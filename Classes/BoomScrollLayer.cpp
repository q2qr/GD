#include "BoomScrollLayer.h"
#include "ExtendedLayer.h"
USING_NS_CC;

// now for the real BoomScrollLayer code
// from what i can tell robtop just took CCScrollLayer's code and added more features to it
BoomScrollLayer::BoomScrollLayer()
{
	m_dotsArray = nullptr;
	m_animatingToPage = 0;
	unk_0x114 = 0.0f;
	unk_0x118 = 0.0f;
	m_looped = false;
	unk_0x120 = nullptr;
	unk_0x124 = false;
	unk_0x128 = 0;
	unk_0x12c = false;
	unk_0x130 = nullptr;
	m_actualPages = nullptr;
	unk_0x138 = 0.0f;
	m_internalLayer = nullptr;
	m_minTouchSpeed = 0.0f;
	m_touchSpeedFast = 0.0f;
	m_touchSpeedMid = 0.0f;
	m_delegate = nullptr;
	m_movingToPage = false;
	m_minimumTouchLengthToSlide = 0.0f;
	m_minimumTouchLengthToChangePage = 0.0f;
	m_marginOffset = 0.0f;
	m_stealTouches = false;
	m_showPagesIndicator = false;
	m_currentScreen = 0;
	m_pagesWidthOffset = 0.0f;
	m_pages = 0;
}

BoomScrollLayer* BoomScrollLayer::create(cocos2d::CCArray* pages, int offset, bool looped)
{
    BoomScrollLayer* ret = new BoomScrollLayer();
    if (ret) {
		if (ret->init(pages, offset, looped)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
    }

    return NULL;
}

bool BoomScrollLayer::init(cocos2d::CCArray* pages, int offset, bool looped)
{
    if (!CCLayer::init())
        return false;

	m_internalLayer = ExtendedLayer::create();
	this->addChild(m_internalLayer);

	setObjType(CCObjectType::BoomScrollLayer);
    setTouchEnabled(true);
	setStealTouches(true);
	setMinimumTouchLengthToSlide(40.0f);
	setMinimumTouchLengthToChangePage(100.0f);

	setMarginOffset(WIN_SIZE.width);
	setShowPagesIndicator(true);
	setPagesIndicatorPosition(ccp(getContentSize().width * 0.5f, 60.0f));
	setPagesIndicatorNormalColor(ccc4(150, 150, 150, 255));
	setPagesIndicatorSelectedColor(ccc4(255, 255, 255, 255));
	setScrollArea(CCRectMake(0.0f, 0.0f, WIN_SIZE.width, WIN_SIZE.height));

	// variables
	m_actualPages = pages;
	m_currentScreen = 0;
	m_pagesWidthOffset = offset;
	m_minTouchSpeed = 0.3f;
	m_touchSpeedMid = 0.4f;
	m_touchSpeedFast = 0.6f;

	pages->retain();
	CCSpriteBatchNode* dots = CCSpriteBatchNode::create("smallDot.png", 29);
    this->addChild(dots, 5);
    m_dotsArray = CCArray::create();
	m_dotsArray->retain();
    
    for (int i = 0; i < getTotalPages(); ++i)
	{
        CCSprite* dot = CCSprite::create("smallDot.png");
        dots->addChild(dot);
		m_dotsArray->addObject(dot);
    }
    
    this->updatePages();

	m_looped = looped;
	if (looped)
		repositionPagesLooped();

	this->schedule(schedule_selector(BoomScrollLayer::updateDots), 0.1f);
    
	return true;
}

void BoomScrollLayer::updateDots(float dt)
{
	if (m_dotsArray) {
		int pageNum = pageNumberForPosition(m_internalLayer->getPosition());
		
		if (m_looped)
			pageNum = getRelativePageForNum(pageNum);

		for (int i = 0; i < getTotalPages(); ++i) {
			if (i < m_dotsArray->count()) {
				CCSprite* dot = ((CCSprite*)m_dotsArray->objectAtIndex(i));
				ccColor3B dotColor = ccWHITE;
				
				if (i != pageNum)
					dotColor = ccGRAY;
				
				dot->setColor(dotColor);
			}
		}
	}
}

void BoomScrollLayer::updatePages()
{
	for (int i = 0; i < m_actualPages->count(); ++i) {
		CCNode* page = (CCNode*)m_actualPages->objectAtIndex(i);
		page->setAnchorPoint(ccp(0,0));
		page->setContentSize(WIN_SIZE);
		page->setPosition(getRelativePosForPage(i));
		
		if (!page->getParent())
			m_internalLayer->addChild(page);
	}
}

bool BoomScrollLayer::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    CCPoint touchLocation = pTouch->getLocation();
    
    // CCLOG("x: %f, y: %f", touchLocation.x, touchLocation.y);
    
    return true;
}

void BoomScrollLayer::quickUpdate()
{
	if (m_movingToPage) {
		m_movingToPage = false;
		m_internalLayer->stopActionByTag(2);
		m_internalLayer->setPosition(m_targetPos);
		moveToPageEnded();
	}
}

void BoomScrollLayer::moveToPage(int page)
{
	if (m_looped || (-1 < page && (page < getTotalPages()))) {
		m_movingToPage = true;
		m_targetPos = positionForPageWithNumber(page);
		m_internalLayer->stopActionByTag(2);

		float speed;
		if (unk_0x118 > m_touchSpeedFast)
			speed = 0.4f;
		else if (unk_0x118 > m_touchSpeedMid)
			speed = 0.6f;
		else
			speed = 0.8f;

		unk_0x118 = 0.0f;
		CCMoveTo* moveAction = CCMoveTo::create((speed * 1.2f), m_targetPos);
		CCEaseElasticOut* elasticMove = CCEaseElasticOut::create(moveAction, 0.5f);
		CCCallFunc* callback = CCCallFunc::create(this, callfunc_selector(BoomScrollLayer::moveToPageEnded));
		CCSequence* sequence = CCSequence::create(elasticMove, callback, nullptr);
		m_internalLayer->runAction(sequence);
		sequence->setTag(2);

		m_currentScreen = page;

		if (m_looped)
			repositionPagesLooped();

	}
}

void BoomScrollLayer::instantMoveToPage(int page)
{
	if (m_looped || ((-1 < page && (page < getTotalPages())))) {
		m_internalLayer->stopActionByTag(2);
		m_movingToPage = false;
		m_internalLayer->setPosition(positionForPageWithNumber(page));
		m_currentScreen = page;

		if (m_looped)
			repositionPagesLooped();

		moveToPageEnded();
	}
}

void BoomScrollLayer::moveToPageEnded()
{
	m_movingToPage = false;

	if (m_animatingToPage != m_currentScreen && m_delegate)
		m_delegate->scrollLayerScrollingStarted(this);

	m_internalLayer->stopActionByTag(2);
	int pageNum = pageNumberForPosition(m_internalLayer->getPosition());
	m_currentScreen = pageNum;
	m_animatingToPage = pageNum;
	this->updateDots(0.0f);
}

// https://github.com/geode-sdk/bindings/blob/main/bindings/2.208/inline/BoomScrollLayer.cpp#L21
CCPoint BoomScrollLayer::positionForPageWithNumber(int page)
{
	return ccp((getContentSize().width - m_pagesWidthOffset) * -page, 0.f);
}

CCPoint BoomScrollLayer::getRelativePosForPage(int page)
{
	return ccp((getContentSize().width - m_pagesWidthOffset) * page, 0.f);
}

void BoomScrollLayer::repositionPagesLooped()
{
    // Wrapped page indices identify nodes; unwrapped indices determine their positions.
    for (int i = 0; i < (int)m_actualPages->count(); ++i)
        ((CCNode*)m_actualPages->objectAtIndex(i))->setVisible(false);
    for (int raw = m_currentScreen - 1; raw <= m_currentScreen + 1; ++raw) {
        CCNode* page = getPage(getRelativePageForNum(raw));
        page->setPosition(getRelativePosForPage(raw));
        page->setVisible(true);
    }
}

void BoomScrollLayer::setPagesIndicatorPosition(CCPoint position)
{
	m_pagesIndicatorPosition = position;
	updateDots(0.0f);
}

unsigned int BoomScrollLayer::getTotalPages()
{
	CCArray* pagesArray = unk_0x124 ? unk_0x120 : m_actualPages;
	return (int)pagesArray->count();
}

int BoomScrollLayer::pageNumberForPosition(CCPoint pos)
{
	float pageWidth = getContentSize().width - m_pagesWidthOffset;
	if (pageWidth <= 0.0f)
		return m_currentScreen;

	float pageFloat = -pos.x / pageWidth;
	int pageNumber = (int)ceilf(pageFloat);
	if (pageNumber - pageFloat >= 0.5f)
		pageNumber--;

	if (!m_looped) {
		pageNumber = MAX(0, pageNumber);
		pageNumber = MIN(getTotalPages() - 1, pageNumber);
	}

	return pageNumber;
}

// https://github.com/geode-sdk/bindings/blob/main/bindings/2.208/inline/BoomScrollLayer.cpp#L111
int BoomScrollLayer::getRelativePageForNum(int page)
{
	int pages = this->getTotalPages();
	if (page > 0) return page % pages;
	while (page < 0) page += pages;
	return page;
}

CCLayer* BoomScrollLayer::getPage(int page)
{
	// somehow this isn't 100% accurate :/
	if (unk_0x124) {
		int totalPgs = m_actualPages->count();
		if (page < 1)
			for (; page < 0; page += totalPgs) {}
		else
			page %= totalPgs;
	}

	return (CCLayer*)m_actualPages->objectAtIndex(page);
}

// Dynamic Pages Control

void BoomScrollLayer::addPage(CCLayer* aPage)
{
	addPage(aPage, m_actualPages->count());
}

void BoomScrollLayer::addPage(CCLayer* aPage, int pageNumber)
{
	pageNumber = MIN(pageNumber, m_actualPages->count());
	pageNumber = MAX(pageNumber, 0);

	m_actualPages->insertObject(aPage, pageNumber);

	updatePages();
	moveToPage(m_currentScreen);
}
