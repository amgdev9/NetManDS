/**
 * @file Snmpv3UserStore.cpp
 * @brief SNMPv3 User Store
 */

// Includes C/C++
#include <fstream>
#include <sstream>

// Own includes
#include "snmp/Snmpv3UserStore.h"
#include "snmp/Snmpv3AuthMD5.h"
#include "snmp/Snmpv3AuthSHA1.h"
#include "snmp/Snmpv3PrivDES.h"

const static std::unordered_map<std::string, u32> authProtos = {
    {"", SNMPV3_AUTHPROTO_NONE},
    {"MD5", SNMPV3_AUTHPROTO_MD5},
    {"SHA", SNMPV3_AUTHPROTO_SHA1}
};

const static std::unordered_map<std::string, u32> privProtos = {
    {"", SNMPV3_PRIVPROTO_NONE},
    {"DES", SNMPV3_PRIVPROTO_DES},
};

namespace NetMan {

/**
 * @brief Get the authentication protocol ID
 * @param text  Identifier text
 * @return The protocol ID
 */
u32 Snmpv3UserStore::getAuthProtoID(const std::string &text) {
    auto it = authProtos.find(text);
    if(it == authProtos.end()) {
        throw std::runtime_error("Wrong auth protocol");
    }
    return it->second;
}

/**
 * @brief Get the privacy protocol ID
 * @param text  Identifier text
 * @return The protocol ID
 */
u32 Snmpv3UserStore::getPrivProtoID(const std::string &text) {
    auto it = privProtos.find(text);
    if(it == privProtos.end()) {
        throw std::runtime_error("Wrong priv protocol");
    }
    return it->second;
}

/**
 * @brief Constructor for a SNMPv3 User Store
 */
Snmpv3UserStore::Snmpv3UserStore() {
    this->userTable = std::unordered_map<std::string, Snmpv3UserStoreEntry>();
    load();
}

/**
 * @brief Load a SNMPv3 user store from a file
 */
void Snmpv3UserStore::load() {

    // Open file
    std::ifstream infile;

    try {
        infile = std::ifstream(SNMPV3_USERSTORE_PATH, std::ios::in);
    } catch(const std::ifstream::failure &e) {
        try {
            infile = std::ifstream(SNMPV3_USERSTORE_PATH, std::ios::out);
            infile.close();
            return;
        } catch (const std::ifstream::failure &e) {
            throw;
        }
    }

    std::string line;
    Snmpv3UserStoreEntry entry;
    std::string name;

    // Read every line
    while (std::getline(infile, line)) {

        std::istringstream iss(line);
        if (iss >> name >> entry.authProto >> entry.authPass >> entry.privProto >> entry.privPass) {
            this->userTable[name] = entry;
        }
    }

    // Close file
    infile.close();
}

/**
 * @brief Add an user to the user store
 * @param name  User name
 * @param entry User parameters
 */
void Snmpv3UserStore::addUser(const std::string &name, const Snmpv3UserStoreEntry &entry) {

    try {
        if(this->userTable.find(name) != this->userTable.end()) {
            throw std::runtime_error("User " + name + " is repeated");
        }
        this->userTable[name] = entry;
    } catch (const std::runtime_error &e) {
        throw;
    } catch (const std::bad_alloc &e) {
        throw;
    }
}

/**
 * @brief Remove an user from the user store
 * @param name  User name
 */
void Snmpv3UserStore::removeUser(const std::string &name) {
    this->userTable.erase(name);
}

/**
 * @brief Get an user from the user store
 * @param name  User name
 * @return The user entry
 */
Snmpv3UserStoreEntry &Snmpv3UserStore::getUser(const std::string &name) {

    try {
        if(this->userTable.find(name) == this->userTable.end()) {
            throw std::runtime_error("User " + name + " not found");
        }
    } catch(const std::runtime_error &e) {
        throw;
    }

    return this->userTable[name];
}

/**
 * @brief Save the user store to a file
 */
void Snmpv3UserStore::save() {

    // Open the file
    FILE *f = fopen(SNMPV3_USERSTORE_PATH, "wb");
    if(f == NULL) {
        throw std::runtime_error(std::string("Couldn't open ") + SNMPV3_USERSTORE_PATH);
    }

    // Write the user store
    for(std::pair<std::string, Snmpv3UserStoreEntry> user : userTable) {
        Snmpv3UserStoreEntry &entry = user.second;
        fprintf(f, "%s %ld %s %ld %s\n", user.first.c_str(), entry.authProto, entry.authPass.c_str(), entry.privProto, entry.privPass.c_str());
    }

    // Close the file
    fclose(f);
}

/**
 * @brief Get the authentication protocol used by an user
 * @param user  User which uses authentication
 * @return Its authentication protocol, or nullptr if not using it
 */
std::shared_ptr<Snmpv3AuthProto> Snmpv3UserStore::getAuthProto(const Snmpv3UserStoreEntry &user) {

    switch(user.authProto) {
        case SNMPV3_AUTHPROTO_MD5:
            return std::make_shared<Snmpv3AuthMD5>();
            break;
        case SNMPV3_AUTHPROTO_SHA1:
            return std::make_shared<Snmpv3AuthSHA1>();
            break;
    }

    return nullptr;
}

/**
 * @brief Get the privacy protocol used by an user
 * @param user  User which uses privaty
 * @return Its privacy protocol, or nullptr if not using it
 */
std::shared_ptr<Snmpv3PrivProto> Snmpv3UserStore::getPrivProto(const Snmpv3UserStoreEntry &user) {
    
    switch(user.privProto) {
        case SNMPV3_PRIVPROTO_DES:
            return std::make_shared<Snmpv3PrivDES>();
            break;
    }

    return nullptr;
}

/**
 * @brief Destructor for a SNMPv3 user store
 */
Snmpv3UserStore::~Snmpv3UserStore() {
    save();
}

/**
 * @brief Get the instance of the User Store
 * @return The singleton instance of the User Store
 */
Snmpv3UserStore &Snmpv3UserStore::getInstance() {
    static Snmpv3UserStore instance;
    return instance;
}

}
