#pragma once

#include "libk/utils/results.h"
#include "libk/utils/variety.h"

enum AsyncStateEnum {
  ASYNC_NONE,
  ASYNC_RUNNING,
  ASYNC_INIT,
  ASYNC_WAIT,
  ASYNC_DONE
};

typedef struct AsyncTaskStruct {
  void *sp;
  void *page;
  enum AsyncStateEnum state;
  union {
    struct AsyncTaskStruct *next;
    result_t res;
  };
} AsyncTask;

typedef struct {
  AsyncTask *run;
  AsyncTask *own;
} AsyncFuture;

void fire(AsyncFuture *future);
result_t await(AsyncFuture *future);
AsyncFuture async(result_t (*func)(variety_t), variety_t arg);

void async_init();
