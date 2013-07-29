/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Implements string handling methods.
 *
 * For detailed description of this class see the Java language documentation.
 */
public final class String
{
    char value[];
    int offset;
    int count;
    int hash;

    /**
     * Creates a new empty String object.
     */
    public String()
    {
        value = new char[0];
        offset = count = 0;
    }

    /**
     * Creates a new String object based on another String.
     */
    public String(String other)
    {
        count = other.length();
        value = new char[count];
        other.getChars(0, count, value, 0);
    }

    /**
     * Creates a new String object based on the supplied byte array.
     */
    public String(char src[])
    {
        count = src.length;
        value = new char[count];
        System.arraycopy(src, 0, value, 0, count);
    }

    /**
     * Creates a new String object based on a part of the supplied char array.
     */
    public String(char src[], int offs, int cnt)
    {
        if (offs < 0 || cnt < 0 || offs > src.length - cnt)
            throw new StringIndexOutOfBoundsException();

        count = cnt;
        value = new char[count];
        System.arraycopy(src, offs, this.value, 0, count);
    }

    /**
     * Creates a new String object based on a part of the supplied byte array.
     */
    public String(byte src[], int offs, int cnt)
    {
        if (offs < 0 || cnt < 0 || offs > src.length - cnt)
            throw new StringIndexOutOfBoundsException();

        count = cnt;
        value = new char[count];
        for (int i=0; i < count; i++)
            value[i] = (char)src[offs+i];
    }

    /**
     * Creates a new String object based on characters of supplied StringBuffer object.
     */
    public String(StringBuffer sb)
    {
        synchronized (sb)
        {
            count = sb.length();
            value = new char[count];
            if (count > 0)
            {
                sb.getChars(0, count, value, 0);
            }
        }
    }

    private String( int sIdx, int eIdx, char[] val)
    {
        value  = val;
        offset = sIdx;
        count  = eIdx - sIdx;
    }

    /**
     * Returns the length of this string.
     */
    public int length()
    {
        return count;
    }

    /**
     * Returns the character at the specified index.
     */
    public char charAt(int index)
    {
        if (index < 0 || index >= count)
            throw new StringIndexOutOfBoundsException();

        return value[index + offset];
    }

    /**
     * Returns characters from this string in the destination character array.
     */
    public void getChars(int srcBegin, int srcEnd, char dst[], int dstBegin)
    {
        if (srcBegin < 0 || srcEnd > count || srcBegin > srcEnd)
            throw new StringIndexOutOfBoundsException();

        System.arraycopy(value, offset + srcBegin, dst, dstBegin, srcEnd - srcBegin);
    }

    /**
     * Returns characters from this string in the destination byte
     * array.
     */
    public void getBytes(int srcBegin, int srcEnd, byte dst[], int dstBegin)
    {
        if (srcBegin < 0 || srcEnd > count || srcBegin > srcEnd)
        {
            throw new StringIndexOutOfBoundsException();
        }

        int len = srcEnd - srcBegin;
        for (int pos=0; pos < len; pos++)
        {
            dst[dstBegin+pos] = (byte)value[offset+srcBegin+pos];
        }
    }

    /**
     * Returns characters from this string in the new byte array.
     */
    public byte[] getBytes()
    {
        int len = value.length - offset;
        byte[] buf = new byte[len];
        for (int pos=0; pos < len; pos++)
        {
            buf[pos] = (byte)value[offset+pos];
        }

        return buf;
    }
    /**
     * Compares with another string.
     */
    public boolean equals(Object anObject)
    {
        if (!(anObject instanceof String))
        {
            return false;
        }
        String other = (String)anObject;

        if (other == null || count != other.count)
        {
            return false;
        }

        int i = offset;
        int j = other.offset;
        int n = offset + count;
        for (; i < n; i++, j++)
        {
            if (value[i] != other.value[j])
            {
                return false;
            }
        }
        return true;
    }

