#include "world/World.h"
#include "raylib.h"
#include <cmath>

namespace
{
    bool HasMovement(const Actor& actor)
    {
        return std::fabs(actor.moveIntent.x) > 0.01f || std::fabs(actor.moveIntent.y) > 0.01f;
    }
}


void World::Draw() const
{
    BeginMode2D(camera_);
    DrawMap();

    for (const auto &npc : npcs_)
    {
        DrawNpc(npc);
        //DrawCircleV(npc.position, 10.0f, GREEN);
    }

    for (const auto &enemy : enemies_)
    {
        if (enemy.alive)
        {
            DrawEnemy(enemy);
            //DrawCircleV(enemy.position, 10.0f, RED);
        }
    }

    for (const auto &drop : drops_)
    {
        if (!drop.taken)
        {
            DrawPickupSprite(drop.position, tiles_.herb, WHITE);
        }
    }

    DrawPlayer();

    for (const auto &remote : remotePlayers_)
    {
        DrawRemotePlayer(remote);
    }

    EndMode2D();

    DrawHud();
    DrawMinimap();

    if (showBigMap_)
    {
        DrawWorldMapOverlay();
    }

    if (choiceUi_.visible)
    {
        DrawChoiceUi();
    }

    if (shopUi_.IsOpen())
    {
        DrawShopUi();
    }

    if (inventoryUi_.IsOpen())
    {
        DrawInventoryUi();
    }

    if (equipmentUi_.IsOpen())
    {
        DrawEquipmentUi();
    }

    if (questLogUi_.IsOpen())
    {
        DrawQuestLogUi();
    }
}

void World::DrawMap() const {
    const int mapHeight = static_cast<int>(map_.size());
    const int mapWidth = mapHeight > 0 ? static_cast<int>(map_[0].size()) : 0;
    const float halfVisibleWidth = screenWidth_ * 0.5f / camera_.zoom;
    const float halfVisibleHeight = screenHeight_ * 0.5f / camera_.zoom;

    const int startX = std::max(0, static_cast<int>((camera_.target.x - halfVisibleWidth) / tileSize_) - 2);
    const int endX = std::min(mapWidth - 1, static_cast<int>((camera_.target.x + halfVisibleWidth) / tileSize_) + 2);
    const int startY = std::max(0, static_cast<int>((camera_.target.y - halfVisibleHeight) / tileSize_) - 2);
    const int endY = std::min(mapHeight - 1, static_cast<int>((camera_.target.y + halfVisibleHeight) / tileSize_) + 2);

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            Rectangle destination{
                static_cast<float>(x * tileSize_),
                static_cast<float>(y * tileSize_),
                static_cast<float>(tileSize_),
                static_cast<float>(tileSize_)
            };

            char tile = map_[y][x];
            const bool isWall = tile == '#';
            const bool isDoor = (tile == 'B' || tile == 'D');

            if (!tilesetLoaded_) {
                DrawRectangleRec(destination, isWall ? DARKGRAY : Color{30, 80, 35, 255});
                DrawRectangleLinesEx(destination, 1.0f, Fade(BLACK, 0.18f));
                continue;
            }

            Rectangle source{};
            if (isWall)
            {
                bool isTopEdge = (y == 0 || map_[y - 1][x] != '#');
                const std::vector<Rectangle> &palette = isTopEdge ? tiles_.wallTop : tiles_.wallFace;
                source = palette[TileVariantIndex(x, y, static_cast<int>(palette.size()))];
            }
            else if (isDoor)
            {
                source = tiles_.path[0];
            }
            else if (tile == 'p')
            {
                source = tiles_.path[TileVariantIndex(x, y, static_cast<int>(tiles_.path.size()))];
            }
            else
            {
                source = tiles_.grass[TileVariantIndex(x, y, static_cast<int>(tiles_.grass.size()))];
            }

            DrawTexturePro(tileset_, source, destination, Vector2{0.0f, 0.0f}, 0.0f, WHITE);

            if (!isWall && tile != 'p' && TileVariantIndex(x + 13, y + 7, 8) == 0) {
                Rectangle dirtDest{
                    destination.x + 8.0f,
                    destination.y + 8.0f,
                    16.0f,
                    16.0f
                };
                DrawTexturePro(
                    tileset_,
                    tiles_.dirt[TileVariantIndex(x + 5, y + 11, static_cast<int>(tiles_.dirt.size()))],
                    dirtDest,
                    Vector2{0.0f, 0.0f},
                    0.0f,
                    Fade(WHITE, 0.75f)
                );
            }
        }
    }
}

