
import uvic.posix.*;

/** This thread will run an operation on a file and then finish. Used for testing. */
class FileRequester extends uvic.posix.Thread
{
	public static final int READ_FILE 		= 0;
	public static final int WRITE_FILE 		= 1;
	public static final int DELETE_FILE		= 2;
	
	private FileSystem fs;
	private String name; ///< name of the thread (for debugging)
	private int inode; ///< inode identifier
	private int type; ///< type of file request (READ_FILE/WRITE_FILE/DELETE_FILE)
	private int[] content; ///< data to be written (if applicable)
	
	public FileRequester(FileSystem fs, String name, int inode, int[] content, int type)
	{
		this.fs = fs;
		this.name = name;
		this.inode = inode;
		this.content = content;
		this.type = type;
	}
	
	public void run()
	{
		try{
			switch (type)
			{
				case READ_FILE:
					System.println(name + "] Reading " + inode);
					int[] data = fs.read_file(inode);
					System.println(name + "] Read " + inode + " w/: '" + data2string(data) + "'");
					break;
				case WRITE_FILE:
					System.println(name + "] Writing to " + inode + " w/ " + data2string(content));
					fs.write_file(inode, content);
					System.println(name + "] Finished writing");
					break;
				case DELETE_FILE:
					System.println(name + "] Deleting " + inode);
					fs.delete_file(inode);
					System.println(name + "] Deleted " + inode);
					break;
				default:
					System.println("Unknown type: " + type);
					break;
			}
		}catch(DiskException e){
			System.println(name + "] DiskException: " + e.getMessage());
			return;
		}
		
	}
	
	private String data2string(int[] data)
	{
		String s = "";
		for (int i=0; i < data.length; i++)
		{
			s = s + data[i] + " ";
		}
		return s;
	}
}
