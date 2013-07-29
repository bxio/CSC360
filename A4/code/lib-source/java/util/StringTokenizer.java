/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/

package java.util;

import java.lang.*;

public class StringTokenizer implements Enumeration
{
    private String input;
    private String delims;
    private boolean retDelim;
    private int position;

    public StringTokenizer(String str)
    {
        this (str, " \t\n\r\f");
    }

    public StringTokenizer(String str, String delim)
    {
        this (str, delim, false);
    }

    public StringTokenizer(String input, String delims, boolean retDelim)
    {
        this.input = input;
        this.delims = delims;
        this.retDelim = retDelim;
        position = 0;
    }

    public int countTokens()
    {
        int count = 0;

        int curPos = position;
        while (nextTokenInternal(false) != null)
            count++;
        position = curPos;

        return count;
    }

    public boolean hasMoreElements()
    {
        return hasMoreTokens();
    }


    public boolean hasMoreTokens()
    {
        int oldPosition = position;
        String ret = nextTokenInternal(false);
        position = oldPosition;
        if (ret == null)
            return false;
        return true;
    }

    public Object nextElement()
    {
        return (Object)nextToken();
    }

    public String nextToken(String delims)
    {
        this.delims = delims;
        return nextToken();
    }

    public String nextToken()
    {
        String ret = nextTokenInternal(true);
        if (ret == null)
            throw new NoSuchElementException("no more elements");
        return ret;
    }

    private String nextTokenInternal(boolean retStr)
    {
        if (position >= input.length())
            return null;

        if (delims.indexOf(input.charAt(position)) != -1)
        {
            position++;
            if (retDelim)
            {
                if (retStr)
                    return input.substring(position-1, position);
                return input;
            }
            while (true)
            {
                if (position >= input.length())
                    return null;
                if (delims.indexOf(input.charAt(position)) == -1)
                    break;
                position++;
            }
        }

        int start = position;
        while (true)
        {
            position++;
            if (position >= input.length() || delims.indexOf(input.charAt(position)) != -1)
                break;
        }

        if (retStr)
            return input.substring(start, position);
        return input;
    }
}
