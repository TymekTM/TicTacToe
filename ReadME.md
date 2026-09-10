# Tic Tac Toe

An extremely short, fully functioning Tic Tac Toe.

In Poland we call it **"Kółko i Krzyżyk"** - Circle and Cross.

### Previous version - code shaped as Kółko i Krzyżyk (O and X)

This was the original 229-char C++ version, with the code itself shaped as those two symbols - a circle (O) and a cross (X). It has since been beaten repeatedly: scroll down for the **158-byte** version.

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

## The code (158 bytes, C)

```c
x,d;char*l,b[]="123\n456\n789";main(p){puts(b);for(d=6;--d;)if(d-2&&x&x>>d&x>>d*2)return;for(;!(l=index(b,getchar()|32)););x|=1<<l-b+(p&16);main(*l=89-p%13);}
```

Fully printable ASCII, 158 bytes without the trailing newline - no raw-byte tricks left. Run it without command-line arguments: the first turn is seeded from `argc == 1`, so extra arguments corrupt the first move.

Compile and run:

```
gcc -std=gnu89 tictactoe.c
./a.out
```

`-std=gnu89` buys the implicit-int globals, K&R `main` and the undeclared `index` call - GCC recognizes `index` as a builtin and types its return as a pointer. `index` is POSIX/BSD and absent from MSVC's CRT, so on MSVC 2022 force a two-line shim before the source:

```
cl /nologo /FImsvc.h tictactoe.c
```

with `msvc.h` containing `#define index strchr` and `char*strchr();`. The declaration is not cosmetic: an implicit `int` return truncates the 64-bit pointer on Win64 and crashes the game.

### Variants (same idea, same validation)

- `strchr` instead of `index`: **159 bytes** - one byte for portability across libcs.
- Strict input `for(;d=getchar(),d/16-3||!(l=index(b,d)););`: **167 bytes** (**168** with `strchr`) - `d/16-3` admits only bytes 48-63, so every non-digit byte, including the control-byte aliases below, is rejected.

## How it works

- **Board is the display.** `b = "123\n456\n789"` is both the state and the grid `puts` prints each turn.
- **One bitboard for both players.** `x` holds X on the board's own offsets `{0,1,2,4,5,6,8,9,10}` (3 and 7 are the newlines) and O sixteen bits higher. The sector gap must stay 16: it exceeds twice the largest stride, and the never-set gap bits (3, 7, 11, 15, ...) kill both row wraparound and any line bridging the two sectors. With a gap of 15, stride-5 triples reach across the sectors and fabricate wins.
- **Win check without a table.** `for(d=6;--d;)if(d-2&&x&x>>d&x>>d*2)return;` walks the strides d = 5, 4, 3, 1: `x & x>>d & x>>2d` is nonzero exactly when three bits spaced by d are all set. Stride 1 is the rows, 4 the columns, 5 the main diagonal (0, 5, 10), 3 the anti-diagonal (2, 5, 8); stride 2 must be skipped, since 0, 2, 4 is not a line. The check sits at the top of every call, so it always tests the player who just moved - an earlier opponent win would have returned one call sooner.
- **Turn, mark and sector in `main`'s argument.** `p` starts as `argc`, i.e. 1. `89-p%13` maps `1 -> 88 ('X')`, `88 -> 79 ('O')` and `79 -> 88`, producing both the mark to write and the next call's `p`; `p&16` picks the bitboard sector (`88 & 16 = 16`, O's half; `79 & 16 = 0`, X's half).
- **Free cell by search, not arithmetic.** `index(b, getchar()|32)` finds the typed digit in `b` itself - a digit stays there only until its cell is taken, so a hit proves the cell is free, hands back the write pointer `l`, and `l-b` doubles as the bit number for `x|=1<<l-b+(p&16)`. A miss (occupied cell, letter, newline, tab) returns NULL and the loop reads on. `|32` folds letter case so stray letters never match; the only leak is control bytes 17-25 (Ctrl-Q through Ctrl-Y), which alias the digits 1-9 - the strict variant above closes it.
- **Toggle, write and recurse in one expression.** `main(*l=89-p%13)` writes the mark through the found pointer and passes it as the next call's `p`.
- **Tail recursion.** `main()` calls itself for the next turn; `return;` stops on a win. A draw or EOF waits or spins, exactly like every previous version.

## Validation

