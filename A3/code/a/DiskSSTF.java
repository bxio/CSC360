// UVic/CS PMM 02/Nov/2002

import uvic.posix.*;
import uvic.disk.*;

/**
 * Shortest-seek-time-first implementation of the disk emulator.
 * Requests nearest to the current head position are processed 
 * first.
 */
public class DiskSSTF extends DiskScheduler
{
	private DiskRequest tail = null;
	private DiskRequest head = null;
	//The doubly linked list should be sorted in ascending order according to the requested cylinder
	/**
	 * Creates a disk at the specified start location.
	 *
	 * @param startPosition initializes head at this position
	 */
	public DiskSSTF(int startPosition)
	{
		super(startPosition);
	}

	/**
	 * Enqueues the request based on its position. Requests are stored
	 * in a doubly linked list. Elements are sorted by block value.
	 *
	 * @param dr	disk request item to be inserted into the list
	 */
	protected void insert(DiskRequest dr)
	{
		System.println("TO BE FILLED OUT BY YOU");
		DiskRequest start = this.head;
		if(start == null){
			//dll is empty. Add it to the front.		
		}else{
			//find the correct position this request should be and EnQ it there.
		}
	}

	/**
	 * Dequeues the current request and schedules next request.
	 */
	protected void remove()
	{
		System.println("TO BE FILLED OUT BY YOU");
		DiskRequest prev = null;
		//check the requests on either side to see which one is closer to your current position.
		//pick the request to serve, move to it, and DeQ the previous request.
	}
}
