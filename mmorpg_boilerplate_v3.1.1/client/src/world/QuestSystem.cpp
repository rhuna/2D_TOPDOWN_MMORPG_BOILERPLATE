#include ../include/world/QuestSystem.h
#include <fstream>
#include <nlohmann/json.hpp>


void QuestSystem::LoadDefinitions(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open quest definitions file: " + filePath);
    }
    nlohmann::json jsonData;
    file >> jsonData;
    for (const auto &questJson : jsonData)
    {
        QuestDefinition questDef;
        questDef.id = questJson["id"].get<std::string>();
        questDef.title = questJson["title"].get<std::string>();
        questDef.description = questJson["description"].get<std::string>();

        for (const auto &objJson : questJson["objectives"])
        {
            QuestObjectiveDefinition objDef;
            objDef.type = objJson["type"].get<std::string>();
            objDef.targetId = objJson["targetId"].get<std::string>();
            objDef.requiredCount = objJson.value("requiredCount", 1);
            questDef.objectives.push_back(objDef);
        }

        const auto &rewardJson = questJson["rewards"];
        QuestRewardDefinition rewardDef;
        rewardDef.gold = rewardJson.value("gold", 0);
        rewardDef.xp = rewardJson.value("xp", 0);
        rewardDef.itemId = rewardJson.value("itemId", "");
        rewardDef.itemCount = rewardJson.value("itemCount", 0);
        questDef.rewards = rewardDef;

        questDef.prerequisiteQuestIds = questJson.value("prerequisiteQuestIds", std::vector<std::string>{});
        questDef.nextQuestIds = questJson.value("nextQuestIds", std::vector<std::string>{});
        questDef.giverNpcId = questJson.value("giverNpcId", "");
        questDef.completerNpcId = questJson.value("completerNpcId", "");
        questDef.mainStory = questJson.value("mainStory", false);

        definitions_[questDef.id] = std::move(questDef);
    }
}

void QuestSystem::InitializePlayerQuests()
{
    states_.clear();
    UpdateAvailability();
}

void QuestSystem::NotifyEvent(const QuestEvent &event)
{
    for (auto &[questId, state] : states_)
    {
        if (state.status != QuestStatus::Active)
            continue;

        const auto &def = definitions_[questId];
        bool updated = false;
        for (size_t i = 0; i < def.objectives.size(); ++i)
        {
            const auto &objDef = def.objectives[i];
            if (objDef.type == event.type && objDef.targetId == event.targetId)
            {
                auto &objState = state.objectives[i];
                if (!objState.complete)
                {
                    objState.currentCount += event.amount;
                    if (objState.currentCount >= objDef.requiredCount)
                    {
                        objState.complete = true;
                        updated = true;
                    }
                }
            }
        }
        if (updated)
        {
            CheckQuestCompletion(questId);
        }
    }
}

bool QuestSystem::CanAcceptQuest(const std::string &questId) const
{
    auto it = definitions_.find(questId);
    if (it == definitions_.end())
        return false;

    const auto &def = it->second;
    for (const auto &prereqId : def.prerequisiteQuestIds)
    {
        auto stateIt = states_.find(prereqId);
        if (stateIt == states_.end() || stateIt->second.status != QuestStatus::Completed)
            return false;
    }
    return true;
}

void QuestSystem::AcceptQuest(const std::string &questId)
{
    if (!CanAcceptQuest(questId))
        return;

    QuestState state;
    state.questId = questId;
    state.status = QuestStatus::Active;
    state.objectives.resize(definitions_[questId].objectives.size());
    states_[questId] = std::move(state);
    UpdateAvailability();
}

void QuestSystem::RewardQuest(const std::string &questId)
{
    auto it = states_.find(questId);
    if (it == states_.end() || it->second.status != QuestStatus::Completed)
        return;

    it->second.status = QuestStatus::Rewarded;
    const auto &def = definitions_[questId];

    auto reward = def.rewards;
    
    for (const auto &nextId : def.nextQuestIds)
    {
        if (CanAcceptQuest(nextId))
        {
            states_[nextId] = QuestState{nextId, QuestStatus::Available};
        }
    }
    UpdateAvailability();
}

const QuestState *QuestSystem::GetQuestState(const std::string &questId) const
{
    auto it = states_.find(questId);
    if (it == states_.end())
        return nullptr;
    return &it->second;
}
std::vector<const QuestState *> QuestSystem::GetActiveQuests() const
{
    std::vector<const QuestState *> activeQuests;
    for (const auto &[id, state] : states_)
    {
        if (state.status == QuestStatus::Active)
        {
            activeQuests.push_back(&state);
        }
    }
    return activeQuests;
}

void QuestSystem::UpdateAvailability()
{
    for (const auto &[id, def] : definitions_)
    {
        if (states_.find(id) == states_.end())
        {
            if (CanAcceptQuest(id))
            {
                states_[id] = QuestState{id, QuestStatus::Available};
            }
        }
    }
}

void QuestSystem::CheckQuestCompletion(const std::string &questId)
{
    auto it = states_.find(questId);
    if (it == states_.end() || it->second.status != QuestStatus::Active)
        return;

    const auto &def = definitions_[questId];
    bool allComplete = true;
    for (size_t i = 0; i < def.objectives.size(); ++i)
    {
        if (!it->second.objectives[i].complete)
        {
            allComplete = false;
            break;
        }
    }
    if (allComplete)
    {
        it->second.status = QuestStatus::Completed;
    }
}

