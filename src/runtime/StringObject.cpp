#include "Escargot.h"
#include "StringObject.h"
#include "Context.h"

namespace Escargot {

StringObject::StringObject(ExecutionState& state, String* value)
    : Object(state, ESCARGOT_OBJECT_BUILTIN_PROPERTY_NUMBER + 1, true)
    , m_primitiveValue(value)
{
    m_structure = state.context()->defaultStructureForStringObject();
    m_values[ESCARGOT_OBJECT_BUILTIN_PROPERTY_NUMBER] = Value(m_primitiveValue->length());
    setPrototype(state, state.context()->globalObject()->stringPrototype());
}

void StringObject::setStringData(ExecutionState& state, String* data)
{
    m_primitiveValue = data;
    defineOwnProperty(state, ObjectPropertyName(state.context()->staticStrings().length), ObjectPropertyDescriptorForDefineOwnProperty(Value(data->length()), ObjectPropertyDescriptor::NotPresent));
}

Object::ObjectGetResult StringObject::getOwnProperty(ExecutionState& state, const ObjectPropertyName& P) ESCARGOT_OBJECT_SUBCLASS_MUST_REDEFINE
{
    Value::ValueIndex idx;
    if (LIKELY(P.isUIntType())) {
        idx = P.uintValue();
    } else {
        idx = P.toValue(state).toIndex(state);
    }
    if (LIKELY(idx != Value::InvalidIndexValue)) {
        size_t strLen = m_primitiveValue->length();
        if (LIKELY(idx < strLen)) {
            return Object::ObjectGetResult(Value(String::fromCharCode(m_primitiveValue->charAt(idx))), false, true, false);
        }
    }
    return Object::getOwnProperty(state, P);
}

bool StringObject::defineOwnProperty(ExecutionState& state, const ObjectPropertyName& P, const ObjectPropertyDescriptorForDefineOwnProperty& desc) ESCARGOT_OBJECT_SUBCLASS_MUST_REDEFINE
{
    auto r = getOwnProperty(state, P);
    if (r.hasValue() && !r.isConfigurable())
        return false;
    return Object::defineOwnProperty(state, P, desc);
}

void StringObject::deleteOwnProperty(ExecutionState& state, const ObjectPropertyName& P) ESCARGOT_OBJECT_SUBCLASS_MUST_REDEFINE
{
    auto r = getOwnProperty(state, P);
    if (r.hasValue() && !r.isConfigurable())
        return;
    return Object::deleteOwnProperty(state, P);
}

void StringObject::enumeration(ExecutionState& state, std::function<bool(const ObjectPropertyName&, const ObjectPropertyDescriptor& desc)> callback) ESCARGOT_OBJECT_SUBCLASS_MUST_REDEFINE
{
    size_t len = m_primitiveValue->length();
    for (size_t i = 0; i < len; i++) {
        if (!callback(ObjectPropertyName(state, Value(i)), ObjectPropertyDescriptor::createDataDescriptor(ObjectPropertyDescriptor::EnumerablePresent))) {
            return;
        }
    }
    Object::enumeration(state, callback);
}
}