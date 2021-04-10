program gcd(void);
// calculate greatest common factor of two numbers using euclidean algorithm
var m, n, temp, r : integer;
begin
    printlnString("Please input two integers:");
    m := readInt;
    n := readInt;
    repeat
    begin
        if m < n then
        begin
            temp := m;
            m := n;
            n := temp;
        end
        else begin end;
        r := m - (m/n)*n;
        m := n;
        n := r;
    end
    until r = 0;
    printlnString("The gcd is:");
    printlnInt(m);
end.
