/*
 * Copyright (c) 2017-present Samsung Electronics Co., Ltd
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

#ifdef ESCARGOT_ENABLE_PROMISE

#include "Escargot.h"
#include "JobQueue.h"
#include "runtime/Job.h"
#include "runtime/Context.h"
#include "runtime/SandBox.h"
#include "runtime/VMInstance.h"

namespace Escargot {

JobQueue* JobQueue::create()
{
    return DefaultJobQueue::create();
}

size_t DefaultJobQueue::enqueueJob(ExecutionState& state, Job* job)
{
    m_jobs.push_back(job);

    if (state.context()->vmInstance()->m_jobQueueListener) {
        state.context()->vmInstance()->m_jobQueueListener(state);
    }

    return 0;
}
}

#endif