# Pointers

Pointer is the way to edit one variable from it's instances

To define pointer type value "~" opreator is used at end of data-type

## Definition

```
[data-type]~
```
### Example

```
var:int~ a;
var:bool~ b;
```
To get pointer to variable "@" operator is used

## Definition

```
@<name>
```

### Example

```
var:int a = 12;
var:int~ b = @a;
```

To get or set value of pointer "~" operator is used

## Definition

```
~<name>
```
### Example

```
var:int a = 123;
var:int~ b = @a;
b = 99;
var:int c = ~b;
```

Also pointers used to hold arrays, to access to member of array "[]" operator is used

## Definiton

```
<name>[[value (int)]]
```

### Example

```
var:int~ array = [1,1,2,3,5,8,13];
var:int a = array[3];
@array[0] = 12;
```
# Function pointers

Function pointers quite similar to simple pointer, but with some differences

## Type Definition

```
[[data-type]%return type%](...%argument types%)
```

### Example
```
var:[int](int,int) toOpInt;
```

## Getting pointer

```
@<name>(...%arguments%)
```

### Example

```
func:int add(int a, int b){
  return(a+b);
}

var:[int](int,int) a = @add(int, int);
```

Calling function by pointer is the same as simple function

