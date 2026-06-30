#pragma once

#include <string>

namespace feast {

enum class FaceTag
{
    LowX,
    HighX,

    LowY,
    HighY,

    LowZ,
    HighZ

};

inline std::string toString(FaceTag tag)
{
    switch (tag)
    {
        case FaceTag::LowX: return "lowx";
        case FaceTag::HighX: return "highx";
        case FaceTag::LowY: return "lowy";
        case FaceTag::HighY: return "highy";
        case FaceTag::LowZ: return "lowz";
        case FaceTag::HighZ: return "highz";

    }
    return "";
}
}
