/**
 * @file BerSequence.h
 * @brief Class to store a SEQUENCE/SEQUENCE OF
 */

#ifndef BERSEQUENCE_H_
#define BERSEQUENCE_H_

// Includes C/C++
#include <memory>
#include <vector>

// Own includes
#include "asn1/BerField.h"

// Defines
#define BER_TAGCLASS_SEQUENCE 	(BER_TAG_UNIVERSAL | BER_TAG_STRUCTURED)
#define BER_TAG_SEQUENCE		16
#define BER_TAG_ANY				0xFFFFFFFF

namespace NetMan {

/**
 * @class BerSequence
 */
class BerSequence: public BerField {
	private:
		std::vector<std::shared_ptr<BerField>> children;
	public:
		BerSequence(u8 tagOptions = BER_TAGCLASS_SEQUENCE, u32 tag = BER_TAG_SEQUENCE);
		void addChild(std::shared_ptr<BerField> child);
		std::shared_ptr<BerField> getChild(u16 i);
		void parseData(u8 **out);
		u32 getTotalSize() override;
        inline u32 getNChildren(){ return this->children.size(); }
		~BerSequence();
		static u32 decode(u8 **data, u8 tagOptions = BER_TAGCLASS_SEQUENCE, u32 tag = BER_TAG_SEQUENCE, u8 *decodedTag = NULL);
		void print() override;
};

}

#endif
