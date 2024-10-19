#pragma once

#include <stddef.h>

#define INITGRAPH_HASH_MAX 256
#define INITGRAPH_STAGE(name, depends) initgraph_add_stage(name, depends, NULL);
#define INITGRAPH_NODE(name, call)                                             \
  initgraph_add_node(name, 0, NULL, 0, NULL, call);
#define INITGRAPH_NODE_STAGE(name, stage)                                      \
  {                                                                            \
    char *ns = stage;                                                          \
    initgraph_add_node(name, 0, NULL, 1, &ns, NULL);                           \
  }
#define INITGRAPH_NODE_DEP(name, dep)                                          \
  {                                                                            \
    char *nd = dep;                                                            \
    initgraph_add_node(name, 1, &nd, 0, NULL, NULL);                           \
  }

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

void initgraph_add_stage(char *name, char *previous_stage, void (*call)());
void initgraph_add_node(char *name, size_t dependency_count,
                        char **dependencies, size_t dependend_count,
                        char **dependends, void (*call)());
