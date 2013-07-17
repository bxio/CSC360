import uvic.posix.*;
import uvic.disk.*;

import java.lang.*;

/**
	The FileSystem is a layer that sits above the Disk and allows a 
	thread to read, write or delete a file.
*/
public class FileSystem extends uvic.posix.Thread
{
	private static final int INODE_OFFSET 		= 0;
	private static final int INODE_HEADER_SIZE	= 1;
	private static final int INODE_DATA_SIZE	= 12;
	private static final int INODE_SIZE			= INODE_HEADER_SIZE + INODE_DATA_SIZE;
	private static final int INODE_TOTAL		= 8; ///*< Total pre-allocated inode files.
	
	private static final int BITMAP_OFFSET		= INODE_TOTAL * INODE_SIZE + INODE_OFFSET;
	private static final int BITMAP_SIZE		= 32; ///*< 32 bits in a int.
	private static final int BITMAP_TOTAL		= 6;
	
	private static final int DATA_OFFSET		= BITMAP_OFFSET + BITMAP_TOTAL;
	private static final int DATA_BLOCK_SIZE 	= 1;
	private static final int DATA_TOTAL			= 192;
	
	private int[] bitmap = new int[BITMAP_TOTAL]; ///< Stores bitmap of free & used blocks.
	private Mutex bitmap_mutex; ///< Lock anytime bitmap cache is being touched.
	private Condition shutdown_cond; ///< Used solely for shutting down.
	
	private Mutex[] inode_mutex = new Mutex[INODE_TOTAL];
	
	private DiskScheduler ds;
	
	private boolean isRunning;
	
	/**
		Initialize FileSystem.
		
		@param ds The disk scheduler we will be reading and writing to.
	*/
	public FileSystem(DiskScheduler ds)
	{
		this.ds = ds;
		// Only one requester should be able to fiddle with an inode
		// at a time. So, we keep a mutex for each.
		for (int i=0; i < INODE_TOTAL; i++)
		{
			inode_mutex[i] = new Mutex();
		}
		bitmap_mutex = new Mutex();
		shutdown_cond = new Condition();
		isRunning = true;
		this.start(REAL_TIME);
	}
	
	
	/**
		The file system itself sometimes needs to write requests.
		These need to be done inside this thread; otherwise, the main
		thread will block during these requests and screw everything up.
	*/
	public void run()
	{
		// We are going to the cache the bitmap in FileSystem
		// and save it when we finish (MUCH easier than constantly
		// reading and writing to it).
		bitmap_mutex.Lock();
		System.println("Caching bitmap...");
		for (int i=0; i < BITMAP_TOTAL; i++)
		{
			bitmap[i] = ds.read(BITMAP_OFFSET + i);
		}
		
		while (isRunning) shutdown_cond.Wait(bitmap_mutex);
		
		// We are here because shutdown() was called.
		// Write the bitmap cache to disk before exiting thread.
		System.println("Writing bitmap...");
		for (int i=0; i < BITMAP_TOTAL; i++)
		{
			ds.write(BITMAP_OFFSET + i, bitmap[i]);
		}
		bitmap_mutex.UnLock();
	}
	
	
	/**
		Call before finishing. This will run operations like writing the cached bitmap
		to disk.
	*/
	public void shutdown()
	{
		bitmap_mutex.Lock();
		isRunning = false;
		shutdown_cond.Signal();
		bitmap_mutex.UnLock();
	}
	
	/**
		Write blocks of data to the file with inode id.
		
		@param inode The inode number
		@param content The blocks of data
	*/
	public void write_file(int inode, int[] content) throws DiskException
	{
		if (content.length > INODE_DATA_SIZE) throw new DiskException("File too big " + content.length + ".");
		inode_mutex[inode].Lock();
		// WRITTEN BY YOU
		//write the file size to the inode
		ds.write(inode+INODE_OFFSET,contents.length);

		//get content.length free blocks from memory
		int[] pointers = new int[content.length];
		pointers = get_free_blocks(content.length);

		//fill the remaining inode pointers
		for(int i=1;i<=content.length;i++){
			ds.write(((inode*(INODE_HEADER_SIZE+INODE_DATA_SIZE))+i),pointers[i-1]);
		}
		//write the file data to the actual blocks
		for(int i=0;i<content.length;i++){
			ds.write(pointers[i], content[i]);
		}
		
		inode_mutex[inode].UnLock();
	}
	
