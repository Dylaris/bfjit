# bfjit

## brief

The JIT compiler for Brainfuck is pretty weak because I built it using my own 'dynasm' method, which is a cool trick (I think so) but really slow. 

So, in this project, the interpreter actually runs faster than the JIT :->

## usage

Only for Linux if you want to enable the JIT

```console
$ lua build.lua
$ ./bfjit test/helloworld.bf            (interpret)

OR 

$ ./bfjit -jit test/helloworld.bf       (jit)

```

## syntax

| Brainfuck | C         |
|:---------:|:---------:|
| >         | ++ptr     |
| <         | --ptr     |
| +         | ++*ptr    |
| -         | --*ptr    |
| .         | fputc(*ptr, stdout) |
| ,         | *ptr=fgetc(stdin)   |
| [         | while(*ptr) {       |
| ]         | }                   |

## reference
- [A nice blog for learning brainfuck](https://blog.csdn.net/nameofcsdn/article/details/110231730)
- [Thans to tsoding for the video](https://www.youtube.com/watch?v=mbFY3Rwv7XM)
