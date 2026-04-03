#include "game/QuestSystem.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool QuestSystem::LoadFromJson(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
        return false;

    json root;
    file >> root;

    definitions_.clear();
    storyFlags_.clear();

    if (!root.contains("quests") || !root["quests"].is_array())
        return false;

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

        if (q.contains("stages"))
        {
            for (const auto &stageJson : q["stages"])
            {
                QuestStageDefinition stage;
                stage.id = stageJson.value("id", "");
                stage.description = stageJson.value("description", "");

                if (stageJson.contains("objectives"))
                {
                    for (const auto &obj : stageJson["objectives"])
                    {
                        QuestObjectiveDefinition objective;
                        objective.type = obj.value("type", "");
                        objective.targetId = obj.value("targetId", "");
                        objective.requiredCount = obj.value("requiredCount", 1);
                        stage.objectives.push_back(objective);
                    }
                }

                def.stages.push_back(stage);
            }
        }

        if (q.contains("rewards"))
        {
            def.rewards.gold = q["rewards"].value("gold", 0);
            def.rewards.xp = q["rewards"].value("xp", 0);
            def.rewards.itemId = q["rewards"].value("itemId", "");
            def.rewards.itemCount = q["rewards"].value("itemCount", 0);
        }

        if (q.contains("branchChoices"))
        {
            for (const auto &choiceJson : q["branchChoices"])
            {
                QuestBranchChoiceDefinition choice;
                choice.id = choiceJson.value("id", "");
                choice.text = choiceJson.value("text", "");

                if (choiceJson.contains("setFlags"))
                {
                    for (const auto &f : choiceJson["setFlags"])
                        choice.setFlags.push_back(f.get<std::string>());
                }

                if (choiceJson.contains("unlockQuestIds"))
                {
                    for (const auto &uq : choiceJson["unlockQuestIds"])
                        choice.unlockQuestIds.push_back(uq.get<std::string>());
                }

                def.branchChoices.push_back(choice);
            }
        }

        if (!def.id.empty())
        {
            definitions_[def.id] = def;
        }
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
        state.status = def.prerequisiteQuestIds.empty()
                           ? QuestStatus::Available
                           : QuestStatus::Locked;
        state.currentStageIndex = 0;

        for (const auto &stageDef : def.stages)
        {
            QuestStageState stageState;
            stageState.objectives.resize(stageDef.objectives.size());
            state.stages.push_back(stageState);
        }

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

        bool allPrereqsMet = true;
        for (const auto &prereq : def.prerequisiteQuestIds)
        {
            auto it = states_.find(prereq);
            if (it == states_.end() || it->second.status != QuestStatus::Rewarded)
            {
                allPrereqsMet = false;
                break;
            }
        }

        if (allPrereqsMet)
        {
            state.status = QuestStatus::Available;
        }
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
    it->second.currentStageIndex = 0;
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

        if (state.currentStageIndex < 0 || state.currentStageIndex >= static_cast<int>(def.stages.size()))
            continue;

        const auto &currentStageDef = def.stages[state.currentStageIndex];
        auto &currentStageState = state.stages[state.currentStageIndex];

        for (std::size_t i = 0; i < currentStageDef.objectives.size(); ++i)
        {
            auto &objectiveState = currentStageState.objectives[i];
            const auto &objectiveDef = currentStageDef.objectives[i];

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

        CheckStageProgress(id);
    }
}

void QuestSystem::CheckStageProgress(const std::string &questId)
{
    auto &state = states_.at(questId);
    const auto &def = definitions_.at(questId);

    if (state.status != QuestStatus::Active)
        return;

    if (state.currentStageIndex < 0 || state.currentStageIndex >= static_cast<int>(def.stages.size()))
        return;

    auto &currentStageState = state.stages[state.currentStageIndex];
    const auto &currentStageDef = def.stages[state.currentStageIndex];

    for (std::size_t i = 0; i < currentStageDef.objectives.size(); ++i)
    {
        if (!currentStageState.objectives[i].complete)
            return;
    }

    currentStageState.complete = true;
    state.currentStageIndex++;

    if (state.currentStageIndex >= static_cast<int>(def.stages.size()))
    {
        if (!def.branchChoices.empty())
            state.status = QuestStatus::AwaitingChoice;
        else
            state.status = QuestStatus::Completed;
    }
}

bool QuestSystem::ChooseBranch(const std::string &questId, const std::string &choiceId)
{
    auto stateIt = states_.find(questId);
    if (stateIt == states_.end() || stateIt->second.status != QuestStatus::AwaitingChoice)
        return false;

    auto defIt = definitions_.find(questId);
    if (defIt == definitions_.end())
        return false;

    const auto &def = defIt->second;

    for (const auto &choice : def.branchChoices)
    {
        if (choice.id == choiceId)
        {
            stateIt->second.chosenBranchId = choiceId;

            for (const auto &flag : choice.setFlags)
                storyFlags_.insert(flag);

            for (const auto &unlockQuestId : choice.unlockQuestIds)
            {
                auto unlockIt = states_.find(unlockQuestId);
                if (unlockIt != states_.end() && unlockIt->second.status == QuestStatus::Locked)
                {
                    unlockIt->second.status = QuestStatus::Available;
                }
            }

            stateIt->second.status = QuestStatus::Completed;
            return true;
        }
    }

    return false;
}

bool QuestSystem::RewardQuest(const std::string &questId,
                              int &outGold,
                              int &outXp,
                              std::string &outItemId,
                              int &outItemCount)
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
    return it != states_.end() &&
           (it->second.status == QuestStatus::Completed ||
            it->second.status == QuestStatus::Rewarded);
}

bool QuestSystem::IsQuestActive(const std::string &questId) const
{
    auto it = states_.find(questId);
    return it != states_.end() && it->second.status == QuestStatus::Active;
}

bool QuestSystem::IsAwaitingChoice(const std::string &questId) const
{
    auto it = states_.find(questId);
    return it != states_.end() && it->second.status == QuestStatus::AwaitingChoice;
}

bool QuestSystem::HasFlag(const std::string &flag) const
{
    return storyFlags_.find(flag) != storyFlags_.end();
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

std::vector<const QuestState *> GetStatesByStatus(const std::unordered_map<std::string, QuestState> &states, QuestStatus status)
{
    std::vector<const QuestState *> result;
    for (const auto &[id, state] : states)
    {
        if (state.status == status)
            result.push_back(&state);
    }
    return result;
}

std::vector<const QuestState *> QuestSystem::GetAvailableQuests() const
{
    return GetStatesByStatus(states_, QuestStatus::Available);
}