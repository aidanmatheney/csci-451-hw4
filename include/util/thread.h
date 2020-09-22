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

void safeMutexInit(
    pthread_mutex_t *mutexOutPtr,
    pthread_mutexattr_t const *attributes,
    char const *callerDescription
);
void safeMutexLock(pthread_mutex_t *mutexPtr, char const *callerDescription);
void safeMutexUnlock(pthread_mutex_t *mutexPtr, char const *callerDescription);
void safeMutexDestroy(pthread_mutex_t *mutexPtr, char const *callerDescription);

void safeConditionInit(
    pthread_cond_t *conditionOutPtr,
    pthread_condattr_t const *attributes,
    char const *callerDescription
);
void safeConditionSignal(pthread_cond_t *conditionPtr, char const *callerDescription);
void safeConditionWait(
    pthread_cond_t *conditionPtr,
    pthread_mutex_t *mutexPtr,
    char const *callerDescription
);
void safeConditionDestroy(pthread_cond_t *conditionPtr, char const *callerDescription);
