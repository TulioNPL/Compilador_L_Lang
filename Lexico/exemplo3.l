int a := 1;
int b, c := 3;
int d[4000];

char e := 'g';
char f, g := 'g';
char h[4000];
char z[20];

boolean i := TRUE;
boolean j, k:= FALSE;
boolean l[2000];

main {
   /* d[0] := a * c;
    z := "aaa";
    h := z; 
    i := a < b;
    j := h = "tulio kkk xd 123"; */

    /* ints */
    i := a = b;
    i := d[0] = b;
    i := a = d[0];
    i := d[0] = d[1];

    a := a + b;
    a := d[0] + b;
    a := a + b[0];
    a := d[0] + d[1];

    a := a * b;
    a := d[0] * b;
    a := a * b[0];
    a := d[0] * d[1];

    /* 
    erros  
    a := d * a;
    a := a * d;
    a := d + a;
    a := a + d;
    a := d = a;
    a := a = d;
    */
    
    /* chars */
}



/* doigenes aqui ->
 utlio aqui ->

int int
int[] int
int int[]
int[] int[]

char char
char[] char
char char[]
char[] char[]

bool bool
bool[] bool
bool bool[]
bool[] bool[] */