#pragma once

#include <string>
#include <vector>

enum class QuestStatus
{
    Locked,
    Available,
    Active,
    Completed,
    Rewarded,
    Failed
};

enum class QuestEventType
{
    EnemyKilled,
    NpcTalkedTo,
    ItemCollected,
    RegionEntered
};

struct QuestObjectiveDefinition
{
    std::string type;     // "kill", "talk", "collect", "visit"
    std::string targetId; // "slime", "elder_rowan", "wanderer", etc.
    int requiredCount = 1;
};

struct QuestRewardDefinition
{
    int gold = 0;
    int xp = 0;
    std::string itemId;
    int itemCount = 0;
};

struct QuestDefinition
{
    std::string id;
    std::string title;
    std::string description;

    std::string giverNpcId;
    std::string completerNpcId;

    bool mainStory = false;

    std::vector<std::string> prerequisiteQuestIds;
    std::vector<std::string> nextQuestIds;

    std::vector<QuestObjectiveDefinition> objectives;
    QuestRewardDefinition rewards;
};

struct QuestObjectiveState
{
    int currentCount = 0;
    bool complete = false;
};

struct QuestState
{
    std::string questId;
    QuestStatus status = QuestStatus::Locked;
    std::vector<QuestObjectiveState> objectives;
};

struct QuestEvent
{
    QuestEventType type;
    std::string targetId;
    int amount = 1;
};