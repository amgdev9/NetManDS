/**
 * @file BerUInteger.h
 * @brief 
 */

#ifndef BERUINTEGER_H_
#define BERUINTEGER_H_

#include "asn1/BerInteger.h"

#define BER_TAGCLASS_UINTEGER 	BER_TAGCLASS_INTEGER
#define BER_TAG_UINTEGER		BER_TAG_INTEGER

namespace NetMan {

class BerUInteger : public BerInteger{
	public:
		BerUInteger(u32 value, u8 tagOptions = BER_TAGCLASS_UINTEGER, u32 tag = BER_TAG_UINTEGER);
};

}

#endif
