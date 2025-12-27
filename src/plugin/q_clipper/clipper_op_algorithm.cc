#include "clipper_op_algorithm.h"

#include <QtConcurrent>
#include <QtConcurrentMap>

#include <qglobal.h>

namespace Solution
{

void ClipperOp(OpList& op_list)
{

  auto type = op_list.ctype;
  auto rule = op_list.rule;

  auto future = QtConcurrent::map(op_list.paths,
    [type, rule](Path& path)
    {
      Clipper2Lib::Clipper64 c;
      c.Clear();

      c.AddSubject({ path.subject });
      c.AddClip({ path.clip });
      c.Execute(type, rule, path.solution);
    });

  future.waitForFinished();
};

}