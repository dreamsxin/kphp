#pragma once

#include "compiler/function-pass.h"

class ConvertLocalPhpdocsPass : public FunctionPassBase {
  VertexPtr visit_phpdoc_and_extract_vars(VertexAdaptor<op_phpdoc_raw> phpdoc_raw);

public:
  string get_description() {
    return "Analyze local phpdocs";
  }

  VertexPtr on_enter_vertex(VertexPtr root, LocalT *);
};
