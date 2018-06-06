#include "LuaConfigParser.h"

bool LuaConfigureParser::LoadConfigFile(std::string filename) {
    auto result = m_lua.script_file(filename.c_str());
    if (!result.valid()) {
        std::cerr << "error open config file !!! " << std::endl;
        return false;
	} else {
		std::cout << "load config file " << filename << " successful!" << std::endl;
	}
    m_lua.open_libraries();
    return true;
}


Vec3f LuaConfigureParser::GetVec3f(std::string str) {
    sol::table t = m_lua.get<sol::table>(str.c_str());
//    float v = table[1];
    return Vec3f(t[1], t[2], t[3]);
}

std::string LuaConfigureParser::GetString(std::string str) {
    return m_lua.get<std::string>(str.c_str());
}
