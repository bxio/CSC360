
package uvic.posix;

import java.lang.Runnable;
import java.lang.System;

/**
 * Class that will run in a separate thread must subclass Thread.
 * By default, main() is created as a SYSTEM_LEVEL thread.
 *
 */
public class Thread implements Runnable
{
    /* currently there are only 4 levels; 0 is the highest */
    public static final int SYSTEM    = 0; ///< FCFS
    public static final int REAL_TIME = 1; ///< Round-robin (quantum = 1)
    public static final int USER      = 2; ///< Round-robin (quantum = 2)
    public static final int DAEMON    = 3; ///< FCFS

    private Runnable target;

    /**
     * Creates a Thread object.
     */
    public Thread()
    {
        this(null);
    }

    /**
     * Creates a Thread object with specified target.
     */
    public Thread(Runnable target)
    {
        this.target = target;

    }

    /** Starts the execution of this thread at the specified level. */
    public synchronized native void start(int level);

    /** Terminate this thread */
    public static native final void stop();

    /** Voluntarily relinquishes the use of processor by this thread. */
    public static native void yield();

    /** Change this thread's level; as a result, pre-emption may occur. */
    public static native void set( int level );

    /**
     * Subclasses must override this method to implement required functionality.
     */
    public void run()
    {
        if (target != null)
            target.run();
    }
}
