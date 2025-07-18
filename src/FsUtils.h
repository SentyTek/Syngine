#include <filesystem>
#include "SyngineCore.h"

namespace Syngine {

// Path depends on OS, and boy do I love platform-specific code
std::filesystem::path GetAppdataPath(const std::string& appName);

} // namespace Syngine
