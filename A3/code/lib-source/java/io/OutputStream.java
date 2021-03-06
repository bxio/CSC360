/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/

package java.io;

public abstract class OutputStream
{
    public OutputStream()
    {

    }

    public void flush() throws IOException
    {
    }

    public void close() throws IOException
    {
    }

    public abstract void write(int b) throws IOException;

    public void write(byte[] buf) throws IOException
    {
        write(buf, 0, buf.length);
    }

    public synchronized void write(byte[] buf, int offset, int len) throws IOException
    {
        for(int i = offset; i < (len + offset); i++)
            write(buf[i]);
    }
}

