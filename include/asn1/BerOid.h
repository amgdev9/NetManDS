/**
 * @file BerOid.h
 * @brief Class to hold an OBJECT IDENTIFIER
 */

#ifndef BEROID_H_
#define BEROID_H_

// Includes C/C++
#include <string>
#include <memory>
#include <vector>

// Own includes
#include "asn1/BerField.h"

// Defines
#define BER_TAGCLASS_OID 	BER_TAG_UNIVERSAL
#define BER_TAG_OID			6

namespace NetMan {

/**
 * @class BerOid
 */
class BerOid: public BerField {
	private:
		std::vector<u32> oid;
		void parseOid(std::vector<u32> &oid);
	public:
		BerOid(u8 tagOptions = BER_TAGCLASS_OID, u32 tag = BER_TAG_OID);
		BerOid(const std::string &oidString, u8 tagOptions = BER_TAGCLASS_OID, u32 tag = BER_TAG_OID);
        BerOid(const std::vector<u32> &oid, u8 tagOptions = BER_TAGCLASS_OID, u32 tag = BER_TAG_OID);
		void parseData(u8 **out);
		virtual ~BerOid();
		static std::shared_ptr<BerOid> decode(u8** data);
		std::string print() override;
};

}

#endif
