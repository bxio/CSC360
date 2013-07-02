/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/

package java.io;

import java.io.*;
import java.lang.*;
import java.util.*;


public class File implements Serializable
{

    public final static String separator = System.getProperty("file.separator", "/");
    public final static char separatorChar = separator.charAt(0);
    public final static String pathSeparator = System.getProperty("path.separator", ":");
    public final static char pathSeparatorChar = pathSeparator.charAt(0);

    private String path;


    public File(File dir, String name)
    {
        this((dir == null) ? null : dir.getPath(), name);
    }

    public File(String path, String name)
    {
        if(name == null)
            throw new NullPointerException();

        if(path == null)
            this.path = name;
        else if(path.length() == 0)
            this.path = separatorChar + name;
        else
        {
            char last = path.charAt(path.length() - 1);

            if(last != separatorChar)
                this.path = path + separatorChar + name;
            else
                this.path = path + name;
        }
    }

    public File(String path)
    {
        if(path == null)
            throw new NullPointerException();
        this.path = path;
    }

    public boolean canRead()
    {
        return(exists() && canRead0(path));
    }

    public boolean canWrite()
    {
        return canWrite0(path);
    }

    public static File createTempFile(String prefix, String suffix) throws IOException
    {
        return createTempFile(prefix, suffix, null);
    }

    public static File createTempFile(String prefix, String suffix, File dir) throws IOException
    {
        if(prefix.length() < 3)
            throw new IllegalArgumentException(prefix);

        if(suffix == null)
            suffix = ".tmp";

        if(dir == null)
            dir = new File(separator);

        int fno = 0;
        while(true)
        {
            File f = new File(dir, prefix + fno + suffix);
            if(!f.exists())
                return f;
        }
    }

    public boolean delete()
    {
        return isDir0(path) ? rmdir0(path) : delete0(path);
    }

    public int compareTo(Object that)
    {
        return compareTo((File)that);
    }

    public int compareTo(File that)
    {
        return this.path.compareTo(that.path);
    }

    public boolean equals(Object that)
    {
        return(that instanceof File) ? path.equals(((File)that).path) : false;
    }

    public boolean exists()
    {
        return exists0(path);
    }

    public String getAbsolutePath()
    {
        if(isAbsolute())
            return getPath();
        return separator + getPath();
    }

    public String getCanonicalPath() throws IOException
    {
        StringTokenizer tok = new StringTokenizer(getAbsolutePath(), separator);
        int len = tok.countTokens();
        String[] array = new String[len+2];
        int j = 0;
        for(int i = 0; i < len; i++)
        {
            String str = tok.nextToken();
            if(str.equals(".."))
                j--;
            else
            {
                array[j] = str;
                j++;
            }
        }

        if(j == 0)
            return separator;

        StringBuffer buf = new StringBuffer();
        for(int i = 0; i < j; i++)
        {
            buf.append(separatorChar);
            buf.append(array[i]);
        }
        return buf.toString();
    }

    public String getName()
    {
        return path.substring(path.lastIndexOf(separatorChar)+1);
    }

    public String getParent()
    {
        int slashIndex = path.lastIndexOf(separatorChar);
        if(slashIndex > 0)
            return(path.substring(0, slashIndex));

        if(slashIndex == 0 && path.length() > 1)
            return(separator);

        return null;
    }

    public String getPath()
    {
        return path;
    }

    public int hashCode()
    {
        return path.hashCode();
    }

    public boolean isAbsolute()
    {
        return(path.charAt(0) == separatorChar);
    }

    public boolean isDirectory()
    {
        return isDir0(path);
    }

    public boolean isFile()
    {
        return isFile0(path);
    }

    public int astModified()
    {
        return lastModified0(path);
    }

    public int length()
    {
        return length0(path);
    }

    public boolean mkdir()
    {
        if(isDirectory())
            return true;
        return(mkdir0(path));
    }

    public boolean mkdirs()
    {
        if(!toString().equals(separator))
        {
            String parentString = getParent();
            if(parentString != null)
            {
                File parent = new File(parentString);
                if(parent.mkdirs() == false)
                    return false;
            }
        }
        return mkdir();
    }

    public boolean renameTo(File dest)
    {
        return renameTo0(path, dest.getPath());
    }

    public String toString()
    {
        return path;
    }

    public String[] list()
    {
        return list(null);
    }

    public String[] list(FilenameFilter filter)
    {
/*
Doesn't work on SC12 if root dir is specified.
        if(!isDir0(path))
            return null;
*/
        int count = getDirItemCount0(path);
        String all[] = new String[count];
        char[] name = new char[maxNameLen0()];

        int filteredCount = 0;
        for(int i=0; i < count; i++)
        {
            int len = getDirItem0(path, i, name);
            String fn = new String(name, 0, len);
            if (filter == null || filter.accept(this, fn))
                all[filteredCount++] = fn;
        }

        if (filter == null)
            return all;

        String flist[] = new String[filteredCount];
        System.arraycopy(all, 0, flist, 0, filteredCount);
        return flist;
    }

    public File[] listFiles()
    {
        return listFiles(null);
    }

    public File[] listFiles(FilenameFilter filter)
    {
        String[] names = list(filter);
        if(names == null)
            return null;

        File[] files = new File[names.length];
        for(int idx = 0; idx < names.length; idx++)
            files[idx] = new File(this, names[idx]);

        return files;
    }


    // native methods of File class
    native private boolean canWrite0(String path);
    native private boolean delete0(String path);
    native private boolean rmdir0(String path);
    native private boolean exists0(String path);
    native private boolean isDir0(String path);
    native private boolean isFile0(String path);
    native private int lastModified0(String path);
    native private int length0(String path);
    native private boolean canRead0(String path);
    native private boolean mkdir0(String path);
    native private boolean renameTo0(String source, String dest);
    native private int maxNameLen0();
    native private int getDirItemCount0(String path);
    native private int getDirItem0(String path, int index, char[] name);
}
