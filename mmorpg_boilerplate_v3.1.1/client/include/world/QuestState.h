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