#include "dnpch.h"

#include <daScript/daScript.h>
#include <daScript/daScriptBind.h>
#include <daScript/ast/ast_typefactory_bind.h>
#include <daScript/ast/ast_interop.h>

#include "Duin/IO/Filesystem.h"
#include "Duin/IO/VirtualFilesystem.h"
#include "Duin/IO/IOStream.h"
#include "Duin/IO/VirtualIOStream.h"
#include "Duin/Core/Debug/DNLog.h"

// ========================================================================
// Enum bindings — duin::fs C-style (must precede the module class)
// ========================================================================

DAS_BIND_ENUM_CAST_98_IN_NAMESPACE(duin::fs::PathType, DnPathType)
DAS_BASE_BIND_ENUM_98(
    duin::fs::PathType, DnPathType, DNFS_PATHTYPE_NONE, DNFS_PATHTYPE_FILE, DNFS_PATHTYPE_DIRECTORY,
    DNFS_PATHTYPE_OTHER)

DAS_BIND_ENUM_CAST_98_IN_NAMESPACE(duin::fs::UserFolder, DnUserFolder)
DAS_BASE_BIND_ENUM_98(
    duin::fs::UserFolder, DnUserFolder, DNFS_FOLDER_HOME, DNFS_FOLDER_DESKTOP, DNFS_FOLDER_DOCUMENTS,
    DNFS_FOLDER_DOWNLOADS, DNFS_FOLDER_MUSIC, DNFS_FOLDER_PICTURES, DNFS_FOLDER_PUBLICSHARE, DNFS_FOLDER_SAVEDGAMES,
    DNFS_FOLDER_SCREENSHOTS, DNFS_FOLDER_TEMPLATES, DNFS_FOLDER_VIDEOS)

DAS_BIND_ENUM_CAST_98_IN_NAMESPACE(duin::fs::EnumerationResult, DnEnumerationResult)
DAS_BASE_BIND_ENUM_98(
    duin::fs::EnumerationResult, DnEnumerationResult, DNFS_ENUM_CONTINUE, DNFS_ENUM_SUCCESS, DNFS_ENUM_FAILURE)

// ========================================================================
// Enum bindings — duin::io enum class
// DAS_BASE_BIND_ENUM generates both the typeFactory specialization (required
// for use as function parameters) and the EnumerationDn* class.
// ========================================================================

DAS_BASE_BIND_ENUM(duin::io::IOWhence, DnIOWhence, Set, Cur, End)

DAS_BASE_BIND_ENUM(duin::io::IOStatus, DnIOStatus, Ready, Error, EOF_, NotReady, Readonly, Writeonly)

// ========================================================================
// PathInfo struct annotation
// ========================================================================

MAKE_TYPE_FACTORY(DnPathInfo, duin::fs::PathInfo)

struct DnPathInfoAnnotation : das::ManagedStructureAnnotation<duin::fs::PathInfo, false, false>
{
    DnPathInfoAnnotation(das::ModuleLibrary &lib) : ManagedStructureAnnotation("DnPathInfo", lib)
    {
        addField<DAS_BIND_MANAGED_FIELD(type)>("typev", "typev");
        addField<DAS_BIND_MANAGED_FIELD(size)>("size", "size");
        addField<DAS_BIND_MANAGED_FIELD(createTime)>("createTime", "createTime");
        addField<DAS_BIND_MANAGED_FIELD(modifyTime)>("modifyTime", "modifyTime");
        addField<DAS_BIND_MANAGED_FIELD(accessTime)>("accessTime", "accessTime");
    }
};

// ========================================================================
// IOStream opaque handle
// ========================================================================

struct DnIOStreamHandle
{
    duin::io::IOStream stream;
};

MAKE_TYPE_FACTORY(DnIOStreamHandle, DnIOStreamHandle)

struct DnIOStreamHandleAnnotation : das::ManagedStructureAnnotation<DnIOStreamHandle, false, false>
{
    DnIOStreamHandleAnnotation(das::ModuleLibrary &lib) : ManagedStructureAnnotation("DnIOStreamHandle", lib)
    {
    }
};

