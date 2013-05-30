import uvic.posix.*;
/**
 This class defines a classic bounded buffer using a mutex and two
 condition variables, one for non-empty and one for non-full.
*/
public class BoundedBuffer implements SBuffer{

	private Mutex m;
	private Condition nonfull, nonempty;
	private int buffer[];
	private int size;
	private int in, out, count;
	private int readers[];

	public BoundedBuffer( int n ) {
		m = new Mutex();
		nonfull = new Condition();
		nonempty = new Condition();
		buffer = new int[n];
		size = n;
		count = 0;
		in = 0;
		out = 0;
	}

	/** Start reading an item at location i wait if buffer is empty. */
    public void StartRead( int i ) {
		// DELETEME: Fill in
	}

    /** Done reading. */
    public void EndRead() {
		// DELETEME: Fill in
	}

    /** Start writing an item wait if buffer is full. */
    public void StartWrite() {
		// DELETEME: Fill in
	}

    /** Done writing. */
    public void EndWrite() {
		// DELETEME: Fill in
	}
	

	public void Put( int i ) {
		// DELETEME: Some starting code, but you might need to modify
		m.Lock();
		while (count == size) nonfull.Wait(m);
		buffer[in] = i;
		in = (in + 1) % size;
		++count;
		nonempty.Signal();
		m.UnLock();
	}


	public int Get() {
		// DELETEME: Some starting code, but you might need to modify
		int v;
		m.Lock();
		while (count == 0) nonempty.Wait(m);
		v = buffer[out];
		out = (out + 1) % size;
		--count;
		nonfull.Signal();
		m.UnLock();

		return v;
	}
}
