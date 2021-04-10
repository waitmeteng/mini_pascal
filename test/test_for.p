PROGRAM test_for(void);
// my test for for loop.
// attention: the values of for loop can only be numbers, e.g. for i:= 1 to 10 
var a: array [ 1 .. 10 ] of array [ 1 .. 10 ] of integer;
var i, j: integer;

begin
    for i:= 1 to 10 do
    begin
        for j:= 1 to 10 do
        begin
            a[i][j] := i + j;
            printString(" ");
            printInt(a[i][j]);
        end;
        printlnString("");
    end
    
end.
