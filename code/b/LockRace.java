
import java.util.*;
import java.lang.*;
import uvic.posix.*;

/**
 Solution to Question 2.
 
 Please see main page for documentation.
 To see the deadlock, comment line 32, uncomment line 33 and run ./Rose.exe 1 LockRace.bin  
*/
public class LockRace extends uvic.posix.Thread 
{
    // Define static variables here
    static int s1;
    static int s2;
    static Spinlock l1;
    static Spinlock l2;
    
    int n; // the increment value
    
    LockRace(int value)
    {
	    this.n = value;
    }
    
	
	static void main(String[] args)
	{
		// Initialize any variables and start race threads
		LockRace lr1 = new LockRace(1);
		LockRace lr2 = new LockRace(2);
		//LockRace1 lr2 = new LockRace1(2);
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
			
			l1.lock();
			l2.lock();
			
			s1 = s1+ n;
			s2 = s2 + n;

			l1.unlock();
			l2.unlock();
		}
		
	}
	
}



class LockRace1 extends LockRace 
{
	LockRace1(int val)
	{
		super(val);
	}
	
	public void run()
	{
		// Each thread's "work"
		for(int i=1; i<=10000; i++)
		{
			
			l2.lock();
			l1.lock();
			
			s1 = s1 + n;
			s2 = s2 + n;
			
			l2.unlock();
			l1.unlock();	
			

		}
		
	}
}