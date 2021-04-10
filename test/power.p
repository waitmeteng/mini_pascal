program power(void);
// calculate x^y
// see https://www.geeksforgeeks.org/write-a-c-program-to-calculate-powxn/
var x, y : integer;

function power(x, y : integer):integer;
begin
    if y = 0 then
        power := 1
    else
    begin
        if (y/2)*2 = y then
            power := power(x, y/2) * power(x, y/2)
        else
            power := x * power(x, y/2) * power(x, y/2);
    end
end;

begin
    printlnString("Please input two integers:");
    x := readInt;
    y := readInt;
    printInt(x);
    printString("^");
    printInt(y);
    printString("=");
    printlnInt(power(x, y));
end.
