import java.util.*;
import java.lang.*;
import uvic.posix.*;
import uvic.disk.*;

/**
	The DiskRequester is a thread that takes an array of requests and sends 
	them one after the other to a DiskScheduler.

*/
public class DiskRequester extends uvic.posix.Thread
{
	private int[][] requests; ///< An array of requests in the form [block, (0-WRITE, 1-READ), VALUE)].
	private DiskScheduler ds;
	private String name; ///< Useful for debugging.
	
	/**
		Initialize the DiskRequester.
	
		@param name Help identify which DiskRequester is running.
		@param ds Operate on this DiskScheduler.
		@param requests An array of length 3 arrays (i.e. {15, 0, 9001}) that should 
		map to {block, [read(1) or write(0)], data (for write)}
	*/
	public DiskRequester(String name, DiskScheduler ds, int[][] requests)
	{
		this.name = name;
		this.ds = ds;
		this.requests = requests;
	}
	
	/**
		Run through the queue of disk requests.
	*/
	public void run()
	{
		for (int i=0; i < requests.length; i++)
		{
			if (requests[i][1] == 1)
			{
				System.println(name +"] Read Request: " + requests[i][0]);
				int data = ds.read(requests[i][0]);
				System.println(name +"] Read Complete: " + requests[i][0] + " (" + data +")");
			}
			else
			{
				System.println(name +"] Write Request: " + requests[i][0] + " (" + requests[i][2] +")");
				ds.write(requests[i][0], requests[i][2]);
				System.println(name +"] Write Complete: " + requests[i][0]);
			}
		}
	}
}