/**
 * @file BerInteger.h
 * @brief 
 */

#ifndef BERINTEGER_H_
#define BERINTEGER_H_

#include "asn1/BerField.h"

// Defines
#define BER_TAGCLASS_INTEGER 	BER_TAG_UNIVERSAL
#define BER_TAG_INTEGER			2
#define INTEGER_MAX(bits)		((1 << ((bits) - 1)) - 1)	// 2^(n-1) - 1
#define INTEGER_MIN(bits)		-(1 << ((bits) - 1))		// -2^(n-1)

namespace NetMan {

class BerInteger: public BerField {
	private:
		s32 value;
	public:
		BerInteger(s32 value, u8 tagOptions = BER_TAGCLASS_INTEGER, u32 tag = BER_TAG_INTEGER);
		void parseData(u8 **out);
		static BerInteger *decode(u8 **data) override;
};

}

#endif
