/**
 * @file BerOctetString.cpp
 * @brief 
 */

#include <string.h>

#include "asn1/BerOctetString.h"

namespace NetMan {

BerOctetString::BerOctetString(const char *value) : BerField(BER_TAGCLASS_OCTETSTRING, BER_TAG_OCTETSTRING){

	u32 strLength = strlen(value);

	this->text = new char[strLength + 1];
	memcpy(this->text, value, strLength);
	this->text[strLength] = '\0';

	this->setLength(strLength);
}

/**
 * @brief Parse octet string data
 * @param out Destination address
 * @note Only primitive BER encoding is supported for this type
 */
void BerOctetString::parseData(u8 **out) {
	memcpy(*out, this->text, this->getLength());
	*out += this->getLength();
}

BerOctetString::~BerOctetString() {
	delete [] text;
}

}
