# Function

Function is reusable instruction combintatio, that can return value an accept different arguments

## Definition

```
func:[data-type]%return type% <name>([value ([data-type]) ... %arguments%){
  [body]
}
```
### Example

```
func:int add(int a, int b){
  return(a+b);
}
func:bool isDigit(char a){
  [body]
}
```

Function can be used as value type

## Function calling
```
<name>(...%args%)
```
### Example
```
var:int a = add(2,3);
var:bool b = isDigit('1');
```
