#ifndef SynthNavToolkit_h
#define SynthNavToolkit_h

#include <assert.h>

/// All files that include this one are part of the SynthNavToolkit, which
/// can both build a Python library (.pyd) for use in standalone prototyping/
/// testing and can be incorporated directly into the main Photosynth viewer
/// project.
/// The SYNTH_NAV_TOOLKIT_DEBUGGING macro, if defined, turns on debugging
/// checks which should be turned OFF (macro not defined) for retail builds.
/// USE_MINI_COMMON is needed for the standalone Python build, and causes
/// inclusion of a (hopefully fully compatible) standalone subset of the
/// Seadragon Common library.
/// Note that the Python library must be built in Release mode, not Debug
/// (unless a special Python build has been installed on the development
/// machine), but even in Release, SYNTH_NAV_TOOLKIT_DEBUGGING together with
/// symbols give us most useful checks and debugging info.

//#define SYNTH_NAV_TOOLKIT_DEBUGGING

#ifdef USE_MINI_COMMON

    // Simple debugging aids.
    #ifdef SYNTH_NAV_TOOLKIT_DEBUGGING
        #include <assert.h>
        #include <stdio.h>
        #define b_assert(XX) assert(XX)
        #define b_printf(...) printf(__VA_ARGS__)
        #define b_wprintf(...) wprintf(__VA_ARGS__)
    #else
        #define b_assert(XX)
        #define b_printf(...)
        #define b_wprintf(...)
    #endif
#else // !defined(USE_MINI_COMMON)


/*
    //#error Define b_assert-- note that we really want it to be zero-cost for release builds without SYNTH_NAV_TOOLKIT_DEBUGGING.
    #define b_assert(XX) DebugOnlyAssert("SynthNavToolkit",XX)
    //#define b_printf(...)  printf(__VA_ARGS__)
    #define b_printf(...)
    //#define b_wprintf(...) wprintf(__VA_ARGS__)
    //#define b_printf(...)  
    #define b_wprintf(...)
*/
    
#endif

#endif // SynthNavToolkit_h
