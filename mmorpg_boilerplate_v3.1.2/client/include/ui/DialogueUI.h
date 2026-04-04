

#pragma once

#include <string>
#include <vector>

class DialogueUI
{
public:
    DialogueUI() = default;
    ~DialogueUI() = default;

    void Open(const std::string &npcName, const std::vector<std::string> &dialogueLines);
    void Close();
    void Update();
    void Draw() const;
    bool IsOpen() const;

private:
    bool visible_ = false;
    std::string npcName_;
    std::vector<std::string> dialogueLines_;
    int currentLineIndex_ = 0;
};

#endif // DIALOGUE_UI_H