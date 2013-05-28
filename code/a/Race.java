import java.util.*;
import java.lang.*;
import uvic.posix.*;
/**
 Solution for Question 1.
 
 When you run this, you will get any random value. This is because there is a race condition for the shared resource total.
 
 In order to get the correct total, uncomment line 30 and comment out line 35. This will set the main thread 
 to DAEMON level before the other threads, effectively allowing the threads to complete one at a time.
*/
public class Race extends uvic.posix.Thread 
{
	// Define static variables here
	static int total; /**< Shared variable for the three threads to race for */
	int n; /**< The number to increment the variable by */
	
	public Race(int value){
		this.n = value;
	}
	
	static void main(String[] args)
	{
		// Initialize any variables and start race threads
		Race r1 = new Race(1);
		Race r2 = new Race(2);
		Race r3 = new Race(3);
		
		/*//Uncomment this block to get correct result! 
		set(DAEMON); 
		*/

		r1.start(1);
		r2.start(1);
		r3.start(1);
		set(DAEMON);

		System.println("Expected Value: 60000");
		System.println("Actual Value: "+total);
	}

	public void run()
	{
		// Each thread's "work"
		for(int i=0;i<10000;i++){
			this.total += this.n;
		}
		//System.println("Thread "+this.n+" Done.");
	}
	
}