The rewrite was exhaustively validated by its author on GCC 15.2 (Linux x86-64) and independently re-verified on gcc 13.3 (WSL) and MSVC 14.44 (Windows x64): all 255,168 terminal game histories, each replayed twice - once plain, once with junk bytes, re-entered occupied cells and LF/CR interleaved - for 510,336 scenarios per build at `-O0`, `-O2` and `-Os`, zero failures, also clean under AddressSanitizer and UndefinedBehaviorSanitizer. A 22-scenario battery (all eight win lines for X, six for O, rejection cases, draw, blocked lines) is byte-identical to the 175-char build's output. Raw MSVC builds fail on the missing `index` symbol (LNK2019); with the shim header above the game builds and plays correctly.

## Dead ends, kept for the record

- A raw newline cannot appear inside a string literal, and `exit()` cannot ride an `&&`/`||` expression - gcc's builtin knows its arity (both from the 175-char era).
- A raw `{1,3,4,5}` stride table measures longer than the loop it replaces (45 vs 42 bytes for the win block).
- Sector gap 15 lets stride-5 triples bridge the sectors into false wins; `p%13` in place of `p&16` overlaps them the same way.
- Dropping `|32` makes `index` accept the marks X/O and even the board newlines as moves - shorter, broken.
- Still open: a shorter stride generator, fusing the win scan with the recursion, and bit-parallel stride detection via multiply-constants - nothing verified below 158 yet.

## Progression

| version                          | date       | model                          | chars | notes                                  |
| -------------------------------- | ---------- | ------------------------------ | ----- | -------------------------------------- |
| original (C++)                   | 2026-03-11 | Claude Opus 4.6                | 229   | needs `#include`, hangs on non-numeric input |
| intermediate (C, `gcc -std=gnu89`) | 2026-07-09 | GLM 5.2                        | 211   | robust input, single `puts` per turn   |
| intermediate (C, `gcc -std=gnu89`) | 2026-07-09 | GLM 5.2 + Claude Fable 5       | 191   | 8-byte win table, folded toggle        |
| intermediate (C, `gcc -std=gnu89`) | 2026-07-09 | GLM 5.2                        | 189   | cell index stashed in `d`, reused for the write |
| intermediate (C, `gcc -std=gnu89`) | 2026-08-20 | GLM 5.3 (ZCode)                | 181   | comma-driven input `for`, guard-as-data win loop, write folded into the recursion argument |
| intermediate (C, `gcc -std=gnu89`) | 2026-08-20 | GLM 5.3 (ZCode)                | 179   | bias-free raw win table (`start*8+step`), decode `d/8`/`d%8` with no subtraction |
| intermediate (C, `gcc -std=gnu89`) | 2026-08-21 | Ox-alpha                       | 175   | input `c<0` guard dropped - junk rejection now rests on an out-of-bounds byte never matching |
| this version (C, `gcc -std=gnu89`) | 2026-09-10 | GPT 6 Astra                    | **158** | one bitboard for both players, four-stride win loop, turn in `main` arg, `index()` free-cell search |

The original 229-char C++ version was written with Claude Opus 4.6. The 211-char step was GLM 5.2. The 191-char version was shortened by **GLM 5.2** using ideas contributed by **Claude Fable 5** (packed `(start,step)` win table, digit-identity cell check, folded player toggle). The 189-char trim is **GLM 5.2**: the board index is computed once into `d` during the input check and reused for the write, dropping a duplicated `c+c/3`. The 181-char trim is **GLM 5.3 (ZCode)**: the `do...while` collapses into a bare `for` whose condition reads the key via a comma expression, the win-table byte itself becomes the loop guard, and the move write rides the recursive call as `main(b[d]=p^=23)`. The 179-char trim is also **GLM 5.3 (ZCode)**: dropping the `+48` printable bias and encoding lines as `start*8+step` makes the win-table decode subtraction-free (`d/8`, `d%8`). The 175-char trim, also **Ox-alpha**, removes the input `c<0` guard: junk rejection now rests on the out-of-bounds byte never matching the typed character, verified empirically on both compilers. The 158-byte rewrite is **GPT 6 Astra** (external coding agent): both players share one bitboard, the eight-entry win table collapses into a four-stride shift loop, the turn rides `main`'s argument seeded from `argc`, and the free-cell check becomes an `index()` search of the board itself. Independently re-verified - byte-identical 22-scenario battery against the 175 build, the 510,336-scenario exhaustive harness clean on gcc 13.3 under sanitizers, and the MSVC shim build - by **GLM 5.3 (ZCode)**.
