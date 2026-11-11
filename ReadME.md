# Tic Tac Toe

An extremely short, fully functioning Tic Tac Toe.

In Poland we call it **"Kółko i Krzyżyk"** - Circle and Cross.

### Previous version - code shaped as Kółko i Krzyżyk (O and X)

This was the original 229-char C++ version, with the code itself shaped as those two symbols - a circle (O) and a cross (X). It has since been beaten repeatedly: scroll down for the **137-byte** version.

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

## The code (137 bytes, C)

```c
x,y;char*l,b[]="O	123\n	456\n	789";main(d){for(puts(b);!(l=d?x&x>>d&x<<d--:index(b,getchar()|32)););l<b||main(6,*l=*b^=23,x^=y|=1<<l-b);}
```

137 bytes without the trailing newline (`wc -c tictactoe.c` prints 138). The three gaps inside the string are literal TAB bytes (byte 9) and have to stay that way: spelled `\t` the source is 140 bytes of printable ASCII and compiles to the identical program, while an editor that expands them to spaces breaks the game.

The board now carries one extra letter:

```
O	123
	456
	789
```

The letter in the left margin is the mark that moved last. It starts as `O` so that X opens, and on the final board it names the winner. Command-line arguments still do not matter: `argc` only seeds the first call's stride counter, and the board is still empty then, so that scan can never fire.

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

### Variants (same idea)

- `strchr` instead of `index`: **138 bytes** - one byte for portability across libcs.
- Plain board, no mark in the margin: **139 bytes**. The turn goes back into a global, and rows one and two end in a TAB that nobody sees (still two raw bytes):
  ```c
  x,y,p=79;char*l,b[]="123	\n456	\n789";main(d){for(puts(b);!(l=d?x&x>>d&x<<d--:index(b,getchar()|32)););l<b||main(6,*l=p^=23,x^=y|=1<<l-b);}
  ```
- Mark glued to row one: **136 bytes**. Dropping the TAB after the mark and padding rows two and three with `_` saves one more byte, but the board then prints as `O123` / `_456` / `_789`, and once X takes cell 1 the top row reads `XX23`. That is too easy to misread, so it is not the main version:
  ```c
  x,y;char*l,b[]="O123\n_456\n_789";main(d){for(puts(b);!(l=d?x&x>>d&x<<d--:index(b,getchar()|32)););l<b||main(6,*l=*b^=23,x^=y|=1<<l-b);}
  ```
- Strict input `(c=getchar())/16-3?0:index(b,c)` in place of `index(b,getchar()|32)`, with `c` added to the globals: **149 bytes** - `c/16-3` admits only bytes 48-63, so every non-digit byte, including the control-byte aliases below, is rejected.
- The 140-byte version needed an `x>>d*2` variant to keep Clang's `shift-base` sanitizer quiet. That is gone: the bitboard now tops out at bit 14 and the widest left shift is 6, so nothing comes near the sign bit.

## How it works

- **Board is the display, five bytes per row.** `b = "O\t123\n\t456\n\t789"` is both the state and what `puts` prints each turn. Offset 0 is the margin mark, and every row is a TAB, three cells and a newline, so the cells sit at offsets `{2,3,4,7,8,9,12,13,14}`.
- **The turn lives in the board.** `b[0]` is the mark that moved last. `*b^=23` flips it between `O` (79) and `X` (88), and the flipped value is the mark to write, so `*l=*b^=23` is the whole turn logic - no turn variable, no initializer, no arithmetic.
- **Two bitboards for one update.** `y` collects every occupied cell, and `x^=y|=1<<l-b` turns `x` into the cells of the player who just moved: before the move `x` holds the opponent's cells (they moved last), so XOR-ing it with the new "everything occupied" leaves exactly the mover's cells, the new one included. `x` never mixes the two players, so there is no sector offset to add and no cross-sector line to guard against.
- **Unfiltered center-bit scan.** `x & x>>d & x<<d` tests whether some cell has same-player neighbors `d` bits to either side. On the five-wide board stride 1 is the rows, 5 the columns, 6 the main diagonal (2, 8, 14) and 4 the anti-diagonal (4, 8, 12). Strides 2 and 3 fit no three cell offsets at all, so the scan simply walks `d` = 6, 5, 4, 3, 2, 1 and needs no `d&5` filter. The second pad byte per row is what buys that: on the old four-wide board stride 2 fabricates lines such as (0, 2, 4).
- **Stride counter in `main`'s parameter, stepped inside the test.** Each new call receives `d = 6`, and `x&x>>d&x<<d--` shifts by `d` twice and decrements it afterwards. Reading and modifying `d` with no sequence point in between is undefined by the C standard; gcc, clang and tcc all shift by the old value at every optimization level tested (see Validation). The scan sits at the top of every call, so it always tests the player who just moved - an earlier opponent win would have returned one call sooner.
- **Fixed point at zero.** Once `d` reaches 0 the condition reads a key instead, and a miss (`NULL`) leaves `d` at 0, so the next pass reads again without rescanning. The first call gets `d = argc`, but `x` is still 0 there, so its scans are all zero.
- **Free cell by search, not arithmetic.** `index(b, getchar()|32)` finds the typed digit in `b` itself - a digit stays there only until its cell is taken, so a hit proves the cell is free, hands back the write pointer `l`, and `l-b` is the bit number for `1<<l-b`. `|32` sets bit 5 of whatever was typed, and the margin mark, the placed marks, the TABs and the newlines all have bit 5 clear, so none of them can ever be matched; letters fold to lower case and miss as well. The only leak is control bytes 17-25 (Ctrl-Q through Ctrl-Y), which alias the digits 1-9 - the strict variant above closes it.
- **`l<b` tells a win from a move.** The loop `for(puts(b);!(l=d?scan:read););` ends with `l` non-null in two ways: a scan hit, which lands in `l` as a small integer (the hit mask, below 16384, converted to `char*` by the GNU C conditional), or a valid key, which leaves a pointer into `b`. Because of the post-decrement a row win ends with `d == 0`, exactly like a valid key, so the 140-byte version's `d||` guard cannot be reused. `l<b` can: a hit mask is far below any address the board can have. `l<b||main(6,*l=*b^=23,x^=y|=1<<l-b);` therefore just returns on a win, and otherwise writes the mark, updates both bitboards and recurses. Comparing a converted integer against `b` is the second thing here the standard does not define; it relies on a flat address space. `main`'s return value is still never used to decide control flow.

