#ifndef UUID_H
#define UUID_H

#include <random>
#include <string>
#include <sstream>

typedef std::string UUID;

namespace uuid {
    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);
	
    UUID generate_uuid_v4();
};

#endif
