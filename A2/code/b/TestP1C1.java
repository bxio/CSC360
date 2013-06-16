
import java.util.*;
import java.lang.*;
import uvic.posix.*;

/**
 This program tests the classic Consumer, Producer and Bounded Buffer problem 
 using Mutex and Condition.
*/
class TestP1C1 
{
	static final int SIZE = 10; 
	static final int ITERATIONS = 50;

	static BoundedBuffer bb;
	static Consumer c;
	static Producer p;

	static void main(String[] args)
	{
		bb = new BoundedBuffer(SIZE);

		c = new Consumer(bb, "C1", ITERATIONS);
		p = new Producer(bb, "P1", ITERATIONS);

		// kick start all threads
		p.start(uvic.posix.Thread.REAL_TIME);
		c.start(uvic.posix.Thread.REAL_TIME);
	}
}
