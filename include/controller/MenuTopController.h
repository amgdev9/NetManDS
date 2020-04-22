/**
 * @file MenuTopController.h
 */

#ifndef _MENU_TOPCONTROLLER_H_
#define _MENU_TOPCONTROLLER_H_

// Own includes
#include "GuiController.h"
#include "gui/TextView.h"
#include "WaveAudio.h"
#include "snmp/Snmpv1Pdu.h"
#include "snmp/Snmpv2Pdu.h"
#include "snmp/Snmpv3Pdu.h"
#include "syslog/SyslogPdu.h"

// Defines
#define BEEP_AUDIO_CHANNEL  8

namespace NetMan {

/**
 * @class MenuTopController
 */
class MenuTopController : public GuiController {
    private:
        std::unique_ptr<WaveAudio> beepAudio;
        std::shared_ptr<TextView> trapText;
        std::shared_ptr<SyslogPdu> syslogPdu;
        std::shared_ptr<Snmpv1Pdu> snmpv1Pdu;
        std::shared_ptr<Snmpv2Pdu> snmpv2Pdu;
        std::shared_ptr<Snmpv3Pdu> snmpv3Pdu;
    public:
        MenuTopController();
        virtual ~MenuTopController();
        void initialize(std::vector<std::shared_ptr<GuiView>> &views) override;
        inline std::shared_ptr<Snmpv1Pdu> getSnmpv1Pdu() { return snmpv1Pdu; }
        inline std::shared_ptr<Snmpv2Pdu> getSnmpv2Pdu() { return snmpv2Pdu; }
        inline std::shared_ptr<Snmpv3Pdu> getSnmpv3Pdu() { return snmpv3Pdu; }
        inline std::shared_ptr<SyslogPdu> getSyslogPdu() { return syslogPdu; }
        inline void beep() { beepAudio->play(BEEP_AUDIO_CHANNEL); }
        inline std::shared_ptr<TextView> getTrapText() { return trapText; }
};

}

#endif
