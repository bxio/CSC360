import java.util.*;
import java.lang.*;
import uvic.posix.*;


/**
 This program tests if preference is being given to writers.
 Correct output: *W1 and W2 should write until completion, with all of
 the reads performed at the end in one big lump*.
*/

class TestPreference
{
    static final int SIZE = 10; 
    static final int ITERATIONS = 50;

    static DatabaseRW db;
    static Reader r1, r2;
    static Writer w1,w2;

    static void main(String[] args)
    {
        db = new DatabaseRW();

        r1 = new Reader(db,"R1",ITERATIONS);
        r2 = new Reader(db,"R2",ITERATIONS);

        w1 = new Writer(db,"W1",ITERATIONS);
        w2 = new Writer(db,"W2",ITERATIONS);

	  	// kick start all threads
		w1.start(uvic.posix.Thread.REAL_TIME);
		r1.start(uvic.posix.Thread.REAL_TIME);
		w2.start(uvic.posix.Thread.REAL_TIME);
		r2.start(uvic.posix.Thread.REAL_TIME);
    }
}
