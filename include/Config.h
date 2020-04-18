/**
 * @file Config.h
 * @brief Configuration helper
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

// Includes 3DS
#include <3ds/types.h>

// Defines
#define CONFIG_PATH             "config.dat"
#define DEFAULT_SNMP_PORT       161
#define DEFAULT_TRAP_PORT       162
#define DEFAULT_TRAP_LIMIT      30
#define DEFAULT_SYSLOG_LIMIT    30
#define DEFAULT_TIMEOUT         10
#define DEFAULT_SYSLOG_PORT     514

namespace NetMan {

/**
 * @struct ConfigData
 */
typedef struct {
    u16 snmpPort;
    bool trapv1Enabled;
    u16 trapv1Port;
    bool trapv2Enabled;
    u16 trapv2Port;
    bool trapv3Enabled;
    u16 trapv3Port;
    u32 trapLimit;
    u32 rcTimeout;
    u32 tcpTimeout;
    u32 udpTimeout;
    u16 syslogPort;
    bool syslogTransport;
    u32 syslogLimit;
} ConfigData;

/**
 * @class Config
 */
class Config {
    private:
        ConfigData data;
        std::string smiPath;
        std::string rcURL;
        std::string rcUser;
        std::string rcPass;
        std::string engineID;
        std::string contextName;
        void writeString(FILE *f, const std::string &text);
        void readString(FILE *f, std::string &text);
        Config();
        virtual ~Config();
    public:
        static Config &getInstance();
        void save();
        inline ConfigData &getData() { return data; }
        inline std::string &getSmiPath() { return smiPath; }
        inline std::string &getRestConfURL() { return rcURL; }
        inline std::string &getRestConfUser() { return rcUser; }
        inline std::string &getRestConfPassword() { return rcPass; }
        inline std::string &getEngineID() { return engineID; }
        inline std::string &getContextName() { return contextName; }
};

}

#endif
