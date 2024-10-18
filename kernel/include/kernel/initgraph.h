#pragma once

#include <stddef.h>

#define INITGRAPH_HASH_MAX 256

enum InitGraphNodeType {
  InitGraphStub,
  InitGraphStage,
  InitGraphNode,
};

typedef struct InitNodeStruct {
  bool done;
  enum InitGraphNodeType type;
  char *name;
  size_t dependency_count;
  struct InitNodeStruct **dependencies;
  void (*call)();
} InitNode;

typedef struct {
  size_t node_count;
  InitNode **nodes;
} InitGraphHashNode;

typedef struct {
  InitGraphHashNode *nodes[INITGRAPH_HASH_MAX];
} InitGraph;

void initgraph_init();
void initgraph_run(char *name);

void initgraph_add_stage(char *name, void (*call)());
void initgraph_add_node(char *name, size_t dependency_count,
                        char **dependencies, size_t dependend_count,
                        char **dependends, void (*call)());
