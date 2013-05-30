import java.util.*;
import java.lang.*;
import uvic.posix.*;

/**
 This program tests a straightforward reader-writer paradigm.

*/
class TestBasicRW
{
    static final int SIZE = 10; 
    static final int ITERATIONS = 50;

    static DatabaseRW db;
    static Reader r1, r2, r3;
    static Writer w;

    static void main(String[] args)
    {
        db = new DatabaseRW();

        r1 = new Reader(db,"RA",ITERATIONS);
        r2 = new Reader(db,"RB",ITERATIONS);
        r3 = new Reader(db,"RC",ITERATIONS);
        w = new Writer(db,"W1",ITERATIONS);

		// kick start all threads
		w.start(uvic.posix.Thread.REAL_TIME);
		r1.start(uvic.posix.Thread.REAL_TIME);
		r2.start(uvic.posix.Thread.REAL_TIME);
		r3.start(uvic.posix.Thread.REAL_TIME);
    }
}
