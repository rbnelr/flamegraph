
namespace win32 {
    
    lstr get_exe_path (Stack* stk) {
        char* exe_path = stk->pushArr<char>(512);
        
        auto ret = GetModuleFileName(NULL, exe_path, 512);
        return lstr(exe_path, (u32)ret);
    }
    lstr find_exe_filename (lstr cr exe_path) {
        return path::find_filename(exe_path);
    }
    lstr get_pc_name (Stack* stk) {
        
        DWORD len = MAX_COMPUTERNAME_LENGTH +1;
        char* buf = stk->pushArr<char>(len);
        
        auto ret = GetComputerNameA(buf, &len);
        if (ret == 0) {
            DBGBREAK_IF_DEBUGGER_PRESENT;
            return "<GetComputerNameA failed>";
        } else {
            return lstr(buf, (u32)len);
        }
    }
    
    DECL void close_handle (HANDLE obj) {
        auto ret = CloseHandle(obj);
        assert(ret != 0);
    }
    DECL void reset_event (HANDLE obj) {
        auto ret = ResetEvent(obj);
        assert(ret != 0);
    }
    
    DECL err_e create_file (char const* filename, HANDLE* out_h, DWORD acc, DWORD share, DWORD mode) {
        
        // TODO: create files that don't exist with CreateDirectory ?
        
        auto ret = CreateFileA(filename, acc, share, NULL, mode,
                    FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        *out_h = ret;
        if (ret == INVALID_HANDLE_VALUE) {
            warning("win32::create_file::CreateFileA failed for '%' [%]!\n", filename, GetLastError());
            return FILE_OPEN_FAIL;
        }
        return OK;
    }
    
    DECL err_e open_existing_file_rd (char const* filename, HANDLE* out_h) {
        return create_file(filename, out_h, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
    }
    DECL err_e overwrite_file_wr (char const* filename, HANDLE* out_h) {
        return create_file(filename, out_h, GENERIC_WRITE, 0, CREATE_ALWAYS);
    }
    DECL err_e overwrite_file_rw (char const* filename, HANDLE* out_h) {
        return create_file(filename, out_h, GENERIC_READ|GENERIC_WRITE, 0, CREATE_ALWAYS);
    }
    DECL err_e open_existing_file (char const* filename, HANDLE* out_h, DWORD acc, DWORD share) {
        return create_file(filename, out_h, acc, share, OPEN_EXISTING);
    }
    // with asserts disabled we return INVALID_HANDLE_VALUE on fail, so the os should make all calls using this handle fail
    DECL HANDLE open_existing_file_rd_assert (char const* filename) {
        HANDLE ret;
        auto err = open_existing_file_rd(filename, &ret);
        assert(!err);
        return ret;
    }
    DECL HANDLE overwrite_file_wr_assert (char const* filename) {
        HANDLE ret;
        auto err = overwrite_file_wr(filename, &ret);
        assert(!err);
        return ret;
    }
    
    DECL err_e open_file_wr_warn_on_overwrite (char const* filename, HANDLE* out_h) {
        
        auto ret = CreateFileA(filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
                    FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        auto err = GetLastError();
        *out_h = ret;
        if (ret == INVALID_HANDLE_VALUE) {
            warning("Error: win32::open_file_wr_warn_on_overwrite::CreateFileA failed for '%' [%]!\n", filename, err);
            return FILE_OPEN_FAIL;
        }
        if (err == ERROR_ALREADY_EXISTS) {
            print("Note: overwriting file '%'!\n", filename);
        }
        return OK;
    }
    DECL HANDLE open_file_wr_assert_warn_on_overwrite (char const* filename) {
        HANDLE ret;
        assert(open_file_wr_warn_on_overwrite(filename, &ret));
        return ret;
    }
    
#if _COMP == _COMP_GCC
    #define GET_FILEPATH_FROM_HANDLE(h) \
            char filepath[MAX_PATH]; \
            filepath[0] = '\0';
#else
    #define GET_FILEPATH_FROM_HANDLE(h) \
            char filepath[MAX_PATH]; \
            filepath[0] = '\0'; \
            GetFinalPathNameByHandleA(h, filepath, MAX_PATH -1, 0) // Msdn says somthing about cchFilePath being without the terminator, but their example simply passes MAX_PATH, pass MAX_PATH -1 for in case anyway
            // ignore return value
#endif
    
    DECL void close_file (HANDLE h) {
        auto ret = CloseHandle(h);
        if (ret == 0) {
            GET_FILEPATH_FROM_HANDLE(h);
            warning("close_file::CloseHandle failed for '%' [%]!\n", filepath, GetLastError());
        }
    }
    
    // TODO: should get_file_size have the ability to fail?
    DECL u64 get_file_size (HANDLE h) {
        LARGE_INTEGER li_size;
        auto ret = GetFileSizeEx(h, &li_size);
        // this is technically a signed value, but the compiler is ok with this even without cast for some reason
        //  and it should never actually be nagative so just ignore this
        u64 size = static_cast<u64>(li_size.QuadPart);
        if (ret == 0) {
            GET_FILEPATH_FROM_HANDLE(h);
            warning("get_file_size::GetFileSizeEx failed for '%' [%]!", filepath, GetLastError());
            size = 0;
        }
        return size;
    }
    
    // TODO: should set_filepointer have the ability to fail?
    DECL void set_filepointer (HANDLE h, u64 fp) {
        LARGE_INTEGER li_fp;
        li_fp.QuadPart = static_cast<s64>(fp); // Filesizes will never be 2^31-1 Bytes
        
        auto ret = SetFilePointerEx(h, li_fp, NULL, FILE_BEGIN);
        if (ret == 0) {
            GET_FILEPATH_FROM_HANDLE(h);
            warning("set_filepointer::SetFilePointerEx failed for '%' [%]!", filepath, GetLastError());
        }
    }
    // TODO: should get_filepointer have the ability to fail?
    DECL u64 get_filepointer (HANDLE h) {
        LARGE_INTEGER li_old_fp, li_new_fp;
        li_old_fp.QuadPart = 0;
        
        auto ret = SetFilePointerEx(h, li_old_fp, &li_new_fp, FILE_CURRENT);
        // this is technically a signed value, but the compiler is ok with this even without cast for some reason
        //  and it should never actually be nagative so just ignore this
        u64 fp = static_cast<u64>(li_new_fp.QuadPart);
        if (ret == 0) {
            GET_FILEPATH_FROM_HANDLE(h);
            warning("get_filepointer::SetFilePointerEx failed for '%' [%]!\n", filepath, GetLastError());
            fp = 0;
        }
        return fp;
    }
    
    // TODO: should set_eof_to_fp have the ability to fail?
    DECL void set_eof_to_fp (HANDLE h) {
        auto ret = SetEndOfFile(h);
        if (ret == 0) {
            GET_FILEPATH_FROM_HANDLE(h);
            warning("set_eof_to_fp::SetEndOfFile failed for '%' [%]!\n", filepath, GetLastError());
        }
    }
    
    // TODO: should set_filepointer have the ability to fail?
    DECL void set_filepointer_and_eof (HANDLE h, u64 fp) {
        set_filepointer(h, fp);
        set_eof_to_fp(h);
    }
    
    DECL err_e read_file (HANDLE h, void* data, u64 size) {
        DWORD dw_size = safe_cast_assert(DWORD, size); // TODO: implement 64bit reads by repeating 32bit ReadFile
        
        DWORD read;
        auto ret = ReadFile(h, data, dw_size, &read, NULL);
        if (ret == 0 || read != dw_size) {
            GET_FILEPATH_FROM_HANDLE(h);
            warning("win32::read_file::ReadFile failed for '%' [%], read_size=% size_to_read=%!\n",
                    filepath, GetLastError(), read, dw_size);
            return FILE_READ_FAIL;
        }
        return OK;
    }
    // with asserts disabled it's up to the os if what happens on fails
    DECL void read_file_assert (HANDLE h, void* data, u64 size) {
        auto err = read_file(h, data, size);
        assert(!err);
    }
    
    DECL err_e write_file (HANDLE h, void const* data, u64 size) {
        DWORD dw_size = safe_cast_assert(DWORD, size); // TODO: implement 64bit reads by repeating 32bit ReadFile
        
        DWORD written;
        auto ret = WriteFile(h, data, dw_size, &written, NULL);
        if (ret == 0 || written != dw_size) {
            GET_FILEPATH_FROM_HANDLE(h);
            warning("win32::write_file::WriteFile failed for '%' [%], size=%%!\n",
                    filepath, GetLastError(), size,
                    ret != 0 && written != dw_size ? ", written_size != size_to_write":"");
            return FILE_WRITE_FAIL;
        }
        return OK;
    }
    // with asserts disabled it's up to the os if what happens on fails
    DECL void write_file_assert (HANDLE h, void const* data, u64 size) {
        auto err = write_file(h, data, size);
        assert(!err);
    }
    
    #undef GET_FILEPATH_FROM_HANDLE
}
