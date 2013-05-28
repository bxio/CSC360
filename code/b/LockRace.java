
import java.util.*;
import java.lang.*;
import uvic.posix.*;

/**
 Solution to Question 2.
 
 Put description here.
*/
public class LockRace extends uvic.posix.Thread 
{
    // Define static variables here
    static int s1;
    static int s2;
    static int n;
    static Spinlock l1;
    static Spinlock l2;
    
	
	static void main(String[] args)
	{
		// Initialize any variables and start race threads
		LockRace lr1 = new LockRace();
		//LockRace1 lr2 = new LockRace1();
		LockRace lr2 = new LockRace();
		s1 = 0;
		s2 = 0;
		l1 = new Spinlock();
		l2 = new Spinlock();
		
		//set(DAEMON);
		
		lr1.start(1);
		lr2.start(1);

		
		set(DAEMON);
		
		System.println("s1: " + s1);
		System.println("s2: " + s2);
		
	}

	public void run()
	{
		// Each thread's "work"
		for(int i=1; i<=10000; i++)
		{
			System.print(".");
			
			l1.lock();
			l2.lock();
			
			s1 = s1+ 1;
			s2 = s2 + 2;

			l1.unlock();
			l2.unlock();
		}
		
	}
	
}



class LockRace1 extends LockRace 
{

	
	public void run()
	{
		// Each thread's "work"
		for(int i=1; i<=10000; i++)
		{
			System.print("*");
			
			l2.lock();
			l1.lock();
			
			s1 = s1 + 1;
			s2 = s2 + 2;
			
			l2.unlock();
			l1.unlock();
			

		}
		
	}
}