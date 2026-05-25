#pragma once

enum class ShellType {
    LIVE,
    BLANK
};

struct Shell {
    ShellType type;
    bool known = false; // revealed by magnifying glass or phone

    Shell();
    Shell(ShellType t);

    bool isLive() const;
    bool isBlank() const;
};
