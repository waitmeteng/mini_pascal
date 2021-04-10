PROGRAM test_repeat(void);
// my test for repeat-until loop
var a, b : integer;

begin
    printlnString("please enter a number:");
    a := readInt;
    b := 10;
    repeat
    begin
        printlnInt(a);
        a := a - 1;
    end
    until a < b;
end.
