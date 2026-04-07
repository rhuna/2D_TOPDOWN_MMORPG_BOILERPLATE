#include "world/World.h"

#include <algorithm>

void World::HandleDrops()
{
    drops_.erase(
        std::remove_if(drops_.begin(), drops_.end(), [](const Drop &drop)
                       { return drop.taken; }),
        drops_.end());
}

void World::HandleBuildingTransitions()
{
    Rectangle playerRect{
        player_.position.x,
        player_.position.y,
        player_.size.x,
        player_.size.y};

    for (const auto &door : buildingDoors_)
    {
        if (CheckCollisionRecs(playerRect, door.trigger) && IsKeyPressed(KEY_E))
        {
            player_.position = door.insideSpawn;
            message_ = "You entered the building.";
            return;
        }
    }

    for (const auto &exitDoor : exitDoors_)
    {
        if (CheckCollisionRecs(playerRect, exitDoor.trigger) && IsKeyPressed(KEY_E))
        {
            player_.position = exitDoor.outsideSpawn;
            message_ = "You left the building.";
            return;
        }
    }
}
