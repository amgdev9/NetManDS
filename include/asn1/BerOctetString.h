/**
 * @file BerOctetString.h
 * @brief 
 */

#ifndef BEROCTETSTRING_H_
#define BEROCTETSTRING_H_

#include "asn1/BerField.h"

// Defines
#define BER_TAGCLASS_OCTETSTRING 	BER_TAG_UNIVERSAL
#define BER_TAG_OCTETSTRING			4

namespace NetMan {

class BerOctetString: public BerField {
	private:
		char *text;
	public:
		BerOctetString(const char *value, u8 tagOptions = BER_TAGCLASS_OCTETSTRING, u32 tag = BER_TAG_OCTETSTRING);
		void parseData(u8 **out);
		virtual ~BerOctetString();
};

}

#endif
