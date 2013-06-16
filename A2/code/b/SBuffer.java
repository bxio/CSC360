/**
 This class defines an abstract shared bounded buffer for mulitple 
 readers and single writer. All readers must read and advance at the
 same time.
*/
public interface SBuffer {
    /** Start reading an item at location i wait if buffer is empty. */
    public void StartRead( int i );

    /** Done reading. */
    public void EndRead();

    /** Start writing an item wait if buffer is full. */
    public void StartWrite();

    /** Done writing. */
    public void EndWrite();

    /** Write an item. */
    public void Put( int i );

    /** Read an item at current location. */
    public int Get();

}
