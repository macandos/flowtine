# flowtine
a json-style parser library made for C++, used for making simple declerations

## Features

### Declaring variables
To declare variables: <br />
```
  name = value
```

C++: <br/>
```cpp
Flowtine ftn("file");
ftn.get("name")
```

### Declaring Arrays
To declare arrays: <br />
```
name = {
   nestedarray = {
      hi = yay
   }
   val1 = 1
   val2 = 2
   val3 = 3
}
```
C++: <br />
```cpp
Flowtine ftn("file");
ftn.get("name.nestedarray.hi");
```
