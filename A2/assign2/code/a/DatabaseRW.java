import uvic.posix.*;

/**
 This class defines a shared database for mulitple readers and
 writers to be implemented by you. 
*/
public class DatabaseRW implements Database {

	private Mutex m;
    private Condition okToRW;
    private int readers; ///< no. of pending/reading readers 
    private int dbState;

    private int val;

    private static final int NOTUSED = 0;
    private static final int READING = 1;
    private static final int WRITING = 2;

    public DatabaseRW() {  
		m = new Mutex();
		okToRW = new Condition();
		readers = 0;
		dbState = NOTUSED;
		val = 0;
    }


    public void StartRead() {
		m.Lock();
		while (dbState == WRITING) okToRW.Wait(m);
		++readers;
		if (readers == 1) dbState = READING;
		m.UnLock();
    }


    public void EndRead() {
		m.Lock();
		--readers;
		if (readers == 0) { // I'm the last to read
			dbState = NOTUSED;
			okToRW.Broadcast();
		}
		m.UnLock();
    }


    public void StartWrite() {
		m.Lock();
		while (dbState != NOTUSED) okToRW.Wait(m);
		dbState = WRITING;
		m.UnLock();
    }


    public void EndWrite() {
		m.Lock();
		dbState = NOTUSED;
		okToRW.Broadcast();
		m.UnLock();
    }


    public void Put( int i ) {
		val = i;
    }


    public int Get() {
		return val;
    }
}
