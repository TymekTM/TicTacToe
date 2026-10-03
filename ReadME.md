# Tic Tac Toe

An extremely short, fully functioning Tic Tac Toe.

In Poland we call it **"Kółko i Krzyżyk"** - Circle and Cross.

### Previous version - code shaped as Kółko i Krzyżyk (O and X)

This was the original 229-char C++ version, with the code itself shaped as those two symbols - a circle (O) and a cross (X). It has since been beaten repeatedly: scroll down for the **145-byte** version.

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

## The code (145 bytes, C)

```c
x,p;char*l,b[]="123\n456\n789";main(d){for(puts(b),p^=9;d?!(--d&5&&x&x>>d&x>>d*2):!(l=index(b,getchar()|32))||main(6,*l=79+p,x|=1<<l-b+p*2)&0;);}
```

Fully printable ASCII, 145 bytes without the trailing newline - no raw-byte tricks left. Command-line arguments no longer matter: `argc` only seeds the first call's stride counter, and the board is still empty then, so that scan can never fire.

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

- `strchr` instead of `index`: **146 bytes** - one byte for portability across libcs.
- Strict input `(c=getchar())/16-3||!(l=index(b,c))` in place of `!(l=index(b,getchar()|32))`, with `c` added to the globals: **156 bytes** (**157** with `strchr`) - `c/16-3` admits only bytes 48-63, so every non-digit byte, including the control-byte aliases below, is rejected.

## How it works

- **Board is the display.** `b = "123\n456\n789"` is both the state and the grid `puts` prints each turn.
- **One bitboard for both players.** `x` holds O on the board's own offsets `{0,1,2,4,5,6,8,9,10}` (3 and 7 are the newlines) and X eighteen bits higher. Every sector gap from 16 to 20 (the most that still fits in `int`) is safe for the four tested strides: the never-set gap bits (3, 7, 11, 15, ...) kill both row wraparound and any line bridging the two sectors. With a gap of 15, stride-5 triples reach across the sectors and fabricate wins. Gap 18 is simply what `p*2` gives, and the top bit used is 28.
- **Win check, input and recursion in one loop condition.** Every call runs a single `for` whose condition is `d ? scan : read`. `x & x>>d & x>>2d` is nonzero exactly when three bits spaced by d are all set. Stride 1 is the rows, 4 the columns, 5 the main diagonal (0, 5, 10) and 3 the anti-diagonal (2, 5, 8).
- **Stride counter in `main`'s parameter.** Each new call receives `d = 6`, and `--d` walks 5, 4, 3, 2, 1, 0. `d&5` is zero exactly for 0 and 2, so the degenerate stride 0 and the false stride 2 (0, 2, 4 is not a line) are skipped for free. A hit makes the condition 0 and the call returns. The scan sits at the top of every call, so it always tests the player who just moved - an earlier opponent win would have returned one call sooner.
- **Fixed point at zero.** Once `d` reaches 0 the condition reads a key, and a miss leaves `d` at 0, so the next pass reads again without rescanning. The first call gets `d = argc`, but `x` is still 0 there, so its few scans are all zero.
- **Turn, mark and sector from one global.** `p` starts at 0 and `p^=9` flips it at the top of every call: 9 on X's turns, 0 on O's. `79+p` gives the mark (`88 = 'X'`, `79 = 'O'`) and `p*2` the bitboard sector (18 for X, 0 for O).
- **Free cell by search, not arithmetic.** `index(b, getchar()|32)` finds the typed digit in `b` itself - a digit stays there only until its cell is taken, so a hit proves the cell is free, hands back the write pointer `l`, and `l-b` doubles as the bit number for `x|=1<<l-b+p*2`. A miss (occupied cell, letter, newline, tab) returns NULL and the loop reads on. `|32` folds letter case so stray letters never match; the only leak is control bytes 17-25 (Ctrl-Q through Ctrl-Y), which alias the digits 1-9 - the strict variant above closes it.
- **Write and recurse inside the condition.** On a hit, `main(6,*l=79+p,x|=1<<l-b+p*2)` writes the mark, sets the bit and starts the next turn with a fresh counter. The two extra arguments are legal despite being unsequenced, since no argument writes an object another reads or writes. When that call returns, the game is over: `&0` forces the condition to 0, so a win unwinds every frame. Strictly, reading the result of a call that falls off its closing brace is undefined in C; `&0` only ever discards it, and every tested compiler accepts that. A draw or EOF waits or spins, exactly like every previous version.

## Validation

The 145-byte version was validated exhaustively on Arch Linux x86-64 with a fresh harness. It replays all 255,168 terminal game histories four ways:

- plain `digit\n`;
- printable junk, re-entered occupied cells and CR/LF/TAB between moves;
- every byte value 1-255 whose `|32` is not a free digit;
- bare digits with no separators.

That is 1,020,672 runs per build. An `LD_PRELOAD` shim feeds `getchar` one byte at a time and turns EOF into a visible marker, so the harness also checks that nothing is read after a win and that a draw keeps waiting. The builds were gcc 16.2 at `-O0`, `-O2` and `-Os`, clang 22.1 at `-O0` and `-O2`, tcc, and gcc and clang with AddressSanitizer and UndefinedBehaviorSanitizer (`-fno-sanitize-recover=all`, shim linked in). There were zero failures. The `strchr` and strict-input variants pass the same run on gcc at `-O0` and `-O2`. A MinGW-w64 cross build with the `msvc.h` shim compiles but was not run, because no Windows host was available, and MSVC itself was not re-tested for this version.

