/**
 * @file Utils.cpp
 * @brief Utility functions
 */

// Includes C/C++
#include <time.h>
#include <dirent.h>

// Own includes
#include "Utils.h"
#include "Application.h"
#include "snmp/Snmpv2Pdu.h"
#include "snmp/Snmpv3Pdu.h"
#include "asn1/BerInteger.h"
#include "Config.h"

namespace NetMan {

/**
 * @brief Add a field to a JSON array
 * @param array The array to use
 * @param field The string to add
 */
void Utils::addJsonField(json_t *array, const std::string &field) {
    json_array_append_new(array, json_string(field.c_str()));
}

std::shared_ptr<json_t> Utils::loadJsonList(const std::string &path) {
    auto root = std::shared_ptr<json_t>(json_load_file(path.c_str(), 0, NULL), [=](json_t* data) { json_decref(data); });
    if(root.get() == NULL) {
        Application::getInstance().messageBox("Can't load JSON list: " + path);
    }
    return root;
}

/**
 * @brief Get current time
 * @return A string with the current time
 */
std::string Utils::getCurrentTime() {

    time_t unixTime = time(NULL);
	struct tm* timeStruct = gmtime((const time_t *)&unixTime);

	int hours = timeStruct->tm_hour;
	int minutes = timeStruct->tm_min;
	int seconds = timeStruct->tm_sec;

    char t[64];
    sprintf(t, "[%02i:%02i:%02i]", hours, minutes, seconds);
    return std::string(t);
}

/**
 * @brief Get a port from a formulary
 * @param params    EditText params
 * @param port      Initial port
 */
u16 Utils::handleFormPort(EditTextParams *params, u16 port) {
    u16 ret = 0;
    if(!params->init) {
        sprintf(params->text, "%d", port);
        params->init = true;
    } else {
        int port = strtol(params->text, NULL, 10);
        if(port == 0 || port > 0xFFFF) {
            params->init = false;
            Application::getInstance().messageBox("Invalid port");
        } else {
            ret = port;
        }
    }

    return ret;
}

/**
 * @brief Get an integer from a formulary
 * @param params    EditText params
 * @param dest      Destination field
 * @param limit     Integer max range
 */
void Utils::handleFormInteger(EditTextParams *params, u32 *dest, u32 limit) {
    if(!params->init) {
        sprintf(params->text, "%ld", *dest);
        params->init = true;
    } else {
        int n = strtol(params->text, NULL, 10);
        if(n == 0 || (u32)n > limit) {
            params->init = false;
            Application::getInstance().messageBox("Invalid number");
        } else {
            *dest = (u32)n;
        }
    }
}

/**
 * @brief Read a folder
 * @param path  Folder path
 * @param ext   Files extension
 * @param out   Out vector with folder entries
 */
void Utils::readFolder(const std::string &path, const std::string &ext, std::vector<std::string> &out) {

    DIR *dp = opendir(path.c_str());
    if(dp == NULL) {
        throw std::runtime_error("Couldn't open " + path);
    }

    struct dirent *entry;
    struct stat statbuf;
    while((entry = readdir(dp)) != NULL) {

        lstat(entry->d_name, &statbuf);

        if(!S_ISDIR(statbuf.st_mode)) {
            if(ext.length() > 0) {
                int dnameLen = strlen(entry->d_name);
                if(strcmp(ext.c_str(), &entry->d_name[dnameLen - ext.length()]) == 0) {
                    out.push_back(entry->d_name);
                }
            } else {
                out.push_back(entry->d_name);
            }
        }
    }

    closedir(dp);
}

/**
 * @brief Check if a string ends in another string
 * @param mainStr   Main string
 * @param toMatch   String to match at the end
 * @return If the first string ends in the second string
 */
bool Utils::endsWith(const std::string &mainStr, const std::string &toMatch) {
	return (mainStr.size() >= toMatch.size() && mainStr.compare(mainStr.size() - toMatch.size(), toMatch.size(), toMatch) == 0);
}

/**
 * @brief Prepare PDU fields for a SET request
 * @param i PDU field to be prepared
 * @return The prepared field
 */
static std::shared_ptr<BerField> prepareSetField(u32 i) {

    std::shared_ptr<BerField> currentField = nullptr;
    auto &pduFields = Application::getInstance().getPduFields();

    switch(pduFields[i].type) {
        case 'i':       // Integer
        {
            s32 val = strtol(pduFields[i].value.c_str(), NULL, 10);
            currentField = std::make_shared<BerInteger>(&val, sizeof(s32), true);
        } break;
        case 's':       // Octet string
            currentField = std::make_shared<BerOctetString>(pduFields[i].value);
            break;
        case 'n':       // Null
            currentField = std::make_shared<BerNull>();
            break;
        case 'O':       // Object Identifier
            currentField = std::make_shared<BerOid>(pduFields[i].value);
            break;
        case 'a':       // Network Address
        {
            char ipOctetString[5];
            in_addr_t ip = inet_addr(pduFields[i].value.c_str());
            if(ip == INADDR_NONE) {
                throw std::runtime_error("Field " + pduFields[i].oidText + " is not a valid IP address");
            }
            memcpy(ipOctetString, &ip, 4);
            ipOctetString[4] = '\0';
            currentField = std::make_shared<BerOctetString>(ipOctetString, SNMPV1_TAGCLASS_NETWORKADDRESS, SNMPV1_TAG_NETWORKADDRESS);
        } break;
        case 'c':       // Counter
        {
            u32 val = strtoul(pduFields[i].value.c_str(), NULL, 10);
            currentField = std::make_shared<BerInteger>(&val, sizeof(u32), false, SNMPV1_TAGCLASS_COUNTER, SNMPV1_TAG_COUNTER);
        } break;
        case 'C':       // Counter64
        {
            u64 val = strtoul(pduFields[i].value.c_str(), NULL, 10);
            currentField = std::make_shared<BerInteger>(&val, sizeof(u64), false, SNMPV2_TAGCLASS_COUNTER64, SNMPV2_TAG_COUNTER64);
        } break;
        case 'g':       // Gauge
        {
            u32 val = strtoul(pduFields[i].value.c_str(), NULL, 10);
            currentField = std::make_shared<BerInteger>(&val, sizeof(u32), false, SNMPV1_TAGCLASS_GAUGE, SNMPV1_TAG_GAUGE);
        } break;
        case 't':       // TimeTicks
        {
            u32 val = strtoul(pduFields[i].value.c_str(), NULL, 10);
            currentField = std::make_shared<BerInteger>(&val, sizeof(u32), false, SNMPV1_TAGCLASS_TIMETICKS, SNMPV1_TAG_TIMETICKS);
        } break;
        case 'o':       // Opaque
            currentField = std::make_shared<BerOctetString>(pduFields[i].value, SNMPV1_TAGCLASS_OPAQUE, SNMPV1_TAG_OPAQUE);
            break;
        default:
            throw std::runtime_error("Field " + pduFields[i].oidText + " has no valid ASN.1 syntax");
    }

    return currentField;
}

/**
 * @brief Send a SNMP PDU to some destination
 * @param params    Request parameters
 */
void Utils::sendSnmpPdu(SnmpThreadParams *params) {

    auto& configStore = Config::getInstance();
    auto& config = configStore.getData();
    auto session = params->session;
    auto& pduFields = Application::getInstance().getPduFields();

    std::shared_ptr<UdpSocket> sock = std::make_shared<UdpSocket>(config.udpTimeout);
    
    if(params->usmEnabled) {
        std::shared_ptr<Snmpv3Pdu> pdu = std::make_shared<Snmpv3Pdu>(configStore.getEngineID(), configStore.getContextName(), params->username);
        std::shared_ptr<BerNull> nullval = std::make_shared<BerNull>();
        std::shared_ptr<BerOid> testOid = std::make_shared<BerOid>("1.3.6.1.2.1.1.7.0");
        pdu->addVarBind(testOid, nullval);
        pdu->sendRequest(SNMPV2_GETREQUEST, sock, session->agentIP, config.snmpPort);
        try {
            pdu->recvResponse(sock, session->agentIP, config.snmpPort);
        } catch (const std::runtime_error &e) { }
        pdu->clear();

        if(session->pduType == SNMPV1_SETREQUEST) {
            for(u32 i = 0; i < pduFields.size(); i++) {
                pdu->addVarBind(pduFields[i].oid, prepareSetField(i));
            }
        } else {
            for(u32 i = 0; i < pduFields.size(); i++) {
                pdu->addVarBind(pduFields[i].oid, nullval);
            }
        }

        if(session->pduType == SNMPV2_GETBULKREQUEST) {
            pdu->sendBulkRequest(session->nonRepeaters, session->maxRepetitions, sock, session->agentIP, config.snmpPort);
        } else {
            pdu->sendRequest(session->pduType, sock, session->agentIP, config.snmpPort);
        }
        for(u32 i = 0; i < 60; i++) gspWaitForVBlank();
        pdu->recvResponse(sock, session->agentIP, config.snmpPort);

        if(session->pduType == SNMPV2_GETBULKREQUEST) {
            for(u32 i = 0; i < pdu->getNVarBinds(); i++) {
                if(i < session->nonRepeaters) {
                    pduFields[i].value = pdu->getVarBind(i)->print();
                } else if(i == session->nonRepeaters) {
                    pduFields[i].value = std::to_string(pdu->getNVarBinds() - session->nonRepeaters) + " fields";
                } else {
                    PduField field;
                    field.oidText = pdu->getVarBindOid(session->nonRepeaters)->print();
                    field.value = pdu->getVarBind(i)->print();
                    pduFields.push_back(field);
                }
            }
        } else {
            for(u32 i = 0; i < pduFields.size(); i++) {
                pduFields[i].value = pdu->getVarBind(i)->print();
            }
        }
    } else {
        if(session->pduType == SNMPV2_GETBULKREQUEST) {
            std::unique_ptr<Snmpv2Pdu> pdu = std::unique_ptr<Snmpv2Pdu>(new Snmpv2Pdu(params->community));
            std::shared_ptr<BerNull> nullval = std::make_shared<BerNull>();

            for(u32 i = 0; i < pduFields.size(); i++) {
                pdu->addVarBind(pduFields[i].oid, nullval);
            }

            pdu->sendBulkRequest(session->nonRepeaters, session->maxRepetitions, sock, session->agentIP, config.snmpPort);
            pdu->recvResponse(sock, session->agentIP, config.snmpPort);

            for(u32 i = 0; i < pdu->getNVarBinds(); i++) {
                if(i < session->nonRepeaters) {
                    pduFields[i].value = pdu->getVarBind(i)->print();
                } else if(i == session->nonRepeaters) {
                    pduFields[i].value = std::to_string(pdu->getNVarBinds() - session->nonRepeaters) + " fields";
                } else {
                    PduField field;
                    field.oidText = pdu->getVarBindOid(session->nonRepeaters)->print();
                    field.value = pdu->getVarBind(i)->print();
                    pduFields.push_back(field);
                }
            }
        } else {
            std::unique_ptr<Snmpv1Pdu> pdu = std::unique_ptr<Snmpv1Pdu>(new Snmpv1Pdu(params->community));
            if(session->pduType == SNMPV1_SETREQUEST) {
                for(u32 i = 0; i < pduFields.size(); i++) {
                    pdu->addVarBind(pduFields[i].oid, prepareSetField(i));
                }
            } else {
                std::shared_ptr<BerNull> nullval = std::make_shared<BerNull>();
                for(u32 i = 0; i < pduFields.size(); i++) {
                    pdu->addVarBind(pduFields[i].oid, nullval);
                }
            }

            pdu->sendRequest(session->pduType, sock, session->agentIP, config.snmpPort);
            pdu->recvResponse(sock, session->agentIP, config.snmpPort);

            for(u32 i = 0; i < pduFields.size(); i++) {
                pduFields[i].value = pdu->getVarBind(i)->print();
            }
        }
    }
}

}
