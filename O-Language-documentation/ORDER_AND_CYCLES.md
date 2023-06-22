# Program order structures

In O language exists 3 order structures and 1 cycle structure

## Order structures

### IF
"if" blocks excepts boolean value, and if it's "true" evaluates it's "body", if "false" gives control to next else block

Description
```
if([value (bool)]){
  [body]
}
```
Example
```
if(true){
  var:int a;
  var:bool b;
}
```
### ELSE
"else" block evaluates if previous if skiped

Description
```
^if^
else{
 [body]
}
```
Example
```
if(false){
  var:int c;
}
else{
  var:int a;
}
```

### ELSE IF
Behave as "else", but with additional condition

Description
```
^if^ or ^else if^
else if([value (bool)]){
  [body]
}
```
Example
```
var:bool c = true;
if(c){
  var:int a;
}else if(false){
  var:int b;
}
```
### WHILE

"while" is a cycle that will evaluate it's body all over again while it's argument is true

Description
```
while([value (bool)]){
  [body]
}
```
Example
```
var:int a = 0;

while(a < 2){
  @a = a + 1;
}
```


