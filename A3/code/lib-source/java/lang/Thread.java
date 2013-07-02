package java.lang;

/*
 * Class that will run in a separate thread must subclass Thread.
 *
 */
public class Thread implements Runnable
{

    private Runnable target;

    /*
     * Creates a Thread object.
     */
    public Thread()
    {
        this(null);
    }

    /*
     * Creates a Thread object with specified target.
     */
    public Thread(Runnable target)
    {
        this.target = target;

    }

      /* starts the execution of this thread at the specified level */
    public synchronized native void start(int level);

      /* terminate this thread */
    public native final void stop();

      /* voluntarily relinquishes the use of processor by this thread */
    public static native void yield();

    /*
     * Subclasses must override this method to implement required functionality.
     */
    public void run()
    {
        if (target != null)
            target.run();
    }
}
