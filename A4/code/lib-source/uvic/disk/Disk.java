package uvic.disk;    // UVic/CS PMM 02/Nov/2002

import java.lang.*;
import java.io.*;
import uvic.posix.*;

/**
 * This is the disk emulator base class. This allows us to tie into native methods 
 * that operate on the disk inside ROSE.
 *
 */
public class Disk extends uvic.posix.Thread
{
	static private final int DATA_LENGTH = 302;

	static private int[] data = new int[DATA_LENGTH];
	static private int position;

	static protected void open_disk(int startPosition)
	{
		position = startPosition;
	}
	
	static protected void close_disk()
	{
		for (int i=0; i < DATA_LENGTH; i++)
		{
			System.println("["+i+"]" + " - " + data[i]);
		}
	}
	
	static protected int size_disk()
	{
		return DATA_LENGTH;
	}
	
	static protected int seek_disk(int block)
	{
		int timeout = Math.abs(block - position) * 10;
		block_disk(timeout);
		position = block;
		return position;
	}
	
	static protected int read_disk()
	{
		block_disk(1);
		return data[position];
	}
	
	static protected void write_disk(int value)
	{
		block_disk(1);
		data[position] = value;
	}
	
	/*
		Saves the data in the disk to a file so that it can be loaded later.
	
	static protected void save_disk(String filename)
	{
		try {
			File file = new File(filename);
			FileOutputStream output = new FileOutputStream(file);
			byte[] outputArray = new byte[DATA_LENGTH * 4];
			for (int i=0; i < DATA_LENGTH; i++)
			{
				int j = 4*i;
				outputArray[j] = (byte)data[i];
				outputArray[j+1] = (byte)(data[i]>>8);
				outputArray[j+2] = (byte)(data[i]>>16);
				outputArray[j+3] = (byte)(data[i]>>24);
			}
			output.write(outputArray);
			
			output.close();
		} catch ( IOException e ) {
			System.println("IOException: " + e.getMessage());
		}
	}*/
	
	/*
		Loads previously saved disk data.
	
	static protected void load_disk(String filename)
	{
		try {
			File file = new File(filename);
			FileInputStream input = new FileInputStream(file);
			byte[] inputArray = new byte[DATA_LENGTH * 4];
			input.read(inputArray);
			for (int i=0; i < DATA_LENGTH; i++)
			{
				int j = 4*i;
				data[i] |= (int)inputArray[j];
				data[i] |= ((int)inputArray[j+1]) << 8;
				data[i] |= ((int)inputArray[j+2]) << 16;
				data[i] |= ((int)inputArray[j+3]) << 24;
			}
			input.close();
		} catch ( IOException e ) {
			System.println("IOException: " + e.getMessage());
		}
	}*/
	
	/**
		Save the current disk to file with identifer id.
	*/
	static public void save(int id)
	{
		save_disk(id, data, DATA_LENGTH);
	}

	/**
		Load the contents of the file with identifer id into the disk.
	*/
	static public void load(int id)
	{
		load_disk(id, data, DATA_LENGTH);
	}
	
	static private native void block_disk(int timeout);
	//static private native void save_disk(int id, int[] buffer, int len);
	//static private native void load_disk(int id, int[] buffer, int len);
	static public native void save_disk(int id, int[] buffer, int len);
	static public native void load_disk(int id, int[] buffer, int len);
	
	
}
