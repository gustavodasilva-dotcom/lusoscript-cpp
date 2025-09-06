# Grammar

The work on this document is ongoing. Even though it's a mess now, things will get better.

---

LusoScript's formal grammar specification is as follows:

| Nonterminal | Rule |
|-------------|------|
| program	  | → ( *declaration* )* **EOF** ; |
| declaration | → *varDecl* \| *statement* ; |
| statement	  | → *exprStmt* \| *ifStmt* \| *imprimaStmt* \| *block* ; |
| ifStmt	  | → `if` `(` *expression* `)` *statement* ( `else` *statement* )? ; |
| block		  | → `{` + ( *declaration* )* + `}` ; |
| varDecl	  | → `var` **IDENTIFIER** ( `=` *expression* )? `;` ; |
| exprStmt	  | → *expression* `;` ; |
| imprimaStmt | → `imprima` + `(` + *expression* + `)` `;` ; |
| expression  | → *comma* ; |
| comma		  | → *assignment* ( `,` *assignment* )* ; |
| assignment  | → **IDENTIFIER** `=` *assigment* \| *ternary* ; |
| ternary	  | → *logic_or* ( `?` *expression* `:` *ternary* )? ; |
| logic_or	  | → *logic_and* ( `ou` *logic_and* )* ; |
| logic_and	  | → *equality* ( `e` *equality* )* ; |
| equality    | → *comparison* ( ( `!=` \| `==` ) *comparison* )* ; |
| comparison  | → *term* ( ( `>` \| `>=` \| `<` \| `<=` ) *term* )* ; |
| term        | → *factor* ( ( `-` \| `+` ) *factor* )* ; |
| factor      | → *unary* ( ( `/` \| `*` ) *unary* )* ; |
| unary       | → ( `!` \| `-` ) *unary* \| *primary* ; |
| primary     | → **NUMBER** \| **STRING** \| `verdadeiro` \| `falso` \| `nulo` \| `(` *expression* `)` \| **IDENTIFIER** ; |

(*varDecl* are declaration statements. A declaration is not restricted to a variable; it can be a function declaration, a class declaration etc.)

## Comments

LusoScript supports multiline strings:

```
var greeting = "Hello world!
	This is my first LusoScript program.";
```

Just, please, don't forget to enclose the text with double quotes for it to be a valid string:

```
// :no_entry_sign: not allowed
var gretting = "Hello world!
	This is my first LusoScript program.;
```

## Data types

### Booleans
_True_ is expressed through the keyword `verdadeiro`; _false_, by `falso`.

```
var bool_true = verdadeiro;
var bool_false = falso;
```

### Numbers
For now, every number in LusoScript is a floating-point number since they cover a range of integers.

```
var address_num = 150;
var weather_in_c = 26.7;
```

### String
A string is represented by enclosing double quotes around a text:

```
var text = "A string literal.";
```
### Null
LusoScript has support for a value that represents "no value." `nulo` is our own version of the well-known _null_:

```
var no_value = nulo;
```

### Precedence and associativity

The rules established by C are adhered to by LusoScript, as illustrated in the table below, with comma having the lowest precedence and unary the highest:

| Name       | Operators | Associates |
|------------|-----------|------------|
| Comma		 | ,		 | Left		  |
| Equality   | == !=     | Left       |
| Comparison | > >= < <= | Left       |
| Term       | - +       | Left       |
| Factor     | / *       | Left       |
| Unary      | ! -       | Right      |

_Extracted from "Crafting Interpreters" by Robert Nystrom_

The precedence can be changed by using parentheses:

```
var c = (4 + 2) / 2;
```

## Expressions

### Arithmetic

```
1 + 1;
2 - 1;
1 * 1;
2 / 1;
```

```
var num = 10;
num = -10;
```

All the arithmetic operations, for now, are only supported in numbers. Concatenating strings with the plus sign (`+`) results in errors:

```
var first_name = "John";
var last_name = "Doe";
var full_name = first_name + last_name; // produces an error
```

### Comparison and equality

```
1 < 2;
1 <= 1;
2 > 1;
1 >= 1;
```

```
1 == 2;
2 != 1;
```

### Logical operators

```
!verdadeiro; // false
!falso; // true
```

```
verdadeiro e false; // false
verdadeiro e verdadeiro; // true
```

```
falso ou falso; // false
verdadeiro ou falso; // true
```

## Statements

Like the previous code snippets implicitly stated, LusoScript requires a semicolon (`;`) at the end of each statement. So, expressions followed by a semicolon are statements. Expressions can be grouped in blocks using a pair of curlies:

```
{
	var first_name = "John";
	var last_name = "Doe";
}
```

Like in most languages, blocks impact the scope.

## Variables

Most programming languages allow variable declaration using the keyword _var_. In static typed languages, like C#, this keyword is used for implicitly typed variable declaration. Since the translation of "variable" in Portuguese is _variável_, LusoScript also uses _var_ for declaring variables. Unlike other scripting languages, such as JavaScript, LusoScript requires the keyword before the variable identifier, like the following example:

```
var greeting = "Hello world!"; // :thumbsup: correct usage

another_greeting = "Hello again!"; // :no_entry_sign: not allowed
```

Since LusoScript is not static typed, you can change a variable's type by changing its value:

```
var version = "1.0"; // a string
version = 1.0; // a number
```

## Control flow

```
se (condition) {
	// first condition
} senao {
	// second condition
}
```

```
enquanto (true) {
	// code...
}
```

## Functions

```
funcao print_name(first_name, second_name) {
	// code...
}

print_name("John", "Doe");
```

## Classes

```
classe Person {
	
}
```

---

This work would not be possible without the help of the book *Crafting Interpreters* by Robert Nystrom.
