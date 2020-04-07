/**
 * @file Snmpv3UserStore.h
 * @brief SNMPv3 User Store
 */

#ifndef _SNMPV3_USER_STORE_H_
#define _SNMPV3_USER_STORE_H_

// Includes C/C++
#include <string>
#include <unordered_map>

// Includes 3DS
#include <3ds.h>

// Own includes
#include "Snmpv3AuthProto.h"
#include "Snmpv3PrivProto.h"

// Defines
#define SNMPV3_USERSTORE_PATH   "userstore.dat"

// Defines authentication protocols
#define SNMPV3_AUTHPROTO_NONE   0
#define SNMPV3_AUTHPROTO_MD5    1
#define SNMPV3_AUTHPROTO_SHA1   2

// Defines privacy protocols
#define SNMPV3_PRIVPROTO_NONE   0
#define SNMPV3_PRIVPROTO_DES    1

namespace NetMan {

typedef struct {
    std::string authPass;
    std::string privPass;
    u32 authProto;
    u32 privProto;
}Snmpv3UserStoreEntry;

/**
 * @class Snmpv3UserStore
 */
class Snmpv3UserStore {
    private:
        std::unordered_map<std::string, Snmpv3UserStoreEntry> userTable;
        Snmpv3UserStore();
        virtual ~Snmpv3UserStore();
    public:
        void load();
        void save();
        void addUser(const std::string &name, const Snmpv3UserStoreEntry &entry);
        void removeUser(const std::string &name);
        Snmpv3UserStoreEntry &getUser(const std::string &name);
        std::shared_ptr<Snmpv3AuthProto> getAuthProto(const Snmpv3UserStoreEntry &user);
        std::shared_ptr<Snmpv3PrivProto> getPrivProto(const Snmpv3UserStoreEntry &user);
        static Snmpv3UserStore &getInstance();
};

}

#endif
