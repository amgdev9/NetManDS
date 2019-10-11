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

namespace NetMan {

class BerInteger: public BerField {
	private:
		s32 value;
	public:
		BerInteger(s32 value);
		void parseData(u8 **out);
		virtual ~BerInteger();
};

}

#endif
