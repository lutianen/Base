#include <luxbase/utils.h>

std::string lux::base::toUpper(const std::string& str) {
    std::string rt = str;
    std::transform(rt.begin(), rt.end(), rt.begin(), ::toupper);
    return rt;
}

std::string lux::base::toLower(const std::string& str) {
    std::string rt = str;
    std::transform(rt.begin(), rt.end(), rt.begin(), ::tolower);
    return rt;
}
