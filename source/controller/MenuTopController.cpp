/**
 * @file MenuTopController.cpp
 */

// Includes C/C++
#include <arpa/inet.h>

// Includes 3DS
#include <3ds/types.h>

// Own includes
#include "controller/MenuTopController.h"
#include "Application.h"
#include "gui/TextView.h"

// Defines
#define MENUTEXT_X      160

namespace NetMan {

MenuTopController::MenuTopController() { }

MenuTopController::~MenuTopController() { }

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
}

void MenuTopController::callMethod(const std::string &method, void *args) { }

}
