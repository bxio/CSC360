
import java.util.*;
import java.lang.*;
import uvic.posix.*;

/**
 Solution to Question 4. This is just a husk class for you to use in testing 
 your native cpu_time function.
 
 Put description here.
*/
public class TestTimer extends uvic.posix.Thread 
{
	int n;
	
	public TestTimer(int n){
		this.n = n;
	}
	
	
    static void main(String[] args)
    {
		TestTimer t1,t2,t3;
		t1 = new TestTimer(1);
		t2 = new TestTimer(2);
		t3 = new TestTimer(3);
		
		t1.start(1);
		t2.start(1);
		t3.start(1);
    }

    public void run()
    {
		int elapsed;
		for(int i=0;i<=10000;i++){
			//do something......
		}
		elapsed = cpu_time();
		System.println("Thread "+this.n+" Elapsed time:"+elapsed);
    }
}