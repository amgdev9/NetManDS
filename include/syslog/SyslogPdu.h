/**
 * @file SyslogPdu.h
 * @brief Syslog PDU processing
 */
#ifndef _SYSLOG_PDU_H_
#define _SYSLOG_PDU_H_

// Includes C/C++
#include <memory>
#include <vector>

// Includes jansson
#include <jansson.h>

// Own includes
#include "socket/UdpSocket.h"
#include "socket/TcpSocket.h"

// Defines
//#define SYSLOG_DEBUG                true
#define SYSLOG_MAX_PDU_SIZE         (4 << 10)
#define SYSLOG_MAX_DIGITS           6
#define SYSLOG_MAX_STRING_LENGTH    0xFFFF
#define SYSLOG_TRANSPORT_TCP        0
#define SYSLOG_TRANSPORT_UDP        1

// Defines syslog tokens
#define SYSLOG_NILVALUE     '-'
#define SYSLOG_LT           '<'
#define SYSLOG_GT           '>'
#define SYSLOG_QUOT         '\"'
#define SYSLOG_SP           ' '
#define SYSLOG_DASH         '-'
#define SYSLOG_T            'T'
#define SYSLOG_COLON        ':'
#define SYSLOG_DOT          '.'
#define SYSLOG_Z            'Z'
#define SYSLOG_PLUS         '+'
#define SYSLOG_MINUS        '-'
#define SYSLOG_OPENBRACKET  '['
#define SYSLOG_CLOSEBRACKET ']'

// Defines syslog priorities
#define SYSLOG_PRI_KERNEL       0
#define SYSLOG_PRI_USER         1
#define SYSLOG_PRI_MAIL         2
#define SYSLOG_PRI_DAEMON       3
#define SYSLOG_PRI_SECURITY     4
#define SYSLOG_PRI_SYSLOGD      5
#define SYSLOG_PRI_LINEPRINTER  6
#define SYSLOG_PRI_NETWORKNEWS  7
#define SYSLOG_PRI_UUCP         8
#define SYSLOG_PRI_CLOCK        9
#define SYSLOG_PRI_SECURITY2    10
#define SYSLOG_PRI_FTPDAEMON    11
#define SYSLOG_PRI_NTP          12
#define SYSLOG_PRI_LOGAUDIT     13
#define SYSLOG_PRI_LOGALERT     14
#define SYSLOG_PRI_CLOCKDAEMON2 15
#define SYSLOG_PRI_LOCAL0       16
#define SYSLOG_PRI_LOCAL1       17
#define SYSLOG_PRI_LOCAL2       18
#define SYSLOG_PRI_LOCAL3       19
#define SYSLOG_PRI_LOCAL4       20
#define SYSLOG_PRI_LOCAL5       21
#define SYSLOG_PRI_LOCAL6       22
#define SYSLOG_PRI_LOCAL7       23

namespace NetMan {

/**
 * @struct SyslogTimeStamp
 */
typedef struct {
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
    u32 fracsecond;
    s8 hour_offset;
    u8 minute_offset;
} SyslogTimeStamp;

/**
 * @struct SyslogElementParam
 */
typedef struct {
    std::string name;
    std::string value;
} SyslogElementParam;

/**
 * @struct SyslogElement
 */
typedef struct {
    std::string name;
    std::vector<SyslogElementParam> params;
} SyslogElement;

/**
 * @class SyslogPdu
 */
class SyslogPdu {
    private:
        u8 priority;
        u8 version;
        u8 subversion;
        SyslogTimeStamp timeStamp;
        std::string hostname;
        std::string appname;
        std::string procid;
        std::string msgid;
        std::vector<SyslogElement> elements;
        std::string message;

        bool checkAlternative(u8 **ptr, char c);
        void checkCharacter(u8 **ptr, char c);
        u32 getNumber(u8 **ptr, u8 mindigits, u8 maxdigits, u32 minrange, u32 maxrange, bool nonzero);
        std::string getString(u8 **ptr, u16 minlength, u16 maxlength, const char *separator, bool escape = false, u8 mincode = 33, u8 maxcode = 126);
        void decodeLog(std::shared_ptr<u8> data, u32 dataSize);
    public:
        SyslogPdu();
        void recvLog(std::shared_ptr<UdpSocket> sock);
        void recvLog(std::shared_ptr<TcpSocket> sock);
        void print();
        std::shared_ptr<json_t> serialize();
        virtual ~SyslogPdu();
};

}

#endif
