import uvic.posix.*;

/**
This class defines a shared database for mulitple readers and
writers to be implemented by you.
*/
public class DatabaseRW implements Database {

	private Mutex m;
	private Condition readQ;
	private Condition writeQ;
	private int waitingReaders;/**< counts how many readers waiting **/
	private int activeReaders; /**< counts how many readers are currently reading content **/
	private boolean activeWriter;/**< Flag to see whether there is an active writer or not **/
	private int waitingWriters;/**< counts how many writers are waiting to write **/
	private int dbState;

	private int val;
	//define for DBState
	private static final int NOTUSED = 0;
	private static final int READING = 1;
	private static final int WRITING = 2;

	public DatabaseRW() {
		m = new Mutex();
		readQ = new Condition();
		writeQ = new Condition();
		activeReaders = waitingReaders = waitingWriters = 0;
		activeWriter = false;
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
		if (activeReaders == 1) {
			dbState = READING;
		}
		m.UnLock();
	}


	public void EndRead() {
		m.Lock();
		--activeReaders;
	if (activeReaders == 0 && waitingWriters > 0) { // I'm the last to read
		dbState = WRITING;//passes state directly to Writing
		writeQ.Signal();//signal the writeQ
	}
		m.UnLock();
	}


	public void StartWrite() {
		m.Lock();
		if (activeWriter == true && dbState != NOTUSED){//If there is someone else writing AND state is not free, block me
			++waitingWriters;
			writeQ.Wait(m);
			--waitingWriters;
		}
		activeWriter = true;
		dbState = WRITING;
		m.UnLock();
	}


	public void EndWrite() {
		m.Lock();
		activeWriter = false;
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
