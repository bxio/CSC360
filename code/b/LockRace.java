
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
    
    LockRace(int value){
		this.n=value;
	}
    
	
	static void main(String[] args)
	{
		// Initialize any variables and start race threads
		LockRace lr1=new LockRace(1);
		LockRace1 lr2=new LockRace1(2);
		s1=0;
		s2=0;
		l1=new Spinlock();
		l2=new Spinlock();
		
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
			s1 = s1 + this.n;
			l1.unlock();
		
			l2.lock();
			s2 = s2 + this.n;
			l2.unlock();
		}
	}
}



class LockRace1 extends LockRace 
{
    
    	LockRace1(int value)
	{
		super(value);
	}
    
	

	public void run()
	{
		// Each thread's "work"
		for(int i=1; i<=10000; i++)
		{
			l2.lock();
			s2 = s2 + this.n;
			l2.unlock();

		
			l1.lock();
			s1 = s1 + this.n;
			l1.unlock();
		}
	}
}