    /**
     * Compares with another String ignoring the case.
     */
    public boolean equalsIgnoreCase (String other)
    {
        if (other == null || count != other.count)
        {
            return false;
        }

        int i = offset;
        int j = other.offset;
        int n = offset + count;

        for (; i < n; i++, j++)
        {
            if (value[i] != other.value[j] && toUpperCase(value[i]) != toUpperCase(other.value[j]))
            {
                return false;
            }
        }
        return true;
    }

    /**
     * Compares two strings.
     */
    public int compareTo(String other)
    {
        int minLen = count < other.count ? count : other.count;

        char c, c1;
        for (int pos=0; pos < minLen; pos++)
        {
            c = value[offset+pos];
            c1 = other.value[other.offset+pos];
            if (c != c1)
            {
                return c - c1;
            }
        }

        return count - other.count;
    }

    /**
     * Checks if two string regions are equal.
     */
    public boolean regionMatches( int toffset, String other, int ooffset, int len)
    {
        return regionMatches(false, toffset, other, ooffset, len);
    }


    /**
     * Checks if two string regions are equal.
     */
    public boolean regionMatches(boolean ignoreCase, int toffset, String other, int ooffset, int len)
    {
        if (toffset < 0 || ooffset < 0)
        {
            return false;
        }
        if (toffset+len > count || ooffset+len > other.count)
            return false;

        char c, c1;
        for (int pos=0; pos<len; pos++)
        {
            c  = value[offset+toffset+pos];
            c1 = other.value[other.offset+ooffset+pos];
            if (c != c1 && ignoreCase)
            {
                c  = toLowerCase(c);
                c1 = toLowerCase(c1);
            }
            if (c != c1)
                return false;
        }
        return true;
    }

    /**
     * Checks if this string starts with the prefix.
     */
    public boolean startsWith(String prefix, int toffset)
    {
        return regionMatches(false, toffset, prefix, 0, prefix.count);
    }

    /**
     * Checks if this string starts with the prefix.
     */
    public boolean startsWith(String prefix)
    {
        return regionMatches(false, 0, prefix, 0, prefix.count);
    }

    /**
     * Checks if this string ends with the suffix.
     */
    public boolean endsWith(String suffix)
    {
        return regionMatches(false, count-suffix.count, suffix, 0, suffix.count);
    }

    /**
     * Returns a hashcode.
     */
    public int hashCode()
    {
        if (hash == 0 && count > 0)
        {
            final int stop = offset + count;
            for (int index = offset; index < stop; index++)
            {
                hash = (31 * hash) + value[index];
            }
        }
        return hash;
    }

    /**
     * Returns the index of the first occurrence of the character.
     */
    public int indexOf(int ch)
    {
        return indexOf(ch, 0);
    }

    /**
     * Returns the index of the first occurence of the character from
     * starting position.
     */
    public int indexOf( int ch, int sIdx)
    {
        char c = (char)ch;
        for (int pos=sIdx; pos < count; pos++)
        {
            if (value[offset+pos] == c)
                return pos;
        }

        return -1;
    }

    /**
     * Returns the index the first occurrence of the character starting at fromIndex.
     */
    public int indexOf(String str, int sIdx)
    {
        int it = offset+sIdx;
        int ic = str.offset;
        int ma = 0;

        if (str.count > count-sIdx)
            return -1;

        for ( ; it<value.length; it++)
        {
            if ( value[it] == str.value[ic] )
            {
                if (++ma == str.count)
                    return it - ma - offset + 1;
                ic++;
            }
            else if ( ma > 0)
            {
                it--;
                ma  = 0;
                ic  = str.offset;
            }
        }
        return -1;
    }


    /**
     * Returns the index of the last occurrence of the character.
     */
    public int lastIndexOf(int ch)
    {
        return lastIndexOf(ch, count - 1);
    }

