/**
 * @file Mib.cpp
 * @brief MIB holder
 */

// Own includes
#include "snmp/Mib.h"

namespace NetMan {

/**
 * @brief Constructor for a MIB object
 * @param tree      MIB OID tree
 */
Mib::Mib(std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<MibOid>>> tree) {
    this->oidTree = tree;
}

/**
 * @brief Resolve an OID using its name
 * @param name      OID name
 * @return The corresponding OID
 */
std::shared_ptr<BerOid> Mib::resolve(const std::string &name) {

    std::vector<u32> oid;
    std::string tokenName = name;

    try {

        // Loop until "iso" is found
        while(tokenName.compare("iso") != 0) {
            auto it = oidTree->find(tokenName);
            if(it == oidTree->end()) {
                throw std::runtime_error("Error resolving " + tokenName);
            }
            oid.insert(oid.begin(), it->second->value);
            tokenName.clear();
            tokenName.append(it->second->parentName);
        }
        oid.insert(oid.begin(), 1);     // Add "iso" OID chunk
        return std::make_shared<BerOid>(oid);
    } catch (const std::bad_alloc &e) {
        throw;
    } catch (const std::runtime_error &e) {
        throw;
    }
}

/**
 * @brief Print a MIB with all its definitions
 */
void Mib::print() {
    FILE *f = fopen("log.txt", "wb");
    fprintf(f, "MIB entries: %d\n", oidTree->size());
    auto mibOids = *oidTree.get();
    for(auto element : mibOids) {
        fprintf(f, "%s: { %s %ld }\n", element.first.c_str(), element.second->parentName.c_str(), element.second->value);
        for(auto child : element.second->children) {
            fprintf(f, "Child: %s\n", child.first.c_str());
        }
        switch(element.second->macroType) {
            case MACRO_MODULE_IDENTITY:
            {
                auto moduleIdentity = (MibModuleIdentity*)element.second->macroData.get();
                fprintf(f, "\tLastUpdated: %s\n", moduleIdentity->lastUpdated.c_str());
                fprintf(f, "\tOrganization: %s\n", moduleIdentity->organization.c_str());
                fprintf(f, "\tContactInfo: %s\n", moduleIdentity->contactInfo.c_str());
                fprintf(f, "\tDescription: %s\n", moduleIdentity->description.c_str());
                for(auto revision : moduleIdentity->revisions) {
                    fprintf(f, "\tRevisionDate: %s\n", revision.date.c_str());
                    fprintf(f, "\tRevision: %s\n", revision.description.c_str());
                }
            } break;
            case MACRO_OBJECT_TYPE:
            {
                auto objectType = (MibObjectType*)element.second->macroData.get();
                fprintf(f, "\tSyntax: %s\n", objectType->syntax.c_str());
                fprintf(f, "\tUnits: %s\n", objectType->units.c_str());
                fprintf(f, "\tMaxAccess: %s\n", objectType->maxAccess.c_str());
                fprintf(f, "\tStatus: %s\n", objectType->status.c_str());
                fprintf(f, "\tDescription: %s\n", objectType->description.c_str());
                fprintf(f, "\tReference: %s\n", objectType->reference.c_str());
                fprintf(f, "\tIndexPart: %s\n", objectType->indexPart.c_str());
                fprintf(f, "\tDefaultValue: %s\n", objectType->defaultValue.c_str());
            } break;
            default:
                break;
        }
    }
    fclose(f);
}

Mib::~Mib() { }

}
