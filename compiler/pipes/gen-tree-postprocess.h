#pragma once

#include "compiler/function-pass.h"

class GenTreePostprocessPass : public FunctionPassBase {
  AUTO_PROF(gentree_postprocess);

  struct builtin_fun {
    Operation op;
    int args;
  };

  static bool is_superglobal(const string &s);
  builtin_fun get_builtin_function(const std::string &name);

public:
  VertexPtr on_enter_vertex(VertexPtr vertex, LocalT *);
  VertexPtr on_exit_vertex(VertexPtr vertex, LocalT *);
};