/**
 * @file BerSequence.h
 * @brief 
 */

#ifndef BERSEQUENCE_H_
#define BERSEQUENCE_H_

#include <vector>

#include "asn1/BerField.h"

// Defines
#define BER_TAGCLASS_SEQUENCE 	(BER_TAG_UNIVERSAL | BER_TAG_STRUCTURED)
#define BER_TAG_SEQUENCE		16

namespace NetMan {

class BerSequence: public BerField {
	private:
		std::vector<BerField*> children;
	public:
		BerSequence(u8 tagOptions = BER_TAGCLASS_SEQUENCE, u32 tag = BER_TAG_SEQUENCE);
		void addChild(BerField *child);
		void parseData(u8 **out);
		u32 getTotalSize() override;
		~BerSequence();
};

}

#endif
