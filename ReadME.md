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

## The code (191 chars, C)

```c
char*l,b[]="123\n456\n789";p=79;main(c,d){puts(b);for(l="1Ia4:@5?";*l;)if((b[d=(c=*l++-48)/6]&b[d+c%6]&b[d+c%6*2])==p)return;do c=getchar()-49;while(c<0|b[c+c/3]-c-49);b[c+c/3]=p^=23;main();}
```

Compile and run:

```
gcc -std=gnu89 tictactoe.c
./a.out
```

`-std=gnu89` enables the implicit-int globals and K&R `main` that make it this short.

## How it works

- **Board is the display.** `b = "123\n456\n789"` is both the state and the grid `puts` prints each turn.
- **One declaration, two globals.** `char*l,b[]` declares the win-scan pointer and the board together.
- **Toggle folded into the move.** `b[c+c/3]=p^=23` flips the player (`'O'(79)` ↔ `'X'(88)`, since `79 ^ 88 == 23`) and writes that mark in a single expression; the win-check at the top then tests whoever just moved.
- **Empty cell = its own label.** A move `1..9` becomes `c = getchar()-'1'`; the cell sits at `c+c/3` (hopping the two `\n`). It's free iff it still equals its digit `c+49`, so `b[c+c/3]-c-49` is the occupied check; `c<0` rejects the newline after each move and EOF.
- **Win table packed to 8 bytes.** Each of the 8 lines is a `(start, step)` pair, encoded as one char `start*6+step+48` → `"1Ia4:@5?"`. Decoded live: `start = c/6`, `step = c%6`, cells `start`, `start+step`, `start+2*step`. `b[..]&b[..]&b[..] == p` only when all three hold the current player (AND of identical marks is the mark; mixing X/O/digits clears the high bits).
- **Tail recursion.** `main()` calls itself for the next turn; `return;` stops on a win.

For digit/newline input (normal play) the cell check is exact and in-bounds; stray non-digit characters are rejected too.

## Progression

| version                          | date       | model                          | chars | notes                                  |
| -------------------------------- | ---------- | ------------------------------ | ----- | -------------------------------------- |
| original (C++)                   | 2026-03-11 | Claude Opus 4.6                | 229   | needs `#include`, hangs on non-numeric input |
| intermediate (C, `gcc -std=gnu89`) | 2026-07-09 | GLM 5.2                        | 211   | robust input, single `puts` per turn   |
| this version (C, `gcc -std=gnu89`) | 2026-07-09 | GLM 5.2 + Claude Fable 5       | **191** | 8-byte win table, folded toggle      |

The original 229-char C++ version was written with Claude Opus 4.6. The 211-char step was GLM 5.2. The 191-char version was shortened by **GLM 5.2** using ideas contributed by **Claude Fable 5** (packed `(start,step)` win table, digit-identity cell check, folded player toggle).
