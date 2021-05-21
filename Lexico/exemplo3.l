int i;
int r := -10;
final d = -10;
char a[10], b[10];
main {
    writeln(r, " ", d);
    for(i:=0;i<10;i:=i+1){
        writeln("ninguem tem contato de ninguem?");
        readln(b);
        if(b = "nao") then writeln("ah me engana que eu gosto");
        else if (b = "tem") then writeln("ok...");
    }
}