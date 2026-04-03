#include <string>

enum class QuestEventType
{
    EnemyKilled,
    NpcTalkedTo,
    ItemCollected,
    RegionEntered,
    PlayerLeveled,
    ObjectInteracted
};

struct QuestEvent
{
    QuestEventType type;
    std::string targetId;
    int amount = 1;
};