# boddy
a toy c compailer
- 2022-03-08
  - Compailer code `3 + 8 - 21 * 42 / 7`
- 2022-03-11
  - Code Generate Support Sample Arithmetic expression to asm
    to run with instruct `make program code="1 + 9 * 3 /2 "`
- 2022-03-12
  - Support assign express  ` make program code="ab = 2;c = ab + 3; d = c + 2 / 2; d * 3 /2;"`
- 2022-03-13
    - Support mod express  ` make program code="ab = 7 % 4;"`
- 2022-03-14 
    - Support Equal,NotEqual,Greater,GreaterEqual,Lesser,LesserEqual express  
      - ` make program code="a = 7 ; a > 1; a < 1;"`
      - ` make program code="a = 6 ; a < 7; a > 0 ;"`
      - ` make program code="a = 5 ; a == 5; a != 0;"`
      - ` make program code="a = 5 ; a != 5 ; a == 1;"`
      - ` make program code="a = 5 ; a >= 1;a <= 0;"`
      - ` make program code="a = 5 ; a <= 6; a >= 1;"`
- 2022-03-15
  - Support if Statements blockStatement `if(){}els{}` while statement `while(){}`
    - `make program code="a = 5; if (a < 7){b =3;b + 1;b * 2;}else {b  = 5;}"`
    - `make program code="a = 5;b = 0; while (a < 700){a = a + 1;b = b + 1;}b;"`
- 2022-03-16
    - Support for dowhile Statement `do{}while{}` while statement `while(){}`
        - `make program code="a = 0;b = 0; for(a = 1;a <= 10; a = a + 1) {b= b +1;}b;"`
        - `make program code="a = 5;b = 0; while (a < 700){a = a + 1;b = b + 1;}b;"`
- 2022-03-17
    - Support for function Statement `func main(){}`
        - `make program code="func main(c) {a = 0;b = 0; for(a = 1;a <= 10; a = a + 1) {b= b +1;}b;}"`
- 2022-03-18 ～ 2022-03-19
    - Support Recursion Fcuntion and  Function call
        - `make program code="func fib(n) { if(n <=1){return 1;}else{ return fib(n -1)  + fib(n-2);}} func prog(){ fib(8);}"`
- 2022-03-20 ～ 2022-03-21
    - Support Int Type Define Such as int a = 1 | int a,b,c = 1; 
        - `"int sum(int x,int m,int j){return x + m + j;}int fib(int n) { if(n <=1){return 1;}else{ return fib(n -1) + fib(n-2);}} int prog(){int a,b,c = 1;int d = sum(a,b,c);  int m = fib(5); return d + m;}"`
- 2022-03-21～ 2022-03-23
  - Support test case and StmtExprNode funcA({statement*}) 
    - `prog({int i = 1;}) int funca(int n){ return n + 1;}`
- 2022-03-23～ 2022-03-27
    - Support test case and annotation  pointer type int a = 1; int b = &a;
    - Support Arithmetic expression priority 
        - `make testCode`
- 2022-03-28 ~ 2022-03-29
    - Support pointr add *(y+2) pointer sub *(y-2)  Pointerdiff  calc  (a  - b)  and support sizeof express can get the 
    - size of VarNode
- 2022-03-30
    - Support pointr array int a[3];
- 2022-03-31 
    - Support array index a[3] ,3[a];  
- 2022-04-01 ~ 2022-04-02
      - Support char short long  type case : `int a = 0 ;char a = 0; short a = 0; long a = 0; ` 
- 2022-04-03 ~ 2022-04-05
    - Support char struct ,scope  case : `struct{int a = 0 ;char a = 0; short a = 0; long a = 0;}a; a[0] = 1;a[1] = 2;a[2] =3;a[0]; ` 
- 2022-04-06 ~ 2022-04-07
    - Support char struct member ->   case : `assert(5,({struct Stu {char *name;char group;int num;int age;} stu; struct Stu *s = &stu;s->name = 5;s->group = 5;s->name=&s->group;s->age=8;*s->name;}));` 