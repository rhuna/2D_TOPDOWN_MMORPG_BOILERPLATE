#pragma once

#include "QuestTypes.h"
#include <string>
#include <unordered_map>
#include <vector>

class QuestSystem
{
public:
    bool LoadFromJson(const std::string &filePath);

    void InitializeStates();

    void NotifyEvent(const QuestEvent &event);

    bool CanAcceptQuest(const std::string &questId) const;
    bool AcceptQuest(const std::string &questId);

    bool IsQuestCompleted(const std::string &questId) const;
    bool IsQuestActive(const std::string &questId) const;

    bool RewardQuest(const std::string &questId, int &outGold, int &outXp, std::string &outItemId, int &outItemCount);

    const QuestDefinition *GetDefinition(const std::string &questId) const;
    const QuestState *GetState(const std::string &questId) const;

    std::vector<const QuestState *> GetActiveQuests() const;
    std::vector<const QuestState *> GetAvailableQuests() const;

private:
    void UpdateAvailability();
    void CheckCompletion(const std::string &questId);
    bool EventMatchesObjective(const QuestEvent &event, const QuestObjectiveDefinition &def) const;

private:
    std::unordered_map<std::string, QuestDefinition> definitions_;
    std::unordered_map<std::string, QuestState> states_;
};