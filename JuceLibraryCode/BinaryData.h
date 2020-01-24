/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   Jost600Semi_otf;
    const int            Jost600Semi_otfSize = 39860;

    extern const char*   Jost700Bold_otf;
    const int            Jost700Bold_otfSize = 39196;

    extern const char*   sawwave_svg;
    const int            sawwave_svgSize = 595;

    extern const char*   sinewave_svg;
    const int            sinewave_svgSize = 663;

    extern const char*   squarewave_svg;
    const int            squarewave_svgSize = 597;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 5;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
