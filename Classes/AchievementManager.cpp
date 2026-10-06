#include "AchievementManager.h"
#include "AchievementNotifier.h"

#include "RT_COCOS/CCContentManager.h"
USING_NS_CC;

AchievementManager::AchievementManager()
{
	m_allAchievements = nullptr;
    // unk_0xec = 0;
	m_reportedAchievements = nullptr;
	m_dontNotifyAch = false;
}

AchievementManager* AchievementManager::sharedState()
{
    static AchievementManager* pAchManager = NULL;
    if (!pAchManager)
    {
        pAchManager = new AchievementManager();
        pAchManager->init();
    }
    
    return pAchManager;
}

bool AchievementManager::init()
{
	m_reportedAchievements = CCDictionary::create();
	m_reportedAchievements->retain();
	m_allAchievements = CCContentManager::sharedManager()->addDict("AchievementsDesc.plist", true);
	m_allAchievements->retain();
	return true;
}

void AchievementManager::notifyAchievementWithID(char const* achID)
{
	if (!m_dontNotifyAch) {
		if (m_allAchievements->objectForKey(achID)) {
			CCDictionary* tempDict = (CCDictionary*)m_allAchievements->objectForKey(achID);
			const char* title = tempDict->valueForKey("title")->getCString();
			const char* description = tempDict->valueForKey("achievedDescription")->getCString();
			const char* icon = tempDict->valueForKey("icon")->getCString();
			AchievementNotifier::sharedState()->notifyAchievement(title, description, icon);
		}
	}
}

void AchievementManager::reportAchievementWithID(char const* achID, int percentage, bool silent)
{
    percentage = MAX(0, MIN(100, percentage));
    int previous = percentForAchievement(achID);
    if (percentage <= previous) return;
    m_reportedAchievements->setObject(CCString::createWithFormat("%i", percentage), achID);
    if (percentage == 100 && previous < 100 && !silent) notifyAchievementWithID(achID);
}

bool AchievementManager::isAchievementEarned(char const* achID)
{
	return 99 < percentForAchievement(achID);
}

bool AchievementManager::areAchievementsEarned(CCArray* achSet)
{
    if (!achSet) return false;
    for (unsigned i = 0; i < achSet->count(); ++i)
        if (!isAchievementEarned(((CCString*)achSet->objectAtIndex(i))->getCString())) return false;
    return true;
}

int AchievementManager::percentForAchievement(char const* achID)
{
	return m_reportedAchievements->valueForKey(achID)->intValue();;
}
