import uvic.posix.*;

/**
 This class defines a shared database for mulitple readers and
 writers to be implemented by you.
*/
public class DatabaseRW implements Database {

	private Mutex m;
	private Condition readQ;
	private Condition writeQ;
	private int waitingReaders;
	private int activeReaders;
	private int waitingWriters;
	private int activeWriters;
	private int dbState;

	private int val;

	private static final int NOTUSED = 0;
	private static final int READING = 1;
	private static final int WRITING = 2;

	public DatabaseRW() {
		m = new Mutex();
		readQ = new Condition();
		writeQ = new Condition();
		activeReaders = waitingReaders = activeWriters = waitingWriters = 0;
		dbState = NOTUSED;
		val = 0;
	}


	public void StartRead() {
		m.Lock();
		while (dbState == WRITING){
			++waitingReaders; //reader is now blocked on writing state
			readQ.Wait(m);
			--waitingReaders;  //switches the state of the reader
		}
		++activeReaders; //switches the state of the reader
		if (activeReaders == 1){
			dbState = READING; // set the state to reading
		}
		m.UnLock();
	}


	public void EndRead() {
		m.Lock();
		--activeReaders;//decrement count of active readers

		if (activeReaders == 0 && waitingWriters > 0) { // I'm the last to read And there is someone waiting to write
			dbState = WRITING; // pass the state to writing
			writeQ.Signal();
		}
		else if(activeReaders == 0)
		{
			dbState = NOTUSED;
		}

		m.UnLock();
	}


public void StartWrite() {
	m.Lock();
	while (dbState == READING){
		++waitingWriters;
		writeQ.Wait(m);
		--waitingWriters;
	}
	++activeWriters;
	dbState = WRITING;
	m.UnLock();
}


public void EndWrite() {
	m.Lock();
	--activeWriters;
	dbState = NOTUSED;
	if(waitingWriters > 0) writeQ.Signal();
	else readQ.Broadcast();
	m.UnLock();
}


public void Put( int i ) {
	val = i;
}


public int Get() {
	return val;
}
}
