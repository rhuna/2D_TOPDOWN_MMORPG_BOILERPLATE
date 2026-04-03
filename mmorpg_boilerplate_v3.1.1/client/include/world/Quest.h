#include <string>
#include <vector>

struct QuestObjectiveDefinition
{
    std::string type;     // kill, talk, collect, visit
    std::string targetId; // slime, elder_rowan, herb, crossroads
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

    std::vector<QuestObjectiveDefinition> objectives;
    QuestRewardDefinition rewards;

    std::vector<std::string> prerequisiteQuestIds;
    std::vector<std::string> nextQuestIds;

    std::string giverNpcId;
    std::string completerNpcId;

    bool mainStory = false;
};