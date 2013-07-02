/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;


/**
 * Super class for some java primitive types.
 *
 * For detailed description of this class see the Java language documentation.
 */
abstract public class Number
{
    public byte byteValue()
    {
        return (byte)intValue();
    }

    abstract public float floatValue();

    abstract public int intValue();

    public short shortValue()
    {
        return (short)intValue();
    }
}
