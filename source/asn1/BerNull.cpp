/**
 * @file BerNull.cpp
 * @brief 
 */

#include "asn1/BerNull.h"

namespace NetMan {

BerNull::BerNull(u8 tagOptions, u32 tag) : BerField(tagOptions, tag) { }

void BerNull::parseData(u8 **out) { }

}
