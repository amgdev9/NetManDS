/**
 * @file BerDocument.h
 * @brief 
 */

#ifndef BERDOCUMENT_H_
#define BERDOCUMENT_H_

// Includes C/C++
#include <vector>

// Own includes
#include "BerField.h"

namespace NetMan {

class BerDocument {
	private:
		std::vector<BerField*> fields;
	public:
		BerDocument();
		void addField(BerField *field);
		const std::vector<BerField*> &getFields();
		virtual ~BerDocument();
};

}

#endif
