/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Encapsulates <strong>float</strong> java primitive type.
 *
 * For detailed description of this class see the Java language documentation.
 */
public final class Float extends Number
{
    private float value;

    public static final float POSITIVE_INFINITY = 1.0f / 0.0f;
    public static final float NEGATIVE_INFINITY = -1.0f / 0.0f;
    public static final float NaN = 0.0f / 0.0f;
    public static final float MAX_VALUE = 3.40282346638528860e+38f;
    public static final float MIN_VALUE = 1.40129846432481707e-38f;

    public Float(float value)
    {
        this.value = value;
    }

    public Float(String s) throws NumberFormatException
    {
        this(Float.valueOf(s).floatValue());
    }

    public boolean equals(Object obj)
    {
        if ((obj!=null) && (obj instanceof Float))
        {
            int leftBits = floatToIntBits(this.floatValue());
            int rightBits = floatToIntBits(((Float)obj).floatValue());
            int nanBits = floatToIntBits(NaN);
            int posBits = floatToIntBits(POSITIVE_INFINITY);
            int negBits = floatToIntBits(NEGATIVE_INFINITY);

            if ((this.isNaN() == true) && (((Float )obj).isNaN() == true))
            {
                return true;
            }

            if (((leftBits == posBits) && (rightBits == negBits)) ||
                ((leftBits == posBits) && (rightBits == negBits)))
            {
                return true;
            }

            return (leftBits == rightBits);
        }

        return false;
    }

    public int hashCode()
    {
        return this.intValue();
    }

    public float floatValue()
    {
        return value;
    }

    public int intValue()
    {
        return (int)value;
    }

    public String toString()
    {
        return toString(this.floatValue());
    }

    public static boolean isNaN(float v)
    {
        return (v != v);
    }

    public boolean isNaN()
    {
        return isNaN(this.floatValue());
    }

    public static boolean isInfinite(float v)
    {
        return (floatToIntBits(v) == floatToIntBits(POSITIVE_INFINITY)) ||
               (floatToIntBits(v) == floatToIntBits(NEGATIVE_INFINITY));
    }

    public boolean isInfinite()
    {
        return isInfinite(this.floatValue());
    }

    public static Float valueOf(String s) throws NumberFormatException
    {
        byte src[] = new byte[s.length()+1];
        s.getBytes(0, s.length(), src, 0);
        src[src.length-1] = 0;
        float f = parseFloat(src);
        if (isNaN(f))
        {
            throw new NumberFormatException();
        }

        return new Float(f);
    }

    public static String toString(float f)
    {
        byte dest[] = new byte[32];
        int count = toString0(f, dest);

        return new String(dest, 0, count);
    }

    public static native int floatToIntBits(float value);
    public static native float intBitsToFloat(int bits);

    private static native float parseFloat(byte s[]);
    private static native int toString0(float f, byte dest[]);
}
