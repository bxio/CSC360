/* Test basic WRITE of one file. */

import uvic.posix.*;
import uvic.disk.*;

class TestWrite extends uvic.posix.Thread
{
	static void main(String args[])
	{
		DiskFCFS disk = new DiskFCFS(0);
		disk.spin(REAL_TIME);
		
		FileSystem fs = new FileSystem(disk);
		
		int[] data = {1000,1001,1002,1003,1004,1005,1006};
		FileRequester req = new FileRequester(fs, "tb1", 0, data, FileRequester.WRITE_FILE);
		req.start(REAL_TIME);
		
		set(REAL_TIME);
		// Keep yielding until all requester threads are finished.
		while (uvic.posix.Thread.count() > 3)
		{
			yield();
		}
		fs.shutdown();
		// Keep yielding until file system is finished.
		while (uvic.posix.Thread.count() > 2)
		{
			yield();
		}
		// Write the disk to file "vdisk_99"
		disk.save(99);
		disk.shutdown();
	}
}