// ========================================================================
// duin::fs:: wrappers
// ========================================================================

static const char *dn_fs_get_file_name_impl(const char *path, das::Context *ctx)
{
    std::string result = duin::fs::GetFileName(path ? path : "");
    return ctx->allocateString(result, nullptr);
}

static const char *dn_fs_get_file_extension_impl(const char *filename, das::Context *ctx)
{
    std::string result = duin::fs::GetFileExtension(filename ? filename : "");
    return ctx->allocateString(result, nullptr);
}

static bool dn_fs_file_copy_impl(const char *oldPath, const char *newPath)
{
    return duin::fs::FileCopy(oldPath ? oldPath : "", newPath ? newPath : "");
}

static bool dn_fs_create_dir_impl(const char *path)
{
    return duin::fs::CreateDir(path ? path : "");
}

static const char *dn_fs_get_base_path_impl(das::Context *ctx)
{
    std::string result = duin::fs::GetBasePath();
    return ctx->allocateString(result, nullptr);
}

static const char *dn_fs_get_bin_path_impl(das::Context *ctx)
{
    std::string result = duin::fs::GetBinPath();
    return ctx->allocateString(result, nullptr);
}

static const char *dn_fs_get_current_dir_impl(das::Context *ctx)
{
    std::string result = duin::fs::GetCurrentDir();
    return ctx->allocateString(result, nullptr);
}

static const char *dn_fs_get_pref_path_impl(const char *org, const char *app, das::Context *ctx)
{
    std::string result = duin::fs::GetPrefPath(org ? org : "", app ? app : "");
    return ctx->allocateString(result, nullptr);
}

static bool dn_fs_set_pref_path_impl(const char *org, const char *app)
{
    return duin::fs::SetPrefPath(org ? org : "", app ? app : "");
}

static bool dn_fs_remove_path_impl(const char *path)
{
    return duin::fs::RemovePath(path ? path : "");
}

static bool dn_fs_rename_path_impl(const char *oldPath, const char *newPath)
{
    return duin::fs::RenamePath(oldPath ? oldPath : "", newPath ? newPath : "");
}

static bool dn_fs_is_path_invalid_impl(const char *path)
{
    return duin::fs::IsPathInvalid(path ? path : "");
}

static const char *dn_fs_ensure_unix_path_impl(const char *path, das::Context *ctx)
{
    std::string result = duin::fs::EnsureUnixPath(path ? path : "");
    return ctx->allocateString(result, nullptr);
}

static const char *dn_fs_map_virtual_to_system_impl(const char *path, das::Context *ctx)
{
    std::string result = duin::fs::MapVirtualToSystemPath(path ? path : "");
    return ctx->allocateString(result, nullptr);
}

static const char *dn_fs_map_system_to_virtual_impl(const char *path, das::Context *ctx)
{
    std::string result = duin::fs::MapSystemToVirtualPath(path ? path : "");
    return ctx->allocateString(result, nullptr);
}

static bool dn_fs_is_virtual_path_impl(const char *path)
{
    return duin::fs::IsVirtualPath(path ? path : "");
}

static bool dn_fs_get_path_info_impl(const char *path, duin::fs::PathInfo &info)
{
    return duin::fs::GetPathInfo(path ? path : "", &info);
}

static const char *dn_fs_get_user_folder_impl(duin::fs::UserFolder folder, das::Context *ctx)
{
    std::string result = duin::fs::GetUserFolder(folder);
    return ctx->allocateString(result, nullptr);
}

static bool dn_fs_set_workspace_path_impl(const char *path)
{
    return duin::fs::SetWorkspacePath(path ? path : "");
}

static const char *dn_fs_get_workspace_path_impl(das::Context *ctx)
{
    std::string result = duin::fs::GetWorkspacePath();
    return ctx->allocateString(result, nullptr);
}

