/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/

package java.io;

public abstract class InputStream
{
    public InputStream()
    {
        return;
    }

    public void mark(int readlimit)
    {
        return;
    }

    public boolean markSupported()
    {
        return(false);
    }

    public void reset() throws IOException
    {
        throw new IOException("Mark not supported in this class");
    }

    public int available() throws IOException
    {
        return(0);
    }

    public int skip(int num_bytes) throws IOException
    {
        int buf_size = 256;
        byte[] buf = new byte[buf_size];

        if(num_bytes <= 0)
            return(0);

        int loop_count = num_bytes / buf_size;
        int extra_bytes = (int)(num_bytes % buf_size);
        int bytes_read = 0;
        int total_read = 0;

        for(int i = 0; i < loop_count; i++)
        {
            bytes_read = read(buf, 0, buf.length);

            if(bytes_read == -1)
                return(total_read);

            if(bytes_read != buf_size)
                return(total_read + bytes_read);

            total_read += bytes_read;
        }

        bytes_read = read(buf, 0, extra_bytes);

        if(bytes_read == -1)
            return(total_read);

        return(total_read + bytes_read);
    }

    public abstract int read() throws IOException;

    public int read(byte[] buf) throws IOException
    {
        return(read(buf, 0, buf.length));
    }

    public int read(byte[] buf, int offset, int len) throws IOException
    {
        if(len == 0)
            return(0);

        // Read the first byte here in order to allow IOException's to
        // propagate up
        int byte_read = read();
        if(byte_read == -1)
            return(-1);
        buf[offset] = (byte)byte_read;

        int total_read = 1;

        // Read the rest of the bytes
        try
        {
            for(int i = 1; i < len; i++)
            {
                byte_read = read();
                if(byte_read == -1)
                    return(total_read);

                buf[offset + i] = (byte)byte_read;

                ++total_read;
            }
        }
        catch(IOException e)
        {
            return(total_read);
        }

        return(total_read);
    }

    public void close() throws IOException
    {
        return;
    }

}

