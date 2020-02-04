/**
 * @file BerOctetString.h
 * @brief Class to hold an OCTET STRING
 */

#ifndef BEROCTETSTRING_H_
#define BEROCTETSTRING_H_

// Includes C/C++
#include <string>
#include <memory>

// Own includes
#include "asn1/BerField.h"

// Defines
#define BER_TAGCLASS_OCTETSTRING 	BER_TAG_UNIVERSAL
#define BER_TAG_OCTETSTRING			4

namespace NetMan {

/**
 * @class BerOctetString
 */
class BerOctetString: public BerField {
	private:
		std::string text;
	public:
		BerOctetString(const std::string &value, u8 tagOptions = BER_TAGCLASS_OCTETSTRING, u32 tag = BER_TAG_OCTETSTRING);
		void parseData(u8 **out);
		static std::shared_ptr<BerOctetString> decode(u8 **data);
		virtual ~BerOctetString();
		void print() override;
		std::string &getValue();
};

}

#endif
