/**
 * @file MenuController.cpp
 */

// Includes C/C++
#include <arpa/inet.h>

// Includes 3DS
#include <3ds/types.h>

// Own includes
#include "controller/MenuController.h"
#include "Config.h"
#include "Application.h"
#include "syslog/SyslogPdu.h"

// Defines
#define MENUTEXT_X          140
#define BEEP_AUDIO_CHANNEL  8

namespace NetMan {

static void goSNMP(void *args) {

}

static void goRestConf(void *args) {

}

static void goLogs(void *args) {
    Application::getInstance().requestLayoutChange("logs");
}

static void goSSH(void *args) {
    Application::getInstance().requestLayoutChange("ssh");
}

static void goCredits(void *args) {
    Application::getInstance().requestLayoutChange("credits");
}

static void goOptions(void *args) {
    Application::getInstance().requestLayoutChange("options");
}

MenuController::MenuController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goSSH", goSSH},
        {"goCredits", goCredits},
        {"goOptions", goOptions},
        {"goSNMP", goSNMP},
        {"goRestConf", goRestConf},
        {"goLogs", goLogs},
    };

    try {
        // Load beep sound (used for trap and log receiving)
        beepAudio = std::unique_ptr<WaveAudio>(new WaveAudio("beep"));
    } catch (const std::bad_alloc &e) {
        throw;
    } catch (const std::runtime_error &e) {
        throw;
    }
}

MenuController::~MenuController() {
    ndspChnWaveBufClear(BEEP_AUDIO_CHANNEL);
}

}
