/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/

package java.io;

public class FileOutputStream extends OutputStream
{
    private int handle;

    public FileOutputStream(String name) throws IOException
    {
        this(name, false);
    }

    public FileOutputStream(File file) throws IOException
    {
        this(file.getPath(), false);
    }

    public FileOutputStream(String name, boolean append) throws IOException
    {
        handle = open(name, append);
    }

    public synchronized void write(int b) throws IOException
    {
        byte[] buf = new byte[1];

        buf[0] = (byte)(b & 0xFF);
        int rv = write0(handle, buf, 0, buf.length);
        if(rv == -1)
            throw new IOException();
    }

    public synchronized void write(byte[] buf) throws IOException
    {
        int rv = write0(handle, buf, 0, buf.length);
        if(rv == -1)
            throw new IOException();
    }

    public synchronized void write(byte[] buf, int offset, int len) throws IOException
    {
        int rv = write0(handle, buf, 0, len);
        if(rv == -1)
            throw new IOException();
    }

    public synchronized void close() throws IOException
    {
        int rv = close0(handle);
        if(rv < 0)
            throw new IOException();

        handle = -1;
    }

    private synchronized int open(String name, boolean append) throws IOException
    {
        int rv = open0(name, append);
        if(rv < 0)
            throw new IOException();

        return rv;
    }

    private native int close0(int handle);
    private native int open0(String name, boolean append);
    private native int write0(int handle, byte[] buf, int offset, int len);
}

