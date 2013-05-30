package uvic.posix;    // UVic/CS MHMC 22/Aug/2002

/**
 This is a standard POSIX style counting Semaphore.
*/
public class Semaphore {

	/// Create a new Semaphore; its initial value is "i".
	public Semaphore( int i ) {
	    s = init_sem( i );
	}

	/// Wait until s > 0, then decrement it by 1.
	public void Wait() {
	    wait_sem( s );
	}

	/// Increment s by 1; as a result, a waiting thread may be resumed.
	public void Signal() {
	    signal_sem( s );
	}

	/// Return this Mutex's unique instance id.
	public int id() {
	    return s;
	}

	// a unique instance id
	private int s;

	// native methods which provide this class's functionalities
	private native int init_sem( int i );
	private native void wait_sem( int s );
	private native void signal_sem( int s );

}
