import java.util.*;
import java.lang.*;
import uvic.posix.*;
/**
 Solution for Question 1.
 
 Put description of what is being done here.
*/
public class Race extends uvic.posix.Thread 
{
	// Define static variables here
	static int total;
	int n;//each instance of Race thread will have its own increment value
	
	public Race(int value){
		this.n = value;
	}
	
	static void main(String[] args)
	{
		// Initialization of race threads
		Race r1 = new Race(1);
		Race r2 = new Race(2);
		Race r3 = new Race(3);
		
		//start all threads at REAL_TIME (1)
		r1.start(1);
		r2.start(1);
		r3.start(1);
		set(DAEMON);

		System.println("Actual Value: "+total);
		System.println("Expected Value: 60000");

	}

	public void run()
	{
		// Each thread's "work"
		for(int i=0;i<10000;i++){
			total += this.n;//increment total by n
		}
	}
	
}