    /**
     * Returns the index of the last occurrence of the character starting at eIdx.
     */
    public int lastIndexOf(int ch, int eIdx)
    {
        char c = (char)ch;

        if ( eIdx >= count)
            eIdx = count-1;

        for (int pos=eIdx; pos>=0; pos--)
        {
            if (value[offset+pos] == c)
                return pos;
        }

        return -1;
    }


    /**
     * Returns the index of the first occurrence of the substring.
     */
    public int indexOf(String str)
    {
        return indexOf(str, 0);
    }

    /**
     * Returns the index of the rightmost occurrence of the substring.
     */
    public int lastIndexOf(String str)
    {
        return lastIndexOf(str, count);
    }

    /**
     * Returns the index of the rightmost occurrence of the substring starting at eIdx.
     */
    public int lastIndexOf(String str, int eIdx)
    {
        int it = offset + eIdx - 1;
        int ic = str.value.length - 1;
        int ma = 0;

        for ( ; it>=offset; it--)
        {
            if ( value[it] == str.value[ic] )
            {
                ic--;
                if (++ma == str.count)
                    return it - offset;
            }
            else if (ma > 0)
            {
                it++;
                ma = 0;
                ic = str.value.length - 1;
            }
        }
        return -1;
    }

    /**
     * Converts the characters in this string to lower case.
     */
    public String toLowerCase()
    {
        for (int i=0; i < count; ++i)
        {
            value[offset+i] = toLowerCase(value[offset+i]);
        }

        return this;
    }

    /**
     * Converts the characters in this string to upper case.
     */
    public String toUpperCase()
    {
        for (int i=0; i < count; ++i)
        {
            value[offset+i] = toUpperCase(value[offset+i]);
        }

        return this;
    }

    /**
     * Removes white space from the ends.
     */
    public String trim()
    {
        while (count > 0 && value[offset] <= ' ')
        {
            offset++;
            count--;
        }

        while (count > 0 && value[offset + count - 1] <= ' ')
            count--;

        return this;
    }

    /**
     * Returns this object.
     */
    public String toString()
    {
        return this;
    }

    /**
     * Converts character to upper case.
     */
    private char toUpperCase(char Ch)
    {
        if (Ch >= 'a' && Ch <= 'z')
            return (char)((int)Ch - 0x20);
        return Ch;
    }

    /**
     * Converts character to lower case.
     */
    private char toLowerCase(char Ch)
    {
        if (Ch >= 'A' && Ch <= 'Z')
            return (char)((int)Ch + 0x20);
        return Ch;
    }

    public char[] toCharArray()
    {
        char buf[] = new char[count];
        if (count > 0)
        {
            getChars(0, count, buf, 0);
        }

        return buf;
    }

    public static String valueOf(Object obj)
    {
        if (obj == null)
        {
            return "null";
        }
        return obj.toString();
    }

    public String substring(int sIdx)
    {
        return substring(sIdx, count);
    }

    public String substring( int sIdx, int eIdx)
    {
        if (sIdx < 0)     throw new StringIndexOutOfBoundsException(sIdx);
        if (eIdx > count) throw new StringIndexOutOfBoundsException(eIdx);
        if (sIdx > eIdx)  throw new StringIndexOutOfBoundsException(eIdx-sIdx);

        if ((sIdx == 0) && (eIdx  == count))
        {
            return this;
        }

        return new String(offset+sIdx, offset+eIdx, value);
    }

    public static String valueOf(boolean b)
    {
        return new Boolean(b).toString();
    }

    public static String valueOf(char c)
    {
        char ca[] = new char[1];
        ca[0] = c;
        return new String(ca);
    }

    public static String valueOf(char data[])
    {
        return new String(data);
    }

    public static String valueOf(char data[], int offset, int count)
    {
        return new String(data, offset, count);
    }

    public static String valueOf(float f)
    {
        return new Float(f).toString();
    }

    public static String valueOf(int i)
    {
        return new Integer(i).toString();
    }
}
