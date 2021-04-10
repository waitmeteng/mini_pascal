program exponential(void);
// calculate e^x
// the algorithm is from https://www.geeksforgeeks.org/program-to-efficiently-calculate-ex/
var i : integer;
function exponential(x: real): real;
var sum, j : real;

begin
    sum := 1.0;
    j := 10.0;
    for i := 10 downto 1 do   // you can change 10 to larger number to get more accuracy
    begin
        sum := 1.0 + x * sum / j;
        j := j - 1.0;
    end;
    exponential := sum;
end;

begin
    printlnReal(exponential(1.0));   // you can change 1.0 to another number to calculate e^(number)
end.
