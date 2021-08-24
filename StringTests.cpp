//--------------------------------------------------------------------------------
// String String Function Demo - (c) 2021 Plummer's Software LLC DBA Dave's Garage
//--------------------------------------------------------------------------------
//
// Provided under the GPL Gnu Public License 2.0
// 
// This code is intended to accompany the "Safe C String" video.  It demonstrates
// how to replace older usafe string functions that lack bounds and buffer checks
// with more modern replacements intended for a hostile world.
// 
//  String Function Replacement Cheat Sheet
//  =======================================
//    
//    Olden Days        Hipsters
//    --------------------------------
//    strlen         -> strnlen_s
//    strcpy         -> strcpy_s
//    strcat         -> strcat_s
//    sprintf        -> _snprintf_s
//    vsprintf       -> vsnprintf_s
//    makepath       -> _makepath_s
//    _splitpath     -> _splitpath_s
//    scanf / sscanf -> sscanf_s
//    snscanf        -> _snscanf_s
//    gets           -> gets_s
// 
//      8/24/2001   Davepl      Created
//
//--------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <wtypes.h>
#include <stdlib.h>
#include <crtdbg.h>
#include <cassert>

// Forward declarations of functions that are defined after main

void TestVarArgs(char *, size_t, const char*, ...);
void TurnOffAsserts();

// main
//
// Entry point. Calls each string function properly as a quick demo
// of how each of the "safe" string functions can be used.

int main(int argc, char * argv [])
{
    // Turns off the Abort/Retry/Fail dialog on parameter validation
    // failures and sets up an "ignore" handler for such events.

    TurnOffAsserts();

    // Declare a too-small output buffer and a source string that is
    // too big to fit, which we shall use to test our failure cases

    char szBuffer[16];
    const char szLongString[]  = "This is a long string which is almost "
                                 "assuredly too big to fit into szBuffer.";
    // strlen -> strnlen_s
    //
    // What's Up: strlen doesn't let you say how long it should try.
    //            strlen also blows up on a null ptr
    // 
    //            strlen_s let's you specify an upper bound on length
    //            and it returns 0 if you pass it a null pointer

    int length1 = strnlen_s(szLongString, sizeof szLongString);

    // Quick test to confirm that passind null is ok

    assert(0 == strnlen_s(nullptr, RSIZE_MAX));

    // strcpy -> strcpy_s
    //
    // What's Up: strcpy doesn't let you specify a max out buf length
    // 
    // This would crash: strcpy(szBuffer, szLongString);
    //
    // strcpy_s allows you to specify the buffer size limit. When this
    // does not fit, it will call the paramter validation handler.

    strcpy_s(szBuffer, sizeof szBuffer, szLongString);

    // strcat -> strcat_s 
    // 
    // Same deal - We need to be able to specify the length.  The following
    // can happen and will call the constraint handler:
    //
    // - src or dest is a null pointer
    // - destsz is zero or greater than max size 
    // - there is no null terminator in the first destsz bytes of dest
    // - truncation would occur (can't fit src plus a nul terminator)

    strcat_s(szBuffer, sizeof szBuffer, szLongString);

    // sprintf -> snprintf_s
    //
    // Allows you to specify the buffer size and it detects the following
    // conditions at runtime, calling the constraint handler as appropriate.
    //
    //  - %n is present
    //  - any of the %s args is a null ptr
    //  - format or buffer is a null pointer
    //  - buffer size is zero or greater than RSIZE_MAX
    //  - any encoding errors during formatting
    //  - too long of a string in total

    _snprintf_s(szBuffer, sizeof szBuffer, "%s", szLongString);

    // makepath -> _makepath_s 
    //
    // Allows you to specify the maximum size of the output buffer

    _makepath_s(szBuffer, sizeof szBuffer, "C", "\\foo", "bar", "txt");

    // splitpath -> _splitpath_s
    //
    // Allows you to specify the size of each path component

    char szDrive [_MAX_DRIVE];
    char szFolder[_MAX_DIR];
    char szFile  [_MAX_FNAME];
    char szExt   [_MAX_EXT];

    _splitpath_s(szBuffer, 
                 szDrive,   sizeof szDrive, 
                 szFolder,  sizeof szFolder, 
                 szFile,    sizeof szFile, 
                 szExt,     sizeof szExt);

    // scanf/sscanf -> sscanf_s 
    //
    // Allows you to specify the max size for each path component that
    // will be extrated from the string

    char szFolderOut[_MAX_DIR];

    // sscanf_s(szBuffer, "%s", szFolderOut, sizeof szFolderOut);

    // snscanf -> _snscanf_s
    //
    // Allows you to pass a length specifier for each component that 
    // you scan out of the main string

    // Make sure these all start out as empty strings

    char szWord1[16];
    char szWord2[16];
    char szWord3[16];
    char szWord4[16];

    _snscanf_s(szLongString, sizeof szLongString, 
               "%s %s %s %s", 
               szWord1, sizeof szWord1, 
               szWord2, sizeof szWord2, 
               szWord3, sizeof szWord3,
               szWord4, sizeof szWord4
               );

    // vsprintf -> vsnprintf_s

    TestVarArgs(szBuffer, sizeof szBuffer, "%s", szLongString);

    // gets -> gets_s
    //
    // Allows you to specify the max buffer length and then writes at
    // most N-1 chars to it, plus the null terminator.  Watches for
    // and throws on the following exceptional cases:
    //
    //  - n is zero
    //  - n is greater than RSIZE_MAX
    //  - string is a null pointer
    //  - Reached n-1 chars without EOL or EOF being hit yet

    puts("Press ENTER to continue.");
    gets_s(szBuffer, sizeof szBuffer);
}

