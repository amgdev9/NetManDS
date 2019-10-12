/**
 * @file BerOid.h
 * @brief 
 */

#ifndef BEROID_H_
#define BEROID_H_

#include <vector>

#include "asn1/BerField.h"

// Defines
#define BER_TAGCLASS_OID 	BER_TAG_UNIVERSAL
#define BER_TAG_OID			6

namespace NetMan {

class BerOid: public BerField {
	private:
		std::vector<u32> oid;
	public:
		BerOid(const char *oidString, u8 tagOptions = BER_TAGCLASS_OID, u32 tag = BER_TAG_OID);
		void parseData(u8 **out);
		virtual ~BerOid();
};

}

#endif
