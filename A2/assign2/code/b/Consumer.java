//
// This class defines a "Consumer" thread which reads from a shared
// bounded buffer.
// The consumer prints the value after each read.
//
import java.util.*;
import java.lang.*;
import uvic.posix.*;

public class Consumer extends uvic.posix.Thread {
	private BoundedBuffer bb;
    private int iterations;
	private String name;

    public Consumer(BoundedBuffer buf, String name, int n ) {
        bb = buf;
		this.name = name;
		iterations = n;
    }

    public void run() {
		for( int i=0; i < iterations; ++i ) {
			//System.print( name + "\\" );
			bb.StartRead(i);
			System.println( name + ": (" + i + "," + bb.Get() + ") " );
			bb.EndRead();
			//System.print( name + "/" );
		}
    }
}
