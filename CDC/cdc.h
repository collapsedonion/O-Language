#ifndef _CDC
#define _CDC

#ifdef __APPLE__
#define cdc_path_separator '/'

#ifdef __cplusplus
	#define DL_EXTERN extern "C"
#else
	#define DL_EXTERN extern
#endif
#elif defined(__CYGWIN__)
#define cdc_path_separator '\\'

#ifdef __cplusplus
	#define DL_EXTERN extern "C" __declspec(dllexport)
#else
	#define DL_EXTERN extern __declspec(dllexport)
#endif
#endif

typedef void* cdc_dynamic_lib_handle; 
#ifdef __cplusplus
extern "C"{
#endif

//returns current work directory
char* cdc_get_current_directory();
//returns path to the current running executable
char* cdc_get_executable_directory();
//opens dynamic library, reqires path to library without file extension (will be added automaticly), if path_to_dl equals to "" returns handler to C standart lib
cdc_dynamic_lib_handle cdc_open_dynamic_lib(char* path_to_dl);
//extracts pointer to symbol at dynamic library
void* cdc_get_dynamic_lib_member(cdc_dynamic_lib_handle dl_handle, char* member_name);
//close dynamic library
void cdc_close_dynamic_lib(cdc_dynamic_lib_handle dl_handle);

#ifdef __cplusplus
}
#endif

//call function from pointer, with arg_num count of arguments, arguments taken from arguments array, according to datatype in arg_type null terminated string, supported datatypes: c-byte s-2 bytes i-4 bytes l-8 bytes
#ifdef __cplusplus
	extern "C" long cdc_invoke(void* pointer, int arg_num, long long* arguments, char* arg_type);
#else
	extern long cdc_invoke(void* pointer, int arg_num, long long* arguments, char* arg_type);
#endif
	
#endif
