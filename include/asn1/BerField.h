/**
 * @file BerField.h
 * @brief BER field
 */

#ifndef BERFIELD_H_
#define BERFIELD_H_

// Includes C/C++
#include <memory>

// Own includes
#include <3ds/types.h>

// Defines
#define BER_TAG_CLASS(x)		((x) << 6)
#define BER_TAG_UNIVERSAL		BER_TAG_CLASS(0)
#define BER_TAG_APPLICATION		BER_TAG_CLASS(1)
#define BER_TAG_CONTEXT			BER_TAG_CLASS(2)
#define BER_TAG_PRIVATE			BER_TAG_CLASS(3)
#define BER_TAG_STRUCTURED		(1 << 5)

namespace NetMan {

/**
 * @class BerField
 * @brief BER field
 */
class BerField {
	private:
		u32 tag;
		u32 length;
		u8 tagOptions;
		u8 tagSize;
		u8 lengthSize;
	protected:
		void setLength(u32 length);
		inline u32 getLength() { return length; }
		static u32 decodeLength(u8 **data);
	public:
		BerField(u8 tagOptions, u32 tag);
		virtual u32 getTotalSize();
		void parseTag(u8 **out);
		void parseLength(u8 **out);
		virtual void parseData(u8 **out) = 0;
		virtual void print() = 0;
		virtual ~BerField() { }
		static std::shared_ptr<BerField> decode(u8 **data);
};

}

#endif