static void dn_fs_glob_directory_impl(
    const char *path, const char *pattern, duin::fs::GlobFlags flags, const das::TBlock<void, const char *> &block,
    das::Context *ctx, das::LineInfoArg *at)
{
    int count = 0;
    char **results = duin::fs::GlobDirectory(path ? path : "", pattern ? pattern : "", flags, &count);
    if (results)
    {
        for (int i = 0; i < count; i++)
            das::das_invoke<void>::invoke(ctx, at, block, results[i]);
        SDL_free(results);
    }
}

static bool dn_fs_enumerate_directory_impl(
    const char *path, const das::TBlock<duin::fs::EnumerationResult, const char *, const char *> &block,
    das::Context *ctx, das::LineInfoArg *at)
{
    struct Ctx
    {
        const das::TBlock<duin::fs::EnumerationResult, const char *, const char *> *block;
        das::Context *ctx;
        das::LineInfoArg *at;
    };
    Ctx c{&block, ctx, at};
    return duin::fs::EnumerateDirectory(
        path,
        [](void *ud, const char *dir, const char *fname) -> duin::fs::EnumerationResult {
            auto *c = static_cast<Ctx *>(ud);
            return das::das_invoke<duin::fs::EnumerationResult>::invoke(c->ctx, c->at, *c->block, dir, fname);
        },
        &c);
}

// ========================================================================
// duin::vfs:: wrappers
// ========================================================================

static const char *dn_vfs_approot_impl(das::Context *ctx)
{
    std::string result = duin::vfs::APPROOT();
    return ctx->allocateString(result, nullptr);
}

static const char *dn_vfs_appdata_impl(das::Context *ctx)
{
    std::string result = duin::vfs::APPDATA();
    return ctx->allocateString(result, nullptr);
}

static const char *dn_vfs_usrdata_impl(das::Context *ctx)
{
    std::string result = duin::vfs::USRDATA();
    return ctx->allocateString(result, nullptr);
}

static const char *dn_vfs_wrkdata_impl(das::Context *ctx)
{
    std::string result = duin::vfs::WRKDATA();
    return ctx->allocateString(result, nullptr);
}

static bool dn_fs_file_copy_virtual_impl(const char *oldPath, const char *newPath)
{
    return duin::vfs::FileCopy(oldPath ? oldPath : "", newPath ? newPath : "");
}

static bool dn_fs_create_dir_virtual_impl(const char *path)
{
    return duin::vfs::CreateDir(path ? path : "");
}

static bool dn_fs_remove_path_virtual_impl(const char *path)
{
    return duin::vfs::RemovePath(path ? path : "");
}

static bool dn_fs_rename_path_virtual_impl(const char *oldPath, const char *newPath)
{
    return duin::vfs::RenamePath(oldPath ? oldPath : "", newPath ? newPath : "");
}

static void dn_fs_glob_directory_virtual_impl(
    const char *path, const char *pattern, duin::fs::GlobFlags flags, const das::TBlock<void, const char *> &block,
    das::Context *ctx, das::LineInfoArg *at)
{
    int count = 0;
    char **results = duin::vfs::GlobDirectory(path ? path : "", pattern ? pattern : "", flags, &count);
    if (results)
    {
        for (int i = 0; i < count; i++)
            das::das_invoke<void>::invoke(ctx, at, block, results[i]);
        SDL_free(results);
    }
}

static bool dn_fs_enumerate_directory_virtual_impl(
    const char *path, const das::TBlock<duin::fs::EnumerationResult, const char *, const char *> &block,
    das::Context *ctx, das::LineInfoArg *at)
{
    struct Ctx
    {
        const das::TBlock<duin::fs::EnumerationResult, const char *, const char *> *block;
        das::Context *ctx;
        das::LineInfoArg *at;
    };
    Ctx c{&block, ctx, at};
    return duin::vfs::EnumerateDirectory(
        path,
        [](void *ud, const char *dir, const char *fname) -> duin::fs::EnumerationResult {
            auto *c = static_cast<Ctx *>(ud);
            return das::das_invoke<duin::fs::EnumerationResult>::invoke(c->ctx, c->at, *c->block, dir, fname);
        },
        &c);
}

