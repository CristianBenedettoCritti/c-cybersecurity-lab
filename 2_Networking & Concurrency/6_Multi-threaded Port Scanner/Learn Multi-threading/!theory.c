// A multi-threaded program has multiple sequences of statements
// that execute at the same period of time. A parallel multi-threaded
// program can be much faster because it executes multiple sequences
// at the same time. A single-threaded program only executes one sequence
// at a time.
//
// To prove this, I wrote two C files (single-thread and multi-threading).
// They do exactly the same work, but one uses multiple threads and the other
// uses only one thread.
//
// (Quick information: both multi-threading files had 2 threads)
//
// pthread results:
// single-thread.c ≈ 3s
// multi-thread.c  ≈ 14-24 ms
//
// windows.h results:
// single-thread.c ≈ 3s
// multi-thread.c  ≈ 12-20 ms
//
//
//          Parallel Execution
//
//        |  int x;         |   int a = 10;
// Time   |  x = 20;        |   int b = 5;
//        |  x++;           |   int c = a + b; 
//        ↓                 ↓  
//
/*
--------------------------------------------------------------------
*/
//          Concurrent Execution
//
//        |  int x;         |
// Time   |                 |   int b = 10;
//        |  x++;           |
//        ↓                 ↓
//