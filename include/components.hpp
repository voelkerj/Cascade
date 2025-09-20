#ifndef CASCADE_COMPONENTS_H
#define CASCADE_COMPONENTS_H

// This file contains some baked-in components for Cascade

namespace Cascade
{
  struct State
  {
    float X{0}, Y{0}, VX{0}, VY{0}, AX{0}, AY{0}, SizeX{1}, SizeY{1}, Angle{0};
  };
}

#endif