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
	
	//Start block cache variables and methods
	private static final int CACHE_SIZE = 16;
	private int[] cache;
	private int[] index;
	private int[] record;
	private boolean[] dirtyBit;
	/**
	 * Finds a victim block to replace, given the block cache's record. This method must be implemented by derived 
	 * classes. The scheduling policy of the disk emulator is derived from 
	 * this method and the insert method.
	 * @param record the record of the block cache
	 */
	abstract protected int findVictim(int[] record);

	/**
	 * Instantiates the Block Cache.
	 *
	 */
	public void createBlockCache(){
		this.cache = new int[CACHE_SIZE];
		this.index = new int[CACHE_SIZE];
		this.dirtyBit = new boolean[CACHE_SIZE];
		this.record = new int[CACHE_SIZE];
	}

	/**
	 * Returns whether the block at the requested index is in the cache or not
	 *
	 * @param	request	 the disk index position of the requested block
	 * @return	true if the block is in the cache, false otherwise.
	 */
	public boolean contains(int request){
		for(int i=0;i<CACHE_SIZE;i++){
			if(index[i]==request){
				return true;
			}
		}
		return false;
	}
	/**
	 * Checks whether the given block index is in the cache, if so, returns its index.
	 *
	 * @param	request	 the disk index position of the requested block
	 * @return	the position of the block in the blockcache's various indices, 
	 * -1 if the block is not found.
	 */
	public int find(int request){
		for(int i=0;i<CACHE_SIZE;i++){
			if(index[i]==request){
				return i;
			}
		}
		return (-1);
	}
	/**
	 * Returns whether the block at the requested index has been edited or not.
	 *
	 * @param	request	 the disk index position of the requested block
	 * @return	whether the block has been edited
	 */
	public boolean wasEdited(int request){
		return dirtyBit[find(request)];
	}
	/**
	 * Replaces the block cache's item with a given block's position and content.
	 * Writes the old block to disk if it's edited.
	 *
	 * @param	out	 the disk index position of the requested block
	 * @param	inPos	the position of the new block coming into the cache
	 * @param	inContent	the content of the new block
	 * @return	true if the block is in the cache, false otherwise.
	 */
	public void replace(int out, int inPos, int inContent){
		//first, write the block if it's dirty
		if(dirtyBit[out]){
			//write the edited block to disk.
			write(index[out], cache[out]);
		}
		//replace the old block with the new block.
		cache[out] = inContent;
		index[out] = inPos;
		dirtyBit[out] = false;
	}
	/**
	 * Writes all the blocks that have been edited to the disk.
	 *
	 */
	public void flush(){
		for(int i=0;i<CACHE_SIZE;i++){
			if(dirtyBit[i]){
				//Schedule a disk request and write this to disk.
				write(index[i], cache[i]);
			}
		}

	}
	//End block cache variables and methods

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
		createBlockCache();
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

		//check the cache first


		// get free request
		DiskRequest dr = getRequest(position, -1, true);

		// queue up the request
		insert(dr);

		// let disk thread know that a new request has arrived
		newRequest.Signal();

		// wait for request to finish
		dr.finished.Wait(m);

		int result = dr.data;
		
		// release request 
		releaseRequest(dr);

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