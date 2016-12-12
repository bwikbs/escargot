/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef SpreadElementNode_h
#define SpreadElementNode_h

#include "Node.h"

namespace Escargot {

class SpreadElementNode : public Node {
public:
    SpreadElementNode(Node* arg)
        : Node()
    {
        m_arg = arg;
    }

    virtual ASTNodeType type()
    {
        return SpreadElement;
    }

protected:
    Node* m_arg;
};
}

#endif