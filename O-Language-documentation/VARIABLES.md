#  Variables

## Basic info

Variable can be every of existing type, data for variable allocated at the stack, also variable can be in the role of value

## Variable definition

To define variable must be used "var" or "global" compiler instruction

### VAR prototype
 ```
 var:[data-type] <name>
 ```
 
### Example
    
    var:bool boolTypeVar;
       
    var:int intTypeVar;
        
    var:float floatTypeVar;

### GLOBAL prototype

```
Can be accesed from any function and using it in function is prohibited
global:[data-type] <name>
```

### Example

```
global:bool boolGlobal;
global:int intGlobal;
```

Also variable can have initialisation data

### Description
```
^var^ = [value ([data-type])]
```

### Example
```
var:bool bi = false;
var:int ii = 1231;
var:float fi = 1.1f;
```

Also variable to variable can be asigned new value of same type

### Description
```
@<name> = [value ([data-type])]
```
### Example
```
var:int a = 1;
var:int b = 2;
@a = 12;
@b = a;
```

