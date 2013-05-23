/**
 This class defines a "spin" lock (a busy-wait polling lock). Since
 we don't have an atomic "swap" or "test-and-set" instruction, we may
 still get into trouble. 
 
 (Where and How?)
 
 But, most of the time, it will work.
*/
public class Spinlock {

    private boolean locked;

	/** Create new Spinlock. */
    public Spinlock() {
		locked = false;
    }

	/** Block until lock is free, then lock.*/
    public void lock() {
		while (locked);
		locked = true;
    }

	/** Release the lock. */
    public void unlock() {
        locked = false ;
    }
}
