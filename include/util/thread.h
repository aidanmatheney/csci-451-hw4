#pragma once

#include "./callback.h"

#include <pthread.h>

DECLARE_FUNC(PthreadCreateStartRoutine, void *, void *)

pthread_t safePthreadCreate(
    pthread_attr_t const *attributes,
    PthreadCreateStartRoutine startRoutine,
    void *startRoutineArg,
    char const *callerDescription
);

void *safePthreadJoin(pthread_t threadId, char const *callerDescription);
