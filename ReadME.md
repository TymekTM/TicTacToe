# Tic Tac Toe

An extremely short, fully functioning Tic Tac Toe.

In Poland we call it **"Kółko i Krzyżyk"** — Circle and Cross.

### Previous version — code shaped as Kółko i Krzyżyk (O and X)

This was the original 229-char C++ version, with the code itself shaped as those two symbols — a circle (O) and a cross (X). It has since been beaten: scroll down for the **even shorter 211-char** version.

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
       -48])==         p)return
     !puts(b);                 }}
```

## The code (211 chars, C)

```c
char b[]="123\n456\n789";p=79;char*l;main(c){puts(b);for(l="01245689:04815926:05:258";*l;l+=3)if((b[*l-48]&b[l[1]-48]&b[l[2]-48])==p)return;p^=23;do c=getchar()-49;while(c<0|c>8|b[c+c/3]&64);b[c+c/3]=p;main(c);}
```

Compile and run:

```
gcc -std=gnu89 tictactoe.c
./a.out
```

`-std=gnu89` enables the implicit-int globals and K&R `main` that make it this short.

## How it works

- **The board is the display.** `b = "123\n456\n789"` is both the state and what `puts` prints each turn, so the grid is drawn for free.
- **One toggle for both players.** `p^=23` flips `'O'(79)` ↔ `'X'(88)` (since `79 ^ 88 == 23`). It starts at `79`, so the first toggle makes `X` move first.
- **Free empty-cell check.** Digits `'1'..'9'` (49–57) have bit 6 clear, while `X`/`O` have it set, so `b[i]&64` is true exactly on taken cells.
- **Skipping the newlines.** A move `1..9` becomes cell `c = getchar()-'1'`; the board index is `c+c/3` so it hops over the two embedded `\n`.
- **Win detection reuses the board.** The string `"01245689:04815926:05:258"` lists the 8 winning lines as triples of board indices (`:` is index 10). `b[a]&b[b]&b[c]` equals `p` only when all three cells hold the current player — because AND of identical marks is the mark, and mixing X/O/digits clears the high bits. No separate mask needed.
- **Loop via tail recursion.** `main` calls itself instead of a `for(;;)`, and `return;` (no value) stops on a win.

Input reads one character per move (`getchar`), so it tolerates junk/letters/extra newlines without locking up — each is simply re-prompted.

## Progression

| version                          | date       | model           | chars | notes                                  |
| -------------------------------- | ---------- | --------------- | ----- | -------------------------------------- |
| original (C++)                   | 2026-03-11 | Claude Opus 4.6 | 229   | needs `#include`, hangs on non-numeric input |
| this version (C, `gcc -std=gnu89`) | 2026-07-09 | GLM 5.2         | **211** | robust input, single `puts` per turn   |

The original 229-char C++ version was written with Claude Opus 4.6. This 211-char version was shortened by **GLM 5.2**.
