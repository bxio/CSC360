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

class UnitTest extends uvic.posix.Thread
{
    static Condition c = new Condition();
	static Mutex	 m = new Mutex();

	static UnitTest p1,p2,p3,p4;

	UnitTest(String name)
	{
		myname = name;
	}

    static void main(String[] args)
    {
        p1 = new UnitTest("a");
        p2 = new UnitTest("b");
        p3 = new UnitTest("c");
        //p4 = new UnitTest("d");

	  	// kick start all threads
		p2.start(uvic.posix.Thread.SYSTEM);
		p1.start(uvic.posix.Thread.SYSTEM);
		//yield();
		p3.start(uvic.posix.Thread.SYSTEM);
		//p4.start(uvic.posix.Thread.SYSTEM);
    }

	public void run()
	{
		System.print("[" + myname);

		m.Lock();
		if(myname == "a"){
			c.Wait(m);
		}
		//if(myname == "b"){
			//c.Signal();
		//}
		c.Signal();
		m.UnLock();
		System.print(myname + "],");
	}

	String myname;
}
