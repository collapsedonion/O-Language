# Structers

Structers is the way to organise several any-type variables

## Definition

To define structure "struct" key-word is used

```
struct:<name>{
  [body]
}
```

### Example

```
struct:ExampleStruct{
  
}
```

After creating structure will be generated new data-type with name of structure

Structure stored as pointer to it at memory

## Value instances

Structure can have instance of every defined datat-type

```
[data-type] <name>
```

### Example 
```
struct:ExampleStruct{
  int ivl;
  bool bvl;
}
```
For geting access to any of structure members "[]" operator is used

```
<struct-name>[<member-name>]
```

### Example
```
var:ExampleStruct es;
@es[ivl] = 125;
var:bool bl = es[bvl];
```

## Function and Methods

Theres two way to add functionality for structures, it is methods and inner functions

Inner function is stored as simple functions, and called according to variable data-type
Methods called according to pointer saved at structure
For accesing structure data from method or function "me" name used

```
%method%
[method [[data-type] <name>()]]{
  [body]
}

%inner function%
func:[data-type] <name>(){
  [body]
}
```

### Example
```
struct:EStruct{
  bool a;

  int c;

  func:bool getA(){
    return(me[]a);
  }

  [method [void addC(int a)]]{
    @me[c] += a;
  }
}
```

To call function or method "[]" operator is used

### Definition
```
[<name> <member>()]
```

### Example
```
var:EStruct ts;
[ts addC(12)];
```

To create instance of structure auto-created "alloc" and "init" functions is used

### Example
```
var:EStruct ts = [[ts alloc()] init()];
```

Also you can create you own "init" function, at that case default "init" will be deleted

### Definition
```
init(){
  [body]
}
```

### Example
```
struct:EStruct{
  int a;

  init(int ia){
    @me[a] = ia;
  }
}
```

## Inerhiting structures

Inerhiting helps to add new properties and functions/methods to alredy existing structures, at that case new structures will be able to casted as parent structure and wil have all members of parent structure

To inerhit sturcture you need to add "extends:<name>" at top of structure

### Example

```
struct:Parent{
  int a;
}

struct:Child{
  extends:Parent;
  int b;
}
```

Also there is "super" keyword, to call parent "init" before new "init", "super" must be used right before new "init" definition

### Example
```
struct:Parent{
  int a;
  init(int a){
    @me[a] = a;
  }
}

struct:Child{
  [super(12)];
  init(){
  }
}
```

# Templates

Also structure can be templated, it is helps to write definiton of functions and methods for using with different types

To create template you should use "universal" right before "struct"

### Example
```
[universal [def T]]
struct:ES{
  T a;

  [method [T getA]]{
    return(me[a]);
  }
}
```

To create instance of template use "<Template-name>\[\[data-type\]\]" at data-type field

### Example

```
var:ES[char] ti = [[ti alloc()] init()];
```

# Enumerations

Enumerations creates named placeholders for any value type

## Definition
```
enum{
    <name> = <value>;
}
```

### Example

```
enum{
    INTE = 1;
    BOOLE = false;
    FLOATE = 1.2;
}
```
