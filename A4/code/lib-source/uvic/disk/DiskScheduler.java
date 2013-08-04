package uvic.disk;

import java.util.*;
import java.lang.*;
import uvic.posix.*;
import uvic.disk.*;

/**
 The DiskScheduler class acts as a husk class for different disk scheduler 
 implementations.
 It provides a basic read/write disk scheduling framework using posix mutex 
 and condition variables to achieve interprocess synchronization. This is 
 not a standalone class and requires extension to achieve full functionality.
 The insert() and remove() methods must be implemented in order to establish
 scheduling.
 
 
 The abstract method deQ must be implemented in order to establish a scheduling
 order.

*/
abstract public class DiskScheduler extends Disk
{
	/**
	 * The currently scheduled disk request
	 */
	protected DiskRequest 	current 	= null;
	protected int			blockCnt = 0;

	private boolean			isRunning 	= false;
	private int				maxLiveReq  = 20;
	private int				headPos 	= 0;
	private int				headTravel	= 0;
	private	int				totalReq	= 0;
	private DiskRequest		free_q		= null;

	private Mutex			m				= new Mutex();
	private Condition		newRequest		= new Condition();
	private Condition		spaceAvailable	= new Condition();
	private static final int CACHE_SIZE = 16;

	protected int[] cacheContent;///< content of the block positions
	protected int[] cacheIndex;///< the block positions
	protected int[] cacheRecord;///< lru or lfu helper
	protected boolean[] cacheDirtyBit; ///< records whether the block changes or not
	protected int cacheCount;

	public void createCache(){
		this.cacheContent = new int[CACHE_SIZE];
		this.cacheIndex = new int[CACHE_SIZE];
		this.cacheRecord = new int[CACHE_SIZE];
		this.cacheDirtyBit = new boolean[CACHE_SIZE];
		this.cacheCount = 0;
	}

	public boolean isInCache(int blockPosition){
		for(int i=0;i<CACHE_SIZE;i++){
			if(cacheIndex[i] == blockPosition){
				return true;
			}
		}
		return false;
	}
	/**
	 * Dequeues the current I/O request and selects the next
	 * request for processing. This method must be implemented by derived 
	 * classes. The scheduling policy of the disk emulator is derived from 
	 * this method and the insert method.
	 */
	public int getCacheIndex(int blockIndex){
		for(int i=0;i<CACHE_SIZE;i++){
			if(cacheIndex[i] == blockIndex){
				return i;
			}
		}
		return -1;
	}

	public int getContent(int blockIndex){
		if(this.isInCache(blockIndex)){
			return cacheContent[this.getCacheIndex(blockIndex)];
		}
		return -1;
	}

	abstract protected int findVictim(int[] record);
	
	abstract protected int[] accessedCache(int[] record, int cacheIndexAccessed);

	public void updateRecord(int cacheIndexAccessed){
		this.cacheRecord  = this.accessedCache(this.cacheRecord,cacheIndexAccessed);
	}
	
	public void insertIntoCache(int blockIndex, int blockContent){
		if(this.cacheCount < CACHE_SIZE){//cache is not full
			this.cacheContent[cacheCount] = blockContent;
			this.cacheIndex[cacheCount] = blockIndex;
			this.cacheRecord[cacheCount] = 0;
			this.cacheCount++;
		}else{ //cache is full
			int victim = findVictim(this.cacheRecord);
			//replace the victim
			this.replace(victim, blockIndex, blockContent);
		}
	}

	public void replace(int victimIndex, int blockIndex, int blockContent){
		if(cacheDirtyBit[victimIndex] == true){
			//content has changed. Write it to disk before replacing
			this.writeToDisk(cacheIndex[victimIndex], cacheContent[victimIndex]);
		}
		//kick victim
		this.cacheContent[victimIndex] = blockContent;
		this.cacheIndex[victimIndex] = blockIndex;
		this.cacheRecord[victimIndex] = 0;
		this.cacheDirtyBit[victimIndex] = false; //the content is fresh, just fetched from disk
	}

