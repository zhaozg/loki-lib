//////////////////////////////////
// Generated header: SmallObj.cpp
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Mon Sep 30 23:14:48 2002
//////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#   include "../../loki/SmallObj.cpp"
#else
#   if (__INTEL_COMPILER)
#       include "../../loki/SmallObj.cpp"
#   elif (__MWERKS__)
#       include "../../loki/SmallObj.cpp"
#   elif (__BORLANDC__ >= 0x560)
#       include "../Borland/SmallObj.cpp"
#   elif (_MSC_VER >= 1301)
#       include "../../loki/SmallObj.cpp"
#   elif (_MSC_VER >= 1300)
#       include "../MSVC/1300/SmallObj.cpp"
#   elif (_MSC_VER >= 1200)
#       include "../MSVC/1200/SmallObj.cpp"
#   else
#       include "../../loki/SmallObj.cpp"
#   endif
#endif