/**
 This class defines an abstract shared database for mulitple readers and
 writers. 
*/
public interface Database {
    /// Start reading an item.
    public void StartRead();

    /// Done reading. 
    public void EndRead();

    /// Start writing an item.
    public void StartWrite();

    /// Done writing.
    public void EndWrite();

    /// Write an item.
    public void Put( int i );

    /// Read an item.
    public int Get();

}