	public void flush(){
		for(int i=0;i<CACHE_SIZE;i++){
			if(this.cacheDirtyBit[i]){
				this.writeToDisk(this.cacheIndex[i],this.cacheContent[i]);
			}
		}
	}

	/**
	 * Dequeues the current I/O request and selects the next
	 * request for processing. This method must be implemented by derived 
	 * classes. The scheduling policy of the disk emulator is derived from 
	 * this method and the insert method.
	 */
	abstract protected void remove();
	
	/**
	 * Inserts a new request into the wait queue. Derived classes implement this
	 * method. The organization of the data structure is defined here.
	 */
	abstract protected void insert(DiskRequest dr);
	
	/**
	 * Constructs a Disk class object with specified size and request capacity.
	 * @param 	liveReq		the maximum number of live requests that can be scheduled
	 * 						simultaneously.
	 * @param	blocks the size of the disk in blocks. Each block holds a 
	 * 						single block.
	 */
	public DiskScheduler(int startPosition)
	{
		blockCnt = size_disk();
		//System.println("Opening");
		open_disk(startPosition);
		headPos = startPosition;
		// create pool of unused request
		for(int i = 0; i < maxLiveReq; i++)
		{
			DiskRequest dr = new DiskRequest();
			free_q = dr.enQ(null,free_q);
		}
		//initiate the cache
		this.cacheContent = new int[CACHE_SIZE];
		this.cacheIndex = new int[CACHE_SIZE];
		this.cacheRecord = new int[CACHE_SIZE];
		this.cacheDirtyBit = new boolean[CACHE_SIZE];
		this.cacheCount = 0;
	}

	/** 
	*
	* Return the number of blocks in the disk.
	*/
	public int getTotalBlocks()
	{
		return blockCnt;
	}
	
	/**
	*
	* Return the current head position.
	*/
	public int getHeadPos()
	{
		return headPos;
	}
	
	/**
	 * Starts the disk emulator. Processing of disk requests begins.
	 */
	public void spin(int level)
	{
		m.Lock();

		if(!isRunning)
		{
			// start the worker thread
			super.start(level);
			isRunning = true;
		}

		m.UnLock();
	}


	/**
	 * Stops the disk emulator. Pending requests are cancelled.
	 */
	public void shutdown()
	{
		//TODO: Put flush method call here
		m.Lock();

		if(isRunning)
		{
			isRunning = false;

			// wake up run thread 
			newRequest.Signal();

			// cancel all live requests
			while(current != null)
			{
				DiskRequest cur = current;
				if(current.next != null)
					current = current.next;
				else 
					current = current.prev;

				cur.data = -1;
				cur.finished.Broadcast();
				cur.deQ();
			}

			// wake up pending requests
			spaceAvailable.Broadcast();	

		}
		System.println("------------------------------------");
		close_disk();

		System.println("");
		System.println("Total disk head travel distance:" + headTravel);
		System.println("Total satisfied disk requests:" + totalReq);

		m.UnLock();
	}

	/**
	 * Reads the specified block into the specified data buffer.
	 * This method blocks until the request is satisfied or 
	 * an error occurs. 
	 *
	 * @param	position	the zero based block number
	 */
	public int read(int position)
	{
		m.Lock();
		System.println("Reading!");
		int result = 0;

		//first, check the cache for the block.
		if(this.isInCache(position)){
			result = this.getContent(position);
			this.updateRecord(this.getCacheIndex(position));
		}else{
			// get free request
			DiskRequest dr = getRequest(position, -1, true);

			// queue up the request
			insert(dr);

			// let disk thread know that a new request has arrived
			newRequest.Signal();

			// wait for request to finish
			dr.finished.Wait(m);

			result = dr.data;

			// release request 
			releaseRequest(dr);

			//insert it into cache
			insertIntoCache(position,result);
			
		}
		m.UnLock();
		return result;
	}

