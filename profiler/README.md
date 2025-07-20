# A Simple Profiler

The philosophy behind it:
- Header Only: No complicated build steps. Just include "Profiler.h" in your project
- Simple Api: You can use simply use it by using Macros like PROFILE_FUNCTION() and PROFILE_SCOPE("name-of-scope")
- Compile Time Disabling: Profiler can be disabled with a single preprocessor definition to avoid any overhead in your release builds
- Pretty Output: The profiler will present outputs in easy to comprehend format
- Thread Safe and Build Agnostic: It will work correctly in multi threaded application and in any kind of build approach you use MTU or STUB