static bool dn_fs_get_path_info_virtual_impl(const char *path, duin::fs::PathInfo &info)
{
    return duin::vfs::GetPathInfo(path ? path : "", &info);
}

static const char *dn_fs_override_path_prefix_virtual_impl(const char *path, const char *prefix, das::Context *ctx)
{
    std::string result = duin::vfs::OverridePathPrefix(path ? path : "", prefix ? prefix : "");
    return ctx->allocateString(result, nullptr);
}

// ========================================================================
// duin::io::IOStream wrappers
// ========================================================================

static DnIOStreamHandle *dn_io_from_file_impl(const char *file, const char *mode)
{
    auto s = duin::io::IOStream::FromFile(file ? file : "", mode ? mode : "");
    if (!s.IsValid())
        return nullptr;
    return new DnIOStreamHandle{std::move(s)};
}

static DnIOStreamHandle *dn_io_from_dynamic_mem_impl()
{
    auto s = duin::io::IOStream::FromDynamicMem();
    if (!s.IsValid())
        return nullptr;
    return new DnIOStreamHandle{std::move(s)};
}

static void dn_io_close_impl(DnIOStreamHandle *handle)
{
    delete handle;
}

static bool dn_io_is_valid_impl(DnIOStreamHandle *handle)
{
    return handle && handle->stream.IsValid();
}

static duin::io::IOStatus dn_io_get_status_impl(DnIOStreamHandle *handle)
{
    if (!handle)
        return duin::io::IOStatus::Error;
    return handle->stream.GetStatus();
}

static int64_t dn_io_get_size_impl(DnIOStreamHandle *handle)
{
    if (!handle)
        return -1;
    return handle->stream.GetSize();
}

static int64_t dn_io_tell_impl(DnIOStreamHandle *handle)
{
    if (!handle)
        return -1;
    return handle->stream.Tell();
}

static int64_t dn_io_seek_impl(DnIOStreamHandle *handle, int64_t offset, duin::io::IOWhence whence)
{
    if (!handle)
        return -1;
    return handle->stream.Seek(offset, whence);
}

static bool dn_io_flush_impl(DnIOStreamHandle *handle)
{
    if (!handle)
        return false;
    return handle->stream.Flush();
}

static bool dn_io_stream_close_impl(DnIOStreamHandle *handle)
{
    if (!handle)
        return false;
    return handle->stream.Close();
}

static const char *dn_io_read_string_impl(DnIOStreamHandle *handle, int64_t size, das::Context *ctx)
{
    if (!handle || size <= 0)
        return nullptr;
    std::string buf(static_cast<size_t>(size), '\0');
    size_t read = handle->stream.Read(buf.data(), static_cast<size_t>(size));
    if (read == 0)
        return nullptr;
    buf.resize(read);
    return ctx->allocateString(buf, nullptr);
}

static size_t dn_io_write_string_impl(DnIOStreamHandle *handle, const char *data)
{
    if (!handle || !data)
        return 0;
    return handle->stream.Write(data, strlen(data));
}

static const char *dn_io_load_file_string_impl(const char *file, das::Context *ctx)
{
    size_t datasize = 0;
    void *data = duin::io::IOStream::LoadFile(file ? file : "", &datasize);
    if (!data)
        return nullptr;
    std::string str(static_cast<const char *>(data), datasize);
    duin::io::IOStream::FreeLoadedData(data);
    return ctx->allocateString(str, nullptr);
}

static bool dn_io_save_file_string_impl(const char *file, const char *data)
{
    if (!file || !data)
        return false;
    return duin::io::IOStream::SaveFile(file, data, strlen(data));
}

// ========================================================================
// duin::vio:: wrappers
// ========================================================================

