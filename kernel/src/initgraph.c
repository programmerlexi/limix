#include <kernel/debug.h>
#include <kernel/initgraph.h>
#include <kernel/kernel.h>
#include <kernel/mm/heap.h>
#include <libk/crypt/crc32.h>
#include <libk/utils/memory/heap_wrap.h>
#include <libk/utils/memory/memory.h>
#include <libk/utils/strings/strings.h>

#undef DEBUG_MODULE
#define DEBUG_MODULE "initgraph"

static InitGraph *initgraph;

static InitNode *_initgraph_get(char *name) {
  if (!initgraph)
    kernel_panic_error("No initgraph");
  if (!name)
    return NULL;
  u32 hash = crc32((u8 *)name, kstrlen(name));
  InitGraphHashNode *hash_node = initgraph->nodes[hash % INITGRAPH_HASH_MAX];
  if (!hash_node)
    return NULL;
  if (!hash_node->nodes)
    return NULL;
  for (size_t i = 0; i < hash_node->node_count; i++) {
    if (!hash_node->nodes[i])
      continue;
    char *node_name = hash_node->nodes[i]->name;
    if (!node_name)
      continue;
    if (kstrlen(name) != kstrlen(node_name))
      continue;
    if (!kstrncmp(name, node_name, kstrlen(name)))
      continue;
    return hash_node->nodes[i];
  }
  return NULL;
}

static InitNode *_initgraph_create(char *name) {
  if (!initgraph)
    kernel_panic_error("No initgraph");
  InitNode *node = kmalloc(sizeof(*node));
  node->name = clone(name, kstrlen(name) + 1);
  node->type = InitGraphStub;
  u32 hash = crc32((u8 *)name, kstrlen(name));
  InitGraphHashNode *hash_node = initgraph->nodes[hash % INITGRAPH_HASH_MAX];
  if (!hash_node) {
    hash_node = kmalloc(sizeof(*hash_node));
    initgraph->nodes[hash % INITGRAPH_HASH_MAX] = hash_node;
  }
  hash_node->node_count++;
  if (hash_node->nodes)
    hash_node->nodes =
        krealloc(hash_node->nodes, (hash_node->node_count - 1) * sizeof(void *),
                 hash_node->node_count * sizeof(void *));
  else
    hash_node->nodes = kmalloc(hash_node->node_count * sizeof(void *));
  hash_node->nodes[hash_node->node_count - 1] = node;
  return node;
}

static InitNode *_initgraph_get_or_create(char *name) {
  if (!initgraph)
    kernel_panic_error("No initgraph");
  InitNode *node = _initgraph_get(name);
  if (node)
    return node;
  return _initgraph_create(name);
}

static void _initgraph_add_dependency(char *name, char *dep) {
  if (!initgraph)
    kernel_panic_error("No initgraph");
  InitNode *node = _initgraph_get_or_create(name);
  InitNode *dep_node = _initgraph_get_or_create(dep);
  node->dependency_count++;
  if (node->dependencies)
    node->dependencies = krealloc(node->dependencies,
                                  (node->dependency_count - 1) * sizeof(void *),
                                  node->dependency_count * sizeof(void *));
  else
    node->dependencies = kmalloc(node->dependency_count * sizeof(void *));
  node->dependencies[node->dependency_count - 1] = dep_node;
  logf(LOGLEVEL_DEBUG,
       "Added dependency '%s' (0x%x) to '%s' (0x%x) with index %i. '%s' now "
       "has %i dependencies.",
       dep, dep_node, name, node, node->dependency_count - 1, name,
       node->dependency_count);
}

void initgraph_init() { initgraph = kmalloc(sizeof(*initgraph)); }

void initgraph_run(char *name) {
  if (!initgraph)
    kernel_panic_error("No initgraph");
  InitNode *node = _initgraph_get_or_create(name);
  if (node->type == InitGraphStub)
    kernel_panic_error("Couldn't run initgraph (Missing Node)");
  if (node->dependencies && node->dependency_count)
    for (size_t i = 0; i < node->dependency_count; i++) {
      if (!node->dependencies[i]) {
        logf(LOGLEVEL_WARN1,
             "Got empty dependency in node '%s' (dependency %i)", name, i);
        continue;
      }
      if (node->dependencies[i]->done)
        continue;
      logf(LOGLEVEL_DEBUG, "Calling '%s' dependency node %i at 0x%x", name, i,
           node->dependencies[i]);
      initgraph_run(node->dependencies[i]->name);
    }
  if (!node->done) {
    if (node->call) {
      logf(LOGLEVEL_INFO, "Running initgraph node '%s'", node->name);
      node->call();
    }
    if (node->type == InitGraphStage)
      logf(LOGLEVEL_INFO, "Reached initgraph stage '%s'", node->name);
  }
  node->done = true;
}

void initgraph_add_stage(char *name, char *previous_stage, void (*call)()) {
  if (!initgraph)
    kernel_panic_error("No initgraph");
  InitNode *node = _initgraph_get_or_create(name);
  node->type = InitGraphStage;
  if (call)
    node->call = call;
  if (previous_stage)
    _initgraph_add_dependency(name, previous_stage);
}
void initgraph_add_node(char *name, size_t dependency_count,
                        char **dependencies, size_t dependend_count,
                        char **dependends, void (*call)()) {
  if (!initgraph)
    kernel_panic_error("No initgraph");
  InitNode *node = _initgraph_get_or_create(name);
  node->type = InitGraphNode;
  if (call)
    node->call = call;
  for (size_t i = 0; i < dependency_count; i++)
    _initgraph_add_dependency(name, dependencies[i]);
  for (size_t i = 0; i < dependend_count; i++)
    _initgraph_add_dependency(dependends[i], name);
}
