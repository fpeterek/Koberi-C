# Kobeři-C
## What is Kobeři-C
Kobeři-C is a language with Lisp-like syntax, which compiles to pure C. 

## Why Kobeři-C
Kobeři-C was built with modern features in mind. It's goal is to introduce useful modern features to the world of C, while keeping the performance on par with pure C. Kobeři-C achieves this by compiling straight to pure C.

## Features
### Simplified syntax
The syntax of Kobeři-C is simplified. Lisp-like syntax may be confusing for people coming from other languages, but in reality it's really simple. Class definition and function definition syntax doesn't differ in any way. This makes learning the syntax a piece of cake, as it follows the same guidelines for every possible statement.
### Function overloading
Kobeři-C finally introduces this well-known feature from languages like C++. While function overloading isn't such an important feature, it certainly allows the programmer to use more readable names for their functions. No more abs, fabs, fabsf.
### Classes with methods and inheritance
Yet another feature introduced in Kobeři-C is also well known from other languages. Classes. With methods, which accept a reference to an object. And although Kobeři-C doesn't support polymorphism, it does support inheritance. Create a new class and inherit all attributes and methods from an already existing class.
### Syntactic sugar
#### Print
Printing to stdout has gotten much easier and readable. No more %d's and %f's. No more puts() vs. printf(). Just call (print) and pass in everything you want to write to stdout as a parameter. Eg.: 
```(print "2 + 2 = " (+ 2 2) "\n" "x! = " (factorial x) "\n" "x = " x "\n")```
#### Comparison operators
Comparing numbers has also been simplified. No more ```(a == b && a == c && c == d)```. No more ```(a < b && b < c)```. Chain them together. Type ```(equals a b c d)``` or ```(< a b c)``` and get the exact same result.

