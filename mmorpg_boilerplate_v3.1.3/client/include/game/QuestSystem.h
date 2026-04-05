#pragma once

#include "QuestTypes.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
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
    bool IsAwaitingChoice(const std::string &questId) const;

    bool ChooseBranch(const std::string &questId, const std::string &choiceId);

    bool RewardQuest(const std::string &questId,
                     int &outGold,
                     int &outXp,
                     std::string &outItemId,
                     int &outItemCount);

    bool HasFlag(const std::string &flag) const;

    const QuestDefinition *GetDefinition(const std::string &questId) const;
    const QuestState *GetState(const std::string &questId) const;

    std::vector<const QuestState *> GetActiveQuests() const;
    std::vector<const QuestState *> GetAvailableQuests() const;

private:
    void UpdateAvailability();
    void CheckStageProgress(const std::string &questId);
    bool EventMatchesObjective(const QuestEvent &event, const QuestObjectiveDefinition &def) const;

private:
    std::unordered_map<std::string, QuestDefinition> definitions_;
    std::unordered_map<std::string, QuestState> states_;
    std::unordered_set<std::string> storyFlags_;
};