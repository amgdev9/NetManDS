/**
 * @file Config.cpp
 * @brief Configuration helper
 */

// Includes C/C++
#include <stdio.h>
#include <stdexcept>
#include <memory>

// Own includes
#include "Config.h"

namespace NetMan {

Config::Config() {

    FILE *f = fopen(CONFIG_PATH, "rb");
    if(f == NULL) {
        data = (ConfigData) {
            .snmpPort = DEFAULT_SNMP_PORT,
            .trapv1Enabled = false,
            .trapv1Port = DEFAULT_TRAP_PORT,
            .trapv2Enabled = false,
            .trapv2Port = DEFAULT_TRAP_PORT,
            .trapv3Enabled = false,
            .trapv3Port = DEFAULT_TRAP_PORT,
            .trapLimit = DEFAULT_TRAP_LIMIT,
            .rcTimeout = DEFAULT_TIMEOUT,
            .tcpTimeout = DEFAULT_TIMEOUT,
            .udpTimeout = DEFAULT_TIMEOUT,
            .syslogPort = DEFAULT_SYSLOG_PORT,
            .syslogTransport = false,
            .syslogLimit = DEFAULT_SYSLOG_LIMIT,
        };
        smiPath = "";
        rcURL = "";
        rcUser = "";
        rcPass = "";
        engineID = "";
        contextName = "";
        try {
            save();
        } catch (const std::runtime_error &e) {
            throw;
        }
    } else {
        fread(&data, sizeof(data), 1, f);
        try {
            readString(f, smiPath);
            readString(f, rcURL);
            readString(f, rcUser);
            readString(f, rcPass);
            readString(f, engineID);
            readString(f, contextName);
        } catch (const std::bad_alloc &e) {
            throw;
        }
        fclose(f);
    }
}

void Config::writeString(FILE *f, const std::string &text) {
    size_t len = text.length();
    fwrite(&len, sizeof(len), 1, f);
    fwrite(text.c_str(), len, 1, f);
}

void Config::readString(FILE *f, std::string &text) {
    size_t len;
    fread(&len, sizeof(len), 1, f);
    std::unique_ptr<char> ptr = nullptr;
    try {
        ptr = std::unique_ptr<char>(new char[len+1]);
    } catch (const std::bad_alloc &e) {
        throw;
    }
    ptr.get()[len] = '\0';
    fread(ptr.get(), len, 1, f);
    text.clear();
    text += ptr.get();
}

Config::~Config() {
    save();
}

void Config::save() {
    FILE *f = fopen(CONFIG_PATH, "wb");
    if(f == NULL) {
        throw std::runtime_error("Couldn't create config file");
    }
    fwrite(&data, sizeof(data), 1, f);
    writeString(f, smiPath);
    writeString(f, rcURL);
    writeString(f, rcUser);
    writeString(f, rcPass);
    writeString(f, engineID);
    writeString(f, contextName);
    fclose(f);
}

Config &Config::getInstance() {
    static Config config;
    return config;
}

}
