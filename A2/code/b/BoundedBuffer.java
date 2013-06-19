import uvic.posix.*;
/**
 This class defines a classic bounded buffer using a mutex and two
 condition variables, one for non-empty and one for non-full.
*/
public class BoundedBuffer implements SBuffer{

	private Mutex m;
	private Condition nonfull, nonempty, waitingReaders;
	private int buffer[];
	private int size;
	private int in, out, count, countReaders, rd, wr;
	private boolean writerActive;
	

	public BoundedBuffer( int n ) {
		m = new Mutex();
		nonfull = new Condition();
		nonempty = new Condition();
		waitingReaders = new Condition();
		buffer = new int[n];
		size = n;
		count = 0;
		countReaders = 0;
		in = 0;
		out = 0;
		rd = 0;
		wr = 0;
		writerActive = false;
	}

	/** Start reading an item at location i wait if buffer is empty. */
    public void StartRead( int i ) {
		int j = i%size;
		m.Lock();
		
		if(j == out){ //if the thread is trying to read the same location as "out", continue
			++countReaders;
		}
		else { //if the thread is trying to read ahead of the "out" location
			waitingReaders.Wait(m); //wait for other readers to finish
			++countReaders;
		}		
		m.UnLock();
	}

    /** Done reading. */
    public void EndRead() {
		if(countReaders == 0){

			waitingReaders.Broadcast();
			
		}	
	}

    /** Start writing an item wait if buffer is full. */
    public void StartWrite() {
		
		
		
			
	}

    /** Done writing. */
    public void EndWrite() {
		++wr;
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
		--countReaders;
		if(countReaders == 0) {
			out = (out + 1) % size;
			--count;
		}
		nonfull.Signal();
		m.UnLock();

		return v;
	}
}
