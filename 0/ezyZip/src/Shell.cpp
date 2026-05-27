#include "Shell.h"

Shell::Shell() : type(ShellType::BLANK), known(false) {}
Shell::Shell(ShellType t) : type(t), known(false) {}

bool Shell::isLive() const { return type == ShellType::LIVE; }
bool Shell::isBlank() const { return type == ShellType::BLANK; }
