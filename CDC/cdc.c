#include "cdc.h"
#include <stdlib.h>
#include <string.h>
#ifdef __APPLE__
	#include <unistd.h>
	#include <mach-o/dyld.h>
	#include <dlfcn.h>
#endif
#if defined(_WIN32) || defined(__CYGWIN__)
	#include <Windows.h>
#endif


//returns current work directory
char* cdc_get_current_directory(){
#ifdef __APPLE__
	size_t buffer_size = 64;
	char* buf = (char*)malloc(buffer_size);
	while(getcwd(buf, buffer_size) == NULL){
		buffer_size += 64;
		buf = (char*)realloc(buf, buffer_size);
	}
	return buf;
#endif
#ifdef WIN32
	size_t buffer_size = GetCurrentDirectory(0, NULL);
	char* buf = (char*)malloc(buffer_size);
	GetCurrentDirectory(buffer_size, buf);
	return buf;
#endif
}

//returns path to the current running executable
char* cdc_get_executable_directory() {
#ifdef __APPLE__
	uint32_t buffer_size = 64;
	char* buf = (char*)malloc(buffer_size);
	while (_NSGetExecutablePath(buf, &buffer_size) == -1) {
		buffer_size += 64;
		buf = (char*)realloc(buf, buffer_size);
	}
#endif
#if defined(WIN32)
	size_t buffer_size = 0;
	char* buf = (char*)malloc(buffer_size);
	do {
		buffer_size += 64;
		buf = (char*)realloc(buf, buffer_size);
		GetModuleFileName(NULL, buf, buffer_size);
	} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);
#endif
	char* last_symbol = strrchr(buf, cdc_path_separator);
	if (last_symbol == NULL) {
		return buf;
	}
	size_t result_length = last_symbol - buf + 1;
	char* result = (char*)malloc(result_length);
	result = memcpy(result, buf, result_length);
	result[result_length - 1] = '\0';
	free(buf);
	return result;
}

//opens dynamic library, reqires path to library without file extension (will be added automaticly), if path_to_dl equals to "" returns handler to C standart lib
cdc_dynamic_lib_handle cdc_open_dynamic_lib(char* path_to_dl) {
	size_t new_str_len = strlen(path_to_dl);
#ifdef __APPLE__
	new_str_len += 6;
	char* extension = ".dylib";
#elif defined(WIN32)
	new_str_len += 4;
	char* extension = ".dll";
#endif

	char* real_path = 0;
	if (strlen(path_to_dl) != 0) {
		real_path = (char*)malloc(new_str_len + 1);
		real_path = memcpy(real_path, path_to_dl, strlen(path_to_dl) + 1);
		real_path = strcat(real_path, extension);
	}
#ifdef __APPLE__
	void* handler = dlopen(real_path, RTLD_NOW);
#elif defined(WIN32)
	HINSTANCE handler;
	if (real_path == 0) {
		handler = GetModuleHandle("ucrtbase.dll");
		if (handler == 0) {
			handler = GetModuleHandle("ucrtbased.dll");
		}
	}
	else {
		handler = LoadLibrary(real_path);
	}
#endif
	if(real_path != 0){	
		free(real_path);
	}
	return (cdc_dynamic_lib_handle)(handler);
}

//extracts pointer to symbol at dynamic library
void* cdc_get_dynamic_lib_member(cdc_dynamic_lib_handle dl_handle, char* member_name){
#ifdef __APPLE__
	return dlsym(dl_handle, member_name);
#elif defined(__CYGWIN__) || defined(WIN32)
	return GetProcAddress(dl_handle, member_name);
#endif
}

//close dynamic library
void cdc_close_dynamic_lib(cdc_dynamic_lib_handle dl_handle){
#ifdef __APPLE__
	dlclose(dl_handle);
#elif defined(WIN32)
	FreeLibrary(dl_handle);
#endif
}


