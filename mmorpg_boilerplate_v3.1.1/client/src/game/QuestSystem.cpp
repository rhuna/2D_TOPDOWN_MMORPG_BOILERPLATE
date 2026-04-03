#include "game/QuestSystem.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static QuestEventType ParseEventType(const std::string &type)
{
    if (type == "kill")
        return QuestEventType::EnemyKilled;
    if (type == "talk")
        return QuestEventType::NpcTalkedTo;
    if (type == "collect")
        return QuestEventType::ItemCollected;
    return QuestEventType::RegionEntered;
}

bool QuestSystem::LoadFromJson(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
        return false;

    json root;
    file >> root;

    definitions_.clear();

    for (const auto &q : root["quests"])
    {
        QuestDefinition def;
        def.id = q.value("id", "");
        def.title = q.value("title", "");
        def.description = q.value("description", "");
        def.giverNpcId = q.value("giverNpcId", "");
        def.completerNpcId = q.value("completerNpcId", "");
        def.mainStory = q.value("mainStory", false);

        if (q.contains("prerequisiteQuestIds"))
        {
            for (const auto &item : q["prerequisiteQuestIds"])
                def.prerequisiteQuestIds.push_back(item.get<std::string>());
        }

        if (q.contains("nextQuestIds"))
        {
            for (const auto &item : q["nextQuestIds"])
                def.nextQuestIds.push_back(item.get<std::string>());
        }

        if (q.contains("objectives"))
        {
            for (const auto &obj : q["objectives"])
            {
                QuestObjectiveDefinition objective;
                objective.type = obj.value("type", "");
                objective.targetId = obj.value("targetId", "");
                objective.requiredCount = obj.value("requiredCount", 1);
                def.objectives.push_back(objective);
            }
        }

        if (q.contains("rewards"))
        {
            def.rewards.gold = q["rewards"].value("gold", 0);
            def.rewards.xp = q["rewards"].value("xp", 0);
            def.rewards.itemId = q["rewards"].value("itemId", "");
            def.rewards.itemCount = q["rewards"].value("itemCount", 0);
        }

        definitions_[def.id] = def;
    }

    InitializeStates();
    return true;
}

void QuestSystem::InitializeStates()
{
    states_.clear();

    for (const auto &[id, def] : definitions_)
    {
        QuestState state;
        state.questId = id;
        state.status = def.prerequisiteQuestIds.empty() ? QuestStatus::Available : QuestStatus::Locked;
        state.objectives.resize(def.objectives.size());
        states_[id] = state;
    }
}

void QuestSystem::UpdateAvailability()
{
    for (auto &[id, state] : states_)
    {
        if (state.status != QuestStatus::Locked)
            continue;

        const auto &def = definitions_.at(id);

        bool allMet = true;
        for (const auto &prereq : def.prerequisiteQuestIds)
        {
            auto it = states_.find(prereq);
            if (it == states_.end() || it->second.status != QuestStatus::Rewarded)
            {
                allMet = false;
                break;
            }
        }

        if (allMet)
            state.status = QuestStatus::Available;
    }
}

bool QuestSystem::CanAcceptQuest(const std::string &questId) const
{
    auto it = states_.find(questId);
    return it != states_.end() && it->second.status == QuestStatus::Available;
}

bool QuestSystem::AcceptQuest(const std::string &questId)
{
    auto it = states_.find(questId);
    if (it == states_.end() || it->second.status != QuestStatus::Available)
        return false;

    it->second.status = QuestStatus::Active;
    return true;
}

bool QuestSystem::EventMatchesObjective(const QuestEvent &event, const QuestObjectiveDefinition &def) const
{
    if (def.targetId != event.targetId)
        return false;

    if (def.type == "kill" && event.type == QuestEventType::EnemyKilled)
        return true;
    if (def.type == "talk" && event.type == QuestEventType::NpcTalkedTo)
        return true;
    if (def.type == "collect" && event.type == QuestEventType::ItemCollected)
        return true;
    if (def.type == "visit" && event.type == QuestEventType::RegionEntered)
        return true;

    return false;
}

void QuestSystem::NotifyEvent(const QuestEvent &event)
{
    for (auto &[id, state] : states_)
    {
        if (state.status != QuestStatus::Active)
            continue;

        const auto &def = definitions_.at(id);

        for (std::size_t i = 0; i < def.objectives.size(); ++i)
        {
            auto &objectiveState = state.objectives[i];
            const auto &objectiveDef = def.objectives[i];

            if (objectiveState.complete)
                continue;

            if (EventMatchesObjective(event, objectiveDef))
            {
                objectiveState.currentCount += event.amount;
                if (objectiveState.currentCount >= objectiveDef.requiredCount)
                {
                    objectiveState.currentCount = objectiveDef.requiredCount;
                    objectiveState.complete = true;
                }
            }
        }

        CheckCompletion(id);
    }
}

void QuestSystem::CheckCompletion(const std::string &questId)
{
    auto &state = states_.at(questId);
    const auto &def = definitions_.at(questId);

    if (state.status != QuestStatus::Active)
        return;

    for (std::size_t i = 0; i < def.objectives.size(); ++i)
    {
        if (!state.objectives[i].complete)
            return;
    }

    state.status = QuestStatus::Completed;
}

bool QuestSystem::RewardQuest(const std::string &questId, int &outGold, int &outXp, std::string &outItemId, int &outItemCount)
{
    auto it = states_.find(questId);
    if (it == states_.end() || it->second.status != QuestStatus::Completed)
        return false;

    const auto &def = definitions_.at(questId);

    outGold = def.rewards.gold;
    outXp = def.rewards.xp;
    outItemId = def.rewards.itemId;
    outItemCount = def.rewards.itemCount;

    it->second.status = QuestStatus::Rewarded;
    UpdateAvailability();
    return true;
}

bool QuestSystem::IsQuestCompleted(const std::string &questId) const
{
    auto it = states_.find(questId);
    return it != states_.end() && (it->second.status == QuestStatus::Completed || it->second.status == QuestStatus::Rewarded);
}

bool QuestSystem::IsQuestActive(const std::string &questId) const
{
    auto it = states_.find(questId);
    return it != states_.end() && it->second.status == QuestStatus::Active;
}

const QuestDefinition *QuestSystem::GetDefinition(const std::string &questId) const
{
    auto it = definitions_.find(questId);
    return it != definitions_.end() ? &it->second : nullptr;
}

const QuestState *QuestSystem::GetState(const std::string &questId) const
{
    auto it = states_.find(questId);
    return it != states_.end() ? &it->second : nullptr;
}

std::vector<const QuestState *> QuestSystem::GetActiveQuests() const
{
    std::vector<const QuestState *> result;
    for (const auto &[id, state] : states_)
    {
        if (state.status == QuestStatus::Active)
            result.push_back(&state);
    }
    return result;
}

std::vector<const QuestState *> QuestSystem::GetAvailableQuests() const
{
    std::vector<const QuestState *> result;
    for (const auto &[id, state] : states_)
    {
        if (state.status == QuestStatus::Available)
            result.push_back(&state);
    }
    return result;
}