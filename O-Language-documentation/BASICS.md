# Basics of O language

## Program Structure

In O language all instructions must end with ";"

### Example
    someFunction();
    
In O language theres no entry point like main function, evaluating starts from top of the file

## Data-types
All datatypes have 8 byte size

In total there is 6 basic data-types

+ bool
+ int
+ float
+ void
+ char
+ pointer

All types non auto-convertable from start

## Literals
O language provides several literal types:

+ (int type) 0, 1, 2, 3, ... or 0i, 1i, 2i, 3i, 4i, ...
+ (bool type) false of true
+ (float type) 0.0, 0.1, 0.2, ... or 0f, 1f, 2f
+ (char type) 'a', 'b', 'c', ... or 0c, 1c, 2c, ... (with 'c' postfix literal return symbol with value of number)
+ (pre-defined array) \[1,2,3, ...\] (allocated space cannot be freed, when code execution hits that literal all previous data in it vanishing)
+ (c-style string) "123abc" (pre-defined array, null-terminated, char-pointer type) 

## Documentation place holders

+ \[data-type\] (place-holder for data-type name)
+ <name> (place-holder for any-text)
+ \[body\] (place-holder for instructions)

## Naming rules

1. All names should use latin alphabet
2. Name should start with letter
3. Allowed symbols: 0-9, A-Z, a-z, -, \_ 