## Validation

The 137-byte version was validated exhaustively on Arch Linux x86-64 (2026-11-09) across all 255,168 terminal game histories replayed four ways:

- plain `digit\n`;
- printable junk, re-entered occupied cells and CR/LF/TAB between moves;
- every byte value 0-255 whose `|32` is not a free digit;
- bare digits with no separators.

That is 1,020,672 runs per build. The harness intercepts `getchar` and `puts` and verifies the exact output after every move - margin mark included - and that nothing is read after a win while a draw keeps waiting for input. Zero failures at gcc 16.2.1 `-O0`, `-O2` and `-Os`, clang 23.1 `-O0` and `-O2`, tcc 0.9.28rc, and under `-std=gnu89 -fsanitize=address,undefined -fno-sanitize-recover=all` with both gcc and clang at their full default checks.

The 139-byte plain-board and 136-byte glued-mark variants pass the same battery on the six non-sanitizer builds. The 138-byte `strchr` and 149-byte strict-input variants pass it on gcc at `-O0` and `-O2`, the strict one with bytes 17-25 fed as junk. The `\t`-escaped 140-byte spelling compiles to byte-identical code and data. The MinGW-w64 cross build with `msvc.h` compiles; it was not run.

The 140-byte version and its 142-byte `x>>d*2` variant passed the same four-mode 1,020,672-run battery at gcc 16.2 `-O0`, `-O2` and `-Os`, clang 22.1 `-O0` and `-O2`, tcc 0.9.28, and `gcc -std=gnu89 -fsanitize=address,undefined -fno-sanitize-recover=all` with zero failures; the 142-byte version also passed `clang -std=gnu89 -fsanitize=address,undefined -fno-sanitize-recover=all` with full default checks (the 140-byte version passed with `-fno-sanitize=shift-base`). Its 141-byte `strchr` and 152-byte strict-input variants passed the same battery on gcc at `-O0` and `-O2`, and the MinGW-w64 cross build with `msvc.h` compiled cleanly.

