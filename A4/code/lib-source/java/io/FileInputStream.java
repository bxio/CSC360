/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/

package java.io;

public class FileInputStream extends InputStream
{
    private int handle;
    int grand_total_read;

    public FileInputStream(String name) throws FileNotFoundException
    {
        this(new File(name));
    }

    public FileInputStream(File file) throws FileNotFoundException
    {
        if(!file.exists())
            throw new FileNotFoundException(file.getName());

        handle = open(file.getPath());
    }

    public int available() throws IOException
    {
        int len = getFileLength0(handle);
        if(len < 0)
            throw new IOException();

        len -= grand_total_read;
        if(len <= 0)
            return 0;

        return len;
    }

    public synchronized int skip(int num_bytes) throws IOException
    {
        if(num_bytes <= 0)
            return 0;

        if(num_bytes > available())
            num_bytes = available();

        int bytes_skipped = skip0(handle, num_bytes);
        if(bytes_skipped < 0)
            throw new IOException();

        grand_total_read += bytes_skipped;

        return bytes_skipped;
    }

    public synchronized int read() throws IOException
    {
        byte[] buf = new byte[1];

        int bytes_read = read0(handle, buf, 0, buf.length);
        if(bytes_read < 0)
            throw new IOException();

        if(bytes_read == 0)
            return -1;

        ++grand_total_read;

        return(buf[0] & 0xFF);
    }

    public int read(byte[] buf) throws IOException
    {
        return read(buf, 0, buf.length);
    }

    public synchronized int read(byte[] buf, int offset, int len) throws IOException
    {
        if(len == 0)
            return 0;

        int bytes_read = read0(handle, buf, offset, len);
        if(bytes_read < 0)
            throw new IOException();

        if(bytes_read == 0)
            return -1;

        grand_total_read += bytes_read;
        return bytes_read;
    }

    private int open(String name) throws FileNotFoundException
    {
        int rv = open0(name);
        if(rv < 0)
            throw new FileNotFoundException();

        return rv;
    }

    public synchronized void close() throws IOException
    {
        int rv = close0(handle);
        if(rv < 0)
            throw new IOException();

        handle = -1;
    }

    private native int close0(int handle);
    private native int open0(String name);
    private native int read0(int handle, byte[] buf, int offset, int len);
    private native int skip0(int handle, int num_bytes);
    private native int getFileLength0(int handle);
}

