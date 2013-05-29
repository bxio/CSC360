package uvic.posix;     // UVic/CS MHMC 20/Aug/2002

//
// This is the standard POSIX style Condition variables.
// A Condition variable MUST be used together a predicate and a Mutex.
// Conceptually, a Condition variable is associated with a boolean
// predicate and a waiting queue. A Mutex must be locked before testing
// the predicate. If the predicate is false, then we need to wait until
// the predicate has changed. Thus, we must unlock the Mutex while we
// we are waiting. When resumed, the Mutex is then automatically relocked.
//

public class Condition {

	public Condition() {
	    c = init_cond();
	}

	  // the calling thread is blocked on this condition
	  // and simultaneously unlock mutex "m"
	public void Wait( Mutex m ) {
	    wait_cond( c, m.id() );
	}

          // the associated predicate has changed; resume
          // a waiting thread if one exists. Otherwise, do nothing
	public void Signal() {
	    signal_cond( c );
	}

	  // return this Condition's unique instance id
	public int id() {
	    return c;
	}

	  // a unique instance id
	private int c;

	  // native methods which provide this class's functionalities
	private native int init_cond();
	private native void wait_cond( int cond_id, int mutex_id );
	private native void signal_cond( int cond_id );

}
