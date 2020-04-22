/**
 * @file MenuTopController.cpp
 */

// Includes C/C++
#include <stdio.h>
#include <arpa/inet.h>

// Includes 3DS
#include <3ds/types.h>

// Own includes
#include "controller/MenuTopController.h"
#include "Application.h"
#include "gui/TextView.h"
#include "gui/UpdateView.h"
#include "Config.h"
#include "Utils.h"

// Defines
#define MENUTEXT_X          160

namespace NetMan {

static const char *trapFile = "snmpTrap.json";
static const char *logFile = "syslog.json";

static void saveLogEntry(const std::string &path, std::shared_ptr<json_t> json, const std::string &name, u32 limit) {
    
    json_object_set_new(json.get(), "name", json_string(name.c_str()));

    FILE *f = fopen(path.c_str(), "rb");
    if(f == NULL) {
        f = fopen(path.c_str(), "wb");
        if(f) {
            json_t *root = json_array();
            json_array_append(root, json.get());
            fprintf(f, json_dumps(root, 0));
            fclose(f);
            json_decref(root);
        }
    } else {

        json_t *root = json_loadf(f, 0, NULL);
        fclose(f);
        if(root) {
            u32 size = json_array_size(root);
            if(size >= limit) {
                for(u32 i = 0; i < (size - limit + 1); i++) {
                    json_array_remove(root, i);
                }
            }
            json_array_append(root, json.get());

            f = fopen(path.c_str(), "wb");
            if(f) {
                fprintf(f, json_dumps(root, 0));
                fclose(f);
            }
            
            json_decref(root);
        }
    }
}

static void onUpdateLogs(void *args) {

    auto trapv1Sock = Application::getInstance().getTrapv1Sock();
    auto trapv2Sock = Application::getInstance().getTrapv2Sock();
    auto trapv3Sock = Application::getInstance().getTrapv3Sock();
    auto syslogUdpSock = Application::getInstance().getSyslogUdpSock();
    auto syslogTcpSock = Application::getInstance().getSyslogTcpSock();
    auto params = (UpdateParams*)args;
    auto controller = std::static_pointer_cast<MenuTopController>(params->controller);
    auto& configData = Config::getInstance().getData();

    // Handle SNMPv1 traps
    if(trapv1Sock != nullptr) {
        try {
            auto pdu = controller->getSnmpv1Pdu();
            pdu->clear();
            pdu->recvTrap(trapv1Sock);
            auto curTime = Utils::getCurrentTime();
            saveLogEntry(trapFile, pdu->serializeTrap(), curTime + " Trap V1", configData.trapLimit);
            controller->getTrapText()->setText(curTime + ": SNMPv1 trap received!");
            controller->beep();
        } catch (const std::runtime_error &e) { }
    }

    // Handle SNMPv2 traps
    if(trapv2Sock != nullptr) {
        try {
            auto pdu = controller->getSnmpv2Pdu();
            pdu->clear();
            pdu->recvTrap(trapv2Sock);
            auto curTime = Utils::getCurrentTime();
            saveLogEntry(trapFile, pdu->serializeTrap(), curTime + " Trap V2", configData.trapLimit);
            controller->getTrapText()->setText(curTime + ": SNMPv2 trap received!");
            controller->beep();
        } catch (const std::runtime_error &e) { }
    }

    // Handle SNMPv3 traps
    if(trapv3Sock != nullptr) {
        try {
            auto pdu = controller->getSnmpv3Pdu();
            pdu->clear();
            auto curTime = Utils::getCurrentTime();
            if(pdu->recvTrap(trapv3Sock)) {
                controller->getTrapText()->setText(curTime + ": SNMPv3 inform received!");
            } else {
                controller->getTrapText()->setText(curTime + ": SNMPv3 trap received!");
            }
            saveLogEntry(trapFile, pdu->serializeTrap(), curTime + " Trap V3", configData.trapLimit);
            controller->beep();
        } catch (const std::runtime_error &e) { }
    }

    // Handle syslogs
    auto syslogPdu = controller->getSyslogPdu();
    if(syslogUdpSock != nullptr) {
        try {
            syslogPdu->recvLog(syslogUdpSock);
            auto curTime = Utils::getCurrentTime();
            saveLogEntry(logFile, syslogPdu->serialize(), curTime + " Syslog UDP", configData.syslogLimit);
            controller->getTrapText()->setText(curTime + ": UDP Syslog received!");
            controller->beep();
        } catch (const std::runtime_error &e) { }
    } else if(syslogTcpSock != nullptr) {
        try {
            std::shared_ptr<TcpSocket> conn = syslogTcpSock->acceptConnection(configData.tcpTimeout);
			if(conn != nullptr) {
				syslogPdu->recvLog(conn);
                auto curTime = Utils::getCurrentTime();
                saveLogEntry(logFile, syslogPdu->serialize(), curTime + " Syslog TCP", configData.syslogLimit);
                controller->getTrapText()->setText(curTime + ": TCP Syslog received!");
                controller->beep();
			}
        } catch (const std::runtime_error &e) { }
    }
}

MenuTopController::MenuTopController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"onUpdateLogs", onUpdateLogs},
    };

    // Load beep sound (used for trap and log receiving)
    try {
        beepAudio = std::unique_ptr<WaveAudio>(new WaveAudio("beep"));
    } catch (const std::bad_alloc &e) {
        throw;
    } catch (const std::runtime_error &e) {
        throw;
    }

    // Create the trap and log PDUs
    auto& config = Config::getInstance();
    snmpv1Pdu = std::make_shared<Snmpv1Pdu>(config.getCommunity());
    snmpv2Pdu = std::make_shared<Snmpv2Pdu>(config.getCommunity());
    snmpv3Pdu = std::make_shared<Snmpv3Pdu>(config.getEngineID(), config.getContextName(), config.getTrapUser());
    syslogPdu = std::make_shared<SyslogPdu>();
}

