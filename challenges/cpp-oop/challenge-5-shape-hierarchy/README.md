# Challenge 5: Shape Hierarchy with Virtual Functions

## Difficulty: Hard | Points: 150 | Topic: Polymorphism & Inheritance

### Problem Statement
Design an abstract base class `Shape` with pure virtual methods:
- `virtual double area() const = 0;`
- `virtual std::string name() const = 0;`

Derive:
- `Circle` (takes radius `r`, area = `3.14159 * r * r`)
- `Rectangle` (takes width `w` and height `h`, area = `w * h`)

Input format:
Line 1: N (number of shapes)
Next N lines:
- `circle <radius>`
- `rectangle <width> <height>`

Output format:
For each shape, print: `<Name>: <Area rounded to 2 decimal places>`
