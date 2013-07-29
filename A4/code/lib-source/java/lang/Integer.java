/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;


/**
 * Encapsulates <strong>int</strong> java primitive type.
 *
 * For detailed description of this class see the Java language documentation.
 */
final public class Integer extends Number
{
    private int value;
    final public static int MIN_VALUE = 0x80000000;
    final public static int MAX_VALUE = 0x7fffffff;

    public Integer(String s) throws NumberFormatException
    {
        this(parseInt(s));
    }

    public Integer(int value)
    {
        this.value = value;
    }

    public static Integer decode(String nm) throws NumberFormatException
    {
        // Strip off negative sign, if any
        int sign = 1;
        if (nm.startsWith("-"))
        {
            sign = -1;
            nm = nm.substring(1);
        }
        // Strip off base indicator, if any
        int base = 10;
        if (nm.startsWith("0x"))
        {
            base = 16;
            nm = nm.substring(2);
        }
        else if (nm.startsWith("#"))
        {
            base = 16;
            nm = nm.substring(1);
        }
        else if (nm.startsWith("0") && nm.length() > 1)
        {
            base = 8;
            nm = nm.substring(1);
        }
        // A string like "0x-1234" must generate an error; disallow it here
        if (nm.startsWith("-"))
        {
            throw new NumberFormatException();
        }
        return new Integer(sign * parseInt(nm, base));
    }

    public boolean equals(Object obj)
    {
        if ((obj!=null) && (obj instanceof Integer))
        {
            return (this.intValue() == ((Integer)obj).intValue());
        }

        return false;
    }

    public float floatValue()
    {
        return (float)value;
    }

    public int hashCode()
    {
        return this.intValue();
    }

    public int intValue()
    {
        return value;
    }

    public static int parseInt(String s) throws NumberFormatException
    {
        return (parseInt(s, 10));
    }

    public static int parseInt(String s, int radix) throws NumberFormatException
    {
        if (s.length()<=0) throw new NumberFormatException();

        // Check for negativity
        if (s.charAt(0)=='-')
        {
            return parseInt(s.substring(1), radix) * -1;
        }

        int result = 0;
        int position;
        for (position=0; position < s.length(); position++)
        {
            int digit=Character.digit(s.charAt(position), radix);
            if (digit == -1)
            {
                throw new NumberFormatException();
            }

            result=(result * radix) + digit;
        }

        return result;
    }

    private static String toUnsignedString(int i, int bits)
    {
        if (i == 0)
        {
            return "0";
        }

        StringBuffer buf = new StringBuffer();
        int radix = 1 << bits, mask = radix - 1;
        while (i != 0)
        {
            char digit = Character.forDigit(i & mask, radix);
            i >>>= bits;
            buf.append(digit);
        }
        buf.reverse();
        return buf.toString();
    }

    public static String toBinaryString(int i)
    {
        return toUnsignedString(i, 1);
    }

    public static String toHexString(int i)
    {
        return toUnsignedString(i, 4);
    }

    public static String toOctalString(int i)
    {
        return toUnsignedString(i, 3);
    }

    public String toString()
    {
        return toString(this.intValue());
    }

    public static String toString(int i)
    {
        return toString(i, 10);
    }

    public static String toString(int i, int radix)
    {
        if (i == 0)
        {
            return "0";
        }

        if (radix < Character.MIN_RADIX || radix > Character.MAX_RADIX)
        {
            radix = 10;
        }

        StringBuffer buf = new StringBuffer();
        int sign = 1;
        if (i < 0)
        {
            sign = -1;
        }
        while (i != 0)
        {
            char digit = Character.forDigit(Math.abs(i % radix), radix);
            i = i / radix;
            buf.append(digit);
        }
        if (sign == -1)
        {
            buf.append('-');
        }
        buf.reverse();
        return (buf.toString());
    }

    public static Integer valueOf(String s) throws NumberFormatException
    {
        return valueOf(s, 10);
    }

    public static Integer valueOf(String s, int radix) throws NumberFormatException
    {
        return new Integer(parseInt(s, radix));
    }
}
