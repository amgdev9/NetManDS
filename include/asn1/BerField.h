/**
 * @file BerField.h
 * @brief 
 */

#ifndef BERFIELD_H_
#define BERFIELD_H_

#include <3ds/types.h>

namespace NetMan {

class BerField {
	public:
		BerField() { }
		virtual u32 getSize() = 0;
		virtual void encode(u8 *out) = 0;
		virtual ~BerField() = 0;
};

}

#endif
