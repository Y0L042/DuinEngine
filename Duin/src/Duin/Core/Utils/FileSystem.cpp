#include "dnpch.h"
#include "FileSystem.h"

namespace duin {
    std::string GetFileName(const std::string& path)
    {
        char sep = '/';

#ifdef _WIN32
        sep = '\\';
#endif

        size_t i = path.rfind(sep, path.length());
        if (i != std::string::npos) {
            return(path.substr(i + 1, path.length() - i));
        }

        return("");
    }

    std::string GetFileExtension(const std::string& filename)
    {
        const char* dot = strrchr(filename.c_str(), '.');
        if (!dot || dot == filename) return "";
        return dot + 1;
    }
}
