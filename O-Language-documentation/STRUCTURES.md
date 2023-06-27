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

