#include "world/World.h"
#include "raylib.h"

void World::OpenChoiceUi(const std::string &questId)
{
    const QuestDefinition *def = questSystem_.GetDefinition(questId);
    if (!def || def->branchChoices.empty())
        return;

    choiceUi_.visible = true;
    choiceUi_.questId = questId;
    choiceUi_.choiceIds.clear();
    choiceUi_.choiceTexts.clear();
    choiceUi_.selectedIndex = 0;

    for (const auto &choice : def->branchChoices)
    {
        choiceUi_.choiceIds.push_back(choice.id);
        choiceUi_.choiceTexts.push_back(choice.text);
    }
}

void World::CloseChoiceUi()
{
    choiceUi_.visible = false;
    choiceUi_.questId.clear();
    choiceUi_.choiceIds.clear();
    choiceUi_.choiceTexts.clear();
    choiceUi_.selectedIndex = 0;
}

void World::UpdateChoiceUi()
{
    if (!choiceUi_.visible)
        return;

    if (IsKeyPressed(KEY_ESCAPE))
    {
        CloseChoiceUi();
        message_ = "Choice cancelled.";
        return;
    }

    if (IsKeyPressed(KEY_UP))
    {
        if (!choiceUi_.choiceTexts.empty())
        {
            choiceUi_.selectedIndex--;
            if (choiceUi_.selectedIndex < 0)
                choiceUi_.selectedIndex = static_cast<int>(choiceUi_.choiceTexts.size()) - 1;
        }
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        if (!choiceUi_.choiceTexts.empty())
        {
            choiceUi_.selectedIndex++;
            if (choiceUi_.selectedIndex >= static_cast<int>(choiceUi_.choiceTexts.size()))
                choiceUi_.selectedIndex = 0;
        }
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        if (choiceUi_.selectedIndex >= 0 &&
            choiceUi_.selectedIndex < static_cast<int>(choiceUi_.choiceIds.size()))
        {
            const std::string chosenId = choiceUi_.choiceIds[choiceUi_.selectedIndex];

            if (questSystem_.ChooseBranch(choiceUi_.questId, chosenId))
            {
                const std::string chosenText = choiceUi_.choiceTexts[choiceUi_.selectedIndex];
                message_ = "Choice made: " + chosenText;
            }
            else
            {
                message_ = "Could not apply choice.";
            }

            CloseChoiceUi();
        }
    }
}

void World::DrawChoiceUi() const
{
    if (!choiceUi_.visible)
        return;

    DrawRectangle(0, 0, screenWidth_, screenHeight_, Fade(BLACK, 0.55f));

    const int boxW = 620;
    const int boxH = 240;
    const int boxX = (screenWidth_ - boxW) / 2;
    const int boxY = (screenHeight_ - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.90f));
    DrawRectangleLines(boxX, boxY, boxW, boxH, WHITE);

    DrawText("Choose Your Path", boxX + 20, boxY + 18, 28, YELLOW);
    DrawText("Use Up/Down to select, Enter to confirm, Esc to cancel",
             boxX + 20, boxY + 52, 18, LIGHTGRAY);

    int y = boxY + 95;
    for (int i = 0; i < static_cast<int>(choiceUi_.choiceTexts.size()); ++i)
    {
        const bool selected = (i == choiceUi_.selectedIndex);

        if (selected)
        {
            DrawRectangle(boxX + 16, y - 4, boxW - 32, 30, Fade(SKYBLUE, 0.25f));
            DrawRectangleLines(boxX + 16, y - 4, boxW - 32, 30, SKYBLUE);
        }

        std::string line = std::to_string(i + 1) + ". " + choiceUi_.choiceTexts[i];
        DrawText(line.c_str(), boxX + 28, y, 22, selected ? WHITE : LIGHTGRAY);
        y += 38;
    }
}
