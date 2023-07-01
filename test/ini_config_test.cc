#include <LuteBase.h>

#define INI_FILE_NAME "conf/system.ini"

std::shared_ptr<Lute::ini::INI> g_iniFilePtr =
    Lute::SingletonPtr<Lute::ini::INI>::GetInstance(INI_FILE_NAME);
std::shared_ptr<Lute::ini::INIStructure> g_iniContentPtr =
    Lute::SingletonPtr<Lute::ini::INIStructure>::GetInstance();

int main() {
    Lute::ini::initIniConfig(INI_FILE_NAME);

    LUTE_INI_WRITE("person", "body", "Lute");
    LUTE_INI_WRITE("money", "RMB", "100");
    LUTE_INI_WRITE("money", "Dollar", "13.87");

    PING(lambda);
    std::cout << LUTE_INI_READ("person", "body") << std::endl;
    std::cout << LUTE_INI_READ("person", "body") << std::endl;
    std::cout << LUTE_INI_READ("money", "RMB") << std::endl;
    std::cout << LUTE_INI_READ("money", "Dollar") << std::endl;
    PONG(lambda);
    
    LUTE_INI_WRITE("person", "body", "Lutianen");

    return 0;
}