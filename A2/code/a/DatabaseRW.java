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
	private int activeWriter;
	private int waitingWriters;
    private int dbState;

    private int val;

    private static final int NOTUSED = 0;
    private static final int READING = 1;
    private static final int WRITING = 2;

    public DatabaseRW() {
		m = new Mutex();
		readQ = new Condition();
		writeQ = new Condition();
		activeReaders = waitingReaders = activeWriter = waitingWriters = 0;
		dbState = NOTUSED;
		val = 0;
    }


    public void StartRead() {
	    	m.Lock();
		if (dbState == WRITING){
			++waitingReaders;
			readQ.Wait(m);
			--waitingReaders;
		}
		++activeReaders;
		if (activeReaders == 1) dbState = READING;
		m.UnLock();
    }


    public void EndRead() {
		m.Lock();
		--activeReaders;

		if (activeReaders == 0 && waitingWriters > 0) { // I'm the last to read
			dbState = WRITING;
			writeQ.Signal();
		}
		m.UnLock();
    }


    public void StartWrite() {
		m.Lock();
		if (activeWriter == 1 && dbState != NOTUSED){
			++waitingWriters;
			writeQ.Wait(m);
			--waitingWriters;
		}
		activeWriter = 1;
		dbState = WRITING;
		m.UnLock();
    }


    public void EndWrite() {
		m.Lock();
		activeWriter = 0;
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
