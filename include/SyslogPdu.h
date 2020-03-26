/**
 * @file SyslogPdu.h
 * @brief Syslog PDU processing
 */
#ifndef _SYSLOG_PDU_H_
#define _SYSLOG_PDU_H_

// Includes C/C++
#include <memory>

// Own includes
#include "UdpSocket.h"

// Defines
#define SYSLOG_MAX_PDU_SIZE     (4 << 10)

namespace NetMan {

/**
 * @class SyslogPdu
 */
class SyslogPdu {
    private:
    public:
        SyslogPdu();
        void recvLog(std::shared_ptr<UdpSocket> sock);
        virtual ~SyslogPdu();
};

}

#endif