void World::DrawHud() const
{
    const int hudHeight = 128;
    DrawRectangle(0, screenHeight_ - hudHeight, screenWidth_, hudHeight, Fade(BLACK, 0.75f));

    if (tilesetLoaded_)
    {
        Rectangle swordDestination{16.0f, static_cast<float>(screenHeight_ - 118), 24.0f, 24.0f};
        DrawTexturePro(tileset_, tiles_.sword, swordDestination, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
    }

    std::ostringstream stats;
    stats << "HP: " << player_.hp << "/" << player_.maxHp
          << "   XP: " << player_.xp
          << "   Gold: " << player_.gold
          << "   Weapon: " << player_.weapon.name
          << " (DMG " << player_.weapon.damage << ")"
          << "   Armor: " << player_.armor.name
          << " (DEF " << player_.armor.defense << ")";
    DrawText(stats.str().c_str(), 48, screenHeight_ - 116, 20, WHITE);

    std::ostringstream quest;
    auto activeQuests = questSystem_.GetActiveQuests();

    if (!activeQuests.empty())
    {
        const QuestState *state = activeQuests.front();
        const QuestDefinition *def = questSystem_.GetDefinition(state->questId);

        if (def)
        {
            quest << "Quest: " << def->title;

            if (state->status == QuestStatus::AwaitingChoice)
            {
                quest << " - Choose [1] or [2]";
            }
            else if (state->currentStageIndex >= 0 &&
                     state->currentStageIndex < static_cast<int>(def->stages.size()))
            {
                const auto &stageDef = def->stages[state->currentStageIndex];
                const auto &stageState = state->stages[state->currentStageIndex];

                quest << " - " << stageDef.description;

                if (!stageDef.objectives.empty() && !stageState.objectives.empty())
                {
                    quest << " ["
                          << stageState.objectives[0].currentCount
                          << "/"
                          << stageDef.objectives[0].requiredCount
                          << "]";
                }
            }
        }
    }
    else
    {
        quest << "Quest: No active quests.";
    }

    DrawText(quest.str().c_str(), 16, screenHeight_ - 88, 20, YELLOW);

    std::ostringstream inventory;
    inventory << "Inventory: ";
    for (std::size_t i = 0; i < player_.inventory.size(); ++i)
    {
        inventory << player_.inventory[i].name << " x" << player_.inventory[i].count;
        if (i + 1 < player_.inventory.size())
        {
            inventory << ", ";
        }
    }

    DrawText(inventory.str().c_str(), 16, screenHeight_ - 60, 20, SKYBLUE);

    const char *controls = showBigMap_
                               ? "TAB closes map | I inventory | C equipment | Q quests"
                               : "Mouse wheel zooms | TAB map | I inventory | C equipment | Q quests";

    DrawText(controls, screenWidth_ - 470, screenHeight_ - 32, 18, LIGHTGRAY);
    DrawText(message_.c_str(), 16, screenHeight_ - 32, 18, LIGHTGRAY);
}

void World::DrawMinimap() const {
    const int miniW = 220;
    const int miniH = 220;
    const int pad = 16;
    const int originX = screenWidth_ - miniW - pad;
    const int originY = 16;
    const int mapHeight = static_cast<int>(map_.size());
    const int mapWidth = mapHeight > 0 ? static_cast<int>(map_[0].size()) : 0;

    DrawRectangle(originX - 4, originY - 4, miniW + 8, miniH + 8, Fade(BLACK, 0.65f));
    DrawRectangleLines(originX - 4, originY - 4, miniW + 8, miniH + 8, Fade(WHITE, 0.35f));

    const float scaleX = static_cast<float>(miniW) / static_cast<float>(mapWidth);
    const float scaleY = static_cast<float>(miniH) / static_cast<float>(mapHeight);

    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            Color c = DARKGREEN;
            if (map_[y][x] == '#') c = GRAY;
            else if (map_[y][x] == 'p') c = Color{166, 124, 82, 255};
            DrawRectangle(
                originX + static_cast<int>(x * scaleX),
                originY + static_cast<int>(y * scaleY),
                std::max(1, static_cast<int>(std::ceil(scaleX))),
                std::max(1, static_cast<int>(std::ceil(scaleY))),
                c
            );
        }
    }

    for (const auto& enemy : enemies_) {
        if (!enemy.alive) continue;
        DrawCircle(
            originX + static_cast<int>((enemy.position.x / tileSize_) * scaleX),
            originY + static_cast<int>((enemy.position.y / tileSize_) * scaleY),
            2.0f,
            RED
        );
    }

    for (const auto& npc : npcs_) {
        DrawCircle(
            originX + static_cast<int>((npc.position.x / tileSize_) * scaleX),
            originY + static_cast<int>((npc.position.y / tileSize_) * scaleY),
            2.0f,
            YELLOW
        );
    }

    DrawCircle(
        originX + static_cast<int>((player_.position.x / tileSize_) * scaleX),
        originY + static_cast<int>((player_.position.y / tileSize_) * scaleY),
        3.0f,
        SKYBLUE
    );

    const float halfVisibleWidth = screenWidth_ * 0.5f / camera_.zoom;
    const float halfVisibleHeight = screenHeight_ * 0.5f / camera_.zoom;
    Rectangle visible{
        originX + ((camera_.target.x - halfVisibleWidth) / tileSize_) * scaleX,
        originY + ((camera_.target.y - halfVisibleHeight) / tileSize_) * scaleY,
        (screenWidth_ / camera_.zoom / tileSize_) * scaleX,
        (screenHeight_ / camera_.zoom / tileSize_) * scaleY
    };
    DrawRectangleLinesEx(visible, 1.0f, WHITE);
    DrawText("Mini Map", originX + 6, originY + miniH + 6, 18, WHITE);
}

