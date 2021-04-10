program test_all(void);

var a, b, c, x, y, i, j : integer;
//var a : integer; // 1. test duplicated declarations.
var f, g, h : real;
var d: array [ 1 .. 10 ] of integer; 
var dd: array [ 1 .. 10 ] of array [ 1 .. 10 ] of integer;

FUNCTION ss(rr :INTEGER) : INTEGER;
BEGIN
  ss := rr
END;

begin
// 23. test printInt, printReal and printString.
printlnString("Welcome to Mini Pascal world!");
printString("This is no new line:");
printInt(a);   // I implicitly initialize integer and real types be zeros.
printlnInt(b);
printReal(f);
printlnReal(h);
// 2. 3. 4. 5. test addition, substraction, multiplication and division operations for both integer and real types.
printlnInt(1+2);
printlnInt(1-2);
printlnInt(3*8);
printlnInt(4/2);
printlnReal(1.4+3.3);
printlnReal(1.4-3.3);
printlnReal(1.4*3.3);
printlnReal(1.4/3.3);
// 10. test constant expressions.
printlnInt(2+5*4/2);
printlnReal((3.2+4.8)*2.0/4.5-2.4);
// 11. test assignment statements of simple types (integer and real).
a := 2;
b := 3+4-2*8/3;
f := 2.5;
h := 2.2/1.1-5.5+(3.2-2.1)*4.5;
printlnInt(a);
printlnInt(b);
printlnReal(f);
printlnReal(h);
// 24. test constant folding of various degrees of difficulty.
c := a + 10 * (b + 2)/2 - 7;
h := f * (h - 0.45)/0.5 + 2.3;
printlnInt(c);
printlnReal(h);
// 6. 7. 16. 17. test comparision operations and simple if statement and simple while loop.
b := 0;
if b > a then a := 111 else a := 222;
printlnInt(a);
if b < a then a := 111 else a := 222;
printlnInt(a);
if b = a then a := 111 else a := 222;
printlnInt(a);
if b >= a then a := 111 else a := 222;
printlnInt(a);
if b <= a then a := 111 else a := 222;
printlnInt(a);
if b != 0 then a := 111 else a := 222;
printlnInt(a);

a := 1;
while a < 5 do
begin
 printInt(a);printString(" ");
 a   := a + 1 
end;
printlnString("");
// test arithmetic + comparision operations
if (a - 1)*10 = 0 + 0 then
    printlnString("Win")
else
    printlnString("Lose");

// 18. 19 test nested if statements and while loops
b := 1;
printlnString("Please enter two integers:"); 
x := readInt;
y := readInt;
if x >= 0 && x <= 7 then    // 44. test && logical operators
begin
    if y >= 7 || y <= 0 then   // 44. test || logical operators
    begin
        while b < 11 do
        begin  
            d[b] := b + b * (b + 3);  // 12. test assignment of 1-dimensional arrays int
            b := b + 1
        end
    end
    else
        printlnString("y is out of range!");
end
else
    printlnString("x is out of range!");
printlnInt(d[4]);

//d[0] := 1;  // 14. test out-of-bound array indices at compile times
//d[20] := 2;

// 8. test arithmetic + comparision operations for variables of one-dimension arrays
if d[x + 3] + 3 - a > d[x/2+4] - 2 then
begin
     printlnInt(d[x + 3] + 3);
     printlnInt(d[x/2+4] - 2);
end
else
     printlnInt(d[2]);

// 13. 45. test assignment of multi-dimension arrays
//         and for loops.
for i:= 1 to 10 do
begin
    for j:= 1 to 10 do
    begin
        dd[i][j] := i + j;
        printString(" ");
        printInt(dd[i][j]);
    end;
    printlnString("");
end;

//46. test repeat-until loops
a := 15;
b := 10;
repeat
begin
    printlnInt(a);
    a := a - 1;
end
until a < b;

// 9. test arithmetic + comparision operations for variables of multi-dimension arrays
a := dd[3][3] + dd[4][5] - 3 * (dd[2][10] - 5);
printlnInt(dd[3][3] + dd[4][5] - 3 * (dd[2][10] - 5));
printlnInt(a);
if (dd[2][5] - 5) * dd[3][8] - d[10] < dd[1][5] then
begin
    printlnInt((dd[2][5] - 5) * dd[3][8] - d[10]);
    printlnInt(d[10]);
end
else
    printlnInt(dd[1][5]); 
d[1] := 1;
d[2] := 2;  
dd[ss(dd[ss(1)][ss(2)])][dd[ss(d[1])][d[2]]] := d[ss(d[ss(d[1])])];
printlnInt(dd[3][3]);

// 20. 21. 22. test : please see test_function.p, test_procedure.p and test_recursion.p files
// For further test, please refer to exponential.p, fibonacci_recursive.p, gcd.p, knigh_tour.p, knight_tour_Warnsdorff.p, power.p and qsort.p files
end.
