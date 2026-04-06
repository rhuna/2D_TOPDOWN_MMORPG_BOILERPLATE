#include "ui/DialogueUI.h" // for DialogueUI class definition
#include "raylib.h" // for drawing functions and types






void DialogueUI::Open(const std::string& npcName, const std::vector<std::string>& dialogueLines) {
    npcName_ = npcName;
    dialogueLines_ = dialogueLines;
    currentLineIndex_ = 0;
    visible_ = true;
}

void DialogueUI::Close() {
    visible_ = false;
    npcName_.clear();
    dialogueLines_.clear();
    currentLineIndex_ = 0;
}

void DialogueUI::Update() {
    if (!visible_) return;

    if (IsKeyPressed(KEY_ENTER)) {
        currentLineIndex_++;
        if (currentLineIndex_ >= dialogueLines_.size()) {
            Close();
        }
    }
}

void DialogueUI::Draw() const {
    if (!visible_) return;

    // Draw a simple dialogue box
    int boxWidth = 600;
    int boxHeight = 150;
    int boxX = (GetScreenWidth() - boxWidth) / 2;
    int boxY = GetScreenHeight() - boxHeight - 50;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(BLACK, 0.8f));
    DrawText(npcName_.c_str(), boxX + 20, boxY + 20, 24, YELLOW);

    if (currentLineIndex_ < dialogueLines_.size()) {
        DrawText(dialogueLines_[currentLineIndex_].c_str(), boxX + 20, boxY + 60, 22, WHITE);
        DrawText("Press Enter to continue...", boxX + 20, boxY + boxHeight - 30, 18, LIGHTGRAY);
    }
}

bool DialogueUI::IsOpen() const {
    return visible_;
}

