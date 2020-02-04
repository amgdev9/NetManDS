/**
 * @file BerInteger64.h
 * @brief Class to hold an Integer64
 */

#ifndef BERINTEGER64_H_
#define BERINTEGER64_H_

// Includes C/C++
#include <memory>

// Own includes
#include "asn1/BerField.h"
#include "Snmpv2Pdu.h"

namespace NetMan {

/**
 * @class BerInteger64
 * @note Not a BER type actually, but fits in here
 */
class BerInteger64: public BerField {
	private:
		s64 value;
	public:
		BerInteger64(s64 value, u8 tagOptions = SNMPV2_TAGCLASS_COUNTER64, u32 tag = SNMPV2_TAG_COUNTER64);
		void parseData(u8 **out);
        static s64 decodeIntegerValue(u8 **data, u8 len);
		static std::shared_ptr<BerInteger64> decode(u8 **data);
		void print() override;
		s64 getValue();
};

}

#endif
