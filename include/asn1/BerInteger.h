/**
 * @file BerInteger.h
 * @brief Class to hold an Integer
 */

#ifndef BERINTEGER_H_
#define BERINTEGER_H_

// Includes C/C++
#include <memory>

// Own includes
#include "asn1/BerField.h"

// Defines
#define BER_TAGCLASS_INTEGER 	BER_TAG_UNIVERSAL
#define BER_TAG_INTEGER			2
#define INTEGER_MAX(bits)		((1 << ((bits) - 1)) - 1)	// 2^(n-1) - 1
#define INTEGER_MIN(bits)		-(1 << ((bits) - 1))		// -2^(n-1)

namespace NetMan {

/**
 * @class BerInteger
 */
class BerInteger: public BerField {
	private:
		std::unique_ptr<u8> value;
		bool sign;
	public:
		BerInteger(void *value, u8 len, bool sign, u8 tagOptions = BER_TAGCLASS_INTEGER, u32 tag = BER_TAG_INTEGER);
		void parseData(u8 **out);
		static std::shared_ptr<BerInteger> decode(u8 **data, bool sign);
		static void decodeIntegerValue(u8 **data, u8 len, bool sign, u8 *dest, u8 maxlen);
		void print() override;
		u32 getValueU32();
		s32 getValueS32();
		u64 getValueU64();
		s64 getValueS64();
};

}

#endif
