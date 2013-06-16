
import java.util.*;
import java.lang.*;
import uvic.posix.*;


/**
 This class defines a Producer thread which writes into a shared
 bounded buffer.
*/
public class Producer extends uvic.posix.Thread {
	private BoundedBuffer bb;
	private String name;
    private int iterations;
	
    public Producer(BoundedBuffer buf, String name, int n ) {
        bb = buf;
		this.name = name;
		iterations = n;
    }

    public void run() {
		
		for( int i=0; i < iterations; ++i ) {
			//System.print( name + "{" );
			bb.StartWrite();
			delay((i*31 + 5) % 11);
			bb.Put( i );
			System.println("\n" + name + ": [" + i + "]" );
			bb.EndWrite();
			//System.print( name + "}" );
		}
    }
	
	/** Idle for approximately t ms. *Approximately* in this case is 
	    intentionally vague as it largely depends on quantum time and 
		number of threads.
	*/
    public void delay(int t) {
		int  s = System.currentTimeMillis();
		while (System.currentTimeMillis() - s < t); // do nothing
	}
}
