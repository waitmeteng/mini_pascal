PROGRAM test_recursion(input, output, error); 
VAR a, b: INTEGER;
VAR c: REAL;

FUNCTION fact(x:INTEGER) : INTEGER;
BEGIN
    if x = 1 || x = 0 then
        fact := x
    else
        fact := x*fact(x-1)
END;

BEGIN
    printlnString("please enter a integer:");
    b := readInt;
    a := fact(b);
    printString("The factorial is: ");
    printlnInt(a);
END.
