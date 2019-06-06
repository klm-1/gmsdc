#ifndef FSMANAGER_H
#define FSMANAGER_H

#include <string>


class FsManager
{
public:
    static void directoryCreate(const std::wstring& path);
    static void directoryDelete(const std::wstring& path);
};

#endif // FSMANAGER_H
