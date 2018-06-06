#pragma once

#include <iostream>
#include <string>
#include <sol2/sol.hpp>
#include "Types.h"

/// @class LuaConfigureParser
class LuaConfigureParser {
public:
    /// @brief load config file by filename
    /// @param filename given filename of lua script
    bool LoadConfigFile(std::string filename);

    /// @brief get POD type values
    /// @param str value name
    template <typename T>
    T Get(std::string str) {
        return m_lua.get<T>(str.c_str());
    }

    /// @brief get vetor3
    /// @param str value name
    Vec3f GetVec3f(std::string str);

    /// @brief get string
    /// @param str value name
    std::string GetString(std::string str);

private:
    /// sol2 lua state
    sol::state m_lua;
};
