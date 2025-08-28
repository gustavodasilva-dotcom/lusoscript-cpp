# Language grammar

## Variable declaration

Most programming languages allow variable declaration using the keyword _var_. In static typed languages, like C#, this keyword is used for implicitly typed variable declaration. Since the translation of "variable" in Portuguese is _variável_, LusoScript also uses _var_ for declaring variables. Unlike other scripting languages, such as JavaScript, LusoScript requires the keyword before the variable identifier, like the following example:

```
var greeting = "Hello world!"; // :thumbsup: correct usage

another_greeting = "Hello again!"; // :no_entry_sign: not allowed
```

LusoScript supports multi-line strings:

```
var greeting = "Hello world!
	This is my first LusoScript program.";
```

Just, please, don't forget to enclose the text with double quotes for it to be a valid string:

```
// :no_entry_sign: not allowed
var gretting = "Hello world!
	This is my first LusoScript program.
```
