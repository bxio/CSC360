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
	private int in, out, count, countReaders;

	public BoundedBuffer( int n ) {
		m = new Mutex();
		nonfull = new Condition();
		nonempty = new Condition();
		waitingReaders = new Condition();
		buffer = new int[n];
		size = n;
		count = 0; /**< the number of things in the buffer **/
		countReaders = 0; /**< Counts the number of active readers (readers currently reading content) */
		in = 0;
		out = 0;
	}

	/** Start reading an item at location i wait if buffer is empty. */
    public void StartRead( int i ) {
		int j = i%size; /**< j is the virtual location, the "wrapped-around" location where you are trying to read */
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
			waitingReaders.Broadcast(); //I'm last to read element. Everyone waiting for me should wake up and continue
		}	
	}

    /** Start writing an item wait if buffer is full. */
    public void StartWrite() {
			
	}

    /** Done writing. */
    public void EndWrite() {
		
	}

	public void Put( int i ) {
		m.Lock();
		while (count == size) {
			nonfull.Wait(m);
		}
		buffer[in] = i;
		in = (in + 1) % size;
		++count;
		nonempty.Signal();
		m.UnLock();
	}

	public int Get() {
		int v;
		m.Lock();
		while (count == 0){
			nonempty.Wait(m);	
		} 
		v = buffer[out];
		--countReaders;
		if(countReaders == 0) { //We only increment out if you are the last reader. If you aren't, you have to wait for everyone else to catch up to you
			out = (out + 1) % size;
			--count;
		}
		nonfull.Signal(); //we removed an element, queue is no longer full
		m.UnLock();

		return v;
	}
}
