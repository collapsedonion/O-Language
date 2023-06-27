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

Theres 



