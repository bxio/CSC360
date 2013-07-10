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
		//System.println("Inserting: "+dr.block);
		if(current == null){ // insert at start of list
			current = head = tail = dr.enQ(null,current);
			//System.println("Added to front of list. Head: "+head.block);
		}else{ // Insert dr in position.
			DiskRequest temp=head;
			if(dr.block < temp.block){
				dr.enQ(null,temp);
				head = dr;
			}else{
				while((temp.next != null) && (temp.next.block <= dr.block)){
					temp = temp.next;
				}
				dr.enQ(temp,temp.next);
				if(dr.next == null){
					tail = dr;
				}
			}
			showQ();
			//System.println("Anchors"+head.block+" -> "+tail.block);
		}
			
	}

	/**
	 * Dequeues the current request and schedules next request.
	 */
	protected void remove()
	{
		System.println("Removing. Head position is "+current.block);

		

		
	}
}
