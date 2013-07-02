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
		System.println("TO BE FILLED OUT BY YOU");
	}

	/**
	 * Dequeues the current request and schedules next request.
	 */
	protected void remove()
	{
		System.println("TO BE FILLED OUT BY YOU");
	}
}
