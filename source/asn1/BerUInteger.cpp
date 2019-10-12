/**
 * @file BerUInteger.cpp
 * @brief 
 */

#include "asn1/BerUInteger.h"

namespace NetMan {

BerUInteger::BerUInteger(u32 value, u8 tagOptions, u32 tag) : BerInteger(*(s32*)(&value), tagOptions, tag){ }

}
