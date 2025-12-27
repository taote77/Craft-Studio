#ifndef CLIPPER_OP_ALGORITHM_H
#define CLIPPER_OP_ALGORITHM_H

#include <clipper2/clipper.h>

namespace Solution
{
struct Path
{
  Clipper2Lib::Path64 subject;   // subject path
  Clipper2Lib::Path64 clip;      // clip path
  Clipper2Lib::Paths64 solution; // solution path
};

struct OpList
{
  Clipper2Lib::ClipType ctype;
  Clipper2Lib::FillRule rule;

  std::vector<Path> paths;
};

static void ClipperOp(OpList& op_list);

} // namespace Solution

#endif // CLIPPER_OP_ALGORITHM_H
