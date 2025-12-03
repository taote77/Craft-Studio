#include "global/resource.hpp"
// #include "ucore/version.h"
#include <global/application.hpp>
#include <global/env.hpp>
#include <global/log.hpp>

const char* globalUcoreReflex20ResourceData =
  "[machine]\ntype = reflex_20\n\n[slave]\nuart=/dev/ttyS1\nsim=0\nzero_high_speed_section = "
  "248\nmax_step = 290\n\n\n[proj]\nwidth = 5760\nheight = 3600\nframe_w = 230.4\nframe_h = "
  "144.0\ndefault_power_max=23\ndefault_power_min=5\ndefault_std_energy=17\n\n[print]\nmark="
  "0\n\n\n[mtl]\nuart=/dev/"
  "ttyS3\nbaud=19200\nled_rows=10\nled_cols=17\nled_hex_width=19.39896904\nled_hex_height=16.8\n";

int main(int argc, char* argv[])
{
  uzmq::Application app(argc, argv);
  std::string printResource;

  printResource = globalUcoreReflex20ResourceData;

  std::istringstream strResource(printResource);
  app.init(strResource);

  return app.exec();
}
