/**
 This program creates a number of processes that all wait on a
 shared condition variable. The broadcast() method is used to
 wake up waiting processes. Processes waiting in the condition
 variable's queue must be run in the order of arrival.
 Expected output : "[a,[b,[c,[d,a],b],c],d]"
*/

import java.util.*;
import java.lang.*;
import uvic.posix.*;

class TestBroadcast extends uvic.posix.Thread
{
    static Condition c = new Condition();
	static Mutex	 m = new Mutex();

	static TestBroadcast p1,p2,p3,p4;

	TestBroadcast(String name)
	{
		myname = name;
	}

    static void main(String[] args)
    {
        p1 = new TestBroadcast("a");
        p2 = new TestBroadcast("b");

	  	// kick start all threads
		p1.start(uvic.posix.Thread.SYSTEM);
		p2.start(uvic.posix.Thread.SYSTEM);
    }

	public void run()
	{
		

		System.print("[" + myname);

		if(myname == "a")
		{
			System.print(",");
			m.Lock();
			c.Wait(m)
		}
		else
		{
			System.print(",");
			c.Signal();

		}
		//System.println("Now Running "+myname);

		m.UnLock();

		System.print(myname + "],");
	}

	String myname;
}
