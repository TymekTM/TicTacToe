# Tic Tac Toe

This is somehow probably the shortest ever written functioning version of Tic Tac Toe.

In Poland we call 'Tic Tac Toe' 'Kółko i Krzyżyk' which translates to 'Circle and Cross'

This is Tic Tac Toe code shaped as such symbols:

```cpp
             char b[]
           ="123\n456\n"
         "789";         int
       main()           {for(
      int m,                p=
      88;;                 p^=
      23){               puts(
      b);R:              scanf
       ("%d",            &m);
         if(          --m<0
           |m>8|b[m+m/3]
              &64)goto
  R;b[m+m/                  3]=p;
    for(auto             l="012"
      "4568"            "9:04"
        "8159"      "26:05:"
            "258";*l;l+=
              3)if((b[
           *l-48]  &b[l[1
         ]-48]&      b[l[2]
      -48])==p)         return
    !puts(b);                 }}
```

And this is unformated C++ Code:
```cpp
char b[]="123\n456\n789";int main(){for(int m,p=88;;p^=23){puts(b);R:scanf("%d",&m);if(--m<0|m>8|b[m+m/3]&64)goto R;b[m+m/3]=p;for(auto l="01245689:04815926:05:258";*l;l+=3)if((b[*l-48]&b[l[1]-48]&b[l[2]-48])==p)return!puts(b);}}
```

Big thanks to Claude Opus 4.6 for quick 30 minutes coding adventure!