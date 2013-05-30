import java.util.*;
import java.lang.*;
import uvic.posix.*;

/**
 This class defines a Writer thread which writes into a shared
 Database object.
*/
public class Writer extends uvic.posix.Thread {
	private Database shared_db; ///< The shared structure we operate on.
    private String name; ///< An identifier for our thread.
	private int iterations; ///< The number of times to keep reading.

	/** Create a writer. */
    public Writer(Database db, String name, int n) {
		shared_db = db;
		this.name = name;
		iterations = n;
	}

    public void run() {
		for( int i=0; i < iterations; ++i ) {
			//System.print( "[" + name );
			shared_db.StartWrite();
			shared_db.Put( i );
			System.println( "\n" + name + ": - " + i );
			yield();
			shared_db.EndWrite();
			//System.print( name + "]");
		}
    }
}