MenuTopController::~MenuTopController() {
    ndspChnWaveBufClear(BEEP_AUDIO_CHANNEL);
}

void MenuTopController::initialize(std::vector<std::shared_ptr<GuiView>> &views) {

    in_addr ip, mask, broad;
    u32 tcpSockets, udpSockets;
    socklen_t bufsize = sizeof(u32);
    u32 mac[6];

    SOCU_GetIPInfo(&ip, &mask, &broad);
    SOCU_GetNetworkOpt(SOL_CONFIG, NETOPT_TCP_NUMBER, &tcpSockets, &bufsize);
    SOCU_GetNetworkOpt(SOL_CONFIG, NETOPT_UDP_NUMBER, &udpSockets, &bufsize);
    bufsize = sizeof(mac);
    SOCU_GetNetworkOpt(SOL_CONFIG, NETOPT_MAC_ADDRESS, mac, &bufsize);

    char macAddress[32];
    sprintf(macAddress, "%02lX:%02lX:%02lX:%02lX:%02lX:%02lX", mac[0]&0xFF, mac[1]&0xFF, mac[2]&0xFF, mac[3]&0xFF, mac[4]&0xFF, mac[5]&0xFF);

    std::shared_ptr<TextView> text = std::make_shared<TextView>(std::string(inet_ntoa(ip)), std::static_pointer_cast<TextView>(views[2]));
    text->setX(MENUTEXT_X);
    views.push_back(text);

    text = std::make_shared<TextView>(std::string(inet_ntoa(mask)), std::static_pointer_cast<TextView>(views[3]));
    text->setX(MENUTEXT_X);
    views.push_back(text);

    text = std::make_shared<TextView>(std::string(macAddress), std::static_pointer_cast<TextView>(views[4]));
    text->setX(MENUTEXT_X);
    views.push_back(text);

    text = std::make_shared<TextView>(std::to_string(tcpSockets), std::static_pointer_cast<TextView>(views[5]));
    text->setX(MENUTEXT_X);
    views.push_back(text);

    text = std::make_shared<TextView>(std::to_string(udpSockets), std::static_pointer_cast<TextView>(views[6]));
    text->setX(MENUTEXT_X);
    views.push_back(text);

    trapText = std::make_shared<TextView>("Nothing happened... yet", 40.0f, 193.0f, 0.6f, C2D_Color32(240, 240, 240, 0xFF));
    views.push_back(trapText);
}

}
