

import java.util.*;
import java.lang.*;
import uvic.posix.*;

/**
 Test the Consumer-Producer problem with 3 consumers.
*/
class TestP1C3 
{
    static final int SIZE = 10; 
    static final int ITERATIONS = 50;

    static BoundedBuffer bb;
    static Consumer c1, c2, c3;
    static Producer p1;

    static void main(String[] args)
    {
        bb = new BoundedBuffer(SIZE);

        p1 = new Producer(bb, "P1", ITERATIONS);
        c1 = new Consumer(bb, "C1", ITERATIONS);
		c2 = new Consumer(bb, "C2", ITERATIONS);
		c3 = new Consumer(bb, "C3", ITERATIONS);

		// kick start all threads
		c1.start(uvic.posix.Thread.REAL_TIME);
		c2.start(uvic.posix.Thread.REAL_TIME);
		c3.start(uvic.posix.Thread.REAL_TIME);
		p1.start(uvic.posix.Thread.REAL_TIME);
    }
}
