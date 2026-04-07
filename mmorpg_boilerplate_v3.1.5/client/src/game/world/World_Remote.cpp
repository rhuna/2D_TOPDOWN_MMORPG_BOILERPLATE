#include "game/world/World.h"
#include <algorithm>
#include <cmath>

namespace
{
    Vector2 NormalizeOrZero(Vector2 value)
    {
        float length = std::sqrt(value.x * value.x + value.y * value.y);
        if (length <= 0.0001f)
        {
            return Vector2{0.0f, 0.0f};
        }
        return Vector2{value.x / length, value.y / length};
    }
}

void World::UpdateRemotePlayers(const std::unordered_map<int, RemotePlayer> &snapshots, int localId, float dt)
{
    remotePlayers_.erase(
        std::remove_if(remotePlayers_.begin(), remotePlayers_.end(),
            [&](const RemoteActor& remote) {
                return snapshots.find(remote.id) == snapshots.end() || remote.id == localId;
            }),
        remotePlayers_.end()
    );

    for (const auto& [id, snapshot] : snapshots) {
        if (id == localId) {
            continue;
        }

        auto it = std::find_if(remotePlayers_.begin(), remotePlayers_.end(),
            [&](const RemoteActor& remote) {
                return remote.id == id;
            });

        if (it == remotePlayers_.end()) {
            RemoteActor remote;
            remote.id = id;
            remote.position = {snapshot.x, snapshot.y};
            remote.targetPosition = {snapshot.x, snapshot.y};
            remote.size = player_.size;
            remote.facing = Direction::Down;
            remote.color = Color{170, 210, 255, 255};
            remote.hp = snapshot.hp;
            remote.name = snapshot.name;
            remotePlayers_.push_back(remote);
            continue;
        }

        it->targetPosition = {snapshot.x, snapshot.y};
        it->hp = snapshot.hp;
        it->name = snapshot.name;

        Vector2 toTarget{
            it->targetPosition.x - it->position.x,
            it->targetPosition.y - it->position.y
        };

        const float distance = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
        if (distance > 0.25f) {
            it->moveIntent = NormalizeOrZero(toTarget);
            it->facing = DirectionFromVector(it->moveIntent);
            it->animClock += dt;

            const float smoothing = std::min(1.0f, dt * 10.0f);
            it->position.x += toTarget.x * smoothing;
            it->position.y += toTarget.y * smoothing;
        } else {
            it->position = it->targetPosition;
            it->moveIntent = {0.0f, 0.0f};
        }
    }
}
