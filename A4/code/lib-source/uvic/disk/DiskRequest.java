package uvic.disk;

import uvic.posix.*;

/**
 * Container class used by disk object for creating a
 * doubly linked priority queue of disk requests. 
 */
public class DiskRequest
{
	Condition	finished 	= new Condition();
	public boolean read 	= true;
	public int block		= 0;
	public DiskRequest prev	= null;
	public DiskRequest next	= null;
	public int data 		= -1;

	public DiskRequest()
	{
	}

	/**
	 * Inserts the request into the list using
	 * with the specified previous and next pointers.
	 *
	 * @param p previous element in list
	 * @param n next element in list
	 */
	public DiskRequest enQ(DiskRequest p, DiskRequest n)
	{
		prev 	= p;
		next 	= n;

		if(prev != null) prev.next = this;
		if(next != null) next.prev = this;

		return this;
	}

	/**
	 * Removes the diskrequest object from the list and
	 * returns it.
	 *
	 * @return removed element
	 */
	public DiskRequest deQ()
	{
		if(prev != null) prev.next = next;
		if(next != null) next.prev = prev;

		prev = null;
		next = null;

		return this;
	}
}
