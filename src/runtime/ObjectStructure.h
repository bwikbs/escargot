#ifndef __EscargotObjectStructure__
#define __EscargotObjectStructure__

#include "runtime/AtomicString.h"
#include "runtime/ExecutionState.h"
#include "runtime/ObjectPropertyDescriptor.h"
#include "runtime/PropertyName.h"

namespace Escargot {

class ObjectStructure;

struct ObjectStructureItem : public gc {
    ObjectStructureItem(const PropertyName& as, const ObjectPropertyDescriptor& desc)
        : m_propertyName(as)
        , m_descriptor(desc)
    {
    }

    PropertyName m_propertyName;
    ObjectPropertyDescriptor m_descriptor;
};

struct ObjectStructureTransitionItem : public gc {
    PropertyName m_propertyName;
    ObjectPropertyDescriptor m_descriptor;
    ObjectStructure* m_structure;

    ObjectStructureTransitionItem(const PropertyName& as, const ObjectPropertyDescriptor& desc, ObjectStructure* structure)
        : m_propertyName(as)
        , m_descriptor(desc)
        , m_structure(structure)
    {
    }
};

typedef Vector<ObjectStructureItem, gc_malloc_atomic_ignore_off_page_allocator<ObjectStructureItem>> ObjectStructureItemVector;
typedef Vector<ObjectStructureTransitionItem, gc_malloc_ignore_off_page_allocator<ObjectStructureTransitionItem>> ObjectStructureTransitionTableVector;
class ObjectStructure : public gc {
    friend class Object;

public:
    ObjectStructure(ExecutionState& state, bool needsTransitionTable = true)
    {
        m_needsTransitionTable = needsTransitionTable;
        m_hasIndexPropertyName = false;
    }

    ObjectStructure(ExecutionState& state, ObjectStructureItemVector&& properties, bool needsTransitionTable, bool hasIndexPropertyName)
        : m_properties(properties)
    {
        m_needsTransitionTable = needsTransitionTable;
        m_hasIndexPropertyName = hasIndexPropertyName;
    }

    size_t findProperty(ExecutionState& state, String* propertyName)
    {
        PropertyName name(state, propertyName);
        return findProperty(name);
    }

    size_t findProperty(ExecutionState& state, const PropertyName& name)
    {
        return findProperty(name);
    }

    ObjectStructureItem readProperty(ExecutionState& state, String* propertyName)
    {
        return readProperty(state, findProperty(state, propertyName));
    }

    const ObjectStructureItem& readProperty(ExecutionState& state, size_t idx)
    {
        return m_properties[idx];
    }

    ObjectStructure* addProperty(ExecutionState& state, const PropertyName& name, const ObjectPropertyDescriptor& desc)
    {
        if (m_needsTransitionTable) {
            size_t r = searchTransitionTable(name, desc);
            if (r != SIZE_MAX) {
                return m_transitionTable[r].m_structure;
            }
        } else {
            ASSERT(m_transitionTable.size() == 0);
        }

        ObjectStructureItem newItem(name, desc);
        ObjectStructureItemVector newProperties(m_properties, newItem);
        ObjectStructure* newObjectStructure = new ObjectStructure(state, std::move(newProperties), m_needsTransitionTable, m_hasIndexPropertyName | isIndexString(name.string()));

        if (m_needsTransitionTable) {
            ObjectStructureTransitionItem newTransitionItem(name, desc, newObjectStructure);
            m_transitionTable.pushBack(newTransitionItem);
        }

        return newObjectStructure;
    }

    ObjectStructure* addProperty(ExecutionState& state, String* propertyName, const ObjectPropertyDescriptor& desc)
    {
        PropertyName name(state, propertyName);
        return addProperty(state, name, desc);
    }

    ObjectStructure* removeProperty(ExecutionState& state, String* propertyName)
    {
        // TODO after implement ErrorObject, we should throw error here
        PropertyName name(state, propertyName);
        ASSERT(findProperty(name) != SIZE_MAX);

        size_t pIndex = findProperty(name);
        return removeProperty(state, pIndex);
    }

    ObjectStructure* removeProperty(ExecutionState& state, size_t pIndex)
    {
        ObjectStructureItemVector newProperties;
        newProperties.resizeWithUninitializedValues(m_properties.size() - 1);

        size_t newIdx = 0;
        bool hasIndexString = false;
        for (size_t i = 0; i < m_properties.size(); i++) {
            if (i == pIndex)
                continue;
            hasIndexString = hasIndexString | isIndexString(m_properties[i].m_propertyName.string());
            newProperties[newIdx].m_propertyName = m_properties[i].m_propertyName;
            newProperties[newIdx].m_descriptor = m_properties[i].m_descriptor;
            newIdx++;
        }

        return new ObjectStructure(state, std::move(newProperties), false, hasIndexString);
    }

    bool inTransitionMode()
    {
        return m_needsTransitionTable;
    }

    bool hasIndexPropertyName()
    {
        return m_hasIndexPropertyName;
    }

    ObjectStructure* escapeTransitionMode(ExecutionState& state)
    {
        ASSERT(inTransitionMode());
        ObjectStructureItemVector newItem(m_properties);
        return new ObjectStructure(state, std::move(newItem), false, m_hasIndexPropertyName);
    }

    size_t propertyCount() const
    {
        return m_properties.size();
    }

private:
    bool m_needsTransitionTable;
    bool m_hasIndexPropertyName;
    ObjectStructureItemVector m_properties;
    ObjectStructureTransitionTableVector m_transitionTable;

    size_t searchTransitionTable(const PropertyName& s, const ObjectPropertyDescriptor& desc)
    {
        ASSERT(m_needsTransitionTable);
        for (size_t i = 0; i < m_transitionTable.size(); i++) {
            if (m_transitionTable[i].m_descriptor == desc && m_transitionTable[i].m_propertyName == s) {
                return i;
            }
        }

        return SIZE_MAX;
    }

    size_t findProperty(PropertyName s)
    {
        for (size_t i = 0; i < m_properties.size(); i++) {
            if (m_properties[i].m_propertyName == s)
                return i;
        }

        return SIZE_MAX;
    }
};
}

#endif