import java.util.*;
import java.lang.*;
import uvic.posix.*;
import uvic.disk.*;

/**
	

*/
public class DiskLFU extends DiskFCFS
{
	private static final int CACHE_SIZE = 16;

		/**
		 * Creates a disk at the specified start location.
		 *
		 * @param startPosition initializes head at this position
		 */
		public DiskLFU(int startPosition)
		{
			super(startPosition);
		}
	}