The 158-byte rewrite was exhaustively validated by its author on GCC 15.2 (Linux x86-64) and independently re-verified on gcc 13.3 (WSL) and MSVC 14.44 (Windows x64): all 255,168 terminal game histories, each replayed twice - once plain, once with junk bytes, re-entered occupied cells and LF/CR interleaved - for 510,336 scenarios per build at `-O0`, `-O2` and `-Os`, zero failures, also clean under AddressSanitizer and UndefinedBehaviorSanitizer. A 22-scenario battery (all eight win lines for X, six for O, rejection cases, draw, blocked lines) is byte-identical to the 175-char build's output. Raw MSVC builds fail on the missing `index` symbol (LNK2019); with the shim header above the game builds and plays correctly.

## Dead ends, kept for the record

- A raw newline cannot appear inside a string literal, and `exit()` cannot ride an `&&`/`||` expression - gcc's builtin knows its arity (both from the 175-char era).
- A raw `{1,3,4,5}` stride table measures longer than the loop it replaces (45 vs 42 bytes for the win block).
- Sector gap 15 lets stride-5 triples bridge the sectors into false wins; `p%13` in place of `p&16` overlaps them the same way.
- Dropping `|32` makes `index` accept the marks X/O and even the board newlines as moves - shorter, broken.
- Measured rewrites that do not beat 158 (2026-09-10, all compiled and gameplay-identical): folding the stride-2 skip into the loop condition `for(d=6;d=--d-(d==2);)` = 162; a `"\5\4\3\1"` stride table walked by `l` = 165; a fully unrolled win expression = 173; `p^=23` toggle designs with a pre-toggle sector mapping = 159-160; a `for(;;)` loop in place of recursion = 163.
- Three exact 158-byte ties, all gameplay-identical: `d` as `main`'s second parameter (the garbage `argv` value is always overwritten before use), the bit insert folded into the recursive call as a second argument (`main(*l=89-p%13,x|=1<<l-b+(p&16))` - legal despite unsequenced arguments, since no argument writes an object another reads or writes), and the same via the comma operator.
- Analytic kills: multiply-constant win detection cannot discriminate a sum of 2 from 3 bits at the carry level without extra masking, and bit 26 + 11 overflows `int`, forcing a `(long)` cast; transposing the board to `"147\n258\n369"` leaves the stride set `{1,3,4,5}` unchanged; a sector gap below 16 always admits a false line (gap 11: stride-1 `(9,10,11)`, 12: stride-4 `(4,8,12)`, 13: stride-5 `(8,13,18)`, 14: stride-4 `(6,10,14)`, 15: stride-5 `(5,10,15)`), so 16 is minimal.
- Measured on the way to 145 (2026-10-03): a width-5 padded board, meant to make stride 2 harmless, admits cross-sector false lines for every gap from 13 to 18 in a 32-bit `int`. `exit()` as the win branch of the fused `?:` condition is rejected by gcc and clang (void operand) and only tcc takes it. Seeding the counter from `puts`'s return value works on glibc (length + 1), but musl and MSVC return 0. A brute-force search over short stride pairs `x>>G(d)&x>>F(d)` (up to 8 characters together, sector gap 16) found no skip-free replacement for `d&5&&...d*2`.
- Still open: a skip-free stride generator and bit-parallel stride detection via multiply-constants - nothing verified below 145 yet.

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
| intermediate (C, `gcc -std=gnu89`) | 2026-09-10 | GPT 6 Astra                    | 158   | one bitboard for both players, four-stride win loop, turn in `main` arg, `index()` free-cell search |
| this version (C, `gcc -std=gnu89`) | 2026-10-03 | Claude Opus 5.5 via Claude Code | **145** | win scan, input and recursion in one `for` condition, counter in `main`'s parameter, `d&5` stride skip |

The original 229-char C++ version was written with Claude Opus 4.6. The 211-char step was GLM 5.2. The 191-char version was shortened by **GLM 5.2** using ideas contributed by **Claude Fable 5** (packed `(start,step)` win table, digit-identity cell check, folded player toggle). The 189-char trim is **GLM 5.2**: the board index is computed once into `d` during the input check and reused for the write, dropping a duplicated `c+c/3`. The 181-char trim is **GLM 5.3 (ZCode)**: the `do...while` collapses into a bare `for` whose condition reads the key via a comma expression, the win-table byte itself becomes the loop guard, and the move write rides the recursive call as `main(b[d]=p^=23)`. The 179-char trim is also **GLM 5.3 (ZCode)**: dropping the `+48` printable bias and encoding lines as `start*8+step` makes the win-table decode subtraction-free (`d/8`, `d%8`). The 175-char trim, also **Ox-alpha**, removes the input `c<0` guard: junk rejection now rests on the out-of-bounds byte never matching the typed character, verified empirically on both compilers. The 158-byte rewrite is **GPT 6 Astra** (external coding agent): both players share one bitboard, the eight-entry win table collapses into a four-stride shift loop, the turn rides `main`'s argument seeded from `argc`, and the free-cell check becomes an `index()` search of the board itself. Independently re-verified - byte-identical 22-scenario battery against the 175 build, the 510,336-scenario exhaustive harness clean on gcc 13.3 under sanitizers, and the MSVC shim build - by **GLM 5.3 (ZCode)**.

The 145-byte trim is **Claude Opus 5.5 via Claude Code**. The win scan, the input loop and the recursion merge into a single `for` condition (`d ? scan : read || recurse`). The stride counter moves into `main`'s parameter with a fixed point at 0, so a missed key costs no rescan. `d&5` skips strides 0 and 2 without a comparison, and the turn becomes a global flipped by `p^=9`. The measured steps on the way were 155, 153, 150 and 146. The build toolchain (gcc, clang, tcc, sanitizer runtimes, MinGW-w64) was installed by a **Gemini 3.8 Flash** subagent in Antigravity.
