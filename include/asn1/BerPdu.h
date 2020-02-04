/**
 * @file BerPdu.h
 * @brief BER PDU handler
 */

#ifndef BERPDU_H_
#define BERPDU_H_

// Includes C/C++
#include <memory>
#include <vector>

// Own includes
#include "asn1/BerField.h"
#include "UdpSocket.h"

namespace NetMan {

/**
 * @class BerPdu
 * @brief BER PDU handler
 */
class BerPdu {
	protected:
		std::vector<std::shared_ptr<BerField>> fields;
		void send(std::shared_ptr<UdpSocket> sock, const std::string &ip, u16 port);
	public:
		BerPdu();
		virtual ~BerPdu();
		virtual void clear();
};

}

#endif