	/**
	 * Writes a block of data at the specified block. This method blocks
	 * until the request is completed or an error occurs. 
	 * @param	position the zero based block number
	 * 			buffer	the data to be written
	 */
	public void write(int position, int value)
	{
		m.Lock();
		if(this.isInCache(position)){
			//block is in the cache.
			int temp = this.getCacheIndex(position);
			this.cacheContent[temp] = value;
			this.cacheDirtyBit[temp] = true;
			this.updateRecord(this.getCacheIndex(position));
		}else{
			// get free request
			DiskRequest dr = getRequest(position,value,false);

			// queue up the request
			insert(dr);

			// let disk thread know that a new request has arrived
			newRequest.Signal();

			// wait for request to finish
			dr.finished.Wait(m);

			this.insertIntoCache(position, value);
			// release request 
			releaseRequest(dr);	
		}
		m.UnLock();
	}


	/**
	 * Writes a block of data at the specified block directly to disk. This method blocks
	 * until the request is completed or an error occurs. 
	 * @param	position the zero based block number
	 * 			buffer	the data to be written
	 */
	public void writeToDisk(int position, int value)
	{
		m.Lock();

		// get free request
		DiskRequest dr = getRequest(position,value,false);

		// queue up the request
		insert(dr);

		// let disk thread know that a new request has arrived
		newRequest.Signal();

		// wait for request to finish
		dr.finished.Wait(m);

		// release request 
		releaseRequest(dr);

		m.UnLock();
	}

	/**
	 * The disk worker thread.
	 */
	public void run()
	{
		m.Lock();

		while(isRunning)
		{

			if(current == null)
			{
				// wait for new element to arrive
				newRequest.Wait(m);
			}
			else
			{
				
				if(headPos != current.block)
				{
					m.UnLock();
					seek(current.block);
					m.Lock();
				}
				else
				{
					//System.println("Serving Request: " + current.block + " - " + current.read); 
					if(current.read)
					{
						current.data = read_disk();
					}
					else
					{
						write_disk(current.data);
					}
					
					// signal waiting thread
					current.finished.Signal();
					// update request counter
					totalReq++;
					// remove current request from queue
					remove();
				}
				
			}
		}
		m.UnLock();
	}

	/**
	 * This method forces the disk head to seek to the specified position.
	 *
	 * @param position zero based block count
	 */
	protected void seek(int position)
	{
		System.println(" -- Seeking Request: " + position);
		//m.UnLock();
		seek_disk(position);
		int travelDistance = Math.abs(headPos - position);
		headTravel += travelDistance;
		headPos = position;
		//m.Lock();
	}
	
	/** This method obtains and initializes a disk request from the free pool
	 */
	private DiskRequest getRequest(int position, int buffer, boolean read )
	{
		// wait for available request
		if(free_q == null)
			spaceAvailable.Wait(m);

		// get first available request
		DiskRequest dr = free_q;
		free_q = free_q.next;

		dr.block = position;
		dr.data  = buffer;
		dr.read  = read;
		return dr.deQ();
	}

	/** This method releases a request to the free pool.
	 */
	private void releaseRequest(DiskRequest dr)
	{
		// add free request to pool
		free_q = dr.enQ(null,free_q);
			
		// wake up the threads waiting for a request
		spaceAvailable.Signal();
	}

	/** debugging method for displaying list of live requests.
		**WARNING**: Be wary of where you cause this. Calling it
		at the wrong time can cause a seg fault.
	*/
	protected void showQ()
	{
		DiskRequest dr = current;

		System.print(" * Request list: ");

		if(dr == null)
		{
			System.println("");
			return;
		}
		
		while(dr.prev != null)
			dr = dr.prev;
		
		while(dr != null)
		{
			System.print(dr.block + " ");
			dr = dr.next;
		}
		System.println("");
	}
}
