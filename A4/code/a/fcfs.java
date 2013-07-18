/* Test of FCFS policy */

import uvic.posix.*;
import uvic.disk.*;

class fcfs extends uvic.posix.Thread
{
	static DiskFCFS disk;
	static int blocks;
	static final int TOTAL_THREADS = 2; ///< # of DiskRequesters
	static final int TOTAL_REQUESTS = 10; ///< # of requests per DiskRequester
	
	static final int READ = 1;
	static final int WRITE = 0;
	
    static final int[] block_space = {10, 15, 44, 77, 99, 22, 21, 43, 102, 
                                      116, 192, 296, 204, 132, 2, 98, 111, 
                                      74, 32, 55, 66 , 200, 100};
    
	static int seed = 0;
    static int a = 32718;
    static int c = 13;
    static int b = 1<<16;
    

	static void main(String args[])
	{
		disk = new DiskFCFS(0);
		blocks = disk.getTotalBlocks();

		disk.spin(REAL_TIME);
		
		// queue up all requests
		for(int r = 0; r < TOTAL_THREADS; r ++)
		{
			int[][] requests = new int[TOTAL_REQUESTS][3];
			for (int i=0; i < TOTAL_REQUESTS; i++)
			{
				requests[i][0] = random_request(); // Pick a "random" block position to read
				requests[i][1] = READ;
				requests[i][2] = requests[i][0] * -1; // Value, if writing
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
        
		disk.shutdown();
	}
	
	static int random_request()
	{
        int t = ((seed * a) + c);
		seed = t % b;
        c = (int)(t / b);
		return block_space[seed % block_space.length];
	}
}