static DnIOStreamHandle *dn_io_from_file_virtual_impl(const char *virtualPath, const char *mode)
{
    auto s = duin::vio::FromFile(virtualPath ? virtualPath : "", mode ? mode : "");
    if (!s.IsValid())
        return nullptr;
    return new DnIOStreamHandle{std::move(s)};
}

static const char *dn_io_load_file_string_virtual_impl(const char *virtualPath, das::Context *ctx)
{
    size_t datasize = 0;
    void *data = duin::vio::LoadFile(virtualPath ? virtualPath : "", &datasize);
    if (!data)
        return nullptr;
    std::string str(static_cast<const char *>(data), datasize);
    duin::vio::FreeLoadedData(data);
    return ctx->allocateString(str, nullptr);
}

static bool dn_io_save_file_string_virtual_impl(const char *virtualPath, const char *data)
{
    if (!virtualPath || !data)
        return false;
    return duin::vio::SaveFile(virtualPath, data, strlen(data));
}

// ========================================================================
// Module
// ========================================================================

class Module_DnFilesystem : public das::Module
{
    bool initialized = false;

  public:
    Module_DnFilesystem() : das::Module("dn_filesystem_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;
        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();

        // Enumerations — duin::fs C-style
        addEnumeration(new EnumerationDnPathType());
        addEnumeration(new EnumerationDnUserFolder());
        addEnumeration(new EnumerationDnEnumerationResult());

        // Enumerations — duin::io enum class
        addEnumeration(new EnumerationDnIOWhence());
        addEnumeration(new EnumerationDnIOStatus());

        // Struct annotations
        addAnnotation(new DnPathInfoAnnotation(lib));
        addAnnotation(new DnIOStreamHandleAnnotation(lib));

        // Constants
        addConstant(*this, "DN_FS_INVALID_PATH", std::string("___INVALID_PATH___"));

        // ---- duin::fs:: ----
        das::addExtern<DAS_BIND_FUN(dn_fs_get_file_name_impl)>(
            *this, lib, "dn_fs_get_file_name", das::SideEffects::none, "dn_fs_get_file_name_impl")
            ->args({"path", "context"});

        das::addExtern<DAS_BIND_FUN(dn_fs_get_file_extension_impl)>(
            *this, lib, "dn_fs_get_file_extension", das::SideEffects::none, "dn_fs_get_file_extension_impl")
            ->args({"filename", "context"});

        das::addExtern<DAS_BIND_FUN(dn_fs_file_copy_impl)>(
            *this, lib, "dn_fs_file_copy", das::SideEffects::modifyExternal, "dn_fs_file_copy_impl")
            ->args({"oldPath", "newPath"});

        das::addExtern<DAS_BIND_FUN(dn_fs_create_dir_impl)>(
            *this, lib, "dn_fs_create_dir", das::SideEffects::modifyExternal, "dn_fs_create_dir_impl")
            ->args({"path"});

        das::addExtern<DAS_BIND_FUN(dn_fs_get_base_path_impl)>(
            *this, lib, "dn_fs_get_base_path", das::SideEffects::accessGlobal, "dn_fs_get_base_path_impl")
            ->args({"context"});

        das::addExtern<DAS_BIND_FUN(duin::fs::SetBinDebugMode)>(
            *this, lib, "dn_fs_set_bin_debug_mode", das::SideEffects::modifyExternal, "duin::fs::SetBinDebugMode")
            ->args({"enabled"});

        das::addExtern<DAS_BIND_FUN(duin::fs::GetBinDebugMode)>(
            *this, lib, "dn_fs_get_bin_debug_mode", das::SideEffects::accessGlobal, "duin::fs::GetBinDebugMode");

        das::addExtern<DAS_BIND_FUN(dn_fs_get_bin_path_impl)>(
            *this, lib, "dn_fs_get_bin_path", das::SideEffects::accessGlobal, "dn_fs_get_bin_path_impl")
            ->args({"context"});

        das::addExtern<DAS_BIND_FUN(dn_fs_get_current_dir_impl)>(
            *this, lib, "dn_fs_get_current_dir", das::SideEffects::accessGlobal, "dn_fs_get_current_dir_impl")
            ->args({"context"});

        das::addExtern<DAS_BIND_FUN(dn_fs_get_pref_path_impl)>(
            *this, lib, "dn_fs_get_pref_path", das::SideEffects::accessGlobal, "dn_fs_get_pref_path_impl")
            ->args({"org", "app", "context"});

        das::addExtern<DAS_BIND_FUN(dn_fs_set_pref_path_impl)>(
            *this, lib, "dn_fs_set_pref_path", das::SideEffects::modifyExternal, "dn_fs_set_pref_path_impl")
            ->args({"org", "app"});

        das::addExtern<DAS_BIND_FUN(dn_fs_remove_path_impl)>(
            *this, lib, "dn_fs_remove_path", das::SideEffects::modifyExternal, "dn_fs_remove_path_impl")
            ->args({"path"});

        das::addExtern<DAS_BIND_FUN(dn_fs_rename_path_impl)>(
            *this, lib, "dn_fs_rename_path", das::SideEffects::modifyExternal, "dn_fs_rename_path_impl")
            ->args({"oldPath", "newPath"});

        das::addExtern<DAS_BIND_FUN(dn_fs_is_path_invalid_impl)>(
            *this, lib, "dn_fs_is_path_invalid", das::SideEffects::none, "dn_fs_is_path_invalid_impl")
            ->args({"path"});

        das::addExtern<DAS_BIND_FUN(dn_fs_ensure_unix_path_impl)>(
            *this, lib, "dn_fs_ensure_unix_path", das::SideEffects::none, "dn_fs_ensure_unix_path_impl")
            ->args({"path", "context"});

        das::addExtern<DAS_BIND_FUN(dn_fs_map_virtual_to_system_impl)>(
            *this,
            lib,
            "dn_fs_map_virtual_to_system",
            das::SideEffects::accessGlobal,
            "dn_fs_map_virtual_to_system_impl")
            ->args({"path", "context"});

        das::addExtern<DAS_BIND_FUN(dn_fs_map_system_to_virtual_impl)>(
            *this,
            lib,
            "dn_fs_map_system_to_virtual",
            das::SideEffects::accessGlobal,
            "dn_fs_map_system_to_virtual_impl")
            ->args({"path", "context"});

        das::addExtern<DAS_BIND_FUN(dn_fs_is_virtual_path_impl)>(
            *this, lib, "dn_fs_is_virtual_path", das::SideEffects::none, "dn_fs_is_virtual_path_impl")
            ->args({"path"});

        das::addExtern<DAS_BIND_FUN(dn_fs_get_path_info_impl)>(
            *this, lib, "dn_fs_get_path_info", das::SideEffects::accessGlobal, "dn_fs_get_path_info_impl")
            ->args({"path", "info"});

        das::addExtern<DAS_BIND_FUN(dn_fs_get_user_folder_impl)>(
            *this, lib, "dn_fs_get_user_folder", das::SideEffects::accessGlobal, "dn_fs_get_user_folder_impl")
            ->args({"folder", "context"});

        das::addExtern<DAS_BIND_FUN(dn_fs_set_workspace_path_impl)>(
            *this, lib, "dn_fs_set_workspace_path", das::SideEffects::modifyExternal, "dn_fs_set_workspace_path_impl")
            ->args({"path"});

        das::addExtern<DAS_BIND_FUN(dn_fs_get_workspace_path_impl)>(
            *this, lib, "dn_fs_get_workspace_path", das::SideEffects::accessGlobal, "dn_fs_get_workspace_path_impl")
            ->args({"context"});

        das::addExtern<DAS_BIND_FUN(dn_fs_glob_directory_impl)>(
            *this, lib, "dn_fs_glob_directory", das::SideEffects::invoke, "dn_fs_glob_directory_impl")
            ->args({"path", "pattern", "flags", "block", "context", "at"});

        das::addExtern<DAS_BIND_FUN(dn_fs_enumerate_directory_impl)>(
            *this, lib, "dn_fs_enumerate_directory", das::SideEffects::invoke, "dn_fs_enumerate_directory_impl")
            ->args({"path", "block", "context", "at"});

        // ---- duin::vfs:: ----
        das::addExtern<DAS_BIND_FUN(dn_vfs_approot_impl)>(
            *this, lib, "dn_vfs_approot", das::SideEffects::accessGlobal, "dn_vfs_approot_impl")
            ->args({"context"});

        das::addExtern<DAS_BIND_FUN(dn_vfs_appdata_impl)>(
            *this, lib, "dn_vfs_appdata", das::SideEffects::accessGlobal, "dn_vfs_appdata_impl")
            ->args({"context"});

        das::addExtern<DAS_BIND_FUN(dn_vfs_usrdata_impl)>(
            *this, lib, "dn_vfs_usrdata", das::SideEffects::accessGlobal, "dn_vfs_usrdata_impl")
            ->args({"context"});

        das::addExtern<DAS_BIND_FUN(dn_vfs_wrkdata_impl)>(
            *this, lib, "dn_vfs_wrkdata", das::SideEffects::accessGlobal, "dn_vfs_wrkdata_impl")
            ->args({"context"});

        das::addExtern<DAS_BIND_FUN(dn_fs_file_copy_virtual_impl)>(
            *this, lib, "dn_fs_file_copy_virtual", das::SideEffects::modifyExternal, "dn_fs_file_copy_virtual_impl")
            ->args({"oldPath", "newPath"});

        das::addExtern<DAS_BIND_FUN(dn_fs_create_dir_virtual_impl)>(
            *this, lib, "dn_fs_create_dir_virtual", das::SideEffects::modifyExternal, "dn_fs_create_dir_virtual_impl")
            ->args({"path"});

        das::addExtern<DAS_BIND_FUN(dn_fs_remove_path_virtual_impl)>(
            *this, lib, "dn_fs_remove_path_virtual", das::SideEffects::modifyExternal, "dn_fs_remove_path_virtual_impl")
            ->args({"path"});

        das::addExtern<DAS_BIND_FUN(dn_fs_rename_path_virtual_impl)>(
            *this, lib, "dn_fs_rename_path_virtual", das::SideEffects::modifyExternal, "dn_fs_rename_path_virtual_impl")
            ->args({"oldPath", "newPath"});

        das::addExtern<DAS_BIND_FUN(dn_fs_glob_directory_virtual_impl)>(
            *this, lib, "dn_fs_glob_directory_virtual", das::SideEffects::invoke, "dn_fs_glob_directory_virtual_impl")
            ->args({"path", "pattern", "flags", "block", "context", "at"});

        das::addExtern<DAS_BIND_FUN(dn_fs_enumerate_directory_virtual_impl)>(
            *this,
            lib,
            "dn_fs_enumerate_directory_virtual",
            das::SideEffects::invoke,
            "dn_fs_enumerate_directory_virtual_impl")
            ->args({"path", "block", "context", "at"});

        das::addExtern<DAS_BIND_FUN(dn_fs_get_path_info_virtual_impl)>(
            *this,
            lib,
            "dn_fs_get_path_info_virtual",
            das::SideEffects::accessGlobal,
            "dn_fs_get_path_info_virtual_impl")
            ->args({"path", "info"});

        das::addExtern<DAS_BIND_FUN(dn_fs_override_path_prefix_virtual_impl)>(
            *this,
            lib,
            "dn_fs_override_path_prefix_virtual",
            das::SideEffects::none,
            "dn_fs_override_path_prefix_virtual_impl")
            ->args({"path", "prefix", "context"});

        // ---- duin::io::IOStream ----
        das::addExtern<DAS_BIND_FUN(dn_io_from_file_impl)>(
            *this, lib, "dn_io_from_file", das::SideEffects::modifyExternal, "dn_io_from_file_impl")
            ->args({"file", "mode"});

        das::addExtern<DAS_BIND_FUN(dn_io_from_dynamic_mem_impl)>(
            *this, lib, "dn_io_from_dynamic_mem", das::SideEffects::modifyExternal, "dn_io_from_dynamic_mem_impl");

        das::addExtern<DAS_BIND_FUN(dn_io_close_impl)>(
            *this, lib, "dn_io_close", das::SideEffects::modifyExternal, "dn_io_close_impl")
            ->args({"handle"});

        das::addExtern<DAS_BIND_FUN(dn_io_is_valid_impl)>(
            *this, lib, "dn_io_is_valid", das::SideEffects::none, "dn_io_is_valid_impl")
            ->args({"handle"});

        das::addExtern<DAS_BIND_FUN(dn_io_get_status_impl)>(
            *this, lib, "dn_io_get_status", das::SideEffects::none, "dn_io_get_status_impl")
            ->args({"handle"});

        das::addExtern<DAS_BIND_FUN(dn_io_get_size_impl)>(
            *this, lib, "dn_io_get_size", das::SideEffects::none, "dn_io_get_size_impl")
            ->args({"handle"});

        das::addExtern<DAS_BIND_FUN(dn_io_tell_impl)>(
            *this, lib, "dn_io_tell", das::SideEffects::none, "dn_io_tell_impl")
            ->args({"handle"});

        das::addExtern<DAS_BIND_FUN(dn_io_seek_impl)>(
            *this, lib, "dn_io_seek", das::SideEffects::modifyExternal, "dn_io_seek_impl")
            ->args({"handle", "offset", "whence"});

        das::addExtern<DAS_BIND_FUN(dn_io_flush_impl)>(
            *this, lib, "dn_io_flush", das::SideEffects::modifyExternal, "dn_io_flush_impl")
            ->args({"handle"});

        das::addExtern<DAS_BIND_FUN(dn_io_stream_close_impl)>(
            *this, lib, "dn_io_stream_close", das::SideEffects::modifyExternal, "dn_io_stream_close_impl")
            ->args({"handle"});

        das::addExtern<DAS_BIND_FUN(dn_io_read_string_impl)>(
            *this, lib, "dn_io_read_string", das::SideEffects::modifyExternal, "dn_io_read_string_impl")
            ->args({"handle", "size", "context"});

        das::addExtern<DAS_BIND_FUN(dn_io_write_string_impl)>(
            *this, lib, "dn_io_write_string", das::SideEffects::modifyExternal, "dn_io_write_string_impl")
            ->args({"handle", "data"});

        das::addExtern<DAS_BIND_FUN(dn_io_load_file_string_impl)>(
            *this, lib, "dn_io_load_file_string", das::SideEffects::modifyExternal, "dn_io_load_file_string_impl")
            ->args({"file", "context"});

        das::addExtern<DAS_BIND_FUN(dn_io_save_file_string_impl)>(
            *this, lib, "dn_io_save_file_string", das::SideEffects::modifyExternal, "dn_io_save_file_string_impl")
            ->args({"file", "data"});

        // ---- duin::vio:: ----
        das::addExtern<DAS_BIND_FUN(dn_io_from_file_virtual_impl)>(
            *this, lib, "dn_io_from_file_virtual", das::SideEffects::modifyExternal, "dn_io_from_file_virtual_impl")
            ->args({"virtualPath", "mode"});

        das::addExtern<DAS_BIND_FUN(dn_io_load_file_string_virtual_impl)>(
            *this,
            lib,
            "dn_io_load_file_string_virtual",
            das::SideEffects::modifyExternal,
            "dn_io_load_file_string_virtual_impl")
            ->args({"virtualPath", "context"});

        das::addExtern<DAS_BIND_FUN(dn_io_save_file_string_virtual_impl)>(
            *this,
            lib,
            "dn_io_save_file_string_virtual",
            das::SideEffects::modifyExternal,
            "dn_io_save_file_string_virtual_impl")
            ->args({"virtualPath", "data"});

        DN_CORE_INFO("Script Module [dn_filesystem_core] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_DnFilesystem);
