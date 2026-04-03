#pragma once

#include <string>
#include <vector>

enum class QuestStatus
{
    Locked,
    Available,
    Active,
    Completed,
    AwaitingChoice,
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
    std::string type;
    std::string targetId;
    int requiredCount = 1;
};

struct QuestRewardDefinition
{
    int gold = 0;
    int xp = 0;
    std::string itemId;
    int itemCount = 0;
};

struct QuestStageDefinition
{
    std::string id;
    std::string description;
    std::vector<QuestObjectiveDefinition> objectives;
};

struct QuestBranchChoiceDefinition
{
    std::string id;
    std::string text;
    std::vector<std::string> setFlags;
    std::vector<std::string> unlockQuestIds;
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

    std::vector<QuestStageDefinition> stages;
    QuestRewardDefinition rewards;

    std::vector<QuestBranchChoiceDefinition> branchChoices;
};

struct QuestObjectiveState
{
    int currentCount = 0;
    bool complete = false;
};

struct QuestStageState
{
    std::vector<QuestObjectiveState> objectives;
    bool complete = false;
};

struct QuestState
{
    std::string questId;
    QuestStatus status = QuestStatus::Locked;
    int currentStageIndex = 0;
    std::vector<QuestStageState> stages;
    std::string chosenBranchId;
};

struct QuestEvent
{
    QuestEventType type;
    std::string targetId;
    int amount = 1;
};