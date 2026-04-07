#include "game/world/World.h"

void World::OpenChoiceUi(const std::string &questId)
{
    const QuestDefinition *def = questSystem_.GetDefinition(questId);
    if (!def || def->branchChoices.empty())
    {
        SyncUiStateFromLegacy();
        return;
    }

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

    SyncUiStateFromLegacy();
}

void World::CloseChoiceUi()
{
    choiceUi_.visible = false;
    choiceUi_.questId.clear();
    choiceUi_.choiceIds.clear();
    choiceUi_.choiceTexts.clear();
    choiceUi_.selectedIndex = 0;
    SyncUiStateFromLegacy();
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
            SyncUiStateFromLegacy();
        }
    }
}

void World::DrawChoiceUi() const
{
    if (!choiceUi_.visible)
        return;

    uiRenderer_.DrawChoicePanel(
        screenWidth_,
        screenHeight_,
        choiceUi_.choiceTexts,
        choiceUi_.selectedIndex);
}
