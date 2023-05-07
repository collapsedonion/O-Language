#ifndef _CDC
#define _CDC

#ifdef __APPLE__
#define cdc_path_separator '/'

#ifdef __cplusplus
    #define DL_EXTERN extern "C"
#else
    #define DL_EXTERN extern
#endif
#elif defined(_WIN32) || defined(__CYGWIN__)
#define cdc_path_separator '\\'

#ifdef __cplusplus
    #define DL_EXTERN extern "C" __declspec(dllexport)
#else
    #define DL_EXTERN extern __declspec(dllexport)
#endif
#endif

typedef void* cdc_dynamic_lib_handle;

//returns current work directory
DL_EXTERN char* cdc_get_current_directory();
//returns path to the current running executable
DL_EXTERN char* cdc_get_executable_directory();
//opens dynamic library, reqires path to library without file extension (will be added automaticly), if path_to_dl equals to "" returns handler to C standart lib
DL_EXTERN cdc_dynamic_lib_handle cdc_open_dynamic_lib(char* path_to_dl);
//extracts pointer to symbol at dynamic library
DL_EXTERN void* cdc_get_dynamic_lib_member(cdc_dynamic_lib_handle dl_handle, char* member_name);
//close dynamic library
DL_EXTERN void cdc_close_dynamic_lib(cdc_dynamic_lib_handle dl_handle);
//set's console cursor position
DL_EXTERN void cdc_set_cursor(int x, int y);
//clears console
DL_EXTERN void cdc_clear_console();
//return handler current module
DL_EXTERN cdc_dynamic_lib_handle cdc_get_current_module();


//call function from pointer, with arg_num count of arguments, arguments taken from arguments array, according to datatype in arg_type null terminated string, supported datatypes: c-byte s-2 bytes i-4 bytes l-8 bytes
DL_EXTERN long cdc_invoke(void* pointer, int arg_num, long long* arguments, char* arg_type);
    
#endif