PROGRAM test_andor(void);
// my test for && and || operators.
// attention: can't deal (...) && (...) && (...) yet.
var a, b, c: integer;

begin
    a := readInt;
    b := readInt;
    if a = 1 && b = 3 then
        printlnString("Win")
    else
        printlnString("Lose");
    if a = 1 || b = 3 then
        printlnString("Win")
    else
        printlnString("Lose");
end.