	/**
		Get free blocks from the bitmap and set them to USED (or 1) in the bitmap.
	*/
	private int[] get_free_blocks(int size) throws DiskException
	{
		bitmap_mutex.Lock();
		int[] assembled = new int[size];
		// WRITTEN BY YOU
		for(int i=0;i<DATA_TOTAL;i++){






		}
		bitmap_mutex.UnLock();
		return assembled;
	}
	
	/**
		Set blocks in the bitmap to FREE (or 0).
	*/
	private void free_blocks(int[] blocks)
	{
		bitmap_mutex.Lock();
		// WRITTEN BY YOU
		int[] targets = new int[blocks.length]; //keeps the index of the bitmap number we're going to change
		int[] blockst = new int[blocks.length]; //keeps the relative position of the block index, all values <32
		int mask = 1;
		for(int i=0;i<blocks.length;i++){
			//System.println(">>Now Freeing:"+blocks[i]);
			if(blocks[i]<BITMAP_SIZE){
				targets[i]=0;
			}else if(blocks[i]<(2*BITMAP_SIZE)){
				targets[i]=1;
			}else if(blocks[i]<(3*BITMAP_SIZE)){
				targets[i]=2;
			}else if(blocks[i]<(4*BITMAP_SIZE)){
				targets[i]=3;
			}else if(blocks[i]<(5*BITMAP_SIZE)){
				targets[i]=4;
			}else if(blocks[i]<(6*BITMAP_SIZE)){
				targets[i]=5;
			}
			blockst[i]=blocks[i]%BITMAP_SIZE;
			//System.println("("+targets[i]+","+blockst[i]+")");
			//System.println("Map:"+Integer.toBinaryString(bitmap[targets[i]])+"("+bitmap[targets[i]]+")");

			for(int j=0;j<=blockst[i];j++){ //construct the mask, 1 the blocks you want to free, 0 elsewhere.
				if(j==0){
					mask = 1;
				}else{
					mask = mask * 2;
				}
			}
			//System.println("Mask:"+Integer.toBinaryString(mask)+"("+mask+")");
			mask = ~mask; //flip the mask, so that it's 0 the blocks you want to free and 1 elsewhere.
			//System.println("Flip:"+Integer.toBinaryString(mask)+"("+mask+")");
			bitmap[targets[i]]=(bitmap[targets[i]] & mask);
			//System.println("New:"+Integer.toBinaryString(bitmap[targets[i]])+"("+bitmap[targets[i]]+")");
			mask = 1;//reset mask
			//System.println(">>Done Freeing:"+blocks[i]);
		
		}

		bitmap_mutex.UnLock();
	}
	
	/**
		Reads the contents of a file and returns them.
		
		@param inode The inode number of the file.
	*/
	public int[] read_file(int inode)
	{
		inode_mutex[inode].Lock();
		// WRITTEN BY YOU
		int startOffset = (INODE_HEADER_SIZE+INODE_DATA_SIZE)*inode+INODE_OFFSET; //calculate the starting offset, according to given inode number
		int blocksToRead = ds.read(startOffset);
		int assembled[] = new int [blocksToRead];
		for(int i=1;i<=blocksToRead;i++){
			assembled[i-1]=ds.read(ds.read(i+startOffset)+DATA_OFFSET);
		}
		inode_mutex[inode].UnLock();
		return assembled;
	}

	
	private void delete_file(int inode, boolean lock)
	{
		// The if lock is a slight hack to deal w/ the
		// mutex not being recursive.
		// Clue: depending on how you implement write_file() 
		// it might make sense why this is like this.
		if (lock) inode_mutex[inode].Lock();
		// WRITTEN BY YOU
		int startOffset = (INODE_HEADER_SIZE+INODE_DATA_SIZE)*inode;
		int blocksToDelete = ds.read(startOffset);
		int assembled[] = new int[blocksToDelete];
		for(int i = 1;i<=blocksToDelete;i++){
			assembled[i-1]=ds.read(i+startOffset); //add the block to the assembled block
		}

		free_blocks(assembled); //Free the assembled blocks from the bitmap
		ds.write(startOffset,0);
		if (lock) inode_mutex[inode].UnLock();
	}
	
	/**
		Set the total blocks for inode to 0 and free its blocks in the
		bitmap.
		
		@param inode The inode number of the file.
	*/
	public void delete_file(int inode)
	{
		delete_file(inode, true);
	}
}