The 145-byte version was validated exhaustively on Arch Linux x86-64 with the same four-mode 1,020,672-run battery across gcc 16.2 (`-O0`, `-O2`, `-Os`), clang 22.1 (`-O0`, `-O2`), tcc, and gcc/clang with AddressSanitizer and UndefinedBehaviorSanitizer.

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
- Measured on the way to 140 (2026-10-16): folding `p^=9` into `*l=79+(p^=9)` and computing the sector offset from `*l` via `*3%22` (gap 17) or `*2%26` (gap 18) eliminates `,p^=9` (-5 bytes) and `,` (-1 byte) but adds `(*l=79+(p^=9))*3%22` (+9 bytes) for a net +3 bytes; toggling `p^=23` with `79^p` and `+p` overflows 32-bit `int` at cells 8 and 9 (`9+23=32`, `10+23=33`).
- Measured on the way to 137 (2026-11-09): the XOR bitboards on a five-wide board with a sequenced scan (`--d&&x&x>>d&x<<d` from `main(7,...)`) and the old `d||` guard are an exact 140-byte tie, fully validated. Keeping `d||` with the post-decrement scan would be 135, but a row win then leaves `d == 0`, the winner's call recurses through the hit mask as a pointer and segfaults, and a piped game loses all of its output. The margin mark right of row one (`"123O\n456\t\n789"` with `b[3]^=23`) is another 137, and on a line of its own (`"O\n123\t\n..."`) it is 138. A space cannot be the pad byte: `index(b,getchar()|32)` matches it for a typed space or NUL.
- An estimate, not a proof: the pieces this design cannot do without - the declarations, `main(d){}`, `puts(b)`, the `index`/`getchar` read, one scan, one mark write, one bitboard update, the recursion and the `for` glue - add up to roughly 130 bytes, so sub-100 looks out of reach for this architecture in C.

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
| intermediate (C, `gcc -std=gnu89`) | 2026-10-03 | Claude Opus 5.5 via Claude Code | 145   | win scan, input and recursion in one `for` condition, counter in `main`'s parameter, `d&5` stride skip |
| intermediate (C, `gcc -std=gnu89`) | 2026-10-16 | Gemini 4 Argon High via Antigravity | 140   | hoisted `!(l=d?scan:read)` condition, post-loop `d\|\|main(...)` recursion, center-bit `x&x>>d&x<<d` win check (142 with `x>>d*2`) |
| this version (C, `gcc -std=gnu89`) | 2026-11-09 | Claude Fable 5.2 via Claude Code | **137** | turn mark kept in the board (`*b^=23`), XOR bitboards `x^=y\|=1<<l-b`, five-wide board with an unfiltered `d--` scan, `l<b` win guard (139 without the margin mark) |

The original 229-char C++ version was written with Claude Opus 4.6. The 211-char step was GLM 5.2. The 191-char version was shortened by **GLM 5.2** using ideas contributed by **Claude Fable 5** (packed `(start,step)` win table, digit-identity cell check, folded player toggle). The 189-char trim is **GLM 5.2**: the board index is computed once into `d` during the input check and reused for the write, dropping a duplicated `c+c/3`. The 181-char trim is **GLM 5.3 (ZCode)**: the `do...while` collapses into a bare `for` whose condition reads the key via a comma expression, the win-table byte itself becomes the loop guard, and the move write rides the recursive call as `main(b[d]=p^=23)`. The 179-char trim is also **GLM 5.3 (ZCode)**: dropping the `+48` printable bias and encoding lines as `start*8+step` makes the win-table decode subtraction-free (`d/8`, `d%8`). The 175-char trim, also **Ox-alpha**, removes the input `c<0` guard: junk rejection now rests on the out-of-bounds byte never matching the typed character, verified empirically on both compilers. The 158-byte rewrite is **GPT 6 Astra** (external coding agent): both players share one bitboard, the eight-entry win table collapses into a four-stride shift loop, the turn rides `main`'s argument seeded from `argc`, and the free-cell check becomes an `index()` search of the board itself. Independently re-verified - byte-identical 22-scenario battery against the 175 build, the 510,336-scenario exhaustive harness clean on gcc 13.3 under sanitizers, and the MSVC shim build - by **GLM 5.3 (ZCode)**.

The 145-byte trim is **Claude Opus 5.5 via Claude Code**. The win scan, the input loop and the recursion merge into a single `for` condition (`d ? scan : read || recurse`). The stride counter moves into `main`'s parameter with a fixed point at 0, so a missed key costs no rescan. `d&5` skips strides 0 and 2 without a comparison, and the turn becomes a global flipped by `p^=9`. The measured steps on the way were 155, 153, 150 and 146. The build toolchain (gcc, clang, tcc, sanitizer runtimes, MinGW-w64) was installed by a **Gemini 3.8 Flash** subagent in Antigravity.

The 140-byte trim is **Gemini 4 Argon High via Antigravity**. Moving `main(6,...)` out of the `for` condition into a post-loop `d||main(...)` statement drops `&0` and lets `l=` and `!(...)` be factored across the entire `d ? scan : read` ternary (`!(l=d?--d&5&&x&x>>d&x<<d:index(b,getchar()|32))`), saving 3 bytes (to 142 bytes with `x>>d*2`). Replacing the end-bit test `x&x>>d&x>>d*2` with the center-bit test `x&x>>d&x<<d` saves another 2 bytes (to 140 bytes).

The 137-byte trim is **Claude Fable 5 via Claude Code**. A second bitboard `y` for all occupied cells makes `x^=y|=1<<l-b` yield the mover's cells directly, which removes the sector offset. A second pad byte per row makes strides 2 and 3 harmless, so the `d&5` filter goes and the decrement moves into the test as `x<<d--`; since a row win then ends with `d == 0`, the post-loop guard becomes `l<b`. That is 139. Storing the turn in the board itself - `*l=*b^=23` flips the mark at `b[0]` and writes it - drops the turn variable and its initializer for 137, at the price of one visible letter in the margin.
