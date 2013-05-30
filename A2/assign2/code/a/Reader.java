import java.util.*;
import java.lang.*;
import uvic.posix.*;

/**
 This class defines a Reader thread which reads from a shared
 database.
*/
public class Reader extends uvic.posix.Thread {
	private Database shared_db; ///< The shared structure we operate on.
    private String name; ///< An identifier for our thread.
    private int iterations; ///< The number of times to keep reading.

	/** Create a Reader object passing a descendent of Database. */
    public Reader(Database db, String name, int n ) {
        shared_db = db;
		this.name = name;
		iterations = n;
    }

    public void run() {
		for( int i=0; i < iterations; ++i ) {
			//System.print( "[" + name );
			shared_db.StartRead();
			System.print( name + ": " + shared_db.Get() + " " );
			yield();
			shared_db.EndRead();
			//System.print( name + "]");
		}
    }
}
