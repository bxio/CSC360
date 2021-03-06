package uvic.posix;

/**
 This is a standard POSIX style Mutex.
 It is basically a binary semaphore. 
 Initially, it is free.
*/
public class Mutex {

	/// Create a new Mutex; initially free.
	public Mutex() {
	    m = init_mutex();
	}

	/// Wait until Mutex is free, then lock it.
	public void Lock() {
	    lock_mutex( m );
	}

	/** Unlock this Mutex; the calling thread must have locked
		it previously, otherwise it is a no-op. */
	public void UnLock() {
	    unlock_mutex( m );
	}

	/// Return this Mutex's unique instance id.
	public int id() {
	    return m;
	}

	// A unique instance id
	private int m;

	// native methods which provide this class's functionalities
	private native int init_mutex();
	private native void lock_mutex( int m );
	private native void unlock_mutex( int m );

}
