#include <unordered_map>
#include "Quest.h"
#include "QuestState.h"
#include "QuestEvent.h"


class QuestSystem
{
public:
    void LoadDefinitions(const std::string &filePath);
    void InitializePlayerQuests();

    void NotifyEvent(const QuestEvent &event);

    bool CanAcceptQuest(const std::string &questId) const;
    void AcceptQuest(const std::string &questId);
    void RewardQuest(const std::string &questId);

    const QuestState *GetQuestState(const std::string &questId) const;
    std::vector<const QuestState *> GetActiveQuests() const;

private:
    void UpdateAvailability();
    void CheckQuestCompletion(const std::string &questId);

private:
    std::unordered_map<std::string, QuestDefinition> definitions_;
    std::unordered_map<std::string, QuestState> states_;
};