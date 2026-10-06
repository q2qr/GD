#include "GaragePage.h"
#include "GameManager.h"
#include "RT_COCOS/CCMenuItemSpriteExtra.h"
USING_NS_CC;


GaragePage* GaragePage::create(IconType type, GJGarageLayer* garage, SEL_MenuHandler selector)
{
	GaragePage* ret = new GaragePage();
	if (ret) {
		if (ret->init(type, garage, selector)) {
			ret->autorelease();
			return ret;
		}

		delete ret;
	}

	return NULL;
}

bool GaragePage::init(IconType type, GJGarageLayer* garage, SEL_MenuHandler selector)
{
	if (!CCLayer::init())
		return false;

    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    m_page = 0;
    m_buttons = CCArray::create(); // retained by a child menu below; array lifetime matches this layer
    m_buttons->retain();
    CCMenu* menu = CCMenu::create();
    menu->setPosition(CCPointZero);
    addChild(menu);
    m_count = type == IconType::Cube ? 38 : type == IconType::Ship ? 14 : 7;
    for (int id = 1; id <= m_count; ++id) {
        CCNode* image;
        if (type == IconType::Special) {
            CCLabelBMFont* label = CCLabelBMFont::create(CCString::createWithFormat("%i", id)->getCString(), "bigFont.fnt");
            label->setScale(0.6f);
            image = label;
        } else {
            SimplePlayer* icon = SimplePlayer::create(1);
            icon->updatePlayerFrame(id, type);
            icon->setColor(GM->colorForIdx(GM->getPlayerColor()));
            icon->setSecondColor(GM->colorForIdx(GM->getPlayerColor2()));
            // SimplePlayer draws around its origin; give its menu wrapper a real hit area.
            CCNode* wrapper = CCNode::create();
            wrapper->setContentSize(CCSize(34, 34));
            wrapper->addChild(icon);
            icon->setPosition(ccp(17, 17));
            image = wrapper;
        }
        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(image, NULL, garage, selector);
        item->setTag(id);
        item->setPosition(ccp(winSize.width * 0.5f - 180 + ((id - 1) % 10) * 40, 145 - (((id - 1) % 20) / 10) * 42));
        menu->addChild(item);
        m_buttons->addObject(item);
    }
    if (m_count > 20) {
        for (int direction = -1; direction <= 1; direction += 2) {
            CCLabelBMFont* label = CCLabelBMFont::create(direction < 0 ? "<" : ">", "bigFont.fnt");
            label->setScale(0.5f);
            CCMenuItemSpriteExtra* arrow = CCMenuItemSpriteExtra::create(label, NULL, this, menu_selector(GaragePage::onPage));
            arrow->setTag(direction);
            arrow->setPosition(ccp(winSize.width * 0.5f + direction * 210, 124));
            menu->addChild(arrow);
        }
    }
    refresh();
    return true;
}

void GaragePage::onPage(CCObject* sender)
{
    m_page = (m_page + ((CCNode*)sender)->getTag() + (m_count + 19) / 20) % ((m_count + 19) / 20);
    refresh();
}

void GaragePage::refresh()
{
    for (unsigned i = 0; i < m_buttons->count(); ++i)
        ((CCNode*)m_buttons->objectAtIndex(i))->setVisible((int)i / 20 == m_page);
}

void GaragePage::onSelect(CCObject* sender)
{
	// whatever bro
}