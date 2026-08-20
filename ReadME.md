# Tic Tac Toe

An extremely short, fully functioning Tic Tac Toe.

In Poland we call it **"Kółko i Krzyżyk"** - Circle and Cross.

### Previous version - code shaped as Kółko i Krzyżyk (O and X)

This was the original 229-char C++ version, with the code itself shaped as those two symbols - a circle (O) and a cross (X). It has since been beaten repeatedly: scroll down for the **179-char** version.

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

## The code (179 chars, C)

```c
char*l,b[]="123\n456\n789";p=79;main(c,d){puts(b);for(l="!A";d=*l++;)if((b[c=d/8]&b[c+d%8]&b[c+d%8*2])==p)return;for(;c=getchar()-49,c<0|b[d=c+c/3]-c-49;);main(b[d]=p^=23);}
```

The listing above is the exact `tictactoe.c`. The win table holds the eight raw bytes `01 21 41 04 0C 14 05 13` - only `21` (`!`) and `41` (`A`) are printable, so this page may display or copy the other six unfaithfully. Clone the repo for the real file, or use the printable form:

### Printable form (181 chars, identical behavior)

```c
char*l,b[]="123\n456\n789";p=79;main(c,d){puts(b);for(l="1Ia4:@5?";d=*l++;)if((b[c=d/6-8]&b[c+d%6]&b[c+d%6*2])==p)return;for(;c=getchar()-49,c<0|b[d=c+c/3]-c-49;);main(b[d]=p^=23);}
```

The only difference: the win table carries a `+48` printable bias and the start decodes as `d/6-8` instead of `d/8`.

Compile and run:

```
gcc -std=gnu89 tictactoe.c
./a.out
```

`-std=gnu89` enables the implicit-int globals and K&R `main` that make it this short. MSVC 2022 also accepts it unchanged (`cl /nologo tictactoe.c`) - no flags, no warnings.

## How it works

- **Board is the display.** `b = "123\n456\n789"` is both the state and the grid `puts` prints each turn.
- **One declaration, two globals.** `char*l,b[]` declares the win-scan pointer and the board together.
- **Input loop as a bare `for`.** `for(;c=getchar()-49,c<0|b[d=c+c/3]-c-49;);` - the comma operator sequences the read before the test, so the old `do...while` body disappears. A move `1..9` becomes `c` via `getchar()-49`; its board index is `c+c/3` (hopping the two `\n`), computed once into `d` inside the condition and reused for the write. A cell is free iff it still equals its digit `c+49`, so `b[d]-c-49` is the occupied check (`c<0` rejects newlines/EOF).
- **Win table packed to 8 raw bytes.** Each of the 8 lines is a `(start, step)` pair encoded as one byte `start*8+step`, with no printable bias - so decoding needs no subtraction: `start = d/8`, `step = d%8`, cells `start`, `start+step`, `start+2*step`. The multiplier must exceed the largest step (5) and must not make any line encode to `0x0A`, since a raw newline cannot appear inside a string literal: 8 satisfies both, while 6 turns column 2 into exactly 10. The table byte is loaded straight into `d`, and the loop guard is the byte itself, so no separate sentinel test. `b[..]&b[..]&b[..] == p` only when all three hold the current player (AND of identical marks is the mark; mixing X/O/digits clears the high bits).
- **Toggle, write and recurse in one expression.** `main(b[d]=p^=23)` flips the player (`'O'(79)` ↔ `'X'(88)`, since `79 ^ 88 == 23`), writes the mark, and passes the result as `main`'s first argument - which the recursive call never reads before overwriting. The win-check at the top of each call then tests whoever just moved.
- **Tail recursion.** `main()` calls itself for the next turn; `return;` stops on a win.

For digit/newline input (normal play) the cell check is exact and in-bounds. The `|` (rather than `||`) means a newline or junk byte also reads one out-of-bounds cell - but that value is discarded because `c<0` already forces the loop to spin, so it can never select a bad move (harmless in practice, technically undefined; compile with `-w`). Likewise the win scan writes `c` and reads it back across the `&` chain, same as the previous version did with `d`; verified byte-identical against the 189-char build on gcc 13 at `-O0` and `-O2`, and on MSVC 14.44 (`x64`). Two dead ends worth recording: a raw newline cannot appear inside a string literal (it terminates the source line - only `\n` works), and `exit()` cannot ride an expression (`&&`/`||` need scalar operands, and gcc's builtin knows its arity).

## Progression

| version                          | date       | model                          | chars | notes                                  |
| -------------------------------- | ---------- | ------------------------------ | ----- | -------------------------------------- |
| original (C++)                   | 2026-03-11 | Claude Opus 4.6                | 229   | needs `#include`, hangs on non-numeric input |
| intermediate (C, `gcc -std=gnu89`) | 2026-07-09 | GLM 5.2                        | 211   | robust input, single `puts` per turn   |
| intermediate (C, `gcc -std=gnu89`) | 2026-07-09 | GLM 5.2 + Claude Fable 5       | 191   | 8-byte win table, folded toggle        |
| intermediate (C, `gcc -std=gnu89`) | 2026-07-09 | GLM 5.2                        | 189   | cell index stashed in `d`, reused for the write |
| intermediate (C, `gcc -std=gnu89`) | 2026-08-20 | GLM 5.3 (ZCode)                | 181   | comma-driven input `for`, guard-as-data win loop, write folded into the recursion argument |
| this version (C, `gcc -std=gnu89`) | 2026-08-20 | GLM 5.3 (ZCode)                | **179** | bias-free raw win table (`start*8+step`), decode `d/8`/`d%8` with no subtraction |

The original 229-char C++ version was written with Claude Opus 4.6. The 211-char step was GLM 5.2. The 191-char version was shortened by **GLM 5.2** using ideas contributed by **Claude Fable 5** (packed `(start,step)` win table, digit-identity cell check, folded player toggle). The 189-char trim is **GLM 5.2**: the board index is computed once into `d` during the input check and reused for the write, dropping a duplicated `c+c/3`. The 181-char trim is **GLM 5.3 (ZCode)**: the `do...while` collapses into a bare `for` whose condition reads the key via a comma expression, the win-table byte itself becomes the loop guard, and the move write rides the recursive call as `main(b[d]=p^=23)`. The 179-char trim is also **GLM 5.3 (ZCode)**: dropping the `+48` printable bias and encoding lines as `start*8+step` makes the win-table decode subtraction-free (`d/8`, `d%8`).