// TestVarArgs
//
// Show use of vsnprintf_s, which takes a va_list, so we need a function
// with a variable number of args to 'demo' it.

void TestVarArgs(char * buffer, size_t cb, const char* format, ...)
{
    // vsprintf -> vsnprintf_s
    //
    // Allows you to specify the buffer size and it detects the following
    // conditions at runtime, calling the constraint handler as appropriate.
    //
    //  - %n is present
    //  - any of the %s args is a null ptr
    //  - format or buffer is a null pointer
    //  - buffer size is zero or greater than RSIZE_MAX
    //  - any encoding errors during formatting
    //  - too long of a string in total
    //
    // vsnprintf_s(szBuffer, sizeof szBuffer, "%s", args1);

    va_list args;
    va_start(args, format);
    vsnprintf_s(buffer, cb, _TRUNCATE, format, args);
    va_end(args);
}

// I don't normally do the Unicode dance anymore, but dealing with
// Microsoft proprietary stuff, I do it thierway, like so:

void OurParameterValidationFailureHandler(
    const wchar_t * expression,
    const wchar_t * function,
    const wchar_t * file,
    unsigned int    line,
    uintptr_t       pReserved)
{
    wprintf_s(L"Bad Mojo!  The invalid parameter handler has been "
        L"called in %snFunction:%snFile:%snLine:%un",
        expression, function, file, line);
}

// TurnOffAsserts
//
// For MSVC, this will turn off the Abort/Retry/Fail dialog that you would
// normally get for a failed assertion, which is how parameter validation
// failures manifest themselves on Debug builds.

void TurnOffAsserts()
{
    // Prevent showing "debug assertion failed" dialog box, since we'll be
    // handling any such errors and continuing.

    _CrtSetReportMode(_CRT_ASSERT, 0);

    // The CRT now provides runtime checks when you're using the _s versions
    // of the string functions.  Normally if something is amiss it will just
    // exit, but if you want to continue, you need to handle it yourself
    // by specifying your own handler function as follows:

    _set_invalid_parameter_handler(OurParameterValidationFailureHandler);
}
