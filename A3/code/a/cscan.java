/* Test of CSCAN policy */

import uvic.posix.*;
import uvic.disk.*;

class cscan extends uvic.posix.Thread
{
	static DiskCSCAN disk;
	static int blocks;
	static final int TOTAL_THREADS = 5; ///< # of DiskRequesters
	static final int TOTAL_REQUESTS = 1; ///< # of requests per DiskRequester
	
	static final int READ = 1;
	static final int WRITE = 0;
	
	static int seed = 0;

	static void main(String args[])
	{
		disk = new DiskCSCAN(0);
		blocks = disk.getTotalBlocks();

		disk.spin(REAL_TIME);
		
		// queue up all requests
		for(int r = 0; r < TOTAL_THREADS; r ++)
		{
			int[][] requests = new int[TOTAL_REQUESTS][3];
			for (int i=0; i < TOTAL_REQUESTS; i++)
			{
				requests[i][0] = random_block(); // Pick a "random" block position to read
				// Switch between reading and writing for test purposes
				if (i % 2 == 0)
					requests[i][1] = WRITE; // Write
				else
					requests[i][1] = READ; // Write
				requests[i][2] = requests[i][0]; // Value, if writing
			}
			DiskRequester req = new DiskRequester("d"+r, disk, requests);
			req.start(REAL_TIME);
		}
		
		set(USER);
		// Keep yielding until all requester threads are finished.
		while (uvic.posix.Thread.count() > 2)
		{
			yield();
		}
		disk.save(0);
		disk.shutdown();
	}
	
	static int random_block()
	{
		seed = ((seed * 37) + 11) % blocks;
		return seed;
	}
}
