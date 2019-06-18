#include "fsmanager.h"

#include <stdexcept>
#include <iterator>

#include "algext.h"

#ifdef __WINNT
#include "windows.h"

#include "utils.h"

void FsManager::directoryCreate(const std::wstring& path)
{
    if (!CreateDirectory(path.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
	{
        throw std::runtime_error("Cannot create directory: " + narrow(path));
    }
}

void FsManager::directoryDelete(const std::wstring& path)
{
    wchar_t zzstr[512];
    int len = GetFullPathName(path.c_str(), 512, zzstr, NULL);
    zzstr[len + 1] = '\0';

    SHFILEOPSTRUCT op;
    op.hwnd = NULL;
    op.wFunc = FO_DELETE;
    op.pFrom = zzstr;
    op.pTo = NULL;
    op.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;

    int err = SHFileOperation(&op);
    if (!(err == 0 || err == 2))
	{
        throw std::runtime_error("Cannot delete directory: " + narrow(path));
    }
}

#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void FsManager::directoryCreate(const std::wstring& path)
{
    std::string p(path.begin(), path.end());
    if (mkdir(p.c_str(), 0755))
	{
        throw std::runtime_error("Cannot create directory " + p);
    }
}

void FsManager::directoryDelete(const std::wstring& path)
{
    std::string p(path.begin(), path.end());
    if (rmdir(p.c_str()))
	{
        throw std::runtime_error("Cannot delete directory " + p);
    }
}

#endif
