import java.util.*;
import java.lang.*;
import uvic.posix.*;
import uvic.disk.*;

/**
	

*/
public class DiskLRU extends DiskScheduler
{
	private static final int CACHE_SIZE = 16;
	private DiskRequest tail = null;
	private int total_requests = 0;

	public DiskLRU(int startPosition)
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
		System.println("inserting!");
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
		System.println("removing!");
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

	protected int findVictim(int[] record){
		int max = 0;
		int position = 0;
		for(int i=0;i < CACHE_SIZE;i++){
			if(record[i]> max){
				max = record[i];
				position = i;
			}
		}
		return position;
	}

	protected int[] accessedCache(int[] record, int cacheIndexAccessed){
		for(int i=0;i<CACHE_SIZE;i++){
			if(record[i] == 0){
				record[i] =1;
			}else{
				record[i] = record[i] << 1 +1;
			}
		}
		//reset the most recently used item
		record[cacheIndexAccessed]  = 0;
		return record;
	}
}
