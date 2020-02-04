/**
 * @file BerNull.h
 * @brief Class to hold a NULL value
 */

#ifndef BERNULL_H_
#define BERNULL_H_

// Includes C/C++
#include <memory>

// Own includes
#include "asn1/BerField.h"

// Defines
#define BER_TAGCLASS_NULL 	BER_TAG_UNIVERSAL
#define BER_TAG_NULL		5

namespace NetMan {

/**
 * @class BerNull
 */
class BerNull : public BerField {
	public:
		BerNull(u8 tagOptions = BER_TAGCLASS_NULL, u32 tag = BER_TAG_NULL);
		void parseData(u8 **out);
		static std::shared_ptr<BerNull> decode(u8 **data);
		virtual ~BerNull();
		void print() override;
};

}

#endif
