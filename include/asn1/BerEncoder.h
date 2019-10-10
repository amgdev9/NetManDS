/**
 * @file BerEncoder.h
 * @brief 
 */

#ifndef BERENCODER_H_
#define BERENCODER_H_

#include "BerDocument.h"
#include "UdpSocket.h"

namespace NetMan {

class BerEncoder {
	public:
		BerEncoder();
		void encodeAndSend(BerDocument *doc, UdpSocket *sock, const char *ip, u16 port);
		virtual ~BerEncoder();
};

}

#endif
