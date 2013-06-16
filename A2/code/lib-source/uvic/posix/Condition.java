package uvic.posix;

/**
 This is the standard POSIX style Condition variables.
 A Condition variable MUST be used together a predicate and a Mutex.
 Conceptually, a Condition variable is associated with a boolean
 predicate and a waiting queue. A Mutex must be locked before testing
 the predicate. If the predicate is false, then we need to wait until
 the predicate has changed. Thus, we must unlock the Mutex while we
 we are waiting. When resumed, the Mutex is then automatically relocked.
*/
public class Condition {

	public Condition() {
	    c = init_cond();
	}

	/** The calling thread is blocked on this condition
		and simultaneously unlock mutex "m". */
	public void Wait( Mutex m ) {
	    wait_cond( c, m.id() );
	}

	/** The associated predicate has changed; resume
		a waiting thread if one exists. Otherwise, do nothing. */
	public void Signal() {
	    signal_cond( c );
	}

	/** The associated predicate has changed; resume
        all waiting threads. */
	public void Broadcast() {
	    broadcast_cond( c );
	}

	/// Return this Condition's unique instance id.
	public int id() {
	    return c;
	}

	private int c; // A unique instance id.

	// native methods which provide this class's functionalities
	private native int init_cond();
	private native void wait_cond( int cond_id, int mutex_id );
	private native void signal_cond( int cond_id );
	private native void broadcast_cond( int cond_id );

}
