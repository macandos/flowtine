# flowtine
a json-style parser library made for C++, used for making simple declerations

## Features

### Declaring variables
To declare variables, you can do: <br />
```
  var name = value; 
```

To access the value in C++, do: <br/>
```cpp
ftn::val("name");
// Output: value
```

### Declaring Arrays
To declare arrays, you can do: <br />
```
arr name = {
   val1 = 1;
   val2 = 2;
   val3 = 3;
};
```
And to access them in C++: <br />
```cpp
// val1 being the var inside the array, and name being the name of the array
ftn::val("val1", "name");
```
