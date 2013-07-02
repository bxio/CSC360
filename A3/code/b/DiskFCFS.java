// UVic/CS PMM 02/Nov/2002

import uvic.posix.*;
import uvic.disk.*;

/**
 * First-come-first-serve implementation of the disk emulator. Requests
 * are processed in the order of arrival.
 */
public class DiskFCFS extends DiskScheduler
{
	private DiskRequest tail = null;
	private int total_requests = 0;

	/**
	 * Creates a disk of the specified size.
	 * @param blocks desired size of disk in blocks 
	 */
	public DiskFCFS(int startPosition)
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
		total_requests++;
		if(current == null) // insert at start of list
			current = tail = dr.enQ(null,current);
		else // insert at the end
			dr.enQ(tail,null);
			tail = dr;
		showQ();
	}

	/**
	 * Dequeues the current request and schedules next request.
	 */
	protected void remove()
	{
		DiskRequest cur = current;
		// update tail reference if required
		if(tail == current)
			tail = null;

		// advance to next request
		current = current.next;

		// remove completed request
		cur.deQ();
		showQ();
	}
}