void World::DrawWorldMapOverlay() const {
    const int pad = 42;
    const int originX = pad;
    const int originY = pad;
    const int overlayW = screenWidth_ - pad * 2;
    const int overlayH = screenHeight_ - pad * 2;
    const int mapHeight = static_cast<int>(map_.size());
    const int mapWidth = mapHeight > 0 ? static_cast<int>(map_[0].size()) : 0;
    const float scaleX = static_cast<float>(overlayW) / static_cast<float>(mapWidth);
    const float scaleY = static_cast<float>(overlayH) / static_cast<float>(mapHeight);

    DrawRectangle(0, 0, screenWidth_, screenHeight_, Fade(BLACK, 0.72f));
    DrawRectangle(originX - 6, originY - 6, overlayW + 12, overlayH + 12, Fade(BLACK, 0.85f));
    DrawRectangleLines(originX - 6, originY - 6, overlayW + 12, overlayH + 12, Fade(WHITE, 0.55f));

    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            Color c = Color{44, 96, 48, 255};
            if (map_[y][x] == '#') c = Color{76, 82, 96, 255};
            else if (map_[y][x] == 'p') c = Color{176, 132, 84, 255};
            DrawRectangle(
                originX + static_cast<int>(x * scaleX),
                originY + static_cast<int>(y * scaleY),
                std::max(1, static_cast<int>(std::ceil(scaleX))),
                std::max(1, static_cast<int>(std::ceil(scaleY))),
                c
            );
        }
    }

    for (const auto& npc : npcs_) {
        DrawCircle(
            originX + static_cast<int>((npc.position.x / tileSize_) * scaleX),
            originY + static_cast<int>((npc.position.y / tileSize_) * scaleY),
            3.5f,
            GOLD
        );
    }

    for (const auto& enemy : enemies_) {
        if (!enemy.alive) continue;
        DrawCircle(
            originX + static_cast<int>((enemy.position.x / tileSize_) * scaleX),
            originY + static_cast<int>((enemy.position.y / tileSize_) * scaleY),
            2.0f,
            RED
        );
    }

    DrawCircle(
        originX + static_cast<int>((player_.position.x / tileSize_) * scaleX),
        originY + static_cast<int>((player_.position.y / tileSize_) * scaleY),
        4.0f,
        SKYBLUE
    );

    const float halfVisibleWidth = screenWidth_ * 0.5f / camera_.zoom;
    const float halfVisibleHeight = screenHeight_ * 0.5f / camera_.zoom;
    Rectangle visible{
        originX + ((camera_.target.x - halfVisibleWidth) / tileSize_) * scaleX,
        originY + ((camera_.target.y - halfVisibleHeight) / tileSize_) * scaleY,
        (screenWidth_ / camera_.zoom / tileSize_) * scaleX,
        (screenHeight_ / camera_.zoom / tileSize_) * scaleY
    };
    DrawRectangleLinesEx(visible, 2.0f, WHITE);

    DrawText("World Map", originX, 10, 28, WHITE);
    DrawText("Starter Village", originX + 18, originY + 18, 20, YELLOW);
    DrawText("North Marsh", originX + static_cast<int>(mapWidth * 0.35f * scaleX), originY + 28, 20, YELLOW);
    DrawText("Central Road", originX + static_cast<int>(mapWidth * 0.45f * scaleX), originY + static_cast<int>(mapHeight * 0.43f * scaleY), 20, YELLOW);
    DrawText("East Frontier", originX + static_cast<int>(mapWidth * 0.73f * scaleX), originY + static_cast<int>(mapHeight * 0.72f * scaleY), 20, YELLOW);
    DrawText("TAB to close", screenWidth_ - 190, 14, 22, LIGHTGRAY);
}

