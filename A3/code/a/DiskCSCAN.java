import uvic.posix.*;
import uvic.disk.*;

/**
 * CSCAN implementation of the disk emulator. The scheduler moves
 * the disk head in the current direction. The head is moved to the
 * start of the disk upon reaching the end of the disk.
 */
public class DiskCSCAN extends DiskScheduler
{
	private DiskRequest tail = null;
	private DiskRequest head = null;
	//The doubly linked list should be sorted in ascending order according to the requested cylinder
	/**
	 * Creates a disk at the specified start location.
	 *
	 * @param startPosition initializes head at this position
	 */
	public DiskCSCAN(int startPosition)
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
		//System.println("Remove Method. Current Position:"+super.getHeadPos());
		DiskRequest cur = current;

		if(head == tail){
			current = null;
		}else if(cur.next == null){
			current = head;
			tail = cur.prev;
			cur.deQ();
		}else{
			current = current.next;
			if(cur == head){
				head = cur.next;
			}
			cur.deQ();
		}


	}
}
