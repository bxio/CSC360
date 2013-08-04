import java.util.*;
import java.lang.*;
import uvic.posix.*;
import uvic.disk.*;

/**
	

*/
public class DiskLFU extends DiskScheduler
{
	private static final int CACHE_SIZE = 16;
	private DiskRequest tail = null;
	private int total_requests = 0;
		/**
		 * Creates a disk at the specified start location.
		 *
		 * @param startPosition initializes head at this position
		 */
		public DiskLFU(int startPosition)
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
			//System.println("inserting!");
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
			//System.println("removing!");
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
			int min = Integer.MAX_VALUE;
			int position = 0;
			for(int i=0;i < CACHE_SIZE;i++){
				if(record[i]< min){
					min = record[i];
					position = i;
				}
			}
			return position;
		}

		protected int[] accessedCache(int[] record, int cacheIndexAccessed){
			++record[cacheIndexAccessed];
			return record;
		}
	}