void World::DrawPlayer() const {
    DrawAnimatedActor(player_, playerAtlas_, WHITE);
}

void World::DrawNpc(const Npc& npc) const {
    DrawAnimatedActor(npc, npcAtlas_, Color{245, 235, 200, 255});
    DrawText(npc.name.c_str(), static_cast<int>(npc.position.x) - 12, static_cast<int>(npc.position.y) - 18, 12, WHITE);
}

void World::DrawEnemy(const Enemy& enemy) const {
    DrawAnimatedActor(enemy, slimeAtlas_, WHITE);
    DrawRectangle(static_cast<int>(enemy.position.x), static_cast<int>(enemy.position.y) - 8, 28, 4, DARKGRAY);
    DrawRectangle(static_cast<int>(enemy.position.x), static_cast<int>(enemy.position.y) - 8, 28 * enemy.hp / enemy.maxHp, 4, RED);
}

void World::DrawRemotePlayer(const RemoteActor &remote) const
{
    DrawAnimatedActor(remote, playerAtlas_, Color{170, 210, 255, 255});

    DrawText(remote.name.c_str(),
             static_cast<int>(remote.position.x - 6.0f),
             static_cast<int>(remote.position.y - 12.0f),
             10,
             WHITE);

    DrawRectangle(static_cast<int>(remote.position.x),
                  static_cast<int>(remote.position.y) - 18,
                  30, 4, DARKGRAY);

    DrawRectangle(static_cast<int>(remote.position.x),
                  static_cast<int>(remote.position.y) - 18,
                  static_cast<int>(30 * remote.hp / 20.0f),
                  4, RED);
}

void World::DrawAnimatedActor(const Actor& actor, const DirectionalSprite& spriteSet, Color tint) const {
    bool walking = HasMovement(actor);
    const AnimationClip& clip = SelectClip(spriteSet, actor.facing, walking);
    const AtlasFrame& frame = ResolveFrame(clip, actor.animClock);

    Rectangle destination{
        actor.position.x,
        actor.position.y + frame.yBob,
        actor.size.x,
        actor.size.y
    };

    if (tilesetLoaded_) {
        Rectangle source = frame.source;
        if (frame.flipX) {
            source.x += source.width;
            source.width *= -1.0f;
        }
        DrawTexturePro(tileset_, source, destination, Vector2{0.0f, 0.0f}, 0.0f, tint);
    } else {
        DrawRectangleRec(destination, tint);
    }
}

void World::DrawPickupSprite(Vector2 position, Rectangle source, Color tint) const {
    Rectangle destination{position.x, position.y, 24.0f, 24.0f};
    if (tilesetLoaded_) {
        DrawTexturePro(tileset_, source, destination, Vector2{0.0f, 0.0f}, 0.0f, tint);
    } else {
        DrawCircle(static_cast<int>(position.x) + 12, static_cast<int>(position.y) + 12, 8.0f, tint);
    }
}