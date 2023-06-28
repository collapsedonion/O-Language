# std.olf

"std.olf" always included in you program by default and implements basic functionality of O language

```

Allocates block of memory of giving size and return pointer to it
func:T malloc(T, int size);

Resizes alredy allocated block of memory to new size
func realloc(void~ pMemory, int size);

Deletes allocated block of memory
func free(T p);

Returns size of allocated memory block
func:int mSize(void~ pMemory);

Returns id of current machine OS
func:int getSysId()

Defines name fo id's that may be returne from "getSysId"
enum{
	MACOS_SYS_ID = 0;
    WINDOWS_SYS_ID = 1;
}

Returns number of clocks passed from program start
func:int clock();

Return time in seconds passed from program start
func:float getTime();

Analog of C "void*"
struct:RPointer{
  Converts O pointer to C pointer
  func:RPointer getRPointer(void~ pointer);

  Set size (1, 2, 4 or 8) bytes of data to size bytes of RPointer
  func setRPointerData(int size, int data);

  Return size (1, 2, 4 or 8) bytes of RPointer
  func:int getRPointerContent(int size);

  Converts RPointer to O pointer
  func:void~ getRelativePointer();
};

Pointer to CRT library
global:DynamicLib c_std_lib_handler;

Returns character is digit or not
func:bool isDigit(char